/*  ESP32 Dual Axis Solar Tracker - Circa 2023
 *   
 *  Written/Assembled By Dougal Plummer B.E.
 *  With help and contributions from many people - my thanks to them all...
 *  
 *  Feel free to use this code to help save our planet any way you can - GO SOLAR !
 *  Newer copies can be found at https://github.com/Dougal121/Solar/tree/master/ESP32_Tracker
 *      
 *  Compiles for LOLIN D32 @ 80Mhz
 *  Minimal SPIFFS (Large APPS with OTA)
 *  
 */
#include <esp_task_wdt.h>    // part of the ESP stuff from board manager 
#include <WiFi.h>            // arduino inbuilt 
#include <WiFiClient.h>      // arduino inbuilt 
#include <WebServer.h>       // arduino inbuilt 
#include <ESPmDNS.h>         // part of the ESP stuff from board manager
#include <WiFiUDP.h>         // arduino inbuilt 

//#include <DNSServer.h>
#include <TimeLib.h>         // arduino standard book of spells   
#include <Wire.h>            // arduino inbuilt 
//#include <SPI.h>           // arduino inbuilt
#include <EEPROM.h>          // arduino inbuilt 
#include <stdio.h>           // arduino inbuilt 
#include <LSM303.h>              
#include <L3G.h>
#include <SFE_BMP180.h>      // Sparkfun github
#include <TinyGPS.h>            
#include "ht16k33.h"         // changed the default constructor (changed copy it in github under libs for this project)
#include <Update.h>          // arduino inbuilt 
#include <ESP_Mail_Client.h>
#include "uEEPROMLib.h"      // SD card lib for the RTC memory logging

#include "SSD1306.h"         // these 3 come from the standard book of spells (arduinoo IDE lib manager)
//#include "SH1106.h"
#include "SH1106Wire.h"
#include "ds3231.h"          // this one is in the GitHub where you found this code - It has changed since I started back in 2016

#define BUFF_MAX 32

#include "StaticPages.h"            // part of this project (tab top right) 

#define MAX_LOCAL_IO 16             // same a sa relay board
#define ESP32_BUILTIN_LED 2

#define MIN_REBOOT  720             //   720   12 hours normally      10 min for testing
const byte MAX_DESCRIPTION = 8 ;

#if defined(ESP32)
#define MaxPinPort  40
#define MinPinPort   0
#define ADC_MAX_CHAN  6 
#elif defined(ESP8266)
#define MaxPinPort  18
#define MinPinPort   0
#define ADC_MAX_CHAN  1 
#endif

#define ADC_MAX_ALARM ADC_MAX_CHAN * 4
#define MINBUSSCANINTERVAL 5  // minimum bus scan time in minutes
#define MINLORASCANINTERVAL 5  // minimum LoRa scan time in minutes

SMTPSession smtp;
ESP_Mail_Session session;  // Declare the global used ESP_Mail_Session for user defined session credentials


//const byte SETPMODE_PIN = D0 ; 
//const byte FLASH_BTN = D3 ;    // GPIO 0 = FLASH BUTTON 
//const byte SCOPE_PIN = D3 ;
//const byte FACTORY_RESET = D0 ;
const byte LED = 2 ;                                                                // emprical number of the board i was using (found by shear dumb luck)
const byte MAX_WIFI_TRIES = 45 ;

SSD1306 display(0x3c, 5, 4);   // GPIO 5 = D1, GPIO 4 = D2                           onboard 0.96" LOED display as per picture
//SH1106Wire display(0x3c, 4, 5);   // arse about ??? GPIO 5 = D1, GPIO 4 = D2       external 1.3" OLED display for a D1 R2 etc

uEEPROMLib rtceeprom(0x57);

#define BUFF_MAX 32
#define PARK_EAST 1
#define PARK_WEST 2
#define PARK_NORTH 3
#define PARK_SOUTH 4
#define PARK_FLAT 5

#define ANG_ABS_MIN_NS -10.0            // this is both mounts
#define ANG_ABS_MAX_NS 90.0
#define ANG_ABS_MIN_EW -80.0            // Equitorial 
#define ANG_ABS_MAX_EW 80.0
#define ANG_ABS_MIN_AZ 0.0              // Alt / Az
#define ANG_ABS_MAX_AZ 360.0

#define ANG_ABS_MAX_HYS_EW 20.0
#define ANG_ABS_MIN_HYS_EW 0.1
#define ANG_ABS_MAX_HYS_NS 20.0
#define ANG_ABS_MIN_HYS_NS 0.1
#define ANG_ABS_MAX_OFS_EW 180.0
#define ANG_ABS_MIN_OFS_EW -180.0
#define ANG_ABS_MAX_OFS_NS 20.0
#define ANG_ABS_MIN_OFS_NS -20.0

#define MINYEAR 2018

#define MOTOR_DWELL 100
#define MAX_MOTOR_PWM 1000

#define HT16K33_DSP_NOBLINK 0   // constants for the half arsed cheapo display
#define HT16K33_DSP_BLINK1HZ 4
#define HT16K33_DSP_BLINK2HZ 2
#define HT16K33_DSP_BLINK05HZ 6


//const byte RELAY_XZ_PWM = D5; // PWM 1    Speed North / South     Was the X- S relay  Orange 
//const byte RELAY_YZ_PWM = D6; // PWM 2    Speed East / West       Was the Y+ W relay  Blue
//const byte RELAY_YZ_DIR = D7; // DIR 2    Y+ Y- East / West       Was the Y- E relay  Yellow
//const byte RELAY_XZ_DIR = D8; // DIR 1    X+ X-  North / South    Was the X+ N relay  Brown

const byte  RELAY_XZ_PWM = 1 ;
const byte  RELAY_YZ_PWM = 2 ;

#define MINUTESPERDAY 1440

static bool hasSD = false;
static bool hasNet = false;
static bool hasGyro = false;
static bool hasRTC = false;
static bool hasPres = false ;
const int MAX_EEPROM = 4096 ;
const int TV_BASE = 450 ;

