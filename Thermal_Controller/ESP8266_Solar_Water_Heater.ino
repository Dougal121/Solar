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
#include "SSD1306.h"
//#include "SH1106.h"
#include "SH1106Wire.h"
#include "ds3231.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266Ping.h>
#include <ESPMail.h>

#define ONE_WIRE_BUS 26
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.
DeviceAddress Thermometer[4];         // arrays to hold device addresses

ESPMail WDmail;

#include <ModbusRtu.h>
#define ID   1
#define MAX_MODBUS_DATA  50
#define MAX_EMAIL_ALARMS 8 
#define MAX_RELAY 4
#define MAX_TEMP_SENSOR 5
#define MAX_MODES 2

uint16_t au16data[MAX_MODBUS_DATA]; //!< data array for modbus network sharing
uint8_t u8state; //!< machine state
uint8_t u8query; //!< pointer to message query

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  unsigned int localPort = 2390;          // 2 local port to listen for NTP UDP packets
  unsigned int localPortCtrl = 8666;      // 4 local port to listen for Control UDP packets
  unsigned int RemotePortCtrl = 8664;     // 6 local port to listen for Control UDP packets
  long lNodeAddress ;                     // 10 
  float fTimeZone ;                       // 14 
  char RCIP[16] ;                         // (192,168,2,255)  30
  char NodeName[32] ;                     // 62 
  char nssid[24] ;                        // 86  
  char npassword[32] ;                    // 108
  time_t AutoOff_t ;                      // 112     auto off until time > this date   
  uint8_t lDisplayOptions  ;              // 113 
  uint8_t lNetworkOptions  ;              // 114 
  uint8_t lSpare1  ;                      // 115 
  uint8_t lSpare2  ;                      // 116 
  char timeServer[24] ;                   // 140   = {"au.pool.ntp.org\0"}
  char cpassword[32] ;                    // 172
  long lVersion  ;                        // 176
  IPAddress IPStatic ;                    // (192,168,0,123)   
  IPAddress IPGateway ;                   // (192,168,0,1)    
  IPAddress IPMask ;                      // (255,255,255,0)   
  IPAddress IPDNS ;                       // (192,168,0,15)   
  long SelfReBoot ;                       // 196    
  IPAddress IPPing  ;                     // 200
  long PingMax ;                          // 204
  long PingFreq ;                         // 208
  float latitude;                         // 212
  float longitude;                        // 216 
} general_housekeeping_stuff_t ;          // computer says it's 136 not 130 ??? is my maths crap ????

general_housekeeping_stuff_t ghks ;

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  float   fCollectorMinTemp ;
  float   fCollectorMaxTemp ;
  float   fSolarTempDiffMax ;
  float   fSolarTempDiffMin ;
  float   fTopBoostTemp ;
  float   fBottomBoostTemp ; 
  float   fTopBoostDiffTemp ;
  float   fBottomBoostDiffTemp ; 
  uint8_t sensor[MAX_TEMP_SENSOR] ;                   // which sensor is where
  uint8_t relayPort[MAX_RELAY] ;
  uint8_t ActiveValue[MAX_RELAY] ;
  uint8_t relayMinActivate[MAX_RELAY] ;         // Min activation time 
  int     iMode ;                       // operation mode
  int     iBoostMode ;                       // operation mode
  float   fAnalogMult ;                 // calibration factors for adc channel  
  float   fAnalogAdd ;
  float   fTankOverTempAlarm ;
  float   fTankUnder1TempAlarm ;
  float   fTankUnder2TempAlarm ;
  float   fRoofOverTempAlarm ;
  float   fRoofUnderTempAlarm ;
  bool    bEmails[MAX_EMAIL_ALARMS] ; 
  bool    bAlarm[MAX_EMAIL_ALARMS] ; 
  bool    bBoostTimes[48] ;           // half hour time slots for off peak power timer
  byte    Boost_wdays   ;             // days of wek plus an enable SOP
} Solar_Heater_App_stuff_t ;          // computer says it's 136 not 130 ??? is my maths crap ????

Solar_Heater_App_stuff_t shas ;

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  int  port;
  char server[48] ;
  char user[48] ;
  char password[48] ;
  char FROM[48] ;
  char TO[48] ;
  char CC[48] ;
  char BCC[48] ;
  bool bSecure ;
  char message[64] ;
  char subject[64] ;
} Email_App_stuff_t ;          // computer says it's 136 not 130 ??? is my maths crap ????

