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
  String message = "" ;

//  SerialOutParams();

  for (uint8_t j=0; j<server.args(); j++){
    bSaveReq = 1 ;
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
          tv.iDoSave = 3 ;
        break;
        case 2: // Save values
          tv.iDoSave = 2 ;
        break;
        case 8: //  Cold Reboot
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 667: // wipe the memory to factory default
          BackIntheBoxMemory();
        break;
        case 665:
          sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server 
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
        
    i = String(server.argName(j)).indexOf("disop");
    if (i != -1){  // 
      ghks.lDisplayOptions = String(server.arg(j)).toInt() ;
      ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,255);
    }     

    i = String(server.argName(j)).indexOf("tmode");
    if (i != -1){  // have a request to set the tracking mode
      tv.iTrackMode = String(server.arg(j)).toInt() ;
      if (( tv.iTrackMode < -1) || ( tv.iTrackMode > 5 )){
        tv.iTrackMode = -1 ;
      }
    }        
    i = String(server.argName(j)).indexOf("iugps");
    if (i != -1){  // have a request to set the gps usage mode
      tv.iUseGPS = String(server.arg(j)).toInt() ;
      if (( tv.iUseGPS < 0) || ( tv.iUseGPS > 2 )){
        tv.iUseGPS = 0 ;
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
  }          

  SendHTTPHeader();
  
  server.sendContent(F("<link rel='icon' href='data:,'></head><body><html><center><h2>"));
  server.sendContent(String(tv.trackername).substring(0,16)+" Solar Tracker</h2>");

  server.sendContent(F("<a href='/'>Refresh</a><br><br>")) ;   
  if ( bSaveReq != 0 ){
    server.sendContent(F("<blink>"));      
  }
  server.sendContent(F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>")) ;         

  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
  server.sendContent("<b>"+ String(buff)) ; 
  if ( year() < 2018 ) {
    server.sendContent(F("--- CLOCK NOT SET ---")) ;
  }
  server.sendContent(F("</b><br>")) ;
  
  if ( bSaveReq != 0 ){
    server.sendContent(F("</blink><font color='red'><b>Changes Have been made to settings.<br>Make sure you save if you want to keep them</b><br></font>")) ;     
  }
  server.sendContent(F("<br>")) ;
  
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
    if (( digitalRead(tv.RELAY_YZ_DIR) == LOW )) {
      MyCheck = "W" ;
    }else{
      MyCheck = "E" ;            
    }
  }
  if ( iPWM_XZ != 0 ) {
    if (( digitalRead(tv.RELAY_XZ_DIR) == LOW )) {
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
    
  server.sendContent(F("<form method=get action=/><tr><td>Display Options</td><td align=center>")) ; 
  server.sendContent(F("<select name='disop'>")) ;
  if (ghks.lDisplayOptions == 0 ){
    server.sendContent(F("<option value='0' SELECTED>0 - Normal")); 
    server.sendContent(F("<option value='1'>1 - Invert")); 
  }else{
    server.sendContent(F("<option value='0'>0 - Normal")); 
    server.sendContent(F("<option value='1' SELECTED>1 - Invert")); 
  }
  server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));
      
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
  server.sendContent("<br><a href='http://" + String(buff) + "/update'>OTA Firmware Update</a><br>");  
  server.sendContent("<a href='/?reboot=" + String(lRebootCode) + "'>Reboot</a><br>");    
  server.sendContent("<a href='https://github.com/Dougal121/Solar'>Source at GitHub</a><br>");  
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