char dayarray[8] = {'S','M','T','W','T','F','S','E'} ;
char Toleo[10] = {"Ver 3.8\0"}  ;
#define LVER 3

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  unsigned int localPort = 2390;          // 2 local port to listen for NTP UDP packets
  unsigned int localPortCtrl = 8666;      // 4 local port to listen for Control UDP packets
  unsigned int RemotePortCtrl = 8664;     // 6 local port to listen for Control UDP packets
  IPAddress MyIP ;
  IPAddress MyIPC ;
  long lProgMethod ;                      // 
  long lVersion ;                         // 
  long lMaxDisplayValve ;                 // 
  long lNodeAddress ;                     //  
  float fTimeZone ;                       //  
  IPAddress RCIP ;                        // 
  char NodeName[24] ;                     //  
  char nssid[32] ;                        //   
  char npassword[24] ;                    // 
  char cssid[32] ;                        //   
  char cpassword[24] ;                    // 
  long lDisplayOptions  ;                 //  
  uint8_t lNetworkOptions  ;              //  
  uint8_t cpufreq ;                       //    240 160 80   not flash at 26
  uint8_t displaytimer ;                  //     how log does the wifi stay on for (minutes) ?  0 fo display on always (display piggbacks on this)
  uint8_t magsens ;                       //     magnet sensor sesitivity use instead of the button
  char timeServer[40] ;                   //    = {"au.pool.ntp.org\0"}
  IPAddress IPStatic ;                    // (192,168,0,123)   
  IPAddress IPGateway ;                   // (192,168,0,1)    
  IPAddress IPMask ;                      // (255,255,255,0)   
  IPAddress IPDNS ;                       // (192,168,0,15)   
  long SelfReBoot ;
  long lRebootTimeDay ;
  long MinRecycleTime ;
  long RebootInterval ;                   // intervale in minutes between reboots  
/*  float ADC_Cal_Mul ;
  float ADC_Cal_Ofs ;
  char  ADC_Unit[5] ;                     // units for display
  uint8_t  ADC_Alarm_Mode ;               // high low etc   0x80 Contious enable 0x40 Master Valve Only Enable  0x20  Alram 2 master  0x10 Alarm 1 master     0x02 Alarm 1 high   0x01 Alarm 2 high
  float ADC_Alarm1 ;
  float ADC_Alarm2 ;                      // 
  uint16_t  ADC_Alarm_Delay ;             // trigger to alarm in seconds
  uint8_t ADC_Input_PIN1 ;
  uint8_t ADC_Input_PIN2 ;  */
  long    spare[20];                      // padding to use up next time  
} general_housekeeping_stuff_t ;          // 

general_housekeeping_stuff_t ghks ;

char cssid[32] = {"Configure_XXXXXXXX\0"} ;

char buff[BUFF_MAX]; 

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

byte rtc_sec ;
byte rtc_min ;
byte rtc_hour ;
byte rtc_fert_hour ;
float rtc_temp ;

L3G gyro;
LSM303 compass;
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};
SFE_BMP180 pressure;
TinyGPS gps;
HT16K33 HT;

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  int iUseGPS ;
  int motor_recycle_x  ;
  int motor_recycle_y  ;
  char trackername[24] ;
  float heading ;          // 
  struct ts tb;            // 
  struct ts tn;            // 
  struct ts td;            //   
  struct ts tg;            // 
  struct ts tc;            //
  float ha ;
  float sunX ;
  float sunrise ;
  float sunset ;
  int iNightShutdown ;     //
  int iMultiDrive   ;      //   do the axis drives run together 
  time_t setchiptime ;     //   if set to non zero this will trigger a time set event
  float zAng ;             // 
  float xMul  ;            // 
  float yMul  ;            // 
  float zMul  ;            // 
  int iXYS  ;              // 
  int iSave ;              //     
  int iDoSave ;            // 
  int iGPSLock  ;          //  
  unsigned long  fixage ;  // 
  float xRoll  ;           // 
  float yRoll  ;           // 
  float zRoll  ;           // 
  float gT ;               //  temp from sensor
  float Pr ;               //  presure sensor
  float alt ;              //  altitude from GPS
  float T;                 //  temperature of board (if has RTC)
  float xzTarget ;         //  target for angles
  float yzTarget ;         // 
  float xzH ;              //  hyserisis zone
  float yzH ;              // 
  float xzAng;             //  current angles
  float yzAng;             // 
  float xzOffset;          //  offset xz
  float yzOffset;          //  offset yz
  float dyPark;            //  parking position
  float dxPark;            //  
  float xMinVal ;          //  Min and Max values  X - N/S
  float xMaxVal ;          //  
  float yMinVal ;          //   Y  -- E/W
  float yMaxVal ;          //  
  float latitude;          // 
  float longitude;         // 
  int iDayNight ;          // 
  float solar_az_deg;      // 
  float solar_el_deg;      // 
  int iTrackMode ;         //  
  int iMode ;              // 
  int iMaxWindSpeed ;      //  max speeed  - set to zero to disable
  int iMountType ;         //  0 - for Equitorial Mount  1 -  for Alt / Az Mount
  int iMaxWindTime ; 
  int iMinWindTime ; 
  float dyParkWind ;       //  parking position
  float dxParkWind ;       //  
  int xMaxMotorSpeed ;    // maximum motor speed
  int yMaxMotorSpeed ;    // maximum motor speed
  LSM303::vector<int16_t> mag_min ;
  LSM303::vector<int16_t> mag_max ;
  int iOutputType ;         //  
  int iTimeSource ;         //  
  int iWindInputSource ;         //  
  float fWindSpeedCal  ;         //  
  float fWindSpeedVel  ;         //  
  uint8_t  RELAY_XZ_PWM ;       // Physical Output that its connected to
  uint8_t  RELAY_YZ_PWM ;
  uint8_t  RELAY_XZ_DIR ;       
  uint8_t  RELAY_YZ_DIR ;
  int iMinWindSpeed ;         //  
  char  Wind_Unit[6] ;                     // units for display
  long  Spare[23] ;                       // padding for next time  
} tracker_stuff_t ;        // 

tracker_stuff_t   tv   ;    // tracker variables

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
  bool bUseEmail ;
  float LowTankQty ;
  bool bSpare ;
  int  iBusScanInterval ;
  int  iBusState[8] ; // 16 x 8 bits of bus state
  int  iLoRaScanInterval ;
  int  iLoRaTimeOut ;       // seconds > 120  
} Email_App_stuff_t ;          

Email_App_stuff_t SMTP;

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  char  ADC_Description[MAX_DESCRIPTION] ;   //
  float ADC_Cal_Mul ;                            // 
  float ADC_Cal_Ofs ;                            //
  char  ADC_Unit[6] ;                            // units for display
  uint8_t ADC_Input_PIN ;
  int ADC_RAW ;
  float ADC_Value ;
} adc_chan_t ;                                   // 

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  uint8_t  ADC_Channel ;
  uint8_t  ADC_Mode ;      // high low etc  
  float    ADC_Value ;     //
  uint16_t ADC_Delay ;     // trigger to alarm in seconds
  uint8_t  ADC_Action ;    // high low etc  
  uint8_t  ADC_Do ;        // email, park ... etc    
  uint16_t ADC_Trigger ;
  bool     ADC_bSentADCAlarmEmail ;
} adc_alarm_t ;                                   // 