Email_App_stuff_t SMTP;

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  struct ts tc;            //
  float ha ;
  float sunX ;
  float sunrise ;
  float sunset ;
  float tst ;
  float solar_az_deg;      //
  float solar_el_deg;      //
  int   iDayNight ;          //
  float decl ;
  float eqtime ;
} Solar_App_stuff_t ;          // computer says it's 136 not 130 ??? is my maths crap ????

Solar_App_stuff_t SolarApp  ;



typedef struct __attribute__((__packed__)) {     // memory stuff
uint16_t wSendEmail ;
float fTemp[MAX_TEMP_SENSOR];
bool  bRelayState[MAX_RELAY] ;
int   TTG[MAX_RELAY] ;
bool  bAlarm[MAX_EMAIL_ALARMS] ;
bool  bPrevAlarm[MAX_EMAIL_ALARMS] ;
} Solar_Heater_App_memory_stuff_t ;         

Solar_Heater_App_memory_stuff_t shams ;

#define MYVER 0x12435678     // change this if you change the structures that hold data that way it will force a "backinthebox" to get safe and sane values from eeprom
#define BUFF_MAX 64

char buff[BUFF_MAX]; 
const byte LED = BUILTIN_LED ;  // = 16  4 ? D4
const byte MAX_WIFI_TRIES = 60 ;
const int MAX_EEPROM = 2000 ;
const int PROG_BASE = 256 ;   // where the program specific information starts in eeprom (above GHKS)


IPAddress MyIP ;
IPAddress MyIPC  ;
const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];   // buffer to hold incoming and outgoing packets

SSD1306 display(0x3c, 5, 4);   // GPIO 5 = D1, GPIO 4 = D2   - onboard display 0.96" 
//SH1106Wire display(0x3c, 4, 5);   // arse about ??? GPIO 5 = D1, GPIO 4 = D2  -- external ones 1.3"

WiFiUDP ntpudp;

ESP8266WebServer server(80) ;
ESP8266WebServer OTAWebServer(81) ;
ESP8266HTTPUpdateServer OTAWebUpdater ;
time_t chiptime ; 

byte rtc_sec = 0 ;
byte rtc_min = 0 ;
byte rtc_hour = 0  ;
uint8_t rtc_status ;
float rtc_temp ;
long lScanCtr = 0 ;
long lScanLast = 0 ;
bool bConfig = false ;
bool hasRTC = false ;

int bSaveReq = 0 ;
int iUploadPos = 0 ;
bool bDoTimeUpdate = false ;
long  MyCheckSum ;
long  MyTestSum ;
long lTimePrev ;
long lTimePrev2 ;
long lMinUpTime = 0 ;

long lRebootCode = 0 ;
struct ts tc;  
bool bPrevConnectionStatus = false;
unsigned long lTimeNext = 0 ;           // next network retry


char Toleo[10] = {"Ver 1.1\0"}  ;
char cssid[32] = {"Solar_Water_XXXXXXXX\0"} ;
char *host = "Solar_Water_00000000\0";                // overwrite these later with correct chip ID

