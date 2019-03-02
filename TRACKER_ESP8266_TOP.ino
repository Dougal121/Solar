#include <ESP8266WiFi.h>
#include <WiFiUDP.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266httpUpdate.h>
//#include <DNSServer.h>
#include <TimeLib.h>
#include <Wire.h>
//#include <SPI.h>
#include <EEPROM.h>
#include <stdio.h>
#include <LSM303.h>       // modified ... fixed a couple of bugs
#include <L3G.h>
#include <SFE_BMP180.h>
#include <TinyGPS.h>
#include "ht16k33.h"

#include "SSD1306.h"
#include "SH1106.h"
#include "SH1106Wire.h"
#include "ds3231.h"


#define BUFF_MAX 32
/* Display settings */
#define minRow       0              /* default =   0 */
#define maxRow     127              /* default = 127 */
#define minLine      0              /* default =   0 */
#define maxLine     63              /* default =  63 */

#define LineText     0
#define Line        12

const byte SETPMODE_PIN = D0 ; 
const byte FLASH_BTN = D3 ;    // GPIO 0 = FLASH BUTTON 
const byte SCOPE_PIN = D3 ;
const byte FACTORY_RESET = D0 ;
const byte LED = BUILTIN_LED ;  // = D4 ;

SSD1306 display(0x3c, 5, 4);   // GPIO 5 = D1, GPIO 4 = D2
//SH1106Wire display(0x3c, 4, 5);   // arse about ??? GPIO 5 = D1, GPIO 4 = D2

#define BUFF_MAX 32
#define PARK_EAST 1
#define PARK_WEST 2
#define PARK_NORTH 3
#define PARK_SOUTH 4
#define PARK_FLAT 5

#define MOTOR_DWELL 100

#define HT16K33_DSP_NOBLINK 0   // constants for the half arsed cheapo display
#define HT16K33_DSP_BLINK1HZ 4
#define HT16K33_DSP_BLINK2HZ 2
#define HT16K33_DSP_BLINK05HZ 6


const byte RELAY_XZ_PWM = D5; // PWM 1    Speed North / South     Was the X- S relay  Orange 
const byte RELAY_YZ_PWM = D6; // PWM 2    Speed East / West       Was the Y+ W relay  Blue
const byte RELAY_YZ_DIR = D7; // DIR 2    Y+ Y- East / West       Was the Y- E relay  Yellow
const byte RELAY_XZ_DIR = D8; // DIR 1    X+ X-  North / South    Was the X+ N relay  Brown



static bool hasSD = false;
static bool hasNet = false;
static bool hasGyro = false;
static bool hasRTC = false;
static bool hasPres = false ;

char dayarray[8] = {'S','M','T','W','T','F','S','E'} ;

char NodeName[16] ={"South_Tower\0"} ;
char nssid[20] ;
char npassword[20] ;
char timeServer[40] = {"au.pool.ntp.org\0"};

char buff[BUFF_MAX]; 

IPAddress MyIP(192,168,2,110) ;
IPAddress RCIP(192,168,2,255) ;

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

byte rtc_sec ;
byte rtc_min ;
byte rtc_hour ;
byte rtc_fert_hour ;
float rtc_temp ;
unsigned int localPort = 2390;      // local port to listen for NTP UDP packets
unsigned int localPortCtrl = 8666;      // local port to listen for Control UDP packets
unsigned int RemotePortCtrl = 8664;      // local port to listen for Control UDP packets

L3G gyro;
LSM303 compass;
SFE_BMP180 pressure;
TinyGPS gps;
HT16K33 HT;

int motor_recycle_x = 0 ;
int motor_recycle_y = 0 ;
char trackername[18] ;
unsigned long  gpschars ; 
float heading ;          // MODBUS MAP
struct ts tb;             // 
struct ts tn;             // 
struct ts td;            //   
struct ts tg;            // 
struct ts tc;            //
float ha ;
float sunX ;
float sunrise ;
float sunset ;
int iNightShutdown ;     //
int iMultiDrive   ;      // 69    do the axis drives run together 
time_t setchiptime ;     // 68    if set to non zero this will trigger a time set event
float zAng ;             // 66
float xMul = 1.0 ;       // 64
float yMul = 1.0 ;       // 62
float zMul = 1.0 ;       // 60
int iXYS = 0 ;           // 59
int iSave = 0 ;          // 58    
int iDoSave = 0 ;        // 57
int iGPSLock = 0  ;      // 56 
unsigned long  fixage ;  // 54
float xRoll = 0.0 ;      // 52
float yRoll = 0.0 ;      // 50
float zRoll = 0.0 ;      // 48
float gT ;               // 46  temp from sensor
float Pr ;               // 44  presure sensor
float alt ;              // 42  altitude from GPS
float T;                 // 40  temperature of board (if has RTC)
float xzTarget ;         // 38  target for angles
float yzTarget ;         // 36
float xzH ;              // 34 hyserisis zone
float yzH ;              // 32
float xzAng;             // 30 current angles
float yzAng;             // 28
float xzOffset;          // 26 offset xz
float yzOffset;          // 24 offset yz
float dyPark;            // 22 parking position
float dxPark;            // 20 
float xMinVal ;          // 18 Min and Max values  X - N/S
float xMaxVal ;          // 16 
float yMinVal ;          // 14  Y  -- E/W
float yMaxVal ;          // 12 
float latitude;          // 10
float longitude;         // 8
int timezone;            // 7
int iDayNight ;          // 6
float solar_az_deg;      // 4
float solar_el_deg;      // 2
int iTrackMode ;         // 1 
int iMode ;              // 0

int iPMode;
int iPWM_YZ ;
int iPWM_XZ ;
int iPowerUp = 0 ;