typedef struct __attribute__((__packed__)) {     // eeprom stuff
  adc_chan_t chan[ADC_MAX_CHAN];
  adc_alarm_t alarm[ADC_MAX_ALARM] ;
} adc_stuff_t ;          // 

adc_stuff_t adcs ;

#define MAX_LOG 288          // 5 min for 24 hrs
#define LOG_PER_HOUR 12

typedef struct __attribute__((__packed__)) {            
  time_t  RecTime ;           //  4   
  float Temp ;                //  8  
  float Pres ;                // 12
  float RSSI ;                // 16
  float EWAngle ;             // 20
  float NSAngle ;             // 24
  float EWTarget ;            // 28
  float NSTarget ;            // 32
  float ADCValue[ADC_MAX_CHAN] ;  // 4 * 6 = 24 
} data_log_t ;                // 14 * 4 bytes  = 56 bytes per log        24 hrs x 12 logs/hr (288)  x 32 bytes = 16128 bytes
data_log_t  DataLog[MAX_LOG] ;


bool bNTPFirst = false ; 
int iPMode;
int iPWM_YZ ;
int iPWM_XZ ;
int iPowerUp = 0 ;
int bSaveReq = 0 ;
unsigned long  gpschars ; 

float xMag = 0 ;           // 
float yMag = 0 ;           // 
float zMag = 0 ;           // 

//long lTimeZone ;
bool  bDataLogDirty = false ; 
long lScanCtr = 0 ;
long lScanLast = 0 ;
time_t AutoOff_t ;         // auto off until time > this date
bool bConfig = false ;
uint8_t rtc_status ;
float decl ;
float eqtime ;
bool iOutputActive = 0 ;
bool bPower = true ;
bool bWindPark = false ;
long iPowerDown = 60 ;
bool bMagCal = false ;
bool bDoTimeUpdate = false ;
long lTimePrev ;
long lTimePrev2 ;
long lRebootCode = 0 ;
uint64_t chipid; 
int iUploadPos = 0 ;
long  MyCheckSum ;
long  MyTestSum ;
unsigned long lTimeNext = 0 ;     // next network retry
bool bPrevConnectionStatus = false;
bool bSendTestEmail = false ;
bool bSendSaveConfirm = false ;
bool bValveActive = false ;
float Temp_Value ; 
long lMinUpTime = 0 ;
long lRet_Email = 0 ;
/*float ADC_Value = 0 ;
int   ADC_Raw = 0 ; 
int   ADC_Raw1 = 0 ; 
int   ADC_Raw2 = 0 ;
bool  bSentADCAlarmEmail = false ; 
long ADC_Trigger = 0 ;  */
int   iAutoResetStatus = 0 ; 
time_t NowTime ;
String strBusResults ;
long lMinBusScan = 30 ;
bool bBusGood = true ;
int magval = 0 ;
int iBusReturn = 0 ;
long iDisplayCountDown = 0 ;
WiFiUDP ntpudp;
//WiFiUDP ctrludp;

WebServer server(80);

//DNSServer dnsServer;


//  ##############################  SETUP   #############################
void setup() {
int i , j = 0; 
String host ;

  chipid=ESP.getEfuseMac();   //The chip ID is essentially its MAC address(length: 6 bytes).
  sprintf(cssid,"Configure_%08X\0",chipid);

  bNTPFirst = true ;
  bSaveReq = 0 ;
  Serial.begin(115200);           // sert same as upload so less issues with serial monitor being open
  Serial.setDebugOutput(true);    // we will switch this off later in the setup
  Serial.println(".");
  
  pinMode(LED,OUTPUT);                  //  builtin LED
//  pinMode(SETPMODE_PIN,INPUT_PULLUP);   // flashy falshy

  EEPROM.begin(MAX_EEPROM);
  LoadParamsFromEEPROM(true);

  Serial.println(ghks.cssid[0]);
  if (( ghks.cssid[0] == 0x00 ) || ( ghks.cssid[0] == 0xff ) || ( ghks.lVersion != LVER )){   // pick a default setup ssid if none
    Serial.println("Blank Memory - Resetting Memory to Defaults");
    BackInTheBoxMemory();           // blank memory detector - cssid should be a roach motel... once set should stay that way
  }

  tv.motor_recycle_x = 0 ;         // these are all in eeprom but thet prolly shouldnt be
  tv.motor_recycle_y = 0 ; 
  tv.iSave = 0 ;              
  tv.iDoSave = 0 ;        
  tv.iGPSLock = 0  ;       
  tv.fixage = 0 ; 

  display.init();
   if (( ghks.lDisplayOptions & 0x01 ) == 0 ) {  // if bit one on then flip the display
    display.flipScreenVertically();
  } 

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);  
  display.setFont(ArialMT_Plain_16);
  display.drawString(63, 0, "ESP Solar");
  display.drawString(63, 16, "Tracker");
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);  
  display.drawString(0, 40, "Copyright (c) 2020");
  display.drawString(0, 50, "Dougal Plummer");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);  
  display.drawString(127, 50, String(Toleo));
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
  pinMode(tv.RELAY_XZ_DIR, OUTPUT); // Outputs for PWM motor control or relays
  pinMode(tv.RELAY_XZ_PWM, OUTPUT); // 
  pinMode(tv.RELAY_YZ_PWM, OUTPUT); //
  pinMode(tv.RELAY_YZ_DIR, OUTPUT); // 
  iPWM_YZ = 0 ;
  iPWM_XZ = 0 ;
  if (tv.iOutputType<2) {                 // need to setup 2 PWM channels
      ledcSetup(RELAY_XZ_PWM, 5000, 10);             // 5 kHz PWM, 10-bit resolution
      ledcSetup(RELAY_YZ_PWM, 5000, 10);
      ledcAttachPin(tv.RELAY_XZ_PWM, RELAY_XZ_PWM);  // assign PWM pins to channels
      ledcAttachPin(tv.RELAY_YZ_PWM, RELAY_YZ_PWM);  //     
  }
  ActivateOutput(0); // call an all stop first

  if (( tv.mag_min.x >= 0 ) || ( tv.mag_max.x <= 0 )){
    compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
    compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};    
  }else{
    compass.m_min = tv.mag_min ;
    compass.m_max = tv.mag_max ;
  }