void setup() {
int i , k , j = 0; 

  for ( i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ) {
    shams.bAlarm[i] = false ;
    shas.bAlarm[i] = false ;
  }
  
  lRebootCode = random(1,+2147483640) ;  // want to change it straight away
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println("");          // new line after the startup burst
 
  pinMode(BUILTIN_LED,OUTPUT);  //  D4 builtin LED
  EEPROM.begin(MAX_EEPROM);
  LoadParamsFromEEPROM(true);  

  display.init();
  if (( ghks.lDisplayOptions & 0x01 ) != 0 ) {  // if bit one on then flip the display
    display.flipScreenVertically();
  }

  /* show start screen */
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);  
  display.setFont(ArialMT_Plain_16);
  display.drawString(63, 0, "Solar Hot");
  display.drawString(63, 16, "Water SCADA");
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);  
  display.drawString(0, 40, "Copyright (c) 2020");
  display.drawString(0, 50, "Dougal Plummer");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);  
  display.drawString(127, 50, String(Toleo));
  display.display();

  display.setFont(ArialMT_Plain_10);

  if ( MYVER != ghks.lVersion ) {
//  if ( false ) {
    BackInTheBoxMemory();         // load defaults if blank memory detected but dont save user can still restore from eeprom
    Serial.println("Loading memory defaults...");
    delay(2000);
  }

  WiFi.disconnect();
  Serial.println("Configuring soft access point...");
  WiFi.mode(WIFI_AP_STA);  // we are having our cake and eating it eee har
  sprintf(cssid,"Solar_Water_%08X\0",ESP.getChipId());
  if ( cssid[0] == 0 || cssid[1] == 0 ){   // pick a default setup ssid if none
    sprintf(ghks.cpassword,"\0");
  }
  MyIPC = IPAddress (192, 168, 5 +(ESP.getChipId() & 0x7f ) , 1);
  WiFi.softAPConfig(MyIPC,MyIPC,IPAddress (255, 255, 255 , 0));  
  Serial.println("Starting access point...");
  Serial.print("SSID: ");
  Serial.println(cssid);
  Serial.print("Password: >");
  Serial.print(ghks.cpassword);
  Serial.println("< " + String(ghks.cpassword[0]));
  if (( ghks.cpassword[0] == 0 ) || ( ghks.cpassword[0] == 0xff)){
    WiFi.softAP((char*)cssid);                   // no passowrd
  }else{
    WiFi.softAP((char*)cssid,(char*) ghks.cpassword);
  }
  MyIPC = WiFi.softAPIP();  // get back the address to verify what happened
  Serial.print("Soft AP IP address: ");
  snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);      
  Serial.println(buff);
  
  bConfig = false ;   // are we in factory configuratin mode
  display.display();
  if ( ghks.lNetworkOptions != 0 ) {
    WiFi.config(ghks.IPStatic,ghks.IPGateway,ghks.IPMask,ghks.IPDNS ); 
  }
  if ( ghks.npassword[0] == 0 ){
    WiFi.begin((char*)ghks.nssid);                    // connect to unencrypted access point      
  }else{
    WiFi.begin((char*)ghks.nssid, (char*)ghks.npassword);  // connect to access point with encryption
  }
  while (( WiFi.status() != WL_CONNECTED ) && ( j < MAX_WIFI_TRIES )) {
    j = j + 1 ;
    delay(500);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "Chip ID " + String(ESP.getChipId(), HEX) );
    display.drawString(0, 9, String("SSID:") );
    display.drawString(0, 18, String("Password:") );
    display.drawString(0, 36 , String(1.0*j/2) + String(" (s)" ));   
    display.drawString(42, 36 , String(ghks.NodeName));   
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128 , 0, String(WiFi.RSSI()));
    display.drawString(128, 9, String(ghks.nssid) );
    display.drawString(128, 18, String(ghks.npassword) );
    display.drawString(j*4, 27 , String(">") );
    snprintf(buff, BUFF_MAX, ">>  IP %03u.%03u.%03u.%03u <<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);            
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(63 , 54 ,  String(buff) );
    display.display();     
    digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
  } 
  if ( j >= MAX_WIFI_TRIES ) {
     bConfig = true ;
     WiFi.disconnect();
/*     IPAddress localIp(192, 168, 5 +(ESP.getChipId() & 0x7f ) , 1);
     IPAddress MaskIp(255, 255, 255 , 0);
     WiFi.softAPConfig(localIp,localIp,MaskIp);
     sprintf(ssid,"Configure_%08X\0",ESP.getChipId());
     WiFi.softAP(ssid); // configure mode no password
     MyIP = WiFi.softAPIP();
     Serial.print("Soft AP IP address: ");
     Serial.println(MyIP);
     display.drawString(0, 22, "Soft AP IP address: "+String(MyIP) );
     display.display();*/
  }else{
     Serial.println("");
     Serial.println("WiFi connected");  
     Serial.print("IP address: ");
     MyIP =  WiFi.localIP() ;
//     Serial.println(MyIP) ;
     snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);            
     Serial.println(buff);
     display.drawString(0 , 53 ,  String(buff) );
//     display.drawString(0, 53, "IP "+String(MyIP) );
     display.display();
  }
  if (ghks.localPortCtrl == ghks.localPort ){             // bump the NTP port up if they ar the same
    ghks.localPort++ ;
  }