int iUseGPS = 0 ;

long lTimeZone ;
long lScanCtr = 0 ;
long lScanLast = 0 ;
time_t AutoOff_t ;         // auto off until time > this date
bool bConfig = false ;
uint8_t rtc_status ;


bool bDoTimeUpdate = false ;
long lTimePrev ;
long lTimePrev2 ;

WiFiUDP ntpudp;
WiFiUDP ctrludp;

ESP8266WebServer server(80);
ESP8266WebServer OTAWebServer(81);
ESP8266HTTPUpdateServer OTAWebUpdater;
//DNSServer dnsServer;



void StopYZ(){
  iPWM_YZ=0 ;
  motor_recycle_y = MOTOR_DWELL ;
}
void StopXZ(){
  iPWM_XZ=0 ;
  motor_recycle_x = MOTOR_DWELL ;
}

void ActivateRelays(int iAllStop) {
  if (motor_recycle_y > 0 ){
    motor_recycle_y-- ;
  }
  if (motor_recycle_x > 0 ){
    motor_recycle_x-- ;
  }
  if ( iAllStop == 0 ) {
    StopYZ() ;
    StopXZ() ;
  } else {
    if (( iPWM_YZ==0 ) && (motor_recycle_y == 0 )){
      if ((( yzAng  ) < ( yzTarget - yzH )) ) {   // do Y ie E/W before N/S
        digitalWrite(RELAY_YZ_DIR, LOW) ;
        iPWM_YZ=2 ;
      }
      if ((( yzAng ) > ( yzTarget + yzH )) ) {
        digitalWrite(RELAY_YZ_DIR, HIGH) ;
        iPWM_YZ=2 ;
      }
    }
    if ( iPWM_YZ>0 ){
      if ((yzAng > yzTarget) && ( digitalRead(RELAY_YZ_DIR)==LOW )) {
        StopYZ() ;
      }
      if ((yzAng < yzTarget) && ( digitalRead(RELAY_YZ_DIR)==HIGH )) {
        StopYZ() ;
      }
    }

    if (( iPWM_YZ==0) || ( iMultiDrive == 1 )) {  // if finished on E/W you can do N/S  or if we are doing multidrive
        if (( iPWM_XZ==0 ) && (motor_recycle_x == 0 )){
          if ((xzAng < ( xzTarget - xzH ))  )  { // turn on if not in tolerance
            digitalWrite(RELAY_XZ_DIR, LOW) ;
            iPWM_XZ=2 ;
          }
          if ((xzAng > ( xzTarget + xzH )) ) { // turn on if not in tolerance
            digitalWrite(RELAY_XZ_DIR, HIGH) ;
            iPWM_XZ=2 ;
          }
        }
    }else{
      if ((iPWM_XZ>0 )){
        StopXZ() ;
      }
    }
    if ( iPWM_XZ>0 ){
      if ((xzAng > xzTarget ) && ( digitalRead(RELAY_XZ_DIR)==LOW ))  { // if on turn off
        StopXZ() ;
      }
      if ((xzAng < xzTarget ) && ( digitalRead(RELAY_XZ_DIR)==HIGH ))  { // if on turn off
        StopXZ() ;
      }
    }
  }
  if (iPWM_XZ>0){
    iPWM_XZ += 1 ;
  }
  if (iPWM_YZ>0){
    iPWM_YZ += 1 ;
  }
  iPWM_XZ = constrain(iPWM_XZ,0,1023);  // 254 in atmel - arduino land 
  iPWM_YZ = constrain(iPWM_YZ,0,1023);  //   
  analogWrite(RELAY_XZ_PWM,iPWM_XZ);
  analogWrite(RELAY_YZ_PWM,iPWM_YZ);
}

// Arduino doesnt have these to we define from a sandard libruary
float arcsin(float x) {
  return (atan(x / sqrt(-x * x + 1)));
}
float arccos(float x) {
  return (atan(x / sqrt(-x * x + 1)) + (2 * atan(1)));
}
// fractional orbital rotation in radians
float gama(struct ts *tm) {
  return ((2 * PI / 365 ) *  DayOfYear(tm->year , tm->mon , tm->mday , tm->hour , tm->min ));
}
// equation of rime
float eqTime(float g) {
  return (229.18 * ( 0.000075 + ( 0.001868 * cos(g)) - (0.032077 * sin(g)) - (0.014615 * cos (2 * g)) - (0.040849 * sin(2 * g))));
}
// declination of sun in radians
float Decl(float g) {
  return ( 0.006918 - (0.399912 * cos(g)) + (0.070257 * sin(g)) - (0.006758 * cos(2 * g)) + ( 0.000907 * sin(2 * g)) - ( 0.002697 * cos(3 * g)) + (0.00148 * sin(3 * g)) );
}
float TimeOffset(float longitude , struct ts *tm ,  int timezone ) {
  float dTmp ;
  dTmp = (-4.0 * longitude ) + (60 * timezone) - eqTime(gama(tm)) ;
  return (dTmp);
}

float TrueSolarTime(float longitude , struct ts *tm ,  int timezone ) {
  float dTmp ;
  dTmp = ( 60.0 * tm->hour ) + (1.0 * tm->min) + (1.0 * tm->sec / 60) - TimeOffset(longitude, tm, timezone) ;
  return (dTmp);
}
float HourAngle(float longitude , struct  ts *tm ,  int timezone) {
  float dTmp;
  dTmp = (TrueSolarTime(longitude, tm, timezone) / 4 ) - 180 ; // 720 minutes is solar noon -- div 4 is 180
  return (dTmp);
}
// Hour angle for sunrise and sunset only
float HA (float lat , struct ts *tm ) {
  float latRad ;
  latRad = lat * 2 * PI / 360 ;
  return ( acos((cos(90.833 * PI / 180 ) / ( cos(latRad) * cos(Decl(gama(tm)))) - (tan(latRad) * tan(Decl(gama(tm)))))) / PI * 180  );
}

