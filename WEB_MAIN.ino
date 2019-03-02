
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
/*  
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
  */
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
          LoadParamsFromEEPROM(true);
        break;
        case 2: // Save values
          LoadParamsFromEEPROM(false);
        break;
        case 3: // 
        break;
        case 4: // 
        break;
        case 5: // 
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
          sendNTPpacket(timeServer); // send an NTP packet to a time server  once and hour  
        break;
        case 668:
        break;
      }  
    }
    
    i = String(server.argName(j)).indexOf("tname"); // wanta do this one BEFORE we dick with captialisation of the request
    if (i != -1){  
      String(server.arg(j)).toCharArray(trackername,sizeof(trackername));
    }        
    i = String(server.argName(j)).indexOf("nssid");    // ssid setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(nssid,sizeof(nssid));
    }        
    i = String(server.argName(j)).indexOf("npass");   // password setup
    if (i != -1){  // have a request to set the time zone
      String(server.arg(j)).toCharArray(npassword,sizeof(npassword));
    }        

    i = String(server.argName(j)).indexOf("naddr");   // ip address setup
    if (i != -1){  // have a request to set the time zone
      MyIP[0] = String(server.arg(j)).substring(0,3).toInt() ;
      MyIP[1] =String(server.arg(j)).substring(4,7).toInt() ;
      MyIP[2] = String(server.arg(j)).substring(8,11).toInt() ;
      MyIP[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }        

    i = String(server.argName(j)).indexOf("tmode");
    if (i != -1){  // have a request to set the tracking mode
      iTrackMode = String(server.arg(j)).toInt() ;
      if (( iTrackMode < -1) || ( iTrackMode > 4 )){
        iTrackMode = -1 ;
      }
    }        
    i = String(server.argName(j)).indexOf("iugps");
    if (i != -1){  // have a request to set the tracking mode
      iUseGPS = String(server.arg(j)).toInt() ;
      if (( iUseGPS < 0) || ( iUseGPS > 2 )){
        iUseGPS = 0 ;
      }
    }        
    i = String(server.argName(j)).indexOf("tzone");
    if (i != -1){  // have a request to set the time zone
      timezone = String(server.arg(j)).toInt() ;
      if (( timezone < -23) || ( timezone > 23 )){
        timezone = 10 ;
      }
    }        
    i = String(server.argName(j)).indexOf("mylat");    //lat  
    if (i != -1){  // have a request to set the latitude
      latitude = String(server.arg(j)).toFloat() ;
      if (( latitude < -90) || ( latitude > 90 )){
        latitude = -34.051219 ;
      }
    }        
    i = String(server.argName(j)).indexOf("mylon");    // long
    if (i != -1){  // have a request to set the logitude
      longitude = String(server.arg(j)).toFloat() ;
      if (( longitude < -180) || ( longitude > 180 )){
        longitude = 142.013618 ;
      }
    }        
    i = String(server.argName(j)).indexOf("minay");
    if (i != -1){  // have a request to set minimum angle Y
      yMinVal = String(server.arg(j)).toFloat() ;
      if (( yMinVal < -70) || ( yMinVal > 50 )){
        yMinVal = -65  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("minax");
    if (i != -1){  // have a request to set minimum angle X 
      xMinVal = String(server.arg(j)).toFloat() ;
      if (( xMinVal < -10) || ( xMinVal > 60 )){
        xMinVal = 0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("maxay");
    if (i != -1){  // have a request to set maximum angle Y
      yMaxVal = String(server.arg(j)).toFloat() ;
      if (( yMaxVal < -70) || ( yMaxVal > 50 )){
        yMaxVal = 45  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("maxax");
    if (i != -1){  // have a request to set maximum angle X
      xMaxVal = String(server.arg(j)).toFloat() ;
      if (( xMaxVal < -10) || ( xMaxVal > 60 )){
        xMaxVal = 50  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("paray");
    if (i != -1){  // have a request to set park angle Y
      dyPark = String(server.arg(j)).toFloat() ;
      if (( dyPark < -70) || ( dyPark > 50 )){
        dyPark = 0 ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("parax");
    if (i != -1){  // have a request to set park angle X
      dxPark = String(server.arg(j)).toFloat() ;
      if (( dxPark < -10) || ( dxPark > 60 )){
        dxPark = 6  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("offay");
    if (i != -1){  // have a request to set offset angle Y
      yzOffset = String(server.arg(j)).toFloat() ;
      if (( yzOffset < -20) || ( yzOffset > 20 )){
        yzOffset = 0 ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("offax");
    if (i != -1){  // have a request to set offset angle X
      xzOffset = String(server.arg(j)).toFloat() ;
      if (( xzOffset < -20) || ( xzOffset > 20 )){
        xzOffset = 0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("hysay");
    if (i != -1){  // have a request to set Hysterisis angle
      yzH = String(server.arg(j)).toFloat() ;
      if (( yzH < -20) || ( yzH > 20 )){
        yzH = 4 ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("hysax");
    if (i != -1){  // have a request to set Hysterisis angle
      xzH = String(server.arg(j)).toFloat() ;
      if (( xzH < -20) || ( xzH > 20 )){
        xzH = 4  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("taray");
    if (i != -1){  // have a request to set Hysterisis angle
      yzTarget = String(server.arg(j)).toFloat() ;
      if (( yzTarget < -70) || ( yzTarget > 50 )){
        yzTarget = 0 ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("tarax");
    if (i != -1){  // have a request to set Hysterisis angle
      xzTarget = String(server.arg(j)).toFloat() ;
      if (( xzTarget < -10) || ( xzTarget > 60 )){
        xzTarget = 0  ; // set to default
      }
    }        
  
    i = String(server.argName(j)).indexOf("mulax");
    if (i != -1){  // have a request to set Axis Multiplier
      xMul = String(server.arg(j)).toFloat() ;
      if (( xMul < -10) || ( xMul > 10 )){
        xMul = 1.0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("mulay");
    if (i != -1){  // have a request to set Axis Multiplier
      yMul = String(server.arg(j)).toFloat() ;
      if (( yMul < -10) || ( yMul > 10 )){
        yMul = 1.0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("mulaz");
    if (i != -1){  // have a request to set Axis Multiplier
      zMul = String(server.arg(j)).toFloat() ;
      if (( zMul < -10) || ( zMul > 10 )){
        zMul = 1.0  ; // set to default
      }
    }        
    i = String(server.argName(j)).indexOf("xyswp");
    if (i != -1){  // have a request to set the time zone
      iXYS = String(server.arg(j)).toInt() ;
      if (( iXYS < 0) || ( iXYS > 1 )){
        iXYS = 0 ;
      }
    }       
    i = String(server.argName(j)).indexOf("nisht");
    if (i != -1){  // have a request to set the time zone
      iNightShutdown = String(server.arg(j)).toInt() ;
      if (( iNightShutdown < 0) || ( iNightShutdown > 1 )){
        iNightShutdown = 0 ;
      }
    }            
    i = String(server.argName(j)).indexOf("mltdr");
    if (i != -1){  // have a request to set the time zone
      iMultiDrive = String(server.arg(j)).toInt() ;
      if (( iMultiDrive < 0) || ( iMultiDrive > 1 )){
        iMultiDrive = 0 ;
      }
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
        tc.sec = second();     
        tc.min = minute();     
        tc.hour = hour();   
        tc.wday = dayOfWeek(makeTime(tm));            
        tc.mday = day();  
        tc.mon = month();   
        tc.year = year();       
        DS3231_set(tc);                       // set the RTC as well
        rtc_status = DS3231_get_sreg();       // get the status
        DS3231_set_sreg(rtc_status & 0x7f ) ; // clear the clock fail bit when you set the time
      }
    }        
    i = String(server.argName(j)).indexOf("tzone");
    if (i != -1){  // 
      lTimeZone = String(server.arg(j)).toInt() ;
      constrain(lTimeZone,-12,12);
      bDoTimeUpdate = true ; // trigger and update to fix the time
    }
  }          

  SendHTTPHeader();
  
  server.sendContent(F("<link rel='icon' href='data:,'></head><body><html><center><h2>"));
  server.sendContent(String(trackername).substring(0,16)+" Solar Tracker</h2>");

  server.sendContent(F("<a href='/'>Refresh</a><br><br>")) ;         
  server.sendContent(F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>")) ;         

  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
  server.sendContent("<b>"+ String(buff)) ; 
  if ( year() < 2000 ) {
    server.sendContent(F("--- CLOCK NOT SET ---")) ;
  }
  server.sendContent(F("</b><br>")) ;  
  if ( AutoOff_t > now() )  {
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(AutoOff_t), month(AutoOff_t), day(AutoOff_t) , hour(AutoOff_t), minute(AutoOff_t), second(AutoOff_t));
    server.sendContent(F("<b><font color=red>Automation OFFLINE Untill ")) ;  
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</font></b><br>")) ; 
  }else{
    if ( year() > 2000 ) {
      server.sendContent(F("<b><font color=green>Automation ONLINE</font></b><br>")) ;  
    }
  }

  if (String(server.uri()).indexOf("stime")>0) {  // setup of the node
    bDefault = false ;
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(), month(), day() , hour(), minute());
    server.sendContent("<br><br><form method=get action=" + server.uri() + "><br>Set Current Time: <input type='text' name='stime' value='"+ String(buff) + "' size=12>");
    server.sendContent(F("<input type='submit' value='SET'><br><br></form>"));
  }
  
  if (String(server.uri()).indexOf("setup")>0) {  // setup of the node
    bDefault = false ;
    server.sendContent("<form method=get action=" + server.uri() + "><table border=1 title='Node Settings'>");
    server.sendContent(F("<tr><th>Parameter</th><th>Value</th><th><input type='submit' value='SET'></th></tr>"));
  
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(AutoOff_t), month(AutoOff_t), day(AutoOff_t) , hour(AutoOff_t), minute(AutoOff_t));
    if (AutoOff_t > now()){
      MyColor =  F("bgcolor=red") ;
    }else{
      MyColor =  "" ;
    }
    server.sendContent(F("<tr><td>Time Zone</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='tzone' value='" + String(lTimeZone) + "' size=12></td><td></td></tr>");
  
    server.sendContent(F("<tr><td>Local UDP Port NTP</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='lpntp' value='" + String(localPort) + "' size=12></td><td></td></tr>");
  
    server.sendContent(F("<tr><td>Local UDP Port Control</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='lpctr' value='" + String(localPortCtrl) + "' size=12></td><td></td></tr>");
  
    server.sendContent(F("<tr><td>Remote UDP Port Control</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='rpctr' value='" + String(RemotePortCtrl) + "' size=12></td><td></td></tr>");
  
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", RCIP[0],RCIP[1],RCIP[2],RCIP[3]);
    server.sendContent(F("<tr><td>Remote IP Address Control</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='rpcip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>");
  
    server.sendContent(F("<tr><td>SSID</td><td align=center>")) ; 
//    snprintf(buff, BUFF_MAX, "%s" , nssid );
    server.sendContent("<input type='text' name='nssid' value='" + String(nssid) + "' maxlength=15 size=12></td><td></td></tr>");

//    Serial.println(String(nssid));
//    Serial.println(String(npassword));
  
    server.sendContent(F("<tr><td>Password</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='npass' value='" + String(npassword) + "' maxlength=15 size=12></td><td></td></tr>");
  
    server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String(ESP.getChipId(), HEX) + "</td><td align=center>"+String(ESP.getChipId())+"</td></tr>" ) ; 
    WiFi.macAddress(mac);      
    snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    server.sendContent("<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>" ) ; 
    server.sendContent("<tr><td>WiFi RSSI</td><td align=center>" + String(WiFi.RSSI()) + "</td><td>(dBm)</td></tr>" ) ; 
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
    server.sendContent("<tr><td>Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
    server.sendContent("<tr><td>Last Scan Speed</td><td align=center>" + String(lScanCtr) + "</td><td>(per second)</td></tr>" ) ;    
    if( hasRTC ){
      rtc_status = DS3231_get_sreg();
      if (( rtc_status & 0x80 ) != 0 ){
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='red'>DEPLETED</td><td></td></tr>")) ;            
      }else{
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='green'>-- OK --</td><td></td></tr>")) ;                    
      }
      server.sendContent("<tr><td>RTC Temperature</td><td align=center>"+String(rtc_temp,1)+"</td><td>(C)</td></tr>") ;                    
    }
    server.sendContent(F("</form></table>"));
  }
  
  
  if (bDefault) {     // default valve control and setup
    server.sendContent(F("<b>Clocks</b><table border=1 title='Clocks'><tr><th>Clock Source</th><th>Time</th></tr>"));
    if ( hasRTC ){
      DS3231_get(&td);
      server.sendContent(F("<tr><td>Dallas RTC</td><td align=right>")) ; 
      snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", td.year, td.mon, td.mday , td.hour, td.min, td.sec);
      server.sendContent(String(buff)) ;
    }
    server.sendContent(F("</td></tr><tr><td>Last NTP</td><td align=right>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tn.year, tn.mon, tn.mday , tn.hour, tn.min, tn.sec);
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr><tr><td>Last Reboot (ish)</td><td align=right>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tb.year, tb.mon, tb.mday , tb.hour, tb.min, tb.sec);
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr><tr><td>Last GPS</td><td align=right>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d<br>", tg.year, tg.mon, tg.mday , tg.hour, tg.min, tg.sec);
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr><tr><td><b>Arduino Time</b></td><td align=right><b>"));
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td></tr></table>"));
  
    server.sendContent(F("<br><b>Tracker Control System</b><table border=1 title='Tracker Control'><tr><th> Parameter</th><th>Value</th></tr>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Tracking Mode</td><td align=center><select name='tmode'>")) ; 
    for (i = -1 ; i < 5 ; i++ ){
      server.sendContent(F("<option value='")); 
      server.sendContent(String(i)) ;
      if ( iTrackMode == i ){
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
      }
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>")) ; 
    
    server.sendContent(F("<form method=get action=/><tr><td>Time Zone</td><td align=center><input type='text' name='tzone' value='")) ; 
    server.sendContent(String(timezone)) ;
    server.sendContent(F("' size=6 maxlength=2></td><td><input type='submit' value='SET'></td></tr></form>")) ; 
    
    server.sendContent(F("<form method=get action=/><tr><td>Latitude +N -S</td><td align=center><input type='text' name='mylat' value='")) ; 
    server.sendContent(String(latitude,8));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Longitude</td><td align=center><input type='text' name='mylon' value='")) ; 
    server.sendContent(String(longitude,8));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>X (N/S) Axis Multiplier</td><td align=center><input type='text' name='mulax' value='")) ; 
    server.sendContent(String(xMul,2));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Y (E/W) Axis Multiplier</td><td align=center><input type='text' name='mulay' value='")) ; 
    server.sendContent(String(yMul,2)) ;
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>Z (Vert) Axis Multiplier</td><td align=center><input type='text' name='mulaz' value='")) ; 
    server.sendContent(String(zMul,2));
    server.sendContent(F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>")) ; 

    server.sendContent(F("<form method=get action=/><tr><td>X<->Y Axis Swap</td><td align=center>")) ; 
    server.sendContent(F("<select name='xyswp'>")) ;
    if (iXYS == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 Normal")); 
      server.sendContent(F("<option value='1'>1 Swapped")); 
    }else{
      server.sendContent(F("<option value='0'>0 Normal")); 
      server.sendContent(F("<option value='1' SELECTED>1 Swapped")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Night Shutdown</td><td align=center>")) ; 
    server.sendContent(F("<select name='nisht'>"));
    if (iNightShutdown == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 Shutdown at Night")); 
      server.sendContent(F("<option value='1'>1 Always Active")); 
    }else{
      server.sendContent(F("<option value='0'>0 Shutdown at Night")); 
      server.sendContent(F("<option value='1' SELECTED>1 Always Active")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Multi Drive</td><td align=center>")) ; 
    server.sendContent(F("<select name='mltdr'>"));
    if (iMultiDrive == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 One Axis at a Time")); 
      server.sendContent(F("<option value='1'>1 Both axis active at once")); 
    }else{
      server.sendContent(F("<option value='0'>0 One Axis at a Time")); 
      server.sendContent(F("<option value='1' SELECTED>1 Both axis active at once")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Tracker Name</td><td align=center><input type='text' name='tname' value='"));
    server.sendContent(String(trackername)) ; 
    server.sendContent(F("' size=16 maxlength=16><input type='hidden' name='dummy' value='0'></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Config SSID</td><td align=center><input type='text' name='nssid' value='"));
    server.sendContent(String(nssid)) ; 
    server.sendContent(F("' size=24 maxlength=24><input type='hidden' name='dummy' value='0'></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Config Password</td><td align=center><input type='text' name='npass' value='"));
    if (String(npassword).length() < 1){
      server.sendContent(" ") ;             
    }else{
      server.sendContent(String(npassword)) ;       
    }
    server.sendContent(F("' size=16 maxlength=16><input type='hidden' name='dummy' value='0'></td><td><input type='submit' value='SET'></td></tr></form>"));

    snprintf(buff, BUFF_MAX, "%03d.%03d.%03d.%03d", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
    server.sendContent(F("<form method=get action=/><tr><td>Config IP Address</td><td align=center><input type='text' name='naddr' value='"));
    server.sendContent(String(buff)) ; 
    server.sendContent(F("' size=16 maxlength=24><input type='hidden' name='dummy' value='0'></td><td><input type='submit' value='SET'></td></tr></form>"));

    server.sendContent(F("<form method=get action=/><tr><td>Hardware GPS</td><td align=center>")) ; 
    server.sendContent(F("<select name='iugps'>"));
    switch (iUseGPS){
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

    server.sendContent(F("<tr><td>WiFi RSSI</td><td align=center>"));
    server.sendContent(String(WiFi.RSSI())) ; 
    server.sendContent(F("</td><td>(dBm)</td></tr>"));

    server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String(ESP.getChipId(), HEX) + "</td><td align=center>"+String(ESP.getChipId())+"</td></tr>" ) ; 
    WiFi.macAddress(mac);      
    snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    server.sendContent("<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>" ) ; 
   
    server.sendContent(F("<tr><td>Solar Elevation Deg</td><td align=center>"));
    server.sendContent(String(solar_el_deg,3)) ; 
    server.sendContent(F("</td><td>(Deg)</td></tr>"));

    server.sendContent(F("<tr><td>Solar Azomuth</td><td align=center>"));
    server.sendContent(String(solar_az_deg,3)) ; 
    server.sendContent(F("</td><td>(Deg)</td></tr>"));

    server.sendContent(F("<tr><td>Sunrise</td><td align=center>"));
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(sunrise), MinSolarTime(sunrise));
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td><td>(hh:mm)</td></tr>"));

    server.sendContent(F("<tr><td>Sunset</td><td align=center>"));
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(sunset), MinSolarTime(sunset));        
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</td><td>(hh:mm)</td></tr>"));
    
    server.sendContent(F("<tr><td>Day or Night</td><td align=center>"));
    if ( iDayNight == 1 ){
      server.sendContent(F("DAY</td></tr>"));
    }else{
      server.sendContent(F("NIGHT</td></tr>"));          
    }

    server.sendContent(F("<tr><td>GPS lock age</td><td align=center>"));
    if ( fixage < 10000 ) {
      server.sendContent(String(fixage)) ; 
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

    if( hasRTC ){
      rtc_status = DS3231_get_sreg();
      if (( rtc_status & 0x80 ) != 0 ){
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='red'>DEPLETED</td><td></td></tr>")) ;            
      }else{
        server.sendContent(F("<tr><td>RTC Battery</td><td align=center bgcolor='green'>-- OK --</td><td></td></tr>")) ;                    
      }
      server.sendContent("<tr><td>RTC Temperature</td><td align=center>"+String(rtc_temp,1)+"</td><td>(C)</td></tr>") ;                    
    }
    
    
    server.sendContent(F("<tr><td>P/T temp</td><td align=center>")) ;
    server.sendContent(String(gT,1)) ; 
    server.sendContent(F("</td><td>(C)</td></tr>")) ;

    server.sendContent(F("<tr><td>Pressue</td><td align=center>"));
    server.sendContent(String(Pr)) ; 
    server.sendContent(F("</td><td>(mBar)</td></tr>")) ;
    
    server.sendContent(F("<tr><td>Gyro X</td><td align=center>")) ;
    server.sendContent(String(xRoll)) ; 
    server.sendContent(F("</td></tr>")) ;
    
    server.sendContent(F("<tr><td>Gyro Y</td><td align=center>")) ;
    server.sendContent(String(yRoll)) ; 
    server.sendContent(F("</td></tr>")) ;
    
    server.sendContent(F("<tr><td>Gyro Z</td><td align=center>")) ;
    server.sendContent(String(zRoll)) ; 
    server.sendContent(F("</td></tr></table><br>")) ;

    server.sendContent(F("<table border=1 title='Stracker Status'>")) ;
    server.sendContent(F("<tr><th>Parameter</th><th>E/W Value</th><th>.</th><th>N/S Value</th><th>.</th></tr>"));
    
    server.sendContent(F("<tr><td>Min Angle</td><td><form method=get action=/><input type='text' name='minay' value='")); 
    server.sendContent(String(yMinVal));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='minax' value='"));
    server.sendContent(String(xMinVal)); 
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td></tr>")); 
    
    server.sendContent(F("<tr><td>Max Angle</td><td><form method=get action=/><input type='text' name='maxay' value='"));
    server.sendContent(String(yMaxVal));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='maxax' value='"));
    server.sendContent(String(xMaxVal));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td></tr>")); 
    
    server.sendContent(F("<tr><td>Park Angle</td><td><form method=get action=/><input type='text' name='paray' value='")) ;
    server.sendContent(String(dyPark)) ;
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='parax' value='")) ;
    server.sendContent(String(dxPark));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    
    server.sendContent(F("<tr><td>Offest Angle</td><td><form method=get action=/><input type='text' name='offay' value='")) ;
    server.sendContent(String(yzOffset));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='offax' value='")) ;
    server.sendContent(String(xzOffset));
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    
    server.sendContent(F("<tr><td>Hysteris Angle</td><td><form method=get action=/><input type='text' name='hysay' value='")) ;
    server.sendContent(String(yzH)) ;
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='hysax' value='")) ;
    server.sendContent(String(xzH)) ;
    server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td></tr>")) ; 

    if ( iTrackMode == 3 ){
      server.sendContent(F("<tr><td>Target Angle</td><td><form method=get action=/><input type='text' name='taray' value='"));
      server.sendContent(String(yzTarget));
      server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td><td><form method=get action=/><input type='text' name='tarax' value='"));
      server.sendContent(String(xzTarget));
      server.sendContent(F("' size=12></td><td><input type='submit' value='SET'></form></td></tr>")) ; 
    }else{
      server.sendContent(F("<tr><td>Target Angle</td><td>"));
      server.sendContent(String(yzTarget));
      server.sendContent(F("</td><td>(Deg)</td><td>")) ;
      server.sendContent(String(xzTarget));
      server.sendContent(F("</td><td>(Deg)</td></tr>")) ;         
    }
    
    server.sendContent(F("<tr><td>Current Sensor Angle</td><td>")) ;
    server.sendContent(String(yzAng)) ;
    server.sendContent(F("</td><td>(Deg)</td><td>")) ;
    server.sendContent(String(xzAng));
    server.sendContent(F("</td><td>(Deg)</td></tr></table><br>")) ; 
  }
  
  server.sendContent(F("<br><br><a href='/?command=1'>Load Parameters from EEPROM</a><br><br><a href='/?command=667'>Reset Memory to Factory Default</a><br><a href='/?command=665'>Sync UTP Time</a><br><a href='/stime'>Manual Time Set</a><br><a href='/scan'>I2C Scan</a><br>")) ;
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
  server.sendContent("<br><a href='http://" + String(buff) + ":81/update'>OTA Firmware Update</a><br>");         
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
  server.sendContent(F("<head><title>Team Trouble - Solar Tracker</title>"));
  server.sendContent(F("<meta name=viewport content='width=320, auto inital-scale=1'>"));
  server.sendContent(F("</head><body><html><center><h2>Solar Tracker Mk3.1</h2>"));  
}