//  compass.m_min = (LSM303::vector<int16_t>) {-3848, -1822, -1551 };   // calibration figures are empirical
//  compass.m_max = (LSM303::vector<int16_t>) { +3353, +5127, +5300};

  
  delay(1000);
  if (tv.iUseGPS==1){
    Serial.begin(9600);    // gps on serial port
  }else{
    Serial.begin(115200);
    Serial.println("Warp Speed no GPS !");
  }


  display.clear();
  display.setFont(ArialMT_Plain_10);

  StartWiFi();

  if (ghks.localPortCtrl == ghks.localPort ){             // bump the NTP port up if they ar the same
    ghks.localPort++ ;
  }
//    Serial.println("Starting UDP");
    ntpudp.begin(ghks.localPort);                      // this is the recieve on NTP port
    display.drawString(0, 44, "NTP UDP " );
    display.display();
    Serial.print("NTP Local UDP port: ");
    Serial.println(ghks.localPort);
//    ctrludp.begin(ghks.localPortCtrl);                 // recieve on the control port
//    display.drawString(64, 44, "CTRL UDP " );
//    display.display();
//    Serial.print("Control Local UDP port: ");
//    Serial.println(ctrludp.localPort());
                                                      // end of the normal setup
 
/*  sprintf(host,"Control_%08X\0",ESP.getChipId());

  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }
*/

  server.on("/", handleRoot);                        // setup web interface server
  server.on("/setup", handleSetup);
  server.on("/scan", i2cScan);
  server.on("/eeprom", DisplayEEPROM);  
  server.on("/info", handleInfo);  
  server.on("/stime", handleTime);
  server.on("/sensor",handleSensor);
  server.on("/adc",adcLocalMap);
  server.on("/log",datalog1_page);
  server.on("/log.csv",datalog1_page);
  server.on("/chart",chart1_page);
  server.on("/rtcmem",DisplayRTCEEPROM);
  server.on("/email",DisplayEmailSetup);
  server.on("/backup", HTTP_GET , handleBackup);
  server.on("/backup.txt", HTTP_GET , handleBackup);
  server.on("/backup.txt", HTTP_POST,  handleRoot, handleFileUpload); 
  server.on("/login", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
    Serial.printf("Display Login Page");
  });
  server.on("/update", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", updatePage);
  });
  server.on("/update", HTTP_POST, []() {   //handling uploading firmware file
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {   // flashing firmware to ESP
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  
  server.onNotFound(handleNotFound);  
  server.begin();
  Serial.println("HTTP server started");

  tv.tg.year = 1970 ;   //  absolute zero of time ;)
  tv.tg.mon = 1 ;
  tv.tg.mday = 1;
  tv.tg.hour = 0;
  tv.tg.min = 0;
  tv.tg.sec = 0;
      
  tv.tc.mon = 0 ;
  tv.tc.wday = 0 ;
  DS3231_init(DS3231_INTCN); // look for a rtc
  DS3231_get(&tv.tc);
  rtc_status = DS3231_get_sreg();
  if (((tv.tc.mon < 1 ) || (tv.tc.mon > 12 )) && ((tv.tc.wday > 8)||(tv.tc.wday < 1))) { // no rtc to load off
    Serial.println("NO RTC ?");
  }else{
    setTime((int)tv.tc.hour,(int)tv.tc.min,(int)tv.tc.sec,(int)tv.tc.mday,(int)tv.tc.mon,(int)tv.tc.year ) ; // set the internal RTC
    hasRTC = true ;
    Serial.println("Has RTC ?");
    rtc_temp = DS3231_get_treg(); 
    DS3231_get(&tv.tb);
  }
  rtc_min = minute();
  rtc_sec = second();

  HT.begin(0x00);                             // start the meatball
  for (int led = 0; led < 127; led++) {       // and clear it  
    HT.clearLed(led);                         
  } 
  HT.sendLed();                               // update display 
//  Serial.println("OTA startup");  
//  OTAWebUpdater.setup(&OTAWebServer);
//  OTAWebServer.begin();  
  Serial.println("End of Setup");
  delay(100);

  lRebootCode = random(1,+2147483640) ;
  tv.fWindSpeedVel = 0 ;
  iPowerDown = ghks.displaytimer ;     // makle sure power is on during the day
}


//  ##############################  LOOP   #############################
void loop() {
long lTime ;  
long lRet ;
int i , j , k  ;
int iSunUp , iSunDn ;
float P;
float sunInc;
float sunAng;
float xzRatio;
float yzRatio;
float dTmp ;
float tst ;
float flat, flon;
unsigned short goodsent;
unsigned short failcs;
String msg ;
int iRelayActiveState ;
bool bSendCtrlPacket = false ;
long lTD ;
bool bTrigger = false ;
bool bTriggerLess = false ;
bool bTriggerMore = false ;
int iMailMsg = 0 ;
int iHM = 0 ;
int iRebootTime = 0 ;
int iDOW = 0  ; 
  server.handleClient();
//  OTAWebServer.handleClient();
  
  lTime = millis() ;

  compass.read();  // this reads all 6 channels
  tv.heading = compass.heading();    //(LSM303::vector<int>) { 1, 0, 0 }

  if (( compass.a.z != 0) && (!compass.timeoutOccurred() ))  {
    tv.zAng = (float)compass.a.z * tv.zMul ;
    if (tv.iXYS == 0 ){                                            // "Proper Job" make it configurable 
      xzRatio = (float)(compass.a.x * tv.xMul) / sqrt(sq(compass.a.z * tv.zMul)+sq(compass.a.y * tv.yMul)) ;         // Normal   NS
      yzRatio = (float)(compass.a.y * tv.yMul) / sqrt(sq(compass.a.z * tv.zMul)+sq(compass.a.x * tv.xMul)) ;         //          EW
    }else{
      xzRatio = (float)(compass.a.y * tv.xMul) / sqrt(sq(compass.a.z * tv.zMul)+sq(compass.a.x * tv.yMul)) ;         // Swapped  NS
      yzRatio = (float)(compass.a.x * tv.yMul) / sqrt(sq(compass.a.z * tv.zMul)+sq(compass.a.y * tv.xMul)) ;         //          EW  
    }
    tv.xzAng = ((float)atan(xzRatio) / PI * 180 ) + tv.xzOffset ;       // NS or Alt  Good old offsets or fudge factors added
    if (tv.iMountType == 0){
      tv.yzAng = ((float)atan(yzRatio) / PI * 180 ) + tv.yzOffset ;     // EW    
    }else{
      tv.yzAng = tv.heading + tv.yzOffset ;                     // Az   
      if ( tv.yzAng > 360 ){
        tv.yzAng = tv.yzAng - 360 ;    
      }
      if ( tv.yzAng < 0 ){
        tv.yzAng = tv.yzAng + 360 ;    
      }
      tv.yzAng = AzFix(tv.yzAng) ; 
    }
    xMag = compass.m.x ;
    yMag = compass.m.y ;
    zMag = compass.m.z ;
    if ( bMagCal ){                                             // this is part of the magnetometer calibration routine
      running_min.x = _min(running_min.x, compass.m.x);
      running_min.y = _min(running_min.y, compass.m.y);
      running_min.z = _min(running_min.z, compass.m.z);
          
      running_max.x = _max(running_max.x, compass.m.x);
      running_max.y = _max(running_max.y, compass.m.y);
      running_max.z = _max(running_max.z, compass.m.z);
    }
  }else{                                                        // try restarting the compass/accelerometer modual - cos he gone walkabout...
    Wire.begin();   // reset the I2C
    compass.init();
    compass.enableDefault();
    compass.setTimeout(1000);                                           // BTW I fixed up the int / long issue in the time out function in the LM303 lib I was using
    
  }
   
//  digitalWrite(SCOPE_PIN,!digitalRead(SCOPE_PIN));  // my scope says we are doing this loop at an unreasonable speed except when we do web stuff
//  pwmtest++ ;
//  if ( pwmtest > 1024 ) {
//    pwmtest = 0 ;
//  }
//  analogWrite(SCOPE_PIN,pwmtest); 

//  delay(1);                  // limmit to 1000 cyclces a second max
  
      
  lScanCtr++ ;
  bSendCtrlPacket = false ;
  if ( rtc_sec != second()){
    lScanLast = lScanCtr;
//    digitalWrite(LED,!digitalRead(LED));  // my scope says we are doing this loop at an unreasonable speed except when we do web stuff
    rtc_sec = second() ;

    display.clear();
//      display.drawLine(minRow, 63, maxRow, 63);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    display.drawString(0 , 0, String(buff) );
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    if ( bPower ) {
      display.drawString(128 , 0, String(WiFi.RSSI()));
    }else{
      display.drawString(128 , 0, String("-X-"));
    }
    display.setTextAlignment(TEXT_ALIGN_LEFT);
//    snprintf(buff, BUFF_MAX, "TX %d TY %d", year(), month() );
    msg = "X  " + String(tv.xzAng,2)  ;
    display.drawString(56 , 11, msg ) ;
    msg = "Y  " + String(tv.yzAng,2)  ;
    display.drawString(0 , 11, msg ) ;
    msg = "TY " + String(tv.yzTarget,2) ;
    display.drawString(0 , 22, msg ) ;
    msg = "TX " + String(tv.xzTarget,2) ;
    display.drawString(56 , 22, msg ) ;
    msg = "DY " + String((tv.yzAng-tv.yzTarget),2) ;
    display.drawString(0 , 33, msg ) ;
    msg = "DX " + String((tv.xzAng-tv.xzTarget),2) ;
    display.drawString(56 , 33, msg ) ;

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    if ( iOutputActive == 0 ) {
      msg = "" ;
    }else{
      msg = "O-" ;      
    }
    if ((tv.iOutputType & 0x02 ) == 0 ){  // both breads of PWM
      if ( iPWM_YZ != 0 ) {
        if (( digitalRead(tv.RELAY_YZ_DIR) == LOW )) {
          msg += "W" ;     // will become Az
        }else{
          msg += "E" ;            
        }
      }
      if ( iPWM_XZ != 0 ) {
        if (( digitalRead(tv.RELAY_XZ_DIR) == LOW )) {
          msg += "N" ;     // will become Alt
        }else{
          msg += "S" ;            
        }
      } 
    }else{                   // using relays
      if ((tv.iOutputType & 0x01 ) == 0 ){ //active low
        iRelayActiveState = LOW ;
      }else{   // active high
        iRelayActiveState = HIGH ;
      }      
      if (( digitalRead(tv.RELAY_YZ_DIR) == iRelayActiveState )) {
        msg += "e" ;
      }        
      if (( digitalRead(tv.RELAY_YZ_PWM) == iRelayActiveState )) {
        msg += "w" ;
      }        
      if (( digitalRead(tv.RELAY_XZ_DIR) == iRelayActiveState )) {
        msg += "n" ;
      }        
      if (( digitalRead(tv.RELAY_XZ_PWM) == iRelayActiveState )) {
        msg += "s" ;
      }        
    }
    display.drawString(128 , 11, msg ) ;
//    display.drawString(128 , 22, String(pwmtest)) ;
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(tv.sunrise), MinSolarTime(tv.sunrise));
    display.drawString(0 , 44, buff ) ;
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    if (tv.iDayNight == 1) {
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

    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(tv.sunset), MinSolarTime(tv.sunset));
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128 , 44, buff ) ;
 

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    switch ((rtc_sec >> 1 ) % 10 ){
      case 1:
        ghks.MyIP =  WiFi.localIP() ;  // update to see if has connection                  
        snprintf(buff, BUFF_MAX, "IP %03u.%03u.%03u.%03u", ghks.MyIP[0],ghks.MyIP[1],ghks.MyIP[2],ghks.MyIP[3]);      
      break;
      case 2:
        snprintf(buff, BUFF_MAX, ">>  IP %03u.%03u.%03u.%03u <<", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);            
      break;
      case 3:
//       snprintf(buff, BUFF_MAX, "%s - %d", ghks.NodeName,ghks.lNodeAddress );   
       if ( bPower ) {
         snprintf(buff, BUFF_MAX, "WiFi ON CPU Freq %d (MHz)", ESP.getCpuFreqMHz() );            
       }else{
         snprintf(buff, BUFF_MAX, "WiFi OFF CPU Freq %d (MHz)", ESP.getCpuFreqMHz() );                    
       }
      break;
      case 4:
       snprintf(buff, BUFF_MAX, "Up Time %d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
      break;
      case 5:
        switch (iAutoResetStatus){
          case -1: snprintf(buff, BUFF_MAX, "Auto Reboot %d",( ghks.lRebootTimeDay & 0xfff )); break ;
          case -2: snprintf(buff, BUFF_MAX, "Auto Reboot Today %d",( ghks.lRebootTimeDay & 0xfff )); break ;
          case -3: snprintf(buff, BUFF_MAX, "REBOOT IN ONE MINUTE"); break ;
          case 1: snprintf(buff, BUFF_MAX, "Reboot interval %d min", ghks.SelfReBoot ); break ;
          case 2: snprintf(buff, BUFF_MAX, "Auto Reboot in %d min", (ghks.SelfReBoot - lMinUpTime )); break ;
          case 3: snprintf(buff, BUFF_MAX, "REBOOT IN ONE MINUTE"); break ;
          default: snprintf(buff, BUFF_MAX, "Auto Reboot OFF") ; break;
        }
      break;
      case 6:
        if ( strBusResults.length() == 0 ){
          snprintf(buff, BUFF_MAX, "--- I2C BUS NORMAL ---" );
        }else{
          snprintf(buff, BUFF_MAX, "%s", strBusResults.c_str() );
        }
      break;  
      case 7:
       snprintf(buff, BUFF_MAX, "%s", tv.trackername );   
      break;

      case 8:
        if (tv.iWindInputSource == -1 ){
          if (bWindPark) {
             snprintf(buff, BUFF_MAX, "PARK ACTIVE NO SOURCE" );   
          }else{
             snprintf(buff, BUFF_MAX, "NO ADC INPUT WIND PARK" );                         
          }
        }else{
          if (bWindPark) {
             snprintf(buff, BUFF_MAX, "%d WINDPARK ACTIVE %3.1f",tv.iWindInputSource, tv.fWindSpeedVel );   
          }else{
             snprintf(buff, BUFF_MAX, "%d Wind Speed %3.1f",tv.iWindInputSource, tv.fWindSpeedVel );   
          }
        }
      break;
      case 9:
        if ( gps.satellites() == 255 ){
          snprintf(buff, BUFF_MAX,"--- No GPS Lock ---") ; 
        }else{
          snprintf(buff, BUFF_MAX,"GPS LOCK %d BIRDS" , gps.satellites()) ;           
        }
      break;

      default:
        snprintf(buff, BUFF_MAX, "%s", ghks.cssid );            
      break;
    }
    display.drawString(64 , 54 ,  String(buff) );

    display.display();
    if (( rtc_sec > 8 ) && ( rtc_sec < 58 )) {  // dont calculate arround the minute when time is updating from NTP or GPS as might get a not so funny result
      digitalWrite(LED,!digitalRead(LED));
      tv.tc.year = year();   // get the time into the structure
      tv.tc.mon = month() ;
      tv.tc.mday = day();
      tv.tc.hour = hour();
      tv.tc.min = minute();
      tv.tc.sec = second();
      rtc_status = DS3231_get_sreg();  // hang the expense thow the cat another canary...

      tv.solar_az_deg = SolarAzimouthRad(tv.longitude, tv.latitude, &tv.tc, ghks.fTimeZone) * 180 / PI ;
      tv.solar_el_deg = SolarElevationRad(tv.longitude, tv.latitude, &tv.tc, ghks.fTimeZone) * 180 / PI ;
    
      decl = Decl(gama(&tv.tc)) * 180 / PI ;
      eqtime = eqTime(gama(&tv.tc)) ;
      tv.ha = HourAngle (tv.longitude , &tv.tc , ghks.fTimeZone )  ;
      tv.sunrise = Sunrise(tv.longitude, tv.latitude, &tv.tc, ghks.fTimeZone) ;
      tv.sunset = Sunset(tv.longitude, tv.latitude, &tv.tc, ghks.fTimeZone);
      tst = TrueSolarTime(tv.longitude, &tv.tc, ghks.fTimeZone);
      if ( tv.latitude <= 0 ) {
        tv.sunX = abs(tv.latitude) + decl ;
      }else{
        tv.sunX = abs(tv.latitude) - decl ;
      }
      if (tv.solar_el_deg >= 0 ){            // day
        tv.iDayNight = 1 ;
        iPowerDown = ghks.displaytimer ;     // makle sure power is on during the day
      }else{                                 // night
        tv.iDayNight = 0 ;
      }
    }

    if (tv.iMountType == 0){         // EQUITORIAL MOUNT
      switch (tv.iTrackMode) {
        case 6: // both axis to wind park
          tv.yzTarget = tv.dyParkWind ;  // night park position  E/W
          tv.xzTarget = tv.dxParkWind ;  // night park position  N/S
          break ;
        case 4: // both axis to park
          tv.yzTarget = tv.dyPark ;  // night park position  E/W
          tv.xzTarget = tv.dxPark ;  // night park position  N/S
          break ;
        case 3: // both axis off no tracking
          break ;
        case 2: // xz tracking  NS
          if ( tv.iDayNight == 1 ) {
            tv.xzTarget = tv.sunX ; // need to map the coordinate system correctly
          } else {
            tv.xzTarget = tv.dxPark ;  // night park position
          }
          break;
        case 1:  // yz tracking   EW
          if (tv.iDayNight == 1) {
            tv.yzTarget = tv.ha ;
          } else {
            tv.yzTarget = tv.dyPark ;  // night park position
          }
          break;
        case -1: // set target to tracking and park both at nigh
          if (tv.iDayNight == 1) {
            tv.yzTarget = tv.ha ;
            tv.xzTarget = tv.sunX ; // need to map the coordinate system correctly
          } else {
            tv.yzTarget = tv.dyPark ;  // night park position  E/W
            tv.xzTarget = tv.dxPark ;  // night park position  N/S
          }
          break;
        default: // set target to tracking
          if (tv.iDayNight == 1) {
            tv.yzTarget = tv.ha ;
            tv.xzTarget = tv.sunX ; // need to map the coordinate system correctly
          } else {
            tv.yzTarget = tv.dyPark ;  // night park position (dont park the other - leave till morning)
          }
          break;
      }
      tv.xzTarget = constrain(tv.xzTarget,tv.xMinVal,tv.xMaxVal);   // NS   constain function... very cool - dont leave home without it !
      tv.yzTarget = constrain(tv.yzTarget,tv.yMinVal,tv.yMaxVal);   // EW 
    }else{
      switch (tv.iTrackMode) {      // ALT/AZ MOUNT
        case 6: // both axis to wind park position
          tv.yzTarget = AzFix(tv.dyParkWind) ;  // night park position  Az
          tv.xzTarget = tv.dxParkWind ;  // night park position  Alt
          break ;
        case 4: // both axis to park
          tv.yzTarget = AzFix(tv.dyPark) ;  // night park position  Az
          tv.xzTarget = tv.dxPark ;  // night park position  Alt
          break ;
        case 3: // both axis off no tracking
          break ;
        case 2: // xz tracking  Alt
          if ( tv.iDayNight == 1 ) {
            tv.xzTarget = 90 - tv.solar_el_deg ; // Alt  need to map the coordinate system correctly
          } else {
            tv.xzTarget = tv.dxPark ;       // Az   night park position
          }
        break;
        case 1:  // yz tracking   Az
          if (tv.iDayNight == 1) {
            tv.yzTarget = AzFix(tv.solar_az_deg) ; // Az
          } else {
            tv.yzTarget = AzFix(tv.dyPark) ;       // Alt  night park position
          }
          break;
        case -1: // set target to tracking and park both at nigh
          if (tv.iDayNight == 1) {
            tv.yzTarget = AzFix(tv.solar_az_deg) ; // Az
            tv.xzTarget = 90 - tv.solar_el_deg ;        // Alt  need to map the coordinate system correctly
          } else {
            tv.yzTarget = AzFix(tv.dyPark) ;       // night park position 
            tv.xzTarget = tv.dxPark ;              // 
          }
          break;
        default: // set target to tracking
          if (tv.iDayNight == 1) {
            tv.yzTarget = AzFix(tv.solar_az_deg) ;  // Az 
            tv.xzTarget = 90 - tv.solar_el_deg ;         // Alt  need to map the coordinate system correctly
          } else {
            tv.yzTarget = AzFix(tv.dyPark) ;        // night park position (dont park the other - leave till morning)
          }
          break;
      }
      if (bWindPark) {
        if (tv.iMountType == 0){         // EQUITORIAL MOUNT
          tv.xzTarget = tv.dxParkWind ;
          tv.yzTarget = tv.dyParkWind ;
        }else{
          tv.yzTarget = AzFix(tv.dyParkWind) ;  // night park position  Az
          tv.xzTarget = tv.dxParkWind ;  // night park position  Alt          
        }
      }
      
      tv.xzTarget = constrain(tv.xzTarget,tv.xMinVal,tv.xMaxVal);   // NS / Alt
      tv.yzTarget = constrain(tv.yzTarget,tv.yMinVal,tv.yMaxVal);   // EW / Az   
    }
    
    DisplayMeatBall() ;

    if ( hasGyro ){
      gyro.read();
      tv.xRoll = gyro.g.x ;
      tv.yRoll = gyro.g.y ;
      tv.zRoll = gyro.g.z ;
    }
    
    if ( tv.iDoSave == 2 ) {  // save them Active via web or 
      LoadParamsFromEEPROM(false);
      tv.iDoSave = 0 ;  // only do once
      bSendSaveConfirm = true ;
    }
    if ( tv.iDoSave == 3 ) {  // load them
      LoadParamsFromEEPROM(true);
      tv.iDoSave = 0 ;  // only do once
    }
    ProcessADC();
    ProcessAlarms();
    
  }else{  // end of theonce per second stuff
    if (WiFi.isConnected()){  // pointless if no wifi
      if ( bSendTestEmail ){
        SendEmailToClient(-1) ;
        bSendTestEmail = false ;
      }    
      if ( bSendSaveConfirm ) {
        if (SMTP.bUseEmail && SMTP.bSpare ){   // using email and confirming changes
          SendEmailToClient(-4);               // email save confimation to eeprom
        }
        bSendSaveConfirm = false ;
      }
    }        
  }

  if (rtc_hour != hour()){
    if ( tv.iTimeSource == 0 ){ // just use the RTC
      if ( hasRTC ){
        DS3231_get(&tv.tc);
        setTime((int)tv.tc.hour,(int)tv.tc.min,(int)tv.tc.sec,(int)tv.tc.mday,(int)tv.tc.mon,(int)tv.tc.year ) ; // set the internal RTC
      }       
    }else{                     // Use NTP once a day  
      if ( !bConfig ) {        // ie we have a network
        if ( hour() == 0 ){    // once a day do this at midnight
          sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server  once and hour
        }
        if (( hour() == 23 ) && ( hasRTC )){    // once a day do this before midnight
          DS3231_get(&tv.tc);
          setTime((int)tv.tc.hour,(int)tv.tc.min,(int)tv.tc.sec,(int)tv.tc.mday,(int)tv.tc.mon,(int)tv.tc.year ) ; // set the chip internal RTC from the external one
        }
      }else{
        if ( hasRTC ){
          DS3231_get(&tv.tc);
          setTime((int)tv.tc.hour,(int)tv.tc.min,(int)tv.tc.sec,(int)tv.tc.mday,(int)tv.tc.mon,(int)tv.tc.year ) ; // set the internal RTC
        }
      }     
    }
    rtc_hour = hour(); 
  }
  if ( rtc_min != minute()){
    lMinUpTime++ ;    
    if (hasPres){
      tv.Pr = getPressure((float *)&tv.gT) ;
    }
    if (hasRTC) {
      tv.T = DS3231_get_treg();
      if ((minute() == 0 ) && ( hour() == 0 )){
        WriteDataLogsToEEPROM();                   // write daily data to memory at midnight
      }
    }
    
    if (((minute() % 5) == 0 )) { // data logging
      i = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) ) ;
      DataLog[i].RecTime = now() ;
      DataLog[i].Temp = tv.gT ;               
      DataLog[i].Pres = tv.Pr ;                
      DataLog[i].RSSI = WiFi.RSSI() ;               
      DataLog[i].EWAngle = tv.yzAng ;            
      DataLog[i].NSAngle = tv.xzAng ;             
      DataLog[i].EWTarget = tv.yzTarget ;            
      DataLog[i].NSTarget = tv.xzTarget ;   
      for ( k = 0 ; k < ADC_MAX_CHAN ; k++ ) {
        DataLog[i].ADCValue[k] = adcs.chan[k].ADC_Value ;
      }    
      bDataLogDirty = true ;             
    }    
    if (( year() < MINYEAR )|| (bDoTimeUpdate)) {  // not the correct time try to fix every minute
      if ( !bConfig ) { // ie we have a network
        sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server  
        bDoTimeUpdate = false ;
      }
    }
    if ( hasRTC ){
      rtc_temp = DS3231_get_treg(); 
    }
    if ( lMinBusScan > 0 ) {
      lMinBusScan -- ;
    }
    if (lMinBusScan == 0 ) {
      if (SMTP.iBusScanInterval>0) {
        iBusReturn = i2cBusCheck();
        if (( iBusReturn != 0 ) && bBusGood ){
          if ( SMTP.bUseEmail ) {
            SendEmailToClient(666); 
          }
          bBusGood = false ;
        }
        if ( SMTP.iBusScanInterval < MINBUSSCANINTERVAL ){
          lMinBusScan = MINBUSSCANINTERVAL ;          
        }else{
          lMinBusScan = SMTP.iBusScanInterval ;          
        }
      }
    }    
    lMinUpTime++ ;

    if (( lMinUpTime == 15 ) && SMTP.bUseEmail ) {
      SendEmailToClient(-2);                         // email that reboot just occureed  
    }
    
    iAutoResetStatus = 0 ;
    if (  ghks.SelfReBoot > 0 ) {
      iAutoResetStatus = 1 ;
      if ( lMinUpTime > MIN_REBOOT ) {
        iAutoResetStatus = 2 ;
        if ( lMinUpTime == ghks.SelfReBoot  ) {
          iAutoResetStatus = 3 ;
          if ( SMTP.bUseEmail ){
            SendEmailToClient(-3);                           // email intention to reboot on minute before you pull the pin
          }
        }
        if ( lMinUpTime > ghks.SelfReBoot ) {
          IndicateReboot() ;
        }
      }
    }
    NowTime = now() + 60 ;
    i = ( hour(NowTime) * 100 ) + minute(NowTime) ;
    iHM = ( hour() * 100 ) + minute() ;
    iRebootTime = ghks.lRebootTimeDay & 0xfff ;
    iDOW = dayOfWeek(now()) ;
    if (( ghks.lRebootTimeDay & 0x80000 ) != 0 ) {   // enabled and the right day
      iAutoResetStatus = -1 ;
      if (( ghks.lRebootTimeDay & ( 0x1000 << ( iDOW - 1))) != 0 )   {
        iAutoResetStatus = -2 ;
        if ( lMinUpTime > MIN_REBOOT ) { 
          if (iRebootTime == 0 ){
            if ( iHM == 2359 ) {
              iAutoResetStatus = 3 ;
              if (SMTP.bUseEmail){
                SendEmailToClient(-3);                         // email intention to reboot on minute before you pull the pin
              }
            }         
          }else{
            if (i == iRebootTime ){
              iAutoResetStatus = -3 ;
              if (SMTP.bUseEmail){
                SendEmailToClient(-3);                         // email intention to reboot on minute before you pull the pin
              }
            }          
          }
          if (iRebootTime == iHM ){
            IndicateReboot() ;
          }
        }
      }
    }

    if ( lMinBusScan > 0 ) {
      lMinBusScan -- ;
    }
    if (lMinBusScan == 0 ) {
      if (SMTP.iBusScanInterval>0) {
        iBusReturn = i2cBusCheck();
        if (( iBusReturn != 0 ) && bBusGood ){
          if ( SMTP.bUseEmail ) {
            SendEmailToClient(666); 
          }
          bBusGood = false ;
        }
        if ( SMTP.iBusScanInterval < MINBUSSCANINTERVAL ){
          lMinBusScan = MINBUSSCANINTERVAL ;          
        }else{
          lMinBusScan = SMTP.iBusScanInterval ;          
        }
      }
    }       
    
    rtc_min = minute() ;
    gps.stats(&gpschars, &goodsent , &failcs );
    gps.f_get_position(&flat, &flon,(long unsigned *) &tv.fixage); // return in degrees
    if ((tv.fixage > 0 ) && ( tv.fixage < 40000 )) {   // wait till our fix is valid before we use the values
      tv.iGPSLock = gps.satellites() ;
      tv.alt = gps.f_altitude() ;
      if (iPowerUp==0) {   // only do this at startup so we have a better position ref for next time
          if ( tv.latitude != flat ){
            tv.latitude = flat ;
          }
          if ( tv.longitude != flon ){
            tv.longitude = flon ;
          }
          iPowerUp = 1 ;
          if (!hasNet ){
            SetTimeFromGPS();
          }
      }
    }else{
      tv.iGPSLock = 0 ;   // if no lock loook at internal clock
    }  
    if ( ghks.displaytimer > 0 ){  
      if ( iPowerDown > 0 ){
        iPowerDown-- ;
        if ( !bPower ) {
          setCpuFrequencyMhz(240) ;   
          StartWiFi() ;
          bPower = true ;
        }
      }else{
        if ( bPower ) {
          StopWiFi() ;
          display.clear() ;
          display.display() ;  // blank the display
          SetSelectedSpeed() ;
          bPower = false ;    // power off
        }
      }    
    }
  }   // end of the one minute stuff (lots of it)

  if ( year() > MINYEAR ){ // dont move if date and time is rubbish
    iSunUp = HrsSolarTime(tv.sunrise) ;
    iSunDn = HrsSolarTime(tv.sunset) ;
    if (((((hour() > _min(iSunDn ,22) ) || ( hour() < _max(iSunUp,1) )) && ( iSunUp < iSunDn ))  || (((hour() < _min(iSunUp ,22) ) && ( hour() > _max(iSunDn,1) )) && ( iSunUp > iSunDn ))) && (tv.iTrackMode < 3)) {    
      if ( tv.iNightShutdown != 0 ){
        iOutputActive = 1 ;   // maintain power for positioning but have night angles selected earler in code    
      }else{
        iOutputActive = 0 ;   // power down at night if in shutdown mode
      }
    }else{
      iOutputActive = 1 ;     // normal path for tracking   
    }
    
  }else{
    iOutputActive = 0 ;       // power down 
  }
  ActivateOutput(iOutputActive) ;  // Send the command
  
  if (second() > 4 ){
    if ( ntpudp.parsePacket() ) {
      processNTPpacket();
    }
  }