//    Serial.println("Starting UDP");
    ntpudp.begin(ghks.localPort);                      // this is the recieve on NTP port
    display.drawString(0, 44, "NTP UDP " );
    display.display();
    Serial.print("NTP Local UDP port: ");
    Serial.println(ntpudp.localPort());
                                                // end of the normal setup
 
  sprintf(host,"Solar_Water_%08X\0",ESP.getChipId());
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
//    Serial.println("MDNS responder started");
//    Serial.print("You can now connect to http://");
//    Serial.print(host);
//    Serial.println(".local");
  }


  Serial.println("HTTP server starting...");
  server.on("/", handleRoot);
  server.on("/setup", handleRoot);
  server.on("/settings", handleRoot);
  server.on("/scan", i2cScan);
  server.on("/stime", handleRoot);
  server.on("/info", handleInfo);
  server.on("/eeprom", DisplayEEPROM);
  server.on("/backup", HTTP_GET , handleBackup);
  server.on("/backup.txt", HTTP_GET , handleBackup);
  server.on("/backup.txt", HTTP_POST,  handleRoot, handleFileUpload);
  server.onNotFound(handleNotFound);  
  server.begin();
  Serial.println("HTTP server UP");

  tc.mon = 0 ;
  tc.wday = 0 ;
  DS3231_init(DS3231_INTCN); // look for a rtc
  DS3231_get(&tc);
  rtc_status = DS3231_get_sreg();
  if (((tc.mon < 1 )|| (tc.mon > 12 ))&& (tc.wday>8)){  // no rtc to load off
    Serial.println("What NO DS3231 RTC ?");
  }else{
    setTime((int)tc.hour,(int)tc.min,(int)tc.sec,(int)tc.mday,(int)tc.mon,(int)tc.year ) ; // set the internal RTC
    hasRTC = true ;
    Serial.println("Look like it has DS3231 RTC ?");
    rtc_temp = DS3231_get_treg(); 
    rtc_hour = hour();  // dont need to updte the time if we have an RTC onboard --- assume its working ok
  }
 
  rtc_min = minute();
  rtc_sec = second();
  OTAWebUpdater.setup(&OTAWebServer);
  OTAWebServer.begin();  

  randomSeed(now());                       // now we prolly have a good time setting use this to roll the dice for reboot code
  lRebootCode = random(1,+2147483640) ;

  sensors.begin(); // Start up the Dallas library

  Serial.print("Parasite power is: ");     // report parasite power requirements
  if (sensors.isParasitePowerMode()) 
    Serial.println("ON");
  else 
    Serial.println("OFF");

  for (i = 0 ; i < 3 ; i++){
    if (sensors.getAddress(Thermometer[i], i)) {
        printAddress(Thermometer[i]);
        sensors.setResolution(Thermometer[i], TEMPERATURE_PRECISION);
        Serial.print(" Device "+String(i)+" Resolution: ");
        Serial.print(sensors.getResolution(Thermometer[i]), DEC);
        Serial.print(" ");
        sensors.requestTemperatures();
        printTemperature(Thermometer[i]);
        Serial.println();
    }
    else
      Serial.println("Unable to find address for Device "+String(i));
  }  
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" Temperature devices found.");

  for ( i = 0 ; i < MAX_RELAY ; i++ ){
    if (( shas.relayPort[i] < 17 ) && ( shas.relayPort[i] >= 0 )){  // check if its rubbish bfore doing it
      pinMode(shas.relayPort[i],OUTPUT);  // relay ouputs initalise
    }
  }
  
}