float Sunrise(float longitude , float lat , struct ts *tm , int timezone) {
  return (720 - ( 4.0 * (longitude + HA(lat, tm))) + (60 * timezone) - eqTime(gama(tm))  ) ;
}
float Sunset(float longitude , float lat , struct ts *tm , int timezone) {
  return (720 - ( 4.0 * (longitude - HA(lat, tm))) + (60 * timezone) - eqTime(gama(tm))  ) ;
}
float SNoon(float longitude , float lat , struct ts *tm , int timezone) {
  return (720 - ( 4.0 * (longitude  + (60 * timezone) - eqTime(gama(tm))))  ) ;
}

float SolarZenithRad(float longitude , float lat , struct ts *tm , int timezone) {
  float latRad ;
  float decRad ;
  float HourAngleRad ;
  float dTmp ;

  latRad = lat * 2 * PI / 360 ;
  decRad = Decl(gama(tm));
  HourAngleRad = HourAngle (longitude , tm , timezone ) * PI / 180 ;
  dTmp = acos((sin(latRad) * sin(decRad)) + (cos(latRad) * cos(decRad) * cos(HourAngleRad)));
  return (dTmp) ;

}
float SolarElevationRad(float longitude , float lat , struct ts *tm ,  int timezone ) {
  return ((PI / 2) - SolarZenithRad(longitude , lat , tm , timezone )) ;
}

float SolarAzimouthRad(float longitude , float lat , struct ts *tm ,  int timezone) {
  float latRad ;
  float decRad ;
  float solarzenRad ;
  float HourAngleRad ;
  float dTmp ;
  latRad = lat * 2 * PI / 360 ;
  decRad = Decl(gama(tm));
  solarzenRad = SolarZenithRad ( longitude , lat , tm , timezone ) ;
  HourAngleRad = HourAngle (longitude , tm , timezone ) * PI / 180 ;
  dTmp = acos(((sin(decRad) * cos(latRad)) - (cos(HourAngleRad) * cos(decRad) * sin(latRad))) / sin(solarzenRad)) ;
  if ( HourAngleRad < 0 ) {
    return (dTmp) ;
  } else {
    return ((2 * PI) - dTmp) ;
  }
}

int NumberOK (float target) {
  int tmp = 0 ;
  tmp = isnan(target);
  if ( tmp != 1 ) {
    tmp = isinf(target);
  }
  return (tmp);
}




unsigned long sendCTRLpacket(IPAddress address){
int j ;  
byte packetBuffer[50];           //buffer to hold outgoing packets  
  Serial.println("sending CTRL packet...");
                    
  memset(packetBuffer, 0, sizeof(packetBuffer[50]));    // set all bytes in the buffer to 0
  packetBuffer[0] = 0xff;                       // broadcast as all stations
  packetBuffer[1] = 0xff;  // 
  packetBuffer[2] = 0xff;  // 
  packetBuffer[3] = 0xff;  // 
  
  ctrludp.beginPacket(address, RemotePortCtrl);      // Send control data to the remote port - Broadcast ???
  ctrludp.write(packetBuffer, sizeof(packetBuffer[50]));
  ctrludp.endPacket();
}




 

void FloatToModbusWords(float src_value , uint16_t * dest_lo , uint16_t * dest_hi ) {
  uint16_t tempdata[2] ;
  float *tf ;
  tf = (float * )&tempdata[0]  ;
  *tf = src_value ;
  *dest_lo = tempdata[1] ;
  *dest_hi = tempdata[0] ;
}
float FloatFromModbusWords( uint16_t dest_lo , uint16_t dest_hi ) {
  uint16_t tempdata[2] ;
  float *tf ;
  tf = (float * )&tempdata[0]  ;
  tempdata[1] = dest_lo ;
  tempdata[0] = dest_hi  ;
  return (*tf) ;
}

float LoadFloatFromEEPROM(int address,float minval,float maxval, float defaultval){
float tmp ;  
int i ;
byte  *ba ;  

  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    ba[i] = EEPROM.read((address*4)+i);  // read the 4 bytes
  }
  if (( tmp < minval ) || ( tmp > maxval )) {
    tmp = defaultval ;
    for ( i = 0 ; i < 4 ; i++ ){
      EEPROM.write((address*4)+i , ba[i] );
      EEPROM.commit();  // save changes in one go ???    
    }  
  }
  return(tmp);  
}

int LoadIntFromEEPROM(int address,int minval,int maxval, int defaultval){
int dummy1 = 0 ; // belt and braces ... dont know which way the stack works
int tmp ;  
int dummy2 = 0 ; // yep write this one as well ... maybe
int i ;
byte  *ba ;  

  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    ba[i] = EEPROM.read((address*4)+i);  // read the 4 bytes
  }
  if (( tmp < minval ) || ( tmp > maxval )) {
    tmp = defaultval ;
    for ( i = 0 ; i < 4 ; i++ ){
      EEPROM.write((address*4)+i , ba[i] );
      EEPROM.commit();  // save changes in one go ???    
    }  
  }
  return(tmp);  
}
int LoadLongFromEEPROM(int address,long minval,long maxval, long defaultval){
long tmp ;
int i ;
byte  *ba ;  

  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    ba[i] = EEPROM.read((address*4)+i);  // read the 4 bytes
  }
  if (( tmp < minval ) || ( tmp > maxval )) {
    tmp = defaultval ;
    for ( i = 0 ; i < 4 ; i++ ){
      EEPROM.write((address*4)+i , ba[i] );
      EEPROM.commit();  // save changes in one go ???    
    }  
  }
  return(tmp);  
}