/*  lRet = ctrludp.parsePacket() ;
  if ( lRet != 0 ) {
    processCtrlUDPpacket(lRet);
  }
*/
  if (lTimePrev > ( lTime + 100000 )){ // Housekeeping --- has wrapped around so back to zero
    lTimePrev = lTime ; // skip a bit 
    Serial.println("Wrap around");
  }

  if (tv.iUseGPS == 1 ){
    while (Serial.available()){ // process the gps buffer from serial port
      gps.encode(Serial.read());
    }  
  }  



  if ( bPower ) {  // dont try this if no power or wifi
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    if ( !bPrevConnectionStatus && WiFi.isConnected() ){
  //    Serial.println(String(buff )+ " WiFi Reconnected OK...");  
      ghks.MyIP =  WiFi.localIP() ;
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
      bPrevConnectionStatus = true ;
    }    
  }else{
    bPrevConnectionStatus = false ;    
  }
//  dnsServer.processNextRequest();
}   // ####################  BOTTOM OF LOOP  ###########################################



void SetSelectedSpeed(void){
  switch(ghks.cpufreq){
    case 13:
      setCpuFrequencyMhz(13);
    break;
    case 26:
      setCpuFrequencyMhz(26);
    break;
    case 80:
      setCpuFrequencyMhz(80);
    break;
    case 160:
      setCpuFrequencyMhz(160);
    break;
    default:
      setCpuFrequencyMhz(240);
    break;    
  }
//  Serial.println("CPU commanded " +String(ghks.cpufreq) + " actual " + String(getCpuFrequencyMhz())+ " MHz");
//  Serial.println("CPU commanded " +String(ghks.cpufreq)+ " MHz");
}




