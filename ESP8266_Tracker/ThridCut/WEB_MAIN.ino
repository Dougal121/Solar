void SerialOutParams(){
String message ;
   
  message = "Web Request URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  Serial.println(message);
  message = "";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  Serial.println(message);
}


void handleRoot() {
  boolean currentLineIsBlank = true;
  tmElements_t tm;
  long  i = 0 ;
  int ii  ;
  int iProgNum = 0;
  int j ;
  int k , kk , iTmp ;
  boolean bExtraValve = false ;
  uint8_t iPage = 1 ;
  boolean bDefault = true ;
//  int td[6];
  long lTmp ; 
  String MyCheck , MyColor   ;
  byte mac[6];
  String message ;

//  SerialOutParams();

  for (uint8_t j=0; j<server.args(); j++){
    bSaveReq = 1 ;
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
//          LoadParamsFromEEPROM(true);
          tv.iDoSave = 3 ;
        break;
        case 2: // Save values
//          LoadParamsFromEEPROM(false);
          tv.iDoSave = 2 ;
        break;
        case 3: // Save the running max
          bMagCal = false ;
          compass.m_min = running_min ;
          tv.mag_min = running_min ;
          compass.m_max = running_max ;
          tv.mag_max = running_max ;    
          tv.iDoSave = 3 ;              // also que the eeprom save 
        break;
        case 4: // start stop mag cal mode
          bMagCal = !bMagCal ;
        break;
        case 5: // reset the calbiration of magnetometer
          tv.mag_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
          tv.mag_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
          compass.m_min = tv.mag_min ;
          compass.m_max = tv.mag_max ;    
          running_min = {32767, 32767, 32767} ;
          running_max = {-32768, -32768, -32768};                      
          bMagCal = true ;
        break;
        case 8: //  Cold Reboot
          ESP.reset() ;
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 667: // wipe the memory to factory default
          BackIntheBoxMemory();
        break;
        case 665:
          sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server  once and hour  
        break;
        case 668:
        break;
      }  
    }
    i = String(server.argName(j)).indexOf("reboot");
    if (i != -1){  // 
      if (( lRebootCode == String(server.arg(j)).toInt()) && ( lRebootCode > 0 )){  // stop the phone browser being a dick and retry resetting !!!!
        ESP.restart() ;        
      }
    }
        
    i = String(server.argName(j)).indexOf("tname"); // wanta do this one BEFORE we dick with captialisation of the request
    if (i != -1){  
      String(server.arg(j)).toCharArray(tv.trackername,sizeof(tv.trackername));
    }        
    i = String(server.argName(j)).indexOf("nssid");    // ssid setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(ghks.nssid,sizeof(ghks.nssid));
    }        
    i = String(server.argName(j)).indexOf("npass");   // password setup
    if (i != -1){  // have a request to set the time zone
      String(server.arg(j)).toCharArray(ghks.npassword,sizeof(ghks.npassword));
    }        
    i = String(server.argName(j)).indexOf("cssid");    // ssid setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(ghks.cssid,sizeof(ghks.cssid));
    }        
    i = String(server.argName(j)).indexOf("cpass");   // password setup
    if (i != -1){  // have a request to set the time zone
      String(server.arg(j)).toCharArray(ghks.cpassword,sizeof(ghks.cpassword));
    }        

    i = String(server.argName(j)).indexOf("naddr");   // ip address setup
    if (i != -1){  // have a request to set the time zone
      ghks.MyIP[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.MyIP[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.MyIP[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.MyIP[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }        

    i = String(server.argName(j)).indexOf("tmode");
    if (i != -1){  // have a request to set the tracking mode
      tv.iTrackMode = String(server.arg(j)).toInt() ;
      if (( tv.iTrackMode < -1) || ( tv.iTrackMode > 5 )){
        tv.iTrackMode = -1 ;
      }
    }        
    i = String(server.argName(j)).indexOf("iugps");
    if (i != -1){  // have a request to set the tracking mode
      tv.iUseGPS = String(server.arg(j)).toInt() ;
      if (( tv.iUseGPS < 0) || ( tv.iUseGPS > 2 )){
        tv.iUseGPS = 0 ;
      }
    }        
    i = String(server.argName(j)).indexOf("tzone");
    if (i != -1){  // have a request to set the time zone
      ghks.fTimeZone = String(server.arg(j)).toFloat() ;
      if (( ghks.fTimeZone < -12) || ( ghks.fTimeZone > 12 )){
        ghks.fTimeZone = 0 ;
      }
    }        
    i = String(server.argName(j)).indexOf("mylat");    //lat  
    if (i != -1){  // have a request to set the latitude
      tv.latitude = String(server.arg(j)).toFloat() ;
      if (( tv.latitude < -90) || ( tv.latitude > 90 )){
        tv.latitude = -34.051219 ;
      }
    }        
    i = String(server.argName(j)).indexOf("mylon");    // long
    if (i != -1){  // have a request to set the logitude
      tv.longitude = String(server.arg(j)).toFloat() ;
      if (( tv.longitude < -180) || ( tv.longitude > 180 )){
        tv.longitude = 142.013618 ;
      }
    }        
    i = String(server.argName(j)).indexOf("minay");
    if (i != -1){  // have a request to set minimum angle Y   E/W
      tv.yMinVal = String(server.arg(j)).toFloat() ;
      if ( tv.iMountType == 0 ) {
        tv.yMinVal = constrain(tv.yMinVal ,ANG_ABS_MIN_EW ,ANG_ABS_MAX_EW )  ; 
      }else{
        tv.yMinVal = constrain(tv.yMinVal , ANG_ABS_MIN_AZ , ANG_ABS_MAX_AZ ) ;                           
      }
    }        
    i = String(server.argName(j)).indexOf("maxay");
    if (i != -1){  // have a request to set maximum angle Y  E/W
      tv.yMaxVal = String(server.arg(j)).toFloat() ;
      if ( tv.iMountType == 0 ) {
        tv.yMaxVal = constrain(tv.yMaxVal ,ANG_ABS_MIN_EW ,ANG_ABS_MAX_EW )  ; 
      }else{
        tv.yMaxVal = constrain(tv.yMaxVal ,ANG_ABS_MIN_AZ ,ANG_ABS_MAX_AZ )  ;         
      }
    } 
    i = String(server.argName(j)).indexOf("minax");
    if (i != -1){  // have a request to set minimum angle X   N/S
      tv.xMinVal = String(server.arg(j)).toFloat() ;
      tv.xMinVal = constrain(tv.xMinVal ,ANG_ABS_MIN_NS , ANG_ABS_MAX_NS)  ; 
    }           
    i = String(server.argName(j)).indexOf("maxax");
    if (i != -1){  // have a request to set maximum angle X    N/S
      tv.xMaxVal = String(server.arg(j)).toFloat() ;
      tv.xMaxVal = constrain(tv.xMaxVal ,ANG_ABS_MIN_NS ,ANG_ABS_MAX_NS )  ; 
    }        
    i = String(server.argName(j)).indexOf("paray");
    if (i != -1){  // have a request to set park angle Y
      tv.dyPark = String(server.arg(j)).toFloat() ;
      if ( tv.iMountType == 0 ) {
        tv.dyPark = constrain(tv.dyPark ,ANG_ABS_MIN_EW ,ANG_ABS_MAX_EW ) ; 
      }else{
        tv.dyPark = constrain(tv.dyPark ,ANG_ABS_MIN_AZ, ANG_ABS_MAX_AZ ) ; 
      }
    }        
    i = String(server.argName(j)).indexOf("parax");
    if (i != -1){  // have a request to set park angle X
      tv.dxPark = String(server.arg(j)).toFloat() ;
      tv.dxPark = constrain(tv.dxPark , ANG_ABS_MIN_NS,ANG_ABS_MAX_NS)  ; 
    }        
    
    i = String(server.argName(j)).indexOf("offay");
    if (i != -1){  // have a request to set offset angle Y
      tv.yzOffset = String(server.arg(j)).toFloat() ;
      tv.yzOffset = constrain(tv.yzOffset , ANG_ABS_MIN_OFS_EW , ANG_ABS_MAX_OFS_EW ) ; 
    }        
    i = String(server.argName(j)).indexOf("offax");
    if (i != -1){  // have a request to set offset angle X
      tv.xzOffset = String(server.arg(j)).toFloat() ;
      tv.xzOffset = constrain(tv.xzOffset ,ANG_ABS_MIN_OFS_NS ,ANG_ABS_MAX_OFS_NS )  ; 
    }        
    i = String(server.argName(j)).indexOf("hysay");
    if (i != -1){  // have a request to set Hysterisis angle
      tv.yzH = String(server.arg(j)).toFloat() ;
      tv.yzH = constrain(tv.yzH , ANG_ABS_MIN_HYS_EW , ANG_ABS_MAX_HYS_EW ) ; 
    }        
    i = String(server.argName(j)).indexOf("hysax");
    if (i != -1){  // have a request to set Hysterisis angle
      tv.xzH = String(server.arg(j)).toFloat() ;
      tv.xzH = constrain(tv.xzH ,ANG_ABS_MIN_HYS_NS ,ANG_ABS_MAX_HYS_NS )  ; 
    }        

    i = String(server.argName(j)).indexOf("mmspy");
    if (i != -1){ 
      tv.yMaxMotorSpeed = String(server.arg(j)).toInt() ;
      tv.yMaxMotorSpeed = constrain(tv.yMaxMotorSpeed , 10 , MAX_MOTOR_PWM ) ;
    }        
    i = String(server.argName(j)).indexOf("mmspx");
    if (i != -1){ 
      tv.xMaxMotorSpeed = String(server.arg(j)).toInt() ;
      tv.xMaxMotorSpeed = constrain(tv.xMaxMotorSpeed ,10 ,MAX_MOTOR_PWM )  ; 
    }        

    
    i = String(server.argName(j)).indexOf("taray");
    if (i != -1){  // target array angle  EW / Az
      tv.yzTarget = String(server.arg(j)).toFloat() ;
      if ( tv.iMountType == 0 ) {
        tv.yzTarget = constrain(tv.yzTarget , ANG_ABS_MIN_EW , ANG_ABS_MAX_EW ) ; 
      }else{
        tv.yzTarget = constrain(tv.yzTarget , ANG_ABS_MIN_AZ , ANG_ABS_MAX_AZ ) ;         
      }
    }        
    i = String(server.argName(j)).indexOf("tarax");
    if (i != -1){  // target array angle NS / Al
      tv.xzTarget = String(server.arg(j)).toFloat() ;
      tv.xzTarget = constrain(tv.xzTarget,ANG_ABS_MIN_NS,ANG_ABS_MAX_NS)  ; 
    }        
  
    i = String(server.argName(j)).indexOf("mulax");
    if (i != -1){  // have a request to set Axis Multiplier
      tv.xMul = String(server.arg(j)).toFloat() ;
      if (( tv.xMul < -10) || ( tv.xMul > 10 )|| ( tv.xMul == 0 )){
        tv.xMul = 1.0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("mulay");
    if (i != -1){  // have a request to set Axis Multiplier
      tv.yMul = String(server.arg(j)).toFloat() ;
      if (( tv.yMul < -10) || ( tv.yMul > 10 ) || ( tv.yMul == 0 )){
        tv.yMul = 1.0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("mulaz");
    if (i != -1){  // have a request to set Axis Multiplier
      tv.zMul = String(server.arg(j)).toFloat() ;
      if (( tv.zMul < -10) || ( tv.zMul > 10 ) || ( tv.zMul == 0 )){
        tv.zMul = 1.0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("xyswp");
    if (i != -1){  // have a request to set the time zone
      tv.iXYS = String(server.arg(j)).toInt() ;
      if (( tv.iXYS < 0) || ( tv.iXYS > 1 )){
        tv.iXYS = 0 ;
      }
    }       
    i = String(server.argName(j)).indexOf("nisht");
    if (i != -1){  // night time shutdown
      tv.iNightShutdown = String(server.arg(j)).toInt() ;
      if (( tv.iNightShutdown < 0) || ( tv.iNightShutdown > 1 )){
        tv.iNightShutdown = 0 ;
      }
    }            
    i = String(server.argName(j)).indexOf("tmsrc");
    if (i != -1){  // time source
      tv.iTimeSource = String(server.arg(j)).toInt() ;
      if (( tv.iTimeSource < 0) || ( tv.iTimeSource > 1 )){
        tv.iTimeSource = 1 ;
      }
    }            

    i = String(server.argName(j)).indexOf("mltdr");
    if (i != -1){  // have a request to set the time zone
      tv.iMultiDrive = String(server.arg(j)).toInt() ;
      if (( tv.iMultiDrive < 0) || ( tv.iMultiDrive > 1 )){
        tv.iMultiDrive = 0 ;
      }
    }            

    i = String(server.argName(j)).indexOf("mount");
    if (i != -1){  // have a request to set the time zone
      tv.iMountType = String(server.arg(j)).toInt() ;
      tv.iMountType = constrain(tv.iMountType,0,1);
    }         
    i = String(server.argName(j)).indexOf("outpt");
    if (i != -1){  // have a request to set the time zone
      tv.iOutputType = String(server.arg(j)).toInt() ;
      tv.iOutputType = constrain(tv.iOutputType,0,3);
    }               
    i = String(server.argName(j)).indexOf("winds");
    if (i != -1){  // have a request to set the time zone
      tv.iMaxWindSpeed = String(server.arg(j)).toInt() ;
      tv.iMaxWindSpeed = constrain(tv.iMaxWindSpeed,0,32000);
    }            
    i = String(server.argName(j)).indexOf("windt");
    if (i != -1){  // have a request to set the time zone
      tv.iMaxWindTime = String(server.arg(j)).toInt() ;
      tv.iMaxWindTime = constrain(tv.iMaxWindTime,0,32000);
    }            
    i = String(server.argName(j)).indexOf("windl");
    if (i != -1){  // have a request to set the time zone
      tv.iMinWindTime = String(server.arg(j)).toInt() ;
      tv.iMinWindTime = constrain(tv.iMinWindTime,0,32000);
    }            
    i = String(server.argName(j)).indexOf("parwy");
    if (i != -1){  // have a request to set park angle Y
      tv.dyParkWind = String(server.arg(j)).toFloat() ;
      if ( tv.iMountType == 0 ) {
        tv.dyParkWind = constrain(tv.dyParkWind ,ANG_ABS_MIN_EW ,ANG_ABS_MAX_EW ) ;
      }else{
        tv.dyParkWind = constrain(tv.dyParkWind ,ANG_ABS_MIN_AZ ,ANG_ABS_MAX_AZ ) ;        
      }
    }        
    i = String(server.argName(j)).indexOf("parwx");
    if (i != -1){  // have a request to set park angle X
      tv.dxParkWind = String(server.arg(j)).toFloat() ;
      tv.dxParkWind = constrain(tv.dxParkWind ,ANG_ABS_MIN_NS ,ANG_ABS_MAX_NS )   ; 
    }        

    i = String(server.argName(j)).indexOf("stime");
    if (i != -1){  // 
      tm.Year = (String(server.arg(j)).substring(0,4).toInt()-1970) ;
      tm.Month =(String(server.arg(j)).substring(5,7).toInt()) ;
      tm.Day = (String(server.arg(j)).substring(8,10).toInt()) ;
      tm.Hour =(String(server.arg(j)).substring(11,13).toInt()) ;
      tm.Minute = (String(server.arg(j)).substring(14,16).toInt()) ;
      tm.Second = 0 ;
      setTime(makeTime(tm));    
      if ( hasRTC ){
        tv.tc.sec = second();     
        tv.tc.min = minute();     
        tv.tc.hour = hour();   
        tv.tc.wday = dayOfWeek(makeTime(tm));            
        tv.tc.mday = day();  
        tv.tc.mon = month();   
        tv.tc.year = year();       
        DS3231_set(tv.tc);                       // set the RTC as well
        rtc_status = DS3231_get_sreg();       // get the status
        DS3231_set_sreg(rtc_status & 0x7f ) ; // clear the clock fail bit when you set the time
      }
    }        
    i = String(server.argName(j)).indexOf("rtcbf");  // rtc battery failure reset flag
    if (i != -1){  
      rtc_status = DS3231_get_sreg();       // get the status
      DS3231_set_sreg(rtc_status & 0x7f ) ; // clear the clock fail bit when you set the time     
    }
    i = String(server.argName(j)).indexOf("tzone");
    if (i != -1){  // 
      ghks.fTimeZone = String(server.arg(j)).toFloat() ;
      ghks.fTimeZone = constrain(ghks.fTimeZone,-12,12);
      bDoTimeUpdate = true ; // trigger and update to fix the time
    }
    i = String(server.argName(j)).indexOf("timsv");
    if (i != -1){                                    // timesvr
     String(server.arg(j)).toCharArray( ghks.timeServer , sizeof(ghks.timeServer)) ;
    }    
    i = String(server.argName(j)).indexOf("lpntp");
    if (i != -1){  // 
      ghks.localPort = String(server.arg(j)).toInt() ;
      ghks.localPort = constrain(ghks.localPort,1,65535);
    }        
    i = String(server.argName(j)).indexOf("lpctr");
    if (i != -1){  // 
      ghks.localPortCtrl = String(server.arg(j)).toInt() ;
      ghks.localPortCtrl = constrain(ghks.localPortCtrl,1,65535);
    }        
    i = String(server.argName(j)).indexOf("rpctr");
    if (i != -1){  // 
      ghks.RemotePortCtrl = String(server.arg(j)).toInt() ;
      ghks.RemotePortCtrl = constrain(ghks.RemotePortCtrl,1,65535);
    }        
    i = String(server.argName(j)).indexOf("dontp");
    if (i != -1){  // have a request to request a time update
      bDoTimeUpdate = true ;
    }
    
  }          

  SendHTTPHeader();
  
  server.sendContent(F("<link rel='icon' href='data:,'></head><body><html><center><h2>"));
  server.sendContent(String(tv.trackername).substring(0,16)+" Solar Tracker</h2>");

  server.sendContent(F("<a href='/'>Refresh</a><br><br>")) ;   
  if ( bSaveReq != 0 ){
    server.sendContent(F("<blink>"));      
  }
  server.sendContent(F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>")) ;         
/*
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
  server.sendContent("<b>"+ String(buff)) ; 
  if ( year() < 2018 ) {
    server.sendContent(F("--- CLOCK NOT SET ---")) ;
  }
  server.sendContent(F("</b><br>")) ;*/  
  if ( bSaveReq != 0 ){
    server.sendContent(F("</blink><font color='red'><b>Changes Have been made to settings.<br>Make sure you save if you want to keep them</b><br></font>")) ;     
  }
  server.sendContent(F("<br>")) ;
/*  if ( AutoOff_t > now() )  {
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(AutoOff_t), month(AutoOff_t), day(AutoOff_t) , hour(AutoOff_t), minute(AutoOff_t), second(AutoOff_t));
    server.sendContent(F("<b><font color=red>Automation OFFLINE Untill ")) ;  
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</font></b><br>")) ; 
  }else{
    if ( year() > 2000 ) {
      server.sendContent(F("<b><font color=green>Automation ONLINE</font></b><br>")) ;  
    }
  }
*/

  if (String(server.uri()).indexOf("stime")>0) {  // setup of the node
    bDefault = false ;
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(), month(), day() , hour(), minute());
    server.sendContent("<br><br><form method=get action=" + server.uri() + "><br>Set Current Time: <input type='text' name='stime' value='"+ String(buff) + "' size=12>");
    server.sendContent(F("<input type='submit' value='SET'><br><br></form>"));
  }

  if (String(server.uri()).indexOf("sensor")>0) {  // setup of the node
    bDefault = false ;
    server.sendContent(F("<a href='/sensor'>Update</a><br><br>")) ;         
    server.sendContent(F("<br><b>Sensor Calibration System</b><table border=1 title='Magnetometer Sensor Calibration'><tr><th rowspan=2> Parameter</th><th colspan=3>Value</th></tr>")) ; 
    server.sendContent(F("<tr><th>X (NS/Alt)</th><th>Y (EW/Az)</th><th>Z</th><th>.</th></tr>")) ; 

    server.sendContent(F("<tr><td>LSM303 Device Type</td><td colspan=3 align=center>")) ;
    i = compass.getDeviceType() ;
    server.sendContent(String(i)) ; 
    switch(i){
      case 0:
        server.sendContent(String(" - DLH")) ; 
      break;
      case 1:
        server.sendContent(String(" - DLM")) ; 
      break;
      case 2:
        server.sendContent(String(" - DLHC")) ; 
      break;
      case 3:
        server.sendContent(String(" - D")) ; 
      break;
      default:
        server.sendContent(String(" - AUTO")) ; 
      break;
    }
    server.sendContent(F("</td><td align=center></td>.</tr>"));
    
    
    server.sendContent(F("<tr><td>Acceleration RAW</td><td align=center>")) ;
    server.sendContent(String(compass.a.x)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.a.y)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.a.z)) ; 
    server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

    server.sendContent(F("<tr><td>Acceleration SI</td><td align=center>")) ;  //   +/- 2g  16Bit 2s complemet  -32767 - 0 - 32767 
    server.sendContent(String(compass.a.x/16383.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.a.y/16383.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.a.z/16383.0,3)) ; 
    server.sendContent(F("</td><td align=center>(g)</td></tr>")) ;

    server.sendContent(F("<tr><td colspan=5>.</td></tr>")) ;

    server.sendContent(F("<tr><td>Magnetometer RAW</td><td align=center>")) ;
    server.sendContent(String(xMag)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(yMag)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(zMag)) ; 
    server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

    server.sendContent(F("<tr><td>Magnetometer SI</td><td align=center>")) ;
    server.sendContent(String(xMag/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(yMag/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(zMag/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>(Gauss)</td></tr>")) ;
    
    server.sendContent(F("<tr><td>Running Min Value</td><td align=center>")) ;
    server.sendContent(String(running_min.x/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(running_min.y/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(running_min.z/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>(Gauss)</td></tr>")) ;

    server.sendContent(F("<tr><td>Running Max Value</td><td align=center>")) ;
    server.sendContent(String(running_max.x/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(running_max.y/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(running_max.z/32767.0*4.0,3)) ; 
    server.sendContent(F("</td><td align=center>(Gauss)</td></tr>")) ;

    server.sendContent(F("<tr><td>Current Cal Min</td><td align=center>")) ;
    server.sendContent(String(compass.m_min.x)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.m_min.y)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.m_min.z)) ; 
    server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

    server.sendContent(F("<tr><td>Current Cal Max</td><td align=center>")) ;
    server.sendContent(String(compass.m_max.x)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.m_max.y)) ; 
    server.sendContent(F("</td><td align=center>")) ;
    server.sendContent(String(compass.m_max.z)) ; 
    server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

    if (bMagCal){
      MyColor =  F("STOP") ;
    }else{
      MyColor =  F("START") ;
    }
    server.sendContent("<tr><td>.</td><td><form method=get action=" + server.uri() + "><input type='hidden' name='command' value='5'><input type='submit' value='RESET'></form></td>") ;
    server.sendContent("<td><form method=get action=" + server.uri() + "><input type='hidden' name='command' value='4'><input type='submit' value='"+MyColor+"'></form></td>") ;
    server.sendContent("<td><form method=get action=" + server.uri() + "><input type='hidden' name='command' value='3'><input type='submit' value='STORE'></form></td></tr>") ;

    server.sendContent(F("<tr><td>Heading (Az)</td><td colspan=3 align=center>")) ;
    server.sendContent(String(tv.heading,1)) ;     
    server.sendContent(F("</td><td>(Deg)</td></tr>")) ;

    server.sendContent(F("</table><br>")) ;
    
  }
  
  if (String(server.uri()).indexOf("setup")>0) {  // setup of the node
    bDefault = false ;
    server.sendContent("<table border=1 title='Node Settings'>");
    server.sendContent(F("<tr><th>Parameter</th><th>Value</th><th></th></tr>"));
  
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(AutoOff_t), month(AutoOff_t), day(AutoOff_t) , hour(AutoOff_t), minute(AutoOff_t));
    if (AutoOff_t > now()){
      MyColor =  F("bgcolor=red") ;
    }else{
      MyColor =  "" ;
    }
    server.sendContent(F("<tr><td>Time Zone</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='tzone' value='" + String(ghks.fTimeZone) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>");
  
    server.sendContent(F("<tr><td>Local UDP Port NTP</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='lpntp' value='" + String(ghks.localPort) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>");
  
    server.sendContent(F("<tr><td>Local UDP Port Control</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='lpctr' value='" + String(ghks.localPortCtrl) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>");
  
    server.sendContent(F("<tr><td>Remote UDP Port Control</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='rpctr' value='" + String(ghks.RemotePortCtrl) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>");
    
    server.sendContent(F("<tr><td>Network SSID</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='nssid' value='" + String(ghks.nssid) + "' maxlength="+String(sizeof(ghks.nssid))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>");
    server.sendContent(F("<tr><td>Network Password</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='npass' value='" + String(ghks.npassword) + "' maxlength="+String(sizeof(ghks.npassword))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>");

    server.sendContent(F("<tr><td>Config SSID</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='cssid' value='" + String(ghks.cssid) + "' maxlength="+String(sizeof(ghks.cssid))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>");
    server.sendContent(F("<tr><td>Config Password</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='cpass' value='" + String(ghks.cpassword) + "' maxlength="+String(sizeof(ghks.cpassword))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>");

    server.sendContent(F("<tr><td>Time Server</td><td align=center>")) ; 
    server.sendContent("<form method=get action=" + server.uri() + "><input type='text' name='timsv' value='" + String(ghks.timeServer) + "' maxlength=23 size=16></td><td><input type='submit' value='SET'></form></td></tr>");

    server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String(ESP.getChipId(), HEX) + "</td><td align=center>"+String(ESP.getChipId())+"</td></tr>" ) ; 
    WiFi.macAddress(mac);      
    snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    server.sendContent("<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>" ) ; 
    server.sendContent("<tr><td>WiFi RSSI</td><td align=center>" + String(WiFi.RSSI()) + "</td><td>(dBm)</td></tr>" ) ; 
    ghks.MyIP =  WiFi.localIP() ;    
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.MyIP[0],ghks.MyIP[1],ghks.MyIP[2],ghks.MyIP[3]);
    server.sendContent("<tr><td>Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
    server.sendContent("<tr><td>Last Scan Speed</td><td align=center>" + String(lScanCtr) + "</td><td>(per second)</td></tr>" ) ;    
    if( hasRTC ){
      rtc_status = DS3231_get_sreg();
      if (( rtc_status & 0x80 ) != 0 ){
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='red'>DEPLETED</td>"));
        server.sendContent("<td><form method=get action=" + server.uri() + "><input type='hidden' name='rtcbf' value='1'><input type='submit' value='RESET'></form></td>");
        server.sendContent(F("</tr>")) ;            
      }else{
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='green'>-- OK --</td><td></td></tr>"));
      }
      server.sendContent("<tr><td>RTC Temperature</td><td align=center>"+String(rtc_temp,1)+"</td><td>(C)</td></tr>") ;                    
    }
    server.sendContent(F("</form></table>"));
  }
  
  
  if (bDefault) {     // default valve control and setup
    server.sendContent(F("<b>Clocks</b><table border=1 title='Clocks'><tr><th>Clock Source</th><th>Time</th></tr>"));
    if ( hasRTC ){
      DS3231_get(&tv.td);
      server.sendContent(F("<tr><td>Dallas RTC</td><td align=right>")) ; 
      snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.td.year, tv.td.mon, tv.td.mday , tv.td.hour, tv.td.min, tv.td.sec);
      server.sendContent(String(buff)) ;
    }
    server.sendContent(F("</td></tr><tr><td>Last NTP</td><td align=right>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tn.year, tv.tn.mon, tv.tn.mday , tv.tn.hour, tv.tn.min, tv.tn.sec);
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr><tr><td>Last Reboot (ish)</td><td align=right>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tb.year, tv.tb.mon, tv.tb.mday , tv.tb.hour, tv.tb.min, tv.tb.sec);
    server.sendContent(String(buff)) ; 
    if ( tv.iUseGPS != 0 ){
      server.sendContent(F("</td></tr><tr><td>Last GPS</td><td align=right>"));
      snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tg.year, tv.tg.mon, tv.tg.mday , tv.tg.hour, tv.tg.min, tv.tg.sec);
      server.sendContent(String(buff)) ;
    } 
    server.sendContent(F("</td></tr><tr><td>Last Calculations</td><td align=right>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tc.year, tv.tc.mon, tv.tc.mday , tv.tc.hour, tv.tc.min, tv.tc.sec);
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr><tr><td><b>Arduino Time</b></td><td align=right><b>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr></table>"));

    MyCheck = "" ;
    if ( iPWM_YZ != 0 ) {
      if (( digitalRead(RELAY_YZ_DIR) == LOW )) {
        MyCheck = "W" ;
      }else{
        MyCheck = "E" ;            
      }
    }
    if ( iPWM_XZ != 0 ) {
      if (( digitalRead(RELAY_XZ_DIR) == LOW )) {
        MyCheck += "N" ;
      }else{
        MyCheck += "S" ;            
      }
    } 
  
    server.sendContent(F("<br><b>Tracker Control System</b><table border=1 title='Tracker Control'><tr><th> Parameter</th><th>Value</th><th><b>"));
    server.sendContent( MyCheck + "</th></tr>") ; 

    server.sendContent(F("<form method=get action=/><tr><td>Tracking Mode</td><td align=center><select name='tmode'>")) ; 
    for (i = -1 ; i < 6 ; i++ ){
      server.sendContent(F("<option value='")); 
      server.sendContent(String(i)) ;
      if ( tv.iTrackMode == i ){
         server.sendContent(F("' SELECTED>"));
      }else{
         server.sendContent(F("'>"));                    
      }
      server.sendContent(String(i) + " ") ;
      switch (i){
        case -1:
           server.sendContent(F("Track Both Park Both"));          
        break;
        case 0:
           server.sendContent(F("Track Both Park Only E/W"));          
        break;
        case 1:
           server.sendContent(F("Track and Park E/W Only"));          
        break;
        case 2:
           server.sendContent(F("Track and Park N/S Only"));          
        break;
        case 3:
           server.sendContent(F("Dont Track Dont Park"));          
        break;
        case 4:
           server.sendContent(F("Dont Track Park Both"));          
        break;
        case 5:
           server.sendContent(F("ALL STOP - Motors Off"));          
        break;
      }
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>")) ; 
    
    server.sendContent(F("<form method=get action=/><tr><td>Time Zone</td><td align=center><input type='text' name='tzone' value='")) ; 
    server.sendContent(String(ghks.fTimeZone)) ;
    server.sendContent(F("' size=6 maxlength=2></td><td><input type='submit' value='SET'></td></tr></form>")) ; 
    
    server.sendContent(F("<form method=get action=/><tr><td>Latitude +N -S</td><td align=center><input type='text' name='mylat' value='")) ; 
    server.sendContent(String(tv.latitude,8));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Longitude</td><td align=center><input type='text' name='mylon' value='")) ; 
    server.sendContent(String(tv.longitude,8));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>X (N/S) Axis Multiplier</td><td align=center><input type='text' name='mulax' value='")) ; 
    server.sendContent(String(tv.xMul,2));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Y (E/W) Axis Multiplier</td><td align=center><input type='text' name='mulay' value='")) ; 
    server.sendContent(String(tv.yMul,2)) ;
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Z (Vert) Axis Multiplier</td><td align=center><input type='text' name='mulaz' value='")) ; 
    server.sendContent(String(tv.zMul,2));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>X<->Y Axis Swap</td><td align=center>")) ; 
    server.sendContent(F("<select name='xyswp'>")) ;
    if (tv.iXYS == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 Normal")); 
      server.sendContent(F("<option value='1'>1 Swapped")); 
    }else{
      server.sendContent(F("<option value='0'>0 Normal")); 
      server.sendContent(F("<option value='1' SELECTED>1 Swapped")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Night Shutdown</td><td align=center>")) ; 
    server.sendContent(F("<select name='nisht'>"));
    if (tv.iNightShutdown == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 Shutdown at Night")); 
      server.sendContent(F("<option value='1'>1 Always Active")); 
    }else{
      server.sendContent(F("<option value='0'>0 Shutdown at Night")); 
      server.sendContent(F("<option value='1' SELECTED>1 Always Active")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Multi Drive</td><td align=center>")) ; 
    server.sendContent(F("<select name='mltdr'>"));
    if (tv.iMultiDrive == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 One Axis at a Time")); 
      server.sendContent(F("<option value='1'>1 Both axis active at once")); 
    }else{
      server.sendContent(F("<option value='0'>0 One Axis at a Time")); 
      server.sendContent(F("<option value='1' SELECTED>1 Both axis active at once")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));


    server.sendContent(F("<form method=get action=/><tr><td>Motor Drive Electrics</td><td align=center>")) ; 
    server.sendContent(F("<select name='outpt'>"));
    switch (tv.iOutputType){
      case 0:     
        server.sendContent(F("<option value='0' SELECTED>0 STANDARD PWM / DIR ")); 
        server.sendContent(F("<option value='1'>1 L298 IN1 IN2")); 
        server.sendContent(F("<option value='2'>2 RELAY FWD / REV Active Low")); 
        server.sendContent(F("<option value='3'>3 RELAY FWD / REV Active High")); 
      break;
      case 1:     
        server.sendContent(F("<option value='0'>0 STANDARD PWM / DIR ")); 
        server.sendContent(F("<option value='1' SELECTED>1 L298 IN1 IN2")); 
        server.sendContent(F("<option value='2'>2 RELAY FWD / REV Active Low")); 
        server.sendContent(F("<option value='3'>3 RELAY FWD / REV Active High")); 
      break;
      case 2:     
        server.sendContent(F("<option value='0'>0 STANDARD PWM / DIR ")); 
        server.sendContent(F("<option value='1'>1 L298 IN1 IN2")); 
        server.sendContent(F("<option value='2' SELECTED>2 RELAY FWD / REV Active Low")); 
        server.sendContent(F("<option value='3'>3 RELAY FWD / REV Active High")); 
      break;
      case 3:     
        server.sendContent(F("<option value='0'>0 STANDARD PWM / DIR ")); 
        server.sendContent(F("<option value='1'>1 L298 IN1 IN2")); 
        server.sendContent(F("<option value='2'>2 RELAY FWD / REV Active Low")); 
        server.sendContent(F("<option value='3' SELECTED>3 RELAY FWD / REV Active High")); 
      break;
    }  
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Tracker Name</td><td align=center><input type='text' name='tname' value='"));
    server.sendContent(String(tv.trackername)) ; 
    server.sendContent(F("' size=16 maxlength=16><input type='hidden' name='dummy' value='0'></td><td><input type='submit' value='SET'></td></tr></form>"));


    server.sendContent(F("<form method=get action=/><tr><td>Hardware GPS</td><td align=center>")) ; 
    server.sendContent(F("<select name='iugps'>"));
    switch (tv.iUseGPS){
      case 1:
        server.sendContent(F("<option value='0'>0 No GPS")); 
        server.sendContent(F("<option value='1' SELECTED>1 Use GPS")); 
        server.sendContent(F("<option value='2'>2 Use Networked GPS")); 
      break;
      case 2:
        server.sendContent(F("<option value='0'>0 No GPS")); 
        server.sendContent(F("<option value='1'>1 Use GPS")); 
        server.sendContent(F("<option value='2' SELECTED>2 Use Networked GPS")); 
      break;
      default:  // 0 as well
        server.sendContent(F("<option value='0' SELECTED>0 No GPS")); 
        server.sendContent(F("<option value='1'>1 Use GPS")); 
        server.sendContent(F("<option value='2'>2 Use Networked GPS")); 
      break;
    }  
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Mount Type</td><td align=center>")) ; 
    server.sendContent(F("<select name='mount'>"));
    if (tv.iMountType == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 Equatorial")); 
      server.sendContent(F("<option value='1'>1 Alt/Az")); 
    }else{
      server.sendContent(F("<option value='0'>0 Equatorial")); 
      server.sendContent(F("<option value='1' SELECTED>1 Alt/Az")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));


    server.sendContent(F("<form method=get action=/><tr><td>Auto Time Updates</td><td align=center>")) ; 
    server.sendContent(F("<select name='tmsrc'>"));
    if (tv.iTimeSource == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 Update From RTC every Hour")); 
      server.sendContent(F("<option value='1'>1 Update from NTP Every 24 Hrs")); 
    }else{
      server.sendContent(F("<option value='0'>0 Update From RTC every Hour")); 
      server.sendContent(F("<option value='1' SELECTED>1 Update from NTP Every 24 Hrs")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));


    server.sendContent(F("<form method=get action=/><tr><td>Max Wind Speed</td><td align=center><input type='text' name='winds' value='")) ; 
    server.sendContent(String(tv.iMaxWindSpeed));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Time above speed to park</td><td align=center><input type='text' name='windt' value='")) ; 
    server.sendContent(String(tv.iMaxWindTime));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 
    server.sendContent(F("<form method=get action=/><tr><td>Time to resume tracking</td><td align=center><input type='text' name='windl' value='")) ; 
    server.sendContent(String(tv.iMinWindTime));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 
/*
    server.sendContent(F("<tr><td>WiFi RSSI</td><td align=center>"));
    server.sendContent(String(WiFi.RSSI())) ; 
    server.sendContent(F("</td><td>(dBm)</td></tr>"));
*/    

    server.sendContent(F("<tr><td>Solar Declination</td><td align=center>"));
    server.sendContent(String(decl,3)) ; 
    server.sendContent(F("</td><td>(Deg)</td></tr>"));

    server.sendContent(F("<tr><td>Equation of Time</td><td align=center>"));
    server.sendContent(String(eqtime,3)) ; 
    server.sendContent(F("</td><td>(min)</td></tr>"));

    server.sendContent(F("<tr><td>Sunrise - State - Sunset</td><td align=center>"));
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(tv.sunrise), MinSolarTime(tv.sunrise));
    server.sendContent(String(buff)) ; 
    if ( tv.iDayNight == 1 ){
      server.sendContent(F(" - DAY - "));
    }else{
      server.sendContent(F(" - NIGHT - "));          
    }
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(tv.sunset), MinSolarTime(tv.sunset));        
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td><td>(hh:mm)</td></tr>"));
        
    if ( tv.iUseGPS != 0 ){
      server.sendContent(F("<tr><td>GPS lock age</td><td align=center>"));
      if ( tv.fixage < 10000 ) {
        server.sendContent(String(tv.fixage)) ; 
      }else{
        server.sendContent(F("-- No Lock --")) ;           
      }
      server.sendContent(F("</td><td>(ms)</td></tr>"));
      
      server.sendContent(F("<tr><td>GPS Satellites</td><td align=center>"));
      if ( gps.satellites() == 255 ){
        server.sendContent(F("-- No Lock --")) ; 
      }else{
        server.sendContent(String(gps.satellites())) ;           
      }
      server.sendContent(F("</td></tr>")) ;
      
      server.sendContent(F("<tr><td>GPS Chars</td><td align=center>")) ;
      server.sendContent(String(gpschars)) ; 
      server.sendContent(F("</td></tr>")) ;
    }
    if( hasRTC ){
      rtc_status = DS3231_get_sreg();
      if (( rtc_status & 0x80 ) != 0 ){
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='red'>DEPLETED</td>"));
        server.sendContent("<td><form method=get action=" + server.uri() + "><input type='hidden' name='rtcbf' value='1'><input type='submit' value='RESET'></form></td>");
        server.sendContent(F("</tr>")) ;            
      }else{
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='green'>-- OK --</td><td></td></tr>")) ;                    
      }
      server.sendContent("<tr><td>RTC Temperature</td><td align=center>"+String(rtc_temp,1)+"</td><td>(C)</td></tr>") ;                    
    }
    
    
    server.sendContent(F("<tr><td>P/T temp</td><td align=center>")) ;
    server.sendContent(String(tv.gT,1)) ; 
    server.sendContent(F("</td><td>(C)</td></tr>")) ;

    server.sendContent(F("<tr><td>Pressue</td><td align=center>"));
    server.sendContent(String(tv.Pr)) ; 
    server.sendContent(F("</td><td>(mBar)</td></tr>")) ;
    
    server.sendContent(F("<tr><td>Heading (Az)</td><td align=center>")) ;
    server.sendContent(String(tv.heading,1)) ;     
    server.sendContent(F("</td><td>(Deg)</td></tr>")) ;

    server.sendContent(F("</table><br>")) ;
    
    server.sendContent(F("<table border=1 title='Solar Tracker Status'>")) ;
    if (tv.iMountType == 0 ){
      server.sendContent(F("<tr><th>Parameter</th><th>(Y) E/W Value</th><th>.</th><th>(X) N/S Value</th><th>.</th></tr>"));
    }else{
      server.sendContent(F("<tr><th>Parameter</th><th>(Y) Az Value</th><th>.</th><th>(X) Alt Value</th><th>.</th></tr>"));          
    }
    if (( tv.yMinVal == tv.yzTarget )||( tv.xMinVal == tv.xzTarget )){
      MyColor = String("bgcolor='orange'");   
    }else{
      MyColor = String("");
    }
    server.sendContent(F("<tr><td>Min Angle</td><td><form method=get action=/><input type='text' name='minay' value='")); 
    server.sendContent(String(tv.yMinVal));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='minax' value='"));
    server.sendContent(String(tv.xMinVal)); 
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")); 
    
    if (( tv.yMaxVal == tv.yzTarget )||( tv.xMaxVal == tv.xzTarget )){ // indicat if on the stops
      MyColor = String("bgcolor='orange'");   
    }else{
      MyColor = String("");
    }
    server.sendContent(F("<tr><td>Max Angle</td><td><form method=get action=/><input type='text' name='maxay' value='"));
    server.sendContent(String(tv.yMaxVal));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='maxax' value='"));
    server.sendContent(String(tv.xMaxVal));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")); 

    if ( tv.iDayNight == 0 ){  // indicate this is the target source
      MyColor = String("bgcolor='yellow'");   
    }else{
      MyColor = String("");   
    }
    server.sendContent("<tr " + MyColor + "><td>Night Park Angle</td><td><form method=get action=/><input type='text' name='paray' value='") ;
    server.sendContent(String(tv.dyPark)) ;
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='parax' value='")) ;
    server.sendContent(String(tv.dxPark));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    
    server.sendContent(F("<tr><td>Wind Park Angle</td><td><form method=get action=/><input type='text' name='parwy' value='")) ;
    server.sendContent(String(tv.dyParkWind)) ;
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='parwx' value='")) ;
    server.sendContent(String(tv.dxParkWind));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    
    server.sendContent(F("<tr><td>Offest Angle</td><td><form method=get action=/><input type='text' name='offay' value='")) ;
    server.sendContent(String(tv.yzOffset));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='offax' value='")) ;
    server.sendContent(String(tv.xzOffset));
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    
    server.sendContent(F("<tr><td>Hysteris Angle</td><td><form method=get action=/><input type='text' name='hysay' value='")) ;
    server.sendContent(String(tv.yzH)) ;
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='hysax' value='")) ;
    server.sendContent(String(tv.xzH)) ;
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")) ; 

    server.sendContent(F("<tr><td>Max Motor Speed</td><td><form method=get action=/><input type='text' name='mmspy' value='")) ;
    server.sendContent(String(tv.yMaxMotorSpeed)) ;
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='mmspx' value='")) ;
    server.sendContent(String(tv.xMaxMotorSpeed)) ;
    server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")) ; 

    if ( tv.iTrackMode == 3 ){
      MyColor = String("bgcolor='yellow'");   
      server.sendContent("<tr " + MyColor + "><td>Target Angle</td><td><form method=get action=/><input type='text' name='taray' value='");
      server.sendContent(String(tv.yzTarget));
      server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='tarax' value='"));
      server.sendContent(String(tv.xzTarget));
      server.sendContent(F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    }else{
      server.sendContent(F("<tr><td>Target Angle</td><td><b>"));
      server.sendContent(String(tv.yzTarget));
      server.sendContent(F("</td><td>(Deg)</td><td><b>")) ;
      server.sendContent(String(tv.xzTarget));
      server.sendContent(F("</td><td>(Deg)</td></tr>")) ;         
    }
    
    server.sendContent(F("<tr><td>Current Sensor Angle</td><td>")) ;
    server.sendContent(String(tv.yzAng)) ;
    server.sendContent(F("</td><td>(Deg)</td><td>")) ;
    server.sendContent(String(tv.xzAng));
    server.sendContent(F("</td><td>(Deg)</td></tr>")) ; 

    server.sendContent(F("<tr><td>Target minus Current</td><td>")) ;
    server.sendContent(String(tv.yzTarget-tv.yzAng)) ;
    server.sendContent(F("</td><td>(Deg)</td><td>")) ;
    server.sendContent(String(tv.xzTarget-tv.xzAng));
    server.sendContent(F("</td><td>(Deg)</td></tr>")) ; 
  
    if ((tv.iMountType == 1 ) && ( tv.iDayNight == 1 )&& ( tv.iTrackMode != 3 )){  // day and mount type
      MyColor = String("bgcolor='yellow'");   
    }else{
      MyColor = String("");   
    }
    server.sendContent("<tr " + MyColor + "><td>Azomouth / Elevation</td><td>");
    server.sendContent(String(tv.solar_az_deg)) ; 
    server.sendContent(F("</td><td>(Deg)</td><td>"));
    server.sendContent(String(tv.solar_el_deg)) ; 
    server.sendContent(F("</td><td>(Deg)</td></tr>"));

    if ((tv.iMountType == 0 )&& ( tv.iDayNight == 1 ) &&  ( tv.iTrackMode != 3 )){ // day and mount type
      MyColor = String("bgcolor='yellow'");   
    }else{
      MyColor = String("");   
    }
    server.sendContent("<tr " + MyColor + "><td>Hour Angle / Elevation</td><td>");
    server.sendContent(String(tv.ha)) ; 
    server.sendContent(F("</td><td>(Deg)</td><td>"));
    server.sendContent(String(tv.sunX)) ; 
    server.sendContent(F("</td><td>(Deg)</td></tr>"));
  
    server.sendContent(F("</table><br>")) ; 
  
  
  }
  SendHTTPPageFooter();
}

void SendHTTPPageFooter(){
  server.sendContent(F("<br><br><a href='/?command=1'>Load Parameters from EEPROM</a><br><br><a href='/?command=667'>Reset Memory to Factory Default</a><br>"));
  server.sendContent(F("<a href='/?command=665'>Sync UTP Time</a><br><a href='/stime'>Manual Time Set</a><br><a href='/scan'>I2C Scan</a><br>")) ;
  server.sendContent(F("<a href='/eeprom'>EEPROM Memory Contents</a><br>"));
  server.sendContent(F("<a href='/sensor'>Sensor Calibration</a><br>"));
  server.sendContent(F("<a href='/info'>Node Infomation</a><br>"));
  server.sendContent(F("<a href='/setup'>WiFi Setup</a><br>"));
  if (( ghks.MyIP[0] == 0 ) && ( ghks.MyIP[1] == 0 )){
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);  
  }else{
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.MyIP[0],ghks.MyIP[1],ghks.MyIP[2],ghks.MyIP[3]);      
  }
  server.sendContent("<br><a href='http://" + String(buff) + ":81/update'>OTA Firmware Update</a><br>");  
  server.sendContent("<a href='/?reboot=" + String(lRebootCode) + "'>Reboot</a><br>");         
  server.sendContent(F("</body></html>\r\n"));
}

void handleNotFound(){
  String message = "Seriously - No way DUDE\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print(message);
}

String sendTDTREnd(){
  return(F("</td></tr>"));
}

void SendHTTPHeader(){
  server.sendHeader(F("Server"),F("ESP8266-on-beetle-juice"),false);
  server.sendHeader(F("X-Powered-by"),F("Dougal-1.1"),false);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(F("<!DOCTYPE HTML>"));
  server.sendContent("<head><title>Team Trouble - Solar Tracker "+ String(Toleo) + " </title>");
  server.sendContent(F("<meta name=viewport content='width=320, auto inital-scale=1'>"));
  if ( tv.iDoSave != 0 ){
    server.sendContent(F("<meta http-equiv='refresh' content='5; url=/'>"));
  }
  server.sendContent(F("</head><body><html><center>"));  
}