time_t LoadTimeFromEEPROM(int address, time_t defaultval){
time_t tmp ;
int i ;
byte  *ba ;  

  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    ba[i] = EEPROM.read((address*4)+i);  // read the 4 bytes
  }
  if ( year(tmp) < 2000 ) {
    tmp = defaultval ;
    for ( i = 0 ; i < 4 ; i++ ){
      EEPROM.write((address*4)+i , ba[i] );
      EEPROM.commit();  // save changes in one go ???    
    }  
  }
  return(tmp);  
}

void LoadCharFromEEPROM(int address , char * target , int targetsize ){
  for ( int i = 0 ; i < targetsize ; i++ ){
    target[i] = EEPROM.read((address*4)+i);  
  }
}
void SaveCharToEEPROM(int address , char * target , int targetsize ){
  for ( int i = 0 ; i < targetsize ; i++ ){
    EEPROM.write((address*4)+i,target[i]);  
  }
}

void LoadIPFromEEPROM(int address , IPAddress * target ){
  for ( int i = 0 ; i < 4 ; i++ ){
    target[i] = EEPROM.read((address*4)+i);  
  }  
}
void SaveIPToEEPROM(int address , IPAddress * target ){
  for ( int i = 0 ; i < 4 ; i++ ){
    EEPROM.write((address*4)+i,target[i]);  
  }  
}
byte LoadDayByteFromEEPROM(int address, int ofs){
byte tmp ;  
  ofs %= 4 ;
  tmp = EEPROM.read((address*4)+ofs);  // read the 4 bytes
  return(tmp);  
}

void SaveDayByteToEEPROM(int address, int ofs,byte val){
byte tmp ;  
  ofs %= 4 ;
  EEPROM.write((address*4)+ofs,val);  // read the 4 bytes
}

void SaveFloatToEEPROM(int address,float val){
float tmp ;  
int i ;
byte  *ba ;  
  tmp = val ;
  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    EEPROM.write((address*4)+i , ba[i] );
  }  
}
void SaveLongToEEPROM(int address,long val){
long tmp ;  
int i ;
byte  *ba ;  
  tmp = val ;
  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    EEPROM.write((address*4)+i , ba[i] );
  }  
}
void SaveTimeToEEPROM(int address,time_t val){
time_t tmp ;  
int i ;
byte  *ba ;  
  tmp = val ;
  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    EEPROM.write((address*4)+i , ba[i] );
  }  
}
void SaveIntToEEPROM(int address,int val){
int dummy1 = 0 ;
int tmp ;  
int dummy2 = 0 ;
int i ;
byte  *ba ;  
  tmp = val ;
  ba =(byte *)&tmp ; // set the byte array to point at the long
  for ( i = 0 ; i < 4 ; i++ ){
    EEPROM.write((address*4)+i , ba[i] );
  }  
}

