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
  String pinname = "" ; 
  
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
          BackInTheBoxMemory();
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

    i = String(server.argName(j)).indexOf("b0");
    if (i != -1) { // output pin
      tv.RELAY_XZ_PWM = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("b1");
    if (i != -1) { // output pin
      tv.RELAY_YZ_PWM = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("b2");
    if (i != -1) { // output pin
      tv.RELAY_XZ_DIR = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("b3");
    if (i != -1) { // output pin
      tv.RELAY_YZ_DIR = String(server.arg(j)).toInt() ;
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
    i = String(server.argName(j)).indexOf("tname");
    if (i != -1){  // have a request to Update Node name
     String(server.arg(j)).toCharArray( tv.trackername , sizeof(tv.trackername)) ;
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
/*    i = String(server.argName(j)).indexOf("winds");
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
    }*/            
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
  
  message += F("<b>Clocks</b><table border=1 title='Clocks'><tr><th>Clock Source</th><th>Time</th></tr>\r\n");
  if ( hasRTC ){
    DS3231_get(&tv.td);
    message += F("<tr><td>Dallas RTC</td><td align=right>") ; 
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.td.year, tv.td.mon, tv.td.mday , tv.td.hour, tv.td.min, tv.td.sec);
    message += String(buff) ;
  }
  message += F("</td></tr>\r\n<tr><td>Last NTP</td><td align=right>");
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tn.year, tv.tn.mon, tv.tn.mday , tv.tn.hour, tv.tn.min, tv.tn.sec);
  message += String(buff) ; 
  message += F("</td></tr>\r\n<tr><td>Last Reboot (ish)</td><td align=right>");
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tb.year, tv.tb.mon, tv.tb.mday , tv.tb.hour, tv.tb.min, tv.tb.sec);
  message += String(buff) ; 
  if ( tv.iUseGPS != 0 ){
    message += F("</td></tr>\r\n<tr><td>Last GPS</td><td align=right>");
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tg.year, tv.tg.mon, tv.tg.mday , tv.tg.hour, tv.tg.min, tv.tg.sec);
    message += String(buff) ;
  } 
  message += F("</td></tr>\r\n<tr><td>Last Calculations</td><td align=right>");
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tv.tc.year, tv.tc.mon, tv.tc.mday , tv.tc.hour, tv.tc.min, tv.tc.sec);
  message += String(buff) ; 
  message += F("</td></tr>\r\n<tr><td><b>ESP32 Time</b></td><td align=right><b>");
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
  message += String(buff) ; 
  message += F("</td></tr></table>");

  server.sendContent(message) ;  
  message = "" ;         


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

  message += F("<br><b>Tracker Control System</b><table border=1 title='Tracker Control'><tr><th> Parameter</th><th>Value</th><th><b>");
  message +=  MyCheck + "</th></tr>\r\n" ; 

  message += F("<form method=get action=/><tr><td>Tracking Mode</td><td align=center><select name='tmode'>") ; 
  for (i = -1 ; i < 6 ; i++ ){
    message += F("<option value='"); 
    message += String(i) ;
    if ( tv.iTrackMode == i ){
       message += F("' SELECTED>");
    }else{
       message += F("'>");                    
    }
    message += String(i) + " " ;
    switch (i){
      case -1:
         message += F("Track Both Park Both");          
      break;
      case 0:
         message += F("Track Both Park Only E/W");          
      break;
      case 1:
         message += F("Track and Park E/W Only");          
      break;
      case 2:
         message += F("Track and Park N/S Only");          
      break;
      case 3:
         message += F("Dont Track Dont Park");          
      break;
      case 4:
         message += F("Dont Track Park Both");          
      break;
      case 5:
         message += F("ALL STOP - Motors Off");          
      break;
      case 6:
         message += F("Dont Track Wind Park Both");          
      break;
    }
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 
  
  message += F("<form method=get action=/><tr><td>Time Zone</td><td align=center><input type='text' name='tzone' value='") ; 
  message += String(ghks.fTimeZone) ;
  message += F("' size=6 maxlength=2></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 
    
  message += F("<form method=get action=/><tr><td>Display Options</td><td align=center>") ; 
  message += F("<select name='disop'>") ;
  if (ghks.lDisplayOptions == 0 ){
    message += F("<option value='0' SELECTED>0 - Normal"); 
    message += F("<option value='1'>1 - Invert"); 
  }else{
    message += F("<option value='0'>0 - Normal"); 
    message += F("<option value='1' SELECTED>1 - Invert"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  message += "<form method=get action=/><tr><td>RELAY_XZ_PWM PIN</td><td align=center><select name='b0'>";
  for (ii = 0; ii < 33; ii++) {
    if (tv.RELAY_XZ_PWM == ii ){
      MyColor = F(" SELECTED ");
    }else{
      MyColor = "";            
    }
//    iTmp = 0 ;
    pinname = strPINName(ii,&iTmp);
/*    switch(ii){
      case 12: pinname = F("GPIO 12") ; break;
      case 13: pinname = F("GPIO 13") ; break;
      case 14: pinname = F("GPIO 14") ; break;
      case 15: pinname = F("GPIO 15") ; break;
      case 16: pinname = F("GPIO 16") ; break;
      case 25: pinname = F("GPIO 25") ; break;
      case 26: pinname = F("GPIO 26") ; break;
      default: pinname = F("- UNKNOWN-") ; iTmp = 1 ; break;
    }*/
    if ( iTmp == 0 ) {
      message +=  "<option value="+String(ii)+ MyColor +">" + pinname  ;          
    }
  }
  message += "</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n" ;


  message += "<form method=get action=/><tr><td>RELAY_YZ_PWM PIN</td><td align=center><select name='b1'>";
  for (ii = 0; ii < 33; ii++) {
    if (tv.RELAY_YZ_PWM == ii ){
      MyColor = F(" SELECTED ");
    }else{
      MyColor = "";            
    }
//    iTmp = 0 ;
    pinname = strPINName(ii,&iTmp);
/*    switch(ii){
      case 27: pinname = F("GPIO 27") ; break;
      case 14: pinname = F("GPIO 14") ; break;
      case 16: pinname = F("GPIO 16") ; break;
      case 17: pinname = F("GPIO 17") ; break;
      default: pinname = F("- UNKNOWN-") ; iTmp = 1 ; break;
    }*/
    if ( iTmp == 0 ) {
      message +=  "<option value="+String(ii)+ MyColor +">" + pinname  ;          
    }
  }
  message += "</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n" ;

  message += "<form method=get action=/><tr><td>RELAY_XZ_DIR PIN</td><td align=center><select name='b2'>";
  for (ii = 0; ii < 33; ii++) {
    if (tv.RELAY_XZ_DIR == ii ){
      MyColor = F(" SELECTED ");
    }else{
      MyColor = "";            
    }
//    iTmp = 0 ;
    pinname = strPINName(ii,&iTmp);
/*    switch(ii){
      case 27: pinname = F("GPIO 27") ; break;
      case 14: pinname = F("GPIO 14") ; break;
      case 16: pinname = F("GPIO 16") ; break;
      case 17: pinname = F("GPIO 17") ; break;
      default: pinname = F("- UNKNOWN-") ; iTmp = 1 ; break;
    }*/
    if ( iTmp == 0 ) {
      message +=  "<option value="+String(ii)+ MyColor +">" + pinname  ;          
    }
  }
  message += "</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n" ;

  message += "<form method=get action=/><tr><td>RELAY_YZ_DIR PIN</td><td align=center><select name='b3'>";
  for (ii = 0; ii < 33; ii++) {
    if (tv.RELAY_YZ_DIR == ii ){
      MyColor = F(" SELECTED ");
    }else{
      MyColor = "";            
    }
//    iTmp = 0 ;
    pinname = strPINName(ii,&iTmp);
/*    
    switch(ii){
      case 27: pinname = F("GPIO 27") ; break;
      case 14: pinname = F("GPIO 14") ; break;
      case 16: pinname = F("GPIO 16") ; break;
      case 17: pinname = F("GPIO 17") ; break;
      default: pinname = F("- UNKNOWN-") ; iTmp = 1 ; break;
    }*/
    if ( iTmp == 0 ) {
      message +=  "<option value="+String(ii)+ MyColor +">" + pinname  ;          
    }
  }
  message += "</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n" ;


  message += F("<form method=get action=/><tr><td>Latitude +N -S</td><td align=center><input type='text' name='mylat' value='") ; 
  message += String(tv.latitude,8);
  message += F("' size=12></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += F("<form method=get action=/><tr><td>Longitude</td><td align=center><input type='text' name='mylon' value='") ; 
  message += String(tv.longitude,8);
  message += F("' size=12></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  server.sendContent(message) ;  
  message = "" ;    

  message += F("<form method=get action=/><tr><td>X (N/S) Axis Multiplier</td><td align=center><input type='text' name='mulax' value='") ; 
  message += String(tv.xMul,2);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += F("<form method=get action=/><tr><td>Y (E/W) Axis Multiplier</td><td align=center><input type='text' name='mulay' value='") ; 
  message += String(tv.yMul,2) ;
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += F("<form method=get action=/><tr><td>Z (Vert) Axis Multiplier</td><td align=center><input type='text' name='mulaz' value='") ; 
  message += String(tv.zMul,2);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += F("<form method=get action=/><tr><td>X<->Y Axis Swap</td><td align=center>") ; 
  message += F("<select name='xyswp'>") ;
  if (tv.iXYS == 0 ){
    message += F("<option value='0' SELECTED>0 Normal"); 
    message += F("<option value='1'>1 Swapped"); 
  }else{
    message += F("<option value='0'>0 Normal"); 
    message += F("<option value='1' SELECTED>1 Swapped"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  message += F("<form method=get action=/><tr><td>Night Shutdown</td><td align=center>") ; 
  message += F("<select name='nisht'>");
  if (tv.iNightShutdown == 0 ){
    message += F("<option value='0' SELECTED>0 Shutdown at Night"); 
    message += F("<option value='1'>1 Always Active"); 
  }else{
    message += F("<option value='0'>0 Shutdown at Night"); 
    message += F("<option value='1' SELECTED>1 Always Active"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  message += F("<form method=get action=/><tr><td>Multi Drive</td><td align=center>") ; 
  message += F("<select name='mltdr'>");
  if (tv.iMultiDrive == 0 ){
    message += F("<option value='0' SELECTED>0 One Axis at a Time"); 
    message += F("<option value='1'>1 Both axis active at once"); 
  }else{
    message += F("<option value='0'>0 One Axis at a Time"); 
    message += F("<option value='1' SELECTED>1 Both axis active at once"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");


  message += F("<form method=get action=/><tr><td>Motor Drive Electrics</td><td align=center>") ; 
  message += F("<select name='outpt'>");
  switch (tv.iOutputType){
    case 0:     
      message += F("<option value='0' SELECTED>0 STANDARD PWM / DIR "); 
      message += F("<option value='1'>1 L298 IN1 IN2"); 
      message += F("<option value='2'>2 RELAY FWD / REV Active Low"); 
      message += F("<option value='3'>3 RELAY FWD / REV Active High"); 
    break;
    case 1:     
      message += F("<option value='0'>0 STANDARD PWM / DIR "); 
      message += F("<option value='1' SELECTED>1 L298 IN1 IN2"); 
      message += F("<option value='2'>2 RELAY FWD / REV Active Low"); 
      message += F("<option value='3'>3 RELAY FWD / REV Active High"); 
    break;
    case 2:     
      message += F("<option value='0'>0 STANDARD PWM / DIR "); 
      message += F("<option value='1'>1 L298 IN1 IN2"); 
      message += F("<option value='2' SELECTED>2 RELAY FWD / REV Active Low"); 
      message += F("<option value='3'>3 RELAY FWD / REV Active High"); 
    break;
    case 3:     
      message += F("<option value='0'>0 STANDARD PWM / DIR "); 
      message += F("<option value='1'>1 L298 IN1 IN2"); 
      message += F("<option value='2'>2 RELAY FWD / REV Active Low"); 
      message += F("<option value='3' SELECTED>3 RELAY FWD / REV Active High"); 
    break;
  }  
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  message += F("<form method=get action=/><tr><td>Tracker Name</td><td align=center><input type='text' name='tname' value='");
  message += String(tv.trackername) ; 
  message += F("' size=16 maxlength=16><input type='hidden' name='dummy' value='0'></td><td><input type='submit' value='SET'></td></tr></form>\r\n");


  message += F("<form method=get action=/><tr><td>Hardware GPS</td><td align=center>") ; 
  message += F("<select name='iugps'>");
  switch (tv.iUseGPS){
    case 1:
      message += F("<option value='0'>0 No GPS"); 
      message += F("<option value='1' SELECTED>1 Use GPS"); 
      message += F("<option value='2'>2 Use Networked GPS"); 
    break;
    case 2:
      message += F("<option value='0'>0 No GPS"); 
      message += F("<option value='1'>1 Use GPS"); 
      message += F("<option value='2' SELECTED>2 Use Networked GPS"); 
    break;
    default:  // 0 as well
      message += F("<option value='0' SELECTED>0 No GPS"); 
      message += F("<option value='1'>1 Use GPS"); 
      message += F("<option value='2'>2 Use Networked GPS"); 
    break;
  }  
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  message += F("<form method=get action=/><tr><td>Mount Type</td><td align=center>") ; 
  message += F("<select name='mount'>");
  if (tv.iMountType == 0 ){
    message += F("<option value='0' SELECTED>0 Equatorial"); 
    message += F("<option value='1'>1 Alt/Az"); 
  }else{
    message += F("<option value='0'>0 Equatorial"); 
    message += F("<option value='1' SELECTED>1 Alt/Az"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  server.sendContent(message) ;  
  message = "" ;    

  message += F("<form method=get action=/><tr><td>Auto Time Updates</td><td align=center>") ; 
  message += F("<select name='tmsrc'>");
  if (tv.iTimeSource == 0 ){
    message += F("<option value='0' SELECTED>0 Update From RTC every Hour"); 
    message += F("<option value='1'>1 Update from NTP Every 24 Hrs"); 
  }else{
    message += F("<option value='0'>0 Update From RTC every Hour"); 
    message += F("<option value='1' SELECTED>1 Update from NTP Every 24 Hrs"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr></form>\r\n");

  message += F("<tr><td>Max Wind Speed</td><td align=center>") ; 
  message += String(tv.iMaxWindSpeed);
  message += "</td><td>("+String(tv.Wind_Unit)+")</td></tr>\r\n" ; 
  message += F("<tr><td>Time above speed to park</td><td align=center>") ; 
  message += String(tv.iMaxWindTime);
  message += F("</td><td>(sec)</td></tr>\r\n") ; 
  message += F("<tr><td>Min Wind Speed</td><td align=center>") ; 
  message += String(tv.iMaxWindSpeed);
  message += "</td><td>("+String(tv.Wind_Unit)+")</td></tr>\r\n" ; 
  message += F("<tr><td>Time below Min to resume</td><td align=center>") ; 
  message += String(tv.iMinWindTime);
  message += F("</td><td>(sec)</td></tr>\r\n") ; 
/*
  message += F("<tr><td>WiFi RSSI</td><td align=center>"));
  message += String(WiFi.RSSI())) ; 
  message += F("</td><td>(dBm)</td></tr>"));
*/    

  message += F("<tr><td>Solar Declination</td><td align=center>");
  message += String(decl,3) ; 
  message += F("</td><td>(Deg)</td></tr>\r\n");

  message += F("<tr><td>Equation of Time</td><td align=center>");
  message += String(eqtime,3) ; 
  message += F("</td><td>(min)</td></tr>\r\n");

  message += F("<tr><td>Sunrise - State - Sunset</td><td align=center>");
  snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(tv.sunrise), MinSolarTime(tv.sunrise));
  message += String(buff) ; 
  if ( tv.iDayNight == 1 ){
    message += F(" - DAY - ");
  }else{
    message += F(" - NIGHT - ");          
  }
  snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(tv.sunset), MinSolarTime(tv.sunset));        
  message += String(buff) ; 
  message += F("</td><td>(hh:mm)</td></tr>\r\n");
      
  if ( tv.iUseGPS != 0 ){
    message += F("<tr><td>GPS lock age</td><td align=center>");
    if ( tv.fixage < 10000 ) {
      message += String(tv.fixage) ; 
    }else{
      message += F("-- No Lock --") ;           
    }
    message += F("</td><td>(ms)</td></tr>\r\n");
    
    message += F("<tr><td>GPS Satellites</td><td align=center>");
    if ( gps.satellites() == 255 ){
      message += F("-- No Lock --") ; 
    }else{
      message += String(gps.satellites()) ;           
    }
    message += F("</td></tr>\r\n") ;
    
    message += F("<tr><td>GPS Chars</td><td align=center>") ;
    message += String(gpschars) ; 
    message += F("</td></tr>\r\n") ;
  }
  if( hasRTC ){
    rtc_status = DS3231_get_sreg();
    if (( rtc_status & 0x80 ) != 0 ){
      message += F("<tr><td>RTC Battery</td><td align=center bgcolor='red'>DEPLETED</td>");
      message += "<td><form method=get action=" + server.uri() + "><input type='hidden' name='rtcbf' value='1'><input type='submit' value='RESET'></form></td>";
      message += F("</tr>\r\n") ;            
    }else{
      message += F("<tr><td>RTC Battery</td><td align=center bgcolor='green'>-- OK --</td><td></td></tr>\r\n") ;                    
    }
    message += "<tr><td>RTC Temperature</td><td align=center>"+String(rtc_temp,1)+"</td><td>(C)</td></tr>\r\n" ;                    
  }
  
  
  message += F("<tr><td>P/T temp</td><td align=center>") ;
  message += String(tv.gT,1) ; 
  message += F("</td><td>(C)</td></tr>\r\n") ;

  message += F("<tr><td>Pressue</td><td align=center>");
  message += String(tv.Pr) ; 
  message += F("</td><td>(mBar)</td></tr>\r\n") ;
  
  message += F("<tr><td>Heading (Az)</td><td align=center>") ;
  message += String(tv.heading,1) ;     
  message += F("</td><td>(Deg)</td></tr>\r\n") ;

  snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
  message += "<tr><td>Computer Uptime</td><td align=center>"+String(buff)+"</td><td>(day:hr:min)</td></tr>"  ;
  message += F("</table><br>\r\n") ;
  
  server.sendContent(message) ;  
  message = "" ;         

  message += F("<b>Tracker Position Status</b>\r\n") ;
  message += F("<table border=1 title='Tracker Position Status'>\r\n") ;
  if (tv.iMountType == 0 ){
    message += F("<tr><th>Parameter</th><th>(Y) E/W Value</th><th>.</th><th>(X) N/S Value</th><th>.</th></tr>\r\n");
  }else{
    message += F("<tr><th>Parameter</th><th>(Y) Az Value</th><th>.</th><th>(X) Alt Value</th><th>.</th></tr>\r\n");          
  }
  if (( tv.yMinVal == tv.yzTarget )||( tv.xMinVal == tv.xzTarget )){
    MyColor = String("bgcolor='orange'");   
  }else{
    MyColor = String("");
  }
  message += F("<tr><td>Min Angle</td><td><form method=get action=/><input type='text' name='minay' value='"); 
  message += String(tv.yMinVal);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='minax' value='");
  message += String(tv.xMinVal); 
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n"); 
  
  if (( tv.yMaxVal == tv.yzTarget )||( tv.xMaxVal == tv.xzTarget )){ // indicat if on the stops
    MyColor = String("bgcolor='orange'");   
  }else{
    MyColor = String("");
  }
  message += F("<tr><td>Max Angle</td><td><form method=get action=/><input type='text' name='maxay' value='");
  message += String(tv.yMaxVal);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='maxax' value='");
  message += String(tv.xMaxVal);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n"); 

  if (( tv.iDayNight == 0 )&& (!bWindPark)) {  // indicate this is the target source
    MyColor = String("bgcolor='yellow'");   
  }else{
    MyColor = String("");   
  }
  message += "<tr " + MyColor + "><td>Night Park Angle</td><td><form method=get action=/><input type='text' name='paray' value='" ;
  message += String(tv.dyPark) ;
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='parax' value='") ;
  message += String(tv.dxPark);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n") ; 

  if ( bWindPark ){
    MyColor = String("bgcolor='yellow'");   
  }else{
    MyColor = String("");   
  }
  message += "<tr " + MyColor + "><td>Wind Park Angle</td><td><form method=get action=/><input type='text' name='parwy' value='" ;
  message += String(tv.dyParkWind) ;
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='parwx' value='") ;
  message += String(tv.dxParkWind);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n") ; 
  
  message += F("<tr><td>Offest Angle</td><td><form method=get action=/><input type='text' name='offay' value='") ;
  message += String(tv.yzOffset);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='offax' value='") ;
  message += String(tv.xzOffset);
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n") ; 
  
  message += F("<tr><td>Hysteris Angle</td><td><form method=get action=/><input type='text' name='hysay' value='") ;
  message += String(tv.yzH) ;
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='hysax' value='") ;
  message += String(tv.xzH) ;
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n") ; 

  message += F("<tr><td>Max Motor Speed</td><td><form method=get action=/><input type='text' name='mmspy' value='") ;
  message += String(tv.yMaxMotorSpeed) ;
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='mmspx' value='") ;
  message += String(tv.xMaxMotorSpeed) ;
  message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n") ; 

  if (( tv.iTrackMode == 3 )&& (!bWindPark)){
    MyColor = String("bgcolor='yellow'");   
    message += "<tr " + MyColor + "><td>Target Angle</td><td><form method=get action=/><input type='text' name='taray' value='";
    message += String(tv.yzTarget);
    message += F("' size=10></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='tarax' value='");
    message += String(tv.xzTarget);
    message += F("' size=10></td><td><input type='submit' value='SET'></form></td></tr>\r\n") ; 
  }else{
    message += F("<tr><td>Target Angle</td><td><b>");
    message += String(tv.yzTarget);
    message += F("</td><td>(Deg)</td><td><b>") ;
    message += String(tv.xzTarget);
    message += F("</td><td>(Deg)</td></tr>\r\n") ;         
  }
  
  message += F("<tr><td>Current Sensor Angle</td><td>") ;
  message += String(tv.yzAng) ;
  message += F("</td><td>(Deg)</td><td>") ;
  message += String(tv.xzAng);
  message += F("</td><td>(Deg)</td></tr>\r\n") ; 

  message += F("<tr><td>Target minus Current</td><td>") ;
  message += String(tv.yzTarget-tv.yzAng) ;
  message += F("</td><td>(Deg)</td><td>") ;
  message += String(tv.xzTarget-tv.xzAng);
  message += F("</td><td>(Deg)</td></tr>\r\n") ; 

  if ((tv.iMountType == 1 ) && ( tv.iDayNight == 1 )&& ( tv.iTrackMode != 3 )&&(!bWindPark)){  // day and mount type
    MyColor = String("bgcolor='yellow'");   
  }else{
    MyColor = String("");   
  }
  message += "<tr " + MyColor + "><td>Azomouth / Elevation</td><td>";
  message += String(tv.solar_az_deg) ; 
  message += F("</td><td>(Deg)</td><td>");
  message += String(tv.solar_el_deg) ; 
  message += F("</td><td>(Deg)</td></tr>\r\n");

  if ((tv.iMountType == 0 )&& ( tv.iDayNight == 1 ) &&  ( tv.iTrackMode != 3 )&&(!bWindPark)){ // day and mount type
    MyColor = String("bgcolor='yellow'");   
  }else{
    MyColor = String("");   
  }
  message += "<tr " + MyColor + "><td>Hour Angle / Elevation</td><td>";
  message += String(tv.ha) ; 
  message += F("</td><td>(Deg)</td><td>");
  message += String(tv.sunX) ; 
  message += F("</td><td>(Deg)</td></tr>\r\n");

  message += F("</table>\r\n") ; 
  server.sendContent(message) ;  
  message = "" ;         
  
  SendHTTPPageFooter();
}

void SendHTTPPageFooter(){
  String message = F("<br><br><a href='/?command=1'>Load Parameters from EEPROM</a><br><br><a href='/?command=667'>Reset Memory to Factory Default</a><br>\r\n");
  message += F("<a href='/?command=665'>Sync UTP Time</a><br><a href='/stime'>Manual Time Set</a><br><a href='/scan'>I2C Scan</a><br>\r\n") ;
  message += F("<a href='/eeprom'>EEPROM Memory Contents</a><br>\r\n");
  message += F("<a href='/sensor'>Sensor Calibration</a><br>\r\n");
  message += F("<a href='/info'>Node Infomation</a><br>\r\n");
  message += F("<a href='/email'>Email Setup</a><br>\r\n");  
  message += F("<a href='/adc'>ADC Setup</a><br>\r\n");  
  message += F("<a href='/log'>Data Log Table</a> <a href='/chart'>Chart Log</a><br>\r\n");  
  if (hasRTC){
    message += F("<a href='/rtcmem'>Display RTC EEPROM memory</a><br>\r\n");  
  }
  message += F("<a href='/setup'>WiFi Setup</a><br>\r\n");
  if (!WiFi.isConnected()){
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);  
  }else{
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u", ghks.MyIP[0],ghks.MyIP[1],ghks.MyIP[2],ghks.MyIP[3]);      
  }
  message += "<br><a href='http://" + String(buff) + "/update'>OTA Firmware Update</a><br>\r\n";  
  message += "<a href='/?reboot=" + String(lRebootCode) + "'>Reboot</a><br>\r\n";    
  message += "<a href='https://github.com/Dougal121/Solar'>Source at GitHub</a><br>\r\n";  
  message += "<a href='http://" + String(buff) + "/backup'>Backup / Restore Settings</a><br>\r\n";  
  snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
  message += "<br>Computer Uptime <b>"+String(buff)+"</b> (day:hr:min) <br>\r\n" ;    
  message += F("</body></html>\r\n");

  server.sendContent(message) ;  
  message = "" ;         
}

void handleNotFound(){
  String message = "Seriously? - No way DUDE\n\n";
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
  server.sendHeader(F("Server"),F("ESP32-on-beetle-juice"),false);
  server.sendHeader(F("X-Powered-by"),F("Dougal-filament-6"),false);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  String message = F("<!DOCTYPE HTML>");
  message += "<head><title>Team Trouble - Solar Tracker "+ String(Toleo) + " </title>\r\n";
  message += F("<meta name=viewport content='width=320, auto inital-scale=1'>\r\n");
  if ( tv.iDoSave != 0 ){
    message += F("<meta http-equiv='refresh' content='5; url=/'>\r\n");
  }
  message += F("<link rel='icon' href='data:,'>\r\n");  
  message += F("</head><body><html><center><h2>\r\n"); 
  message += String(WiFi.RSSI()) + "(dB) <a href='/'>" + String(tv.trackername).substring(0,16)+" Solar Tracker</a></h2>\r\n";

  if ( bSaveReq != 0 ){
    message += F("<blink>");      
  }
  message += F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>") ;         
  if ( bSaveReq != 0 ){
    message += F("</blink><font color='red'><b>Changes Have been made to settings.<br>Make sure you save if you want to keep them</b><br></font>") ;     
  }   
  server.sendContent(message) ;  
  message = "" ;         
}

String strPINName(int iPin,int *iTmp){
  *iTmp = 0 ;
  String pinname = "" ;
    switch(iPin){
      case 0: pinname = F("GPIO 0") ; break;
      case 2: pinname = F("GPIO 2") ; break;
      case 12: pinname = F("GPIO 12") ; break;
      case 13: pinname = F("GPIO 13") ; break;
      case 14: pinname = F("GPIO 14") ; break;
      case 15: pinname = F("GPIO 15") ; break;
      case 16: pinname = F("GPIO 16") ; break;
      case 25: pinname = F("GPIO 25") ; break;
      case 26: pinname = F("GPIO 26") ; break;
      default: pinname = F("- UNKNOWN-") ; *iTmp = 1 ; break;
    }
    return(pinname);
}