void loop() {
long lTime ;  
long lRet ;
int i , j , k  ;
int x , y ;
bool bSendCtrlPacket ;
bool bDirty = false ;
bool bDirty2 = false ;
String csTmp ;
long lTD ; 
  
  server.handleClient();
  OTAWebServer.handleClient();
  lScanCtr++ ;
  if (second() != rtc_sec) {                                // do onlyonce a second
//    if ( bComsIn ){
      digitalWrite(LED,!digitalRead(LED));
//      bComsIn= false ;
//    }

    rtc_sec = second();
    lScanLast = lScanCtr ;
    lScanCtr = 0 ;

    for ( i = 0 ; i < 4 ; i++ ){
      shams.fTemp[i] = sensors.getTempC(Thermometer[shas.sensor[i]]) ;
    }
    shams.fTemp[4] = ( analogRead(A0) * shas.fAnalogMult / 4096 ) + shas.fAnalogAdd ;

    display.clear();
    //      display.drawLine(minRow, 63, maxRow, 63);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    display.drawString(0 , 0, String(buff) );
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(127 , 0, String(WiFi.RSSI()));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    switch (rtc_sec & 0x03) {
      case 1:
        snprintf(buff, BUFF_MAX, "IP %03u.%03u.%03u.%03u", MyIP[0], MyIP[1], MyIP[2], MyIP[3]);
        break;
      case 2:
        snprintf(buff, BUFF_MAX, ">>  IP %03u.%03u.%03u.%03u <<", MyIPC[0], MyIPC[1], MyIPC[2], MyIPC[3]);
        break;
      default:
        snprintf(buff, BUFF_MAX, "%s", cssid );
        break;
    }
    display.drawString(64 , 53 ,  String(buff) );

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 10 , String("Roof   " ) + String(shams.fTemp[4],1) + " (C)") ;
    display.drawString(0, 20 , String("Top    " ) + String(shams.fTemp[0],1) + " (C)") ;
    display.drawString(0, 30 , String("Bottom " ) + String(shams.fTemp[1],1) + " (C)") ;
//    display.drawString(0, 40 , String("Air    " ) + String(shams.fTemp[2],1) + " (C)") ;
//    display.drawString(0, 50 , String("Spare  " ) + String(shams.fTemp[3],1) + " (C)") ;

    csTmp = "" ;
    for ( i = 0 ; i < MAX_RELAY ; i++ ){
      if (shams.bRelayState[MAX_RELAY - 1 - i] ==  shas.ActiveValue[MAX_RELAY - 1 - i]) {
        csTmp += String(MAX_RELAY - 1 - i) ;
      }else{
        csTmp += "_" ;        
      }
    }
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 20 , csTmp ) ;
    csTmp = "" ;
    for (i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ){
      if ( shams.bAlarm[MAX_EMAIL_ALARMS - 1 - i] ){
        csTmp += String(MAX_EMAIL_ALARMS - 1 - i) ;
      }else{
        csTmp += "_" ;        
      }
    }
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 10 , csTmp ) ;

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));    
    display.drawString(63 , 43, String(buff));
    display.display();

    for ( i = 0 ; i < MAX_RELAY ; i++ ){
    }
    switch(shas.iMode){        // selected operational mode for pump  work out that has to be on
      case 0:                  // temp differental control
        if (( shams.fTemp[4] - shams.fTemp[1]) < shas.fSolarTempDiffMin ){ // turn off the pump relay  (collector - tank bottom)
          shams.bRelayState[0] = !shas.ActiveValue[0] ;
        }
        if (( shams.fTemp[4] - shams.fTemp[1]) > shas.fSolarTempDiffMax ) { // turn on the pump relay   (collector - tank bottom)
          shams.bRelayState[0] = shas.ActiveValue[0] ;
        }
        if (( shams.fTemp[1] < shas.fCollectorMinTemp  ) || ( shams.fTemp[1] > shas.fCollectorMaxTemp  )){  // set to run for five minutes if over or under temp
          shams.TTG[i] = 5 ;
        }
      break;
      case 1:                  // timer solar disc at 10 Deg (45 ish minutes after sunrise and before sunset)
          if (SolarApp.solar_el_deg >= 10 ) {          // day
            if ( year() > 2019 ){                      // rtc look remotely valid
              shams.bRelayState[0] = shas.ActiveValue[0] ;
            }
          } else {                                    // night
            shams.bRelayState[0] = !shas.ActiveValue[0] ;
          }         
      break;
      case 2:                  // ?
      break;
    }
    
    if ( shams.fTemp[0] < shas.fTopBoostTemp ){
      shams.bRelayState[1] = shas.ActiveValue[1] ;
    }
    if ( shams.fTemp[0] > ( shas.fTopBoostTemp + shas.fTopBoostDiffTemp )){
      shams.bRelayState[1] = !shas.ActiveValue[1] ;      
    }
    if ( shams.fTemp[1] < shas.fBottomBoostTemp ){
      shams.bRelayState[2] = shas.ActiveValue[2] ;
    }
    if ( shams.fTemp[1] > ( shas.fBottomBoostTemp + shas.fBottomBoostDiffTemp )){
      shams.bRelayState[2] = !shas.ActiveValue[2] ;      
    }
    shams.bRelayState[3] = shams.bRelayState[0] ;  // may as well as mirror the first relay without the frost protection
    
    for ( i = 0 ; i < MAX_RELAY ; i++ ){
      if (shams.TTG[i] > 0 ) {   // if yo have done an activate then switch it on for test
        shams.bRelayState[i] = shas.ActiveValue[i] ;
      }
      if (( shas.relayPort[i] >= 0 ) && ( shas.relayPort[i] < 17 )){    // write active status to ports if they look remotely correct
        digitalWrite( shas.relayPort[i] , shams.bRelayState[i] );
      }
    }

    if (( shams.fTemp[0] < shas.fTankUnder1TempAlarm ) && !( shams.fTemp[0] < shas.fTankUnder2TempAlarm )) {
      shams.bAlarm[0] = true ;                     // tank luke warm
    }
    if ( shams.fTemp[0] < shas.fTankUnder1TempAlarm ){
      shams.bAlarm[1] = true ;                     // tank cold temp
    }
    if ( shams.fTemp[0] > shas.fTankOverTempAlarm ){
      shams.bAlarm[2] = true ;                     // tank over temp
    }
    if ( shams.fTemp[4] > shas.fCollectorMaxTemp ){ // collector over temp
      shams.bAlarm[3] = true ;
    }
    if ( shams.fTemp[4] < shas.fCollectorMinTemp ){ // collector under temp
      shams.bAlarm[4] = true ;      
    }
    if ( shams.bRelayState[1] == shas.ActiveValue[1] ){                            // boost energised 1
      shams.bAlarm[5] = true ;      
    }
    if ( shams.bRelayState[1] == shas.ActiveValue[2] ){                            // boost energised 2
      shams.bAlarm[6] = true ;      
    }
    
  }
  
  if ( rtc_min != minute()) {
    GetTempLogs(); // grab the data if there is any
    lMinUpTime++ ;
     if ((bDoTimeUpdate)) {  // not the correct time try to fix every minute
      sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server
      bDoTimeUpdate = false ;
    }
    if ( hasRTC ) {
      rtc_temp = DS3231_get_treg();
    }
    DoSolarCalcs();
    rtc_min = minute() ;
    for ( i = 0 ; i < MAX_RELAY ; i++ ){
      if (shams.TTG[i] > 0 ) {
        shams.TTG[i]-- ;
        if (shams.TTG[i] == 0 ){
          shams.bRelayState[i] = !shas.ActiveValue[i] ;  // set to inactive then timer expires
        }
      }
    }
  }
  
  if ( rtc_hour != hour()) {   // once a day do a time update or once an hour if not set yet
    if ((hour() == 6) || ( year() < 2020 )) {
      if (WiFi.isConnected())  {
        bDoTimeUpdate = true ;
      }  
    }
    if ( rtc_hour == 14 ){
      for (i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ){                  // do the single shot on alarm messages
        shams.bAlarm[i] = false ;
        shams.bPrevAlarm[i] = false ;
      }
    }
    rtc_hour = hour() ;
  }
  
  
  lRet = ntpudp.parsePacket() ; // this is actually server not a client ie we are doing time
  if ( lRet != 0 ) {
    processNTPpacket();
  }
  

  for (i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ){                  // do the single shot on alarm messages
    if ( shams.bAlarm[i] && !shams.bPrevAlarm[i] && shas.bEmails[i] ){
      shams.wSendEmail |=  ( 0x01 << i ) ;
    }
    shams.bPrevAlarm[i] = shams.bAlarm[i] ; 
  }

  if (( shams.wSendEmail != 0 )&& WiFi.isConnected())  {
    for (i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ){
      if ((( 0x01 << i ) & shams.wSendEmail ) != 0 ){
        SendEmailToClient(i);
        shams.wSendEmail = shams.wSendEmail &  (( 0x01 << i ) ^ 0xffff ) ;
      }
    }
  }

  if (!WiFi.isConnected())  {
    lTD = (long)lTimeNext-(long) millis() ;
    if (( abs(lTD)>40000)||(bPrevConnectionStatus)){ // trying to get roll over protection and a 30 second retry
      lTimeNext = millis() - 1 ;
/*      Serial.print(millis());
      Serial.print(" ");
      Serial.print(lTimeNext);
      Serial.print(" ");
      Serial.println(abs(lTD));*/
    }
    bPrevConnectionStatus = false;
    if ( lTimeNext < millis() ){
      Serial.println(String(buff )+ " Trying to reconnect WiFi ");
      WiFi.disconnect(false);
//      Serial.println("Connecting to WiFi...");
      WiFi.mode(WIFI_AP_STA);
      if ( ghks.lNetworkOptions != 0 ) {            // use ixed IP
        WiFi.config(ghks.IPStatic, ghks.IPGateway, ghks.IPMask, ghks.IPDNS );
      }
      if ( ghks.npassword[0] == 0 ) {
        WiFi.begin((char*)ghks.nssid);                    // connect to unencrypted access point
      } else {
        WiFi.begin((char*)ghks.nssid, (char*)ghks.npassword);  // connect to access point with encryption
      }
      lTimeNext = millis() + 30000 ;
    }
  }else{
    if ( !bPrevConnectionStatus  ){
      MyIP = WiFi.localIP() ;
      bPrevConnectionStatus = true ;
    }
  }


}