void LoadParamsFromEEPROM(bool bLoad){
  if ( bLoad ) {
    xzH = LoadFloatFromEEPROM(0,0.1,20.0,4.0);  // hysterisis NS
    yzH = LoadFloatFromEEPROM(1,0.1,20.0,4.0);  //    ""      EW
  
    dyPark = LoadFloatFromEEPROM(2,-70.0,50.0,0);  
    dxPark = LoadFloatFromEEPROM(3,-5.0,50.0,0.0);  
    
    xzOffset = LoadFloatFromEEPROM(4,-90.0,90.0,0);  // NS
    yzOffset = LoadFloatFromEEPROM(5,-90.0,90.0,0);  // EW
  
    xzTarget = LoadFloatFromEEPROM(6,-90.0,90.0,0);  // NS
    yzTarget = LoadFloatFromEEPROM(7,-90.0,90.0,0);  // EW
  
    xMinVal = LoadFloatFromEEPROM(8,-10.0,60.0,0.0);   // NS
    xMaxVal = LoadFloatFromEEPROM(9,-10.0,60.0,45);
  
    yMinVal = LoadFloatFromEEPROM(10,-70.0,50.0,-65);  // EW
    yMaxVal = LoadFloatFromEEPROM(11,-70.0,50.0,45);
  
    iTrackMode = LoadIntFromEEPROM(12,-1,4,0);
      
    latitude = LoadFloatFromEEPROM(13,-90.0,90.0,-34.051219);
    longitude = LoadFloatFromEEPROM(14,-180.0,180.0,142.013618);
    timezone = LoadIntFromEEPROM(15,0,23,10);  
    xMul = LoadFloatFromEEPROM(16,-10,10,1);  
    yMul = LoadFloatFromEEPROM(17,-10,10,1);  
    zMul = LoadFloatFromEEPROM(18,-10,10,1);  
    iXYS = LoadIntFromEEPROM(19,0,1,0);
    if ( xMul == 0.0 )  // zero is rubbish value so take 1.0 as the default
      xMul = 1.0 ;
    if ( yMul == 0.0 )
      yMul = 1.0 ;
    if ( zMul == 0.0 )
      zMul = 1.0 ;
    iNightShutdown = LoadIntFromEEPROM(20,0,1,1);
    iMultiDrive = LoadIntFromEEPROM(21,0,1,0);
    EEPROM.get((23 * sizeof(float)) , iUseGPS ) ;
    if (digitalRead(FACTORY_RESET)== LOW) {
        MyIP = IPAddress(192,168,42,1);
        sprintf(trackername,"Most Excellent\0");
        sprintf(nssid , "Configure\0") ;
        sprintf(npassword, "\0");      
    }else{
      EEPROM.get((22 * sizeof(float)) , MyIP );
      if ((( MyIP[0] == 255 ) && ( MyIP[1] == 255 ))) {
        MyIP = IPAddress(192,168,42,1);
      }
      EEPROM.get((30 * sizeof(float)) , trackername );
      if ( String(trackername).length() < 2 ){
        sprintf(trackername,"Most Excellent\0");
      }
      EEPROM.get((40 * sizeof(float)) , nssid );
      if ( String(nssid).length() < 2 ) {
        sprintf(nssid , "Configure\0") ;
        sprintf(npassword, "\0");
      }else{
        EEPROM.get((45 * sizeof(float)) , npassword );
      }
      EEPROM.get((50 * sizeof(float)) , timeServer );
    }
  }else{
    EEPROM.put( 0 , xzH );
    EEPROM.put(0 + (1 * sizeof(float)) , yzH );
    EEPROM.put(0 + (2 * sizeof(float)) , dyPark );
    EEPROM.put(0 + (3 * sizeof(float)) , dxPark );
    EEPROM.put(0 + (4 * sizeof(float)) , xzOffset );
    EEPROM.put(0 + (5 * sizeof(float)) , yzOffset );
    EEPROM.put(0 + (6 * sizeof(float)) , xzTarget );
    EEPROM.put(0 + (7 * sizeof(float)) , yzTarget );
    EEPROM.put(0 + (8 * sizeof(float)) , xMinVal );
    EEPROM.put(0 + (9 * sizeof(float)) , xMaxVal );
    EEPROM.put(0 + (10 * sizeof(float)) , yMinVal );
    EEPROM.put(0 + (11 * sizeof(float)) , yMaxVal );
    EEPROM.put(0 + (12 * sizeof(float)) , iTrackMode );
    EEPROM.put(0 + (13 * sizeof(float)) , latitude );
    EEPROM.put(0 + (14 * sizeof(float)) , longitude );
    EEPROM.put(0 + (15 * sizeof(float)) , timezone );  
    EEPROM.put(0 + (16 * sizeof(float)) , xMul );
    EEPROM.put(0 + (17 * sizeof(float)) , yMul );
    EEPROM.put(0 + (18 * sizeof(float)) , zMul );
    EEPROM.put(0 + (19 * sizeof(float)) , iXYS );  
    EEPROM.put(0 + (20 * sizeof(float)) , iNightShutdown );  
    EEPROM.put(0 + (21 * sizeof(float)) , iMultiDrive );  
    EEPROM.put(0 + (22 * sizeof(float)) , MyIP );  
    EEPROM.put(0 + (23 * sizeof(float)) , iUseGPS );      
    EEPROM.put(0 + (30 * sizeof(float)) , trackername);
    EEPROM.put(0 + (40 * sizeof(float)) , nssid);
    EEPROM.put(0 + (45 * sizeof(float)) , npassword);
    EEPROM.put(0 + (50 * sizeof(float)) , timeServer);
    EEPROM.commit();                                                       // save changes in one go ???
  }
}


//  ##############################  SETUP   #############################
void setup() {
int i , j = 0; 
String host ;
 
  pinMode(LED,OUTPUT);  //  builtin LED
  pinMode(SETPMODE_PIN,INPUT_PULLUP);
  
  display.init();
  display.flipScreenVertically();

  /* show start screen */
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "ESP Solar");
  display.drawString(0, 16, "Tracker");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 40, "Copyright (c) 2018");
  display.drawString(0, 50, "Dougal Plummer");
  display.display();

  compass.init();
  compass.enableDefault();
  compass.setTimeout(1000);
  
  if (gyro.init()) {
    gyro.enableDefault();
    hasGyro = true ;
  }
  if (pressure.begin()){
    Serial.println("BMP180 init success");
    hasPres = true ;
  }      
//  pinMode(FACTORY_RESET,INPUT_PULLUP);
  pinMode(RELAY_XZ_DIR, OUTPUT); // Outputs for PWM motor control
  pinMode(RELAY_XZ_PWM, OUTPUT); // 
  pinMode(RELAY_YZ_PWM, OUTPUT); //
  pinMode(RELAY_YZ_DIR, OUTPUT); // 
  iPWM_YZ = 0 ;
  iPWM_XZ = 0 ;
  ActivateRelays(0); // call an all stop first

  EEPROM.begin(1024);
  LoadParamsFromEEPROM(true);

  compass.m_min = (LSM303::vector<int16_t>) {-3848, -1822, -1551 };   // calibration figures are empirical
  compass.m_max = (LSM303::vector<int16_t>) { +3353, +5127, +5300};

  
  delay(1000);
  if (iUseGPS==0){
    Serial.begin(115200);
    Serial.println("Warp Speed no GPS !");
  }else{
    Serial.begin(9600);    
  }
  Serial.setDebugOutput(true);  
  Serial.println("Chip ID " + String(ESP.getChipId(), HEX));
  Serial.println("Configuring WiFi...");

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 11, "Chip ID " + String(ESP.getChipId(), HEX) );
  display.display();

//  sprintf(nssid,"WLAN-PLUMMER\0") ;
//  sprintf(npassword,"cheegh5S\0") ;
  sprintf(nssid,"TP-LINK_52FC8C\0");
  sprintf(npassword,"0052FC8C\0");

  if ((digitalRead(SETPMODE_PIN) == HIGH) ){
    bConfig = true ;
    IPAddress localIp(192, 168, 5 +(ESP.getChipId() & 0x7f ) , 1);
    IPAddress MaskIp(255, 255, 255 , 0);
    WiFi.softAPConfig(localIp,localIp,MaskIp);
    WiFi.softAP(nssid); // configure mode no password
    MyIP = WiFi.softAPIP();
    Serial.print("Soft AP IP address: ");
    Serial.println(MyIP);
    display.drawString(0, 22, "Soft AP IP address: "+String(MyIP) );
    display.display();
  }else{
    bConfig = false ;   // are we in factory configuratin mode
    Serial.println(String(nssid));
    Serial.println(String(npassword));
    display.drawString(0, 22, String(nssid) );
    display.drawString(0, 33, String(npassword) );
    display.display();
    if ( npassword[0] == 0 ){
      WiFi.begin((char*)nssid);                    // connect to unencrypted access point      
    }else{
      WiFi.begin((char*)nssid, (char*)npassword);  // connect to access point with encryption
    }
    while (( WiFi.status() != WL_CONNECTED ) && ( j < 20 )) {
     j = j + 1 ;
     delay(500);
     Serial.print("+");
    } 
    if ( j >= 20 ) {
       bConfig = true ;
       WiFi.disconnect();
       IPAddress localIp(192, 168, 5 +(ESP.getChipId() & 0x7f ) , 1);
       IPAddress MaskIp(255, 255, 255 , 0);
       WiFi.softAPConfig(localIp,localIp,MaskIp);
       WiFi.softAP(nssid); // configure mode no password
       MyIP = WiFi.softAPIP();
       Serial.print("Soft AP IP address: ");
       Serial.println(MyIP);
       display.drawString(0, 22, "Soft AP IP address: "+String(MyIP) );
       display.display();
    }else{
       Serial.println("");
       Serial.println("WiFi connected");  
       Serial.print("IP address: ");
       MyIP =  WiFi.localIP() ;
       Serial.println(MyIP) ;
       display.drawString(0, 53, "IP "+String(MyIP) );
       display.display();
       hasNet = true ;
    }
    if (localPortCtrl == localPort ){             // bump the NTP port up if they ar the same
      localPort++ ;
    }
    Serial.println("Starting UDP");
    ntpudp.begin(localPort);                      // this is the recieve on NTP port
    display.drawString(0, 44, "NTP UDP " );
    display.display();
    Serial.print("NTP Local UDP port: ");
    Serial.println(ntpudp.localPort());
    ctrludp.begin(localPortCtrl);                 // recieve on the control port
    display.drawString(64, 44, "CTRL UDP " );
    display.display();
    Serial.print("Control Local UDP port: ");
    Serial.println(ctrludp.localPort());
  }                                              // end of the normal setup
  host = trackername ;
  String(host).replace(" ","_");
  String(host).toCharArray(buff,sizeof(buff));
  if (MDNS.begin(buff)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }

  server.on("/", handleRoot);
  server.on("/setup", handleRoot);
  server.on("/scan", i2cScan);
  server.on("/stime", handleRoot);
  server.onNotFound(handleNotFound);  
  server.begin();
  Serial.println("HTTP server started");
 
//  dnsServer.setTTL(300);
//  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
//  dnsServer.start(53,"injector.local",myIP);
  tc.mon = 0 ;
  tc.wday = 0 ;
  DS3231_init(DS3231_INTCN); // look for a rtc
  DS3231_get(&tc);
  rtc_status = DS3231_get_sreg();
  if (((tc.mon < 1 )|| (tc.mon > 12 ))&& (tc.wday>8)){  // no rtc to load off
    Serial.println("NO RTC ?");
  }else{
    setTime((int)tc.hour,(int)tc.min,(int)tc.sec,(int)tc.mday,(int)tc.mon,(int)tc.year ) ; // set the internal RTC
    hasRTC = true ;
    Serial.println("Has RTC ?");
    rtc_temp = DS3231_get_treg(); 
    DS3231_get(&tb);
  }
  rtc_min = minute();
  rtc_sec = second();


  HT.begin(0x00);
  for (int led = 0; led < 127; led++) {
    HT.clearLed(led);
  } 
  HT.sendLed();  
  Serial.println("OTA startup");  
  OTAWebUpdater.setup(&OTAWebServer);
  OTAWebServer.begin();  
  Serial.println("End of Setup");

}

//  ##############################  LOOP   #############################
void loop() {
long lTime ;  
long lRet ;
int i , j , k  ;
float P;
float sunInc;
float sunAng;
float xzRatio;
float yzRatio;
float decl ;
float eqtime ;
float dTmp ;
float heading ;
float tst ;
float flat, flon;
unsigned short goodsent;
unsigned short failcs;
String msg ;

bool bSendCtrlPacket = false ;

  server.handleClient();
  OTAWebServer.handleClient();
  
  lTime = millis() ;

  compass.read();  // this reads all 6 channels

  if (( compass.a.z != 0) && (!compass.timeoutOccurred() ))  {
    zAng = (float)compass.a.z ;
    if (iXYS == 0 ){                                            // Proper Job make it configurable 
      xzRatio = (float)compass.a.x * xMul / abs(zAng) ;         // Normal
      yzRatio = (float)compass.a.y * yMul / abs(zAng) ;
    }else{
      xzRatio = (float)compass.a.y * xMul / abs(zAng) ;         // Swapped
      yzRatio = (float)compass.a.x * yMul / abs(zAng) ;      
    }
    xzAng = ((float)atan(xzRatio) / PI * 180 ) + xzOffset ;     // good old offsets or fudge factors
    yzAng = ((float)atan(yzRatio) / PI * 180 ) + yzOffset ;
  }else{                                                        // try restarting the compass/accelerometer modual - cos he gone walkabout...
    Wire.begin();   // reset the I2C
    compass.init();
    compass.enableDefault();
    compass.setTimeout(1000);                                           // BTW I fixed up the int / long issue in the time out function in the LM303 lib I was using
    compass.m_min = (LSM303::vector<int16_t>) {-3848, -1822, -1551 };   // calibration figures are empirical (just whirl it around a bit and records the min max !!)
    compass.m_max = (LSM303::vector<int16_t>) { +3353, +5127, +5300 };
  }
   
//  digitalWrite(SCOPE_PIN,!digitalRead(SCOPE_PIN));  // my scope says we are doing this loop at an unreasonable speed except when we do web stuff
//  pwmtest++ ;
//  if ( pwmtest > 1024 ) {
//    pwmtest = 0 ;
//  }
//  analogWrite(SCOPE_PIN,pwmtest); 

  delay(1);                  // limmit to 1000 cyclces a second max
  
  if (digitalRead(FLASH_BTN) == LOW) { // what to do if the button be pressed
  }
      
  lScanCtr++ ;
  bSendCtrlPacket = false ;
  if ( rtc_sec != second()){
//    digitalWrite(LED,!digitalRead(LED));  // my scope says we are doing this loop at an unreasonable speed except when we do web stuff
    rtc_sec = second() ;

    display.clear();
//      display.drawLine(minRow, 63, maxRow, 63);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    display.drawString(0 , LineText, String(buff) );
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128 , LineText, String(WiFi.RSSI()));
    display.setTextAlignment(TEXT_ALIGN_LEFT);
//    snprintf(buff, BUFF_MAX, "TX %d TY %d", year(), month() );
    msg = "X  " + String(xzAng,2)  ;
    display.drawString(0 , 11, msg ) ;
    msg = "Y  " + String(yzAng,2)  ;
    display.drawString(56 , 11, msg ) ;
    msg = "TX " + String(xzTarget,2) ;
    display.drawString(0 , 22, msg ) ;
    msg = "TY " + String(yzTarget,2) ;
    display.drawString(56 , 22, msg ) ;
    msg = "DX " + String((xzAng-xzTarget),2) ;
    display.drawString(0 , 33, msg ) ;
    msg = "DY " + String((yzAng-yzTarget),2) ;
    display.drawString(56 , 33, msg ) ;

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    msg = "" ;
    if ( iPWM_YZ != 0 ) {
      if (( digitalRead(RELAY_YZ_DIR) == LOW )) {
        msg = "W" ;
      }else{
        msg = "E" ;            
      }
    }
    if ( iPWM_XZ != 0 ) {
      if (( digitalRead(RELAY_XZ_DIR) == LOW )) {
        msg += "N" ;
      }else{
        msg += "S" ;            
      }
    } 
    display.drawString(128 , 11, msg ) ;
//    display.drawString(128 , 22, String(pwmtest)) ;
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(sunrise), MinSolarTime(sunrise));
    display.drawString(0 , 44, buff ) ;
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    if (iDayNight == 1) {
      msg = " DAY " ;
    } else {
      msg = "NIGHT" ;
    }
    if (hasRTC) {
      if (( rtc_status & 0x80 ) != 0 ){
        msg = "RTC FLT" ;   // detect RTC battery and other faults and display them on screen        
      }
    }
    display.drawString(64 , 44, msg ) ;

    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(sunset), MinSolarTime(sunset));
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128 , 44, buff ) ;
 

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    if ( bConfig ){
      snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
    }else{
      snprintf(buff, BUFF_MAX, "IP %03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);      
    }
    display.drawString(64 , 54 ,  String(buff) );

    display.display();
    if (( rtc_sec > 8 ) && ( rtc_sec < 58 )) {  // dont calculate arround the minute when time is updating from NTP or GPS as might get a not so funny result
      digitalWrite(LED,!digitalRead(LED));
      solar_az_deg = SolarAzimouthRad(longitude, latitude, &tc, timezone) * 180 / PI ;
      solar_el_deg = SolarElevationRad(longitude, latitude, &tc, timezone) * 180 / PI ;
    
      decl = Decl(gama(&tc)) * 180 / PI ;
      ha = HourAngle (longitude , &tc , timezone )  ;
      sunrise = Sunrise(longitude, latitude, &tc, timezone) ;
      sunset = Sunset(longitude, latitude, &tc, timezone);
      tst = TrueSolarTime(longitude, &tc, timezone);
      sunX = abs(latitude) + decl ;
      if (solar_el_deg >= 0 ){           // day
        iDayNight = 1 ;
      }else{                             // night
        iDayNight = 0 ;
      }
    }
    switch (iTrackMode) {
      case 4: // both axis to park
        yzTarget = dyPark ;  // night park position  E/W
        xzTarget = dxPark ;  // night park position  N/S
        break ;
      case 3: // both axis off no tracking
        break ;
      case 2: // xz tracking  NS
        if ( iDayNight == 1 ) {
          xzTarget = sunX ; // need to map the coordinate system correctly
        } else {
          xzTarget = dxPark ;  // night park position
        }
        break;
      case 1:  // yz tracking   EW
        if (iDayNight == 1) {
          yzTarget = ha ;
        } else {
          yzTarget = dyPark ;  // night park position
        }
        break;
      case -1: // set target to tracking and park both at nigh
        if (iDayNight == 1) {
          yzTarget = ha ;
          xzTarget = sunX ; // need to map the coordinate system correctly
        } else {
          yzTarget = dyPark ;  // night park position  E/W
          xzTarget = dxPark ;  // night park position  N/S
        }
        break;
      default: // set target to tracking
        if (iDayNight == 1) {
          yzTarget = ha ;
          xzTarget = sunX ; // need to map the coordinate system correctly
        } else {
          yzTarget = dyPark ;  // night park position (dont park the other - leave till morning)
        }
        break;
    }
    xzTarget = constrain(xzTarget,xMinVal,xMaxVal);   // constain function... very cool - dont leave home without it !
    yzTarget = constrain(yzTarget,yMinVal,yMaxVal);

    DisplayMeatBall() ;
    heading = compass.heading((LSM303::vector<int>) { 1, 0, 0 });

    if ( hasGyro ){
      gyro.read();
      xRoll = gyro.g.x ;
      yRoll = gyro.g.y ;
      zRoll = gyro.g.z ;
    }
    
    if ( iDoSave == 2 ) {  // save them Active via web or 
      LoadParamsFromEEPROM(false);
      iDoSave = 0 ;  // only do once
    }
    if ( iDoSave == 3 ) {  // load them
      LoadParamsFromEEPROM(true);
      iDoSave = 0 ;  // only do once
    }
  }

  if (rtc_hour != hour()){
    bSendCtrlPacket = true ;
    if ( !bConfig ) { // ie we have a network
      sendNTPpacket(timeServer); // send an NTP packet to a time server  once and hour
    }else{
      if ( hasRTC ){
        DS3231_get(&tc);
        setTime((int)tc.hour,(int)tc.min,(int)tc.sec,(int)tc.mday,(int)tc.mon,(int)tc.year ) ; // set the internal RTC
      }
    }
    rtc_hour = hour(); 
  }
  if ( rtc_min != minute()){
    if (hasPres){
      Pr = getPressure((float *)&gT) ;
    }
    if (hasRTC) {
      T = DS3231_get_treg();
    }
    
    if (( year() < 1980 )|| (bDoTimeUpdate)) {  // not the correct time try to fix every minute
      if ( !bConfig ) { // ie we have a network
        sendNTPpacket(timeServer); // send an NTP packet to a time server  
        bDoTimeUpdate = false ;
      }
    }
    if ( hasRTC ){
      rtc_temp = DS3231_get_treg(); 
    }
    rtc_min = minute() ;
    gps.stats(&gpschars, &goodsent , &failcs );
    gps.f_get_position(&flat, &flon,(long unsigned *) &fixage); // return in degrees
    if ((fixage > 0 ) && ( fixage < 40000 )) {   // wait till our fix is valid before we use the values
      iGPSLock = gps.satellites() ;
      alt = gps.f_altitude() ;
      if (iPowerUp==0) {   // only do this at startup so we have a better position ref for next time
          if ( latitude != flat ){
            latitude = flat ;
            EEPROM.put(0 + (13 * sizeof(float)) , latitude );
            EEPROM.commit();
          }
          if ( longitude != flon ){
            longitude = flon ;
            EEPROM.put(0 + (14 * sizeof(float)) , longitude );   
            EEPROM.commit();     
          }
          iPowerUp = 1 ;
          if (!hasNet ){
            SetTimeFromGPS();
          }
      }
    }else{
      iGPSLock = 0 ;   // if no lock loook at internal clock
    }  
  }

  if ( year() > 2000 ){ // dont move if date and time is rubbish
    if (((tc.hour > 19 ) || ( tc.hour < 5 )) && (iTrackMode < 3)) {
      if ( iNightShutdown != 0 ){
        ActivateRelays(1) ;        
      }else{
        ActivateRelays(0) ;  // power down at night if in tracking mode
      }
    }else{
      ActivateRelays(1) ;    
    }
  }else{
    ActivateRelays(0) ;  // power down 
  }
  
  if (second() > 4 ){
    if ( ntpudp.parsePacket() ) {
      processNTPpacket();
    }
  }

  lRet = ctrludp.parsePacket() ;
  if ( lRet != 0 ) {
//    processCtrlUDPpacket(lRet);
  }

  if (lTimePrev > ( lTime + 100000 )){ // Housekeeping --- has wrapped around so back to zero
    lTimePrev = lTime ; // skip a bit 
    Serial.println("Wrap around");
  }

  if (iUseGPS!=0){
    while (Serial.available()){ // process the gps buffer
      gps.encode(Serial.read());
    }  
  }
  
//  dnsServer.processNextRequest();
}   // ####################  BOTTOM OF LOOP  ###########################################

float DayOfYear(uint16_t iYear , uint8_t iMon , uint8_t iDay , uint8_t iHour , uint8_t iMin ) {
  int i ;
  float iTDay ;

  iTDay = iDay - 1 ;  // this is zero referenced
  for ( i = 1 ; i < iMon ; i++ ) {
    switch (i) {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
        iTDay += 31 ;
        break;
      case 4:
      case 6:
      case 9:
      case 11:
        iTDay += 30 ;
        break;
      case 2 :
        if ((iYear % 4) == 0 ) {
          iTDay += 29 ;
        } else {
          iTDay += 28 ;
        }
        break;
    }
  }
  iTDay += (( 1.0 * iHour - 12 ) / 24 ) ;
  //  iDay +=  1.0 * iMin  / 1440 ;
  return (iTDay);
}


int HrsSolarTime(float target) {
  int i ;
  i = target ;
  return (  i / 60 );
}
int MinSolarTime(float target) {
  int i ;
  i = target  ;
  return (   i % 60 );
}

float sign(float target) {
  if (target > 0 ) {
    return (1);
  } else {
    if (target < 0 ) {
      return (-1);
    } else {
      return (0);
    }
  }
}



float getPressure(float* Temp)
{
  char status;
  double T,P;

  status = pressure.startTemperature();
  if (status != 0){
    delay(status);    // Wait for the measurement to complete:
    status = pressure.getTemperature(T);
    if (status != 0){
      status = pressure.startPressure(3);
      if (status != 0){
        // Wait for the measurement to complete:
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0){ 
          gT = T ;
          return(float(P));
        }
      }
    }
  }
}




