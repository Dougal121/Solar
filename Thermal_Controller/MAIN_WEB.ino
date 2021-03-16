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

void SendHTTPHeader(){
String message ;
  server.sendHeader(F("Server"),F("ESP8266-on-ice"),false);
  server.sendHeader(F("X-Powered-by"),F("Dougal-1.0"),false);
  if (shams.bDoGetWeather){
    server.sendHeader(F("Refresh"),F("30"),false);  
  }
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  message = F("<!DOCTYPE HTML>") ;
  message += "<head><title>Team Trouble - Solar Water Heater Controller SCADA " + String(Toleo) + "</title>";
  message += F("<meta name=viewport content='width=320, auto inital-scale=1'>");
  message += F("</head><body><html lang='en'><center><h3>");   
  message += "<a title='click for home / refresh' href='/'>"+String(ghks.NodeName)+"</a></h3>";
  server.sendContent(message);
}



void SendHTTPPageFooter(){
String message ;
  message = F("<br><a href='/?command=1'>Load Parameters from EEPROM</a><br><br><a href='/?command=667'>Reset Memory to Factory Default</a><br><a href='/?command=665'>Sync UTP Time</a><br><a href='/?command=5'>Manual Get Weather</a><br><a href='/stime'>Manual Time Set</a><br><a href='/scan'>I2C Scan</a><br>") ;     
  message += "<a href='/?reboot=" + String(lRebootCode) + "'>Reboot</a><br>";
  message += F("<a href='/eeprom'>EEPROM Memory Contents</a><br>");
  message += F("<a href='/setup'>Node Setup</a><br>");
  message += F("<a href='/info'>Node Infomation</a><br>");
  message += F("<a href='/settings'>Solar Settings</a><br>");
  message += F("<a href='/email'>Email Settings</a><br>");
  message += F("<a href='/weather'>Open Weather Forcast</a><br>");
  if (!WiFi.isConnected() || (( MyIP[0]==0) && (MyIP[3]==0))) {
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);
  }else{
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
  }
  message += "<a href='http://" + String(buff) + ":81/update'>OTA Firmware Update</a><br>";  
  message += "<a href='https://github.com/Dougal121/Solar/tree/master/Thermal_Controller'>Source at GitHub</a><br>";  
  message += "<a href='http://" + String(buff) + "/backup'>Backup / Restore Settings</a><br><br>";  
  snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
  message += "Computer Uptime <b>"+String(buff)+"</b> (day:hr:min) <br>"  ;
  message += F("</body></html>\r\n");

  server.sendContent(message);
}


void handleNotFound(){
  String message = F("Seriously - No way DUDE\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
//  Serial.print(message);
}

 
void handleRoot() {
  boolean currentLineIsBlank = true;
  tmElements_t tm;
  long  i = 0 ;
  int ii  ;
  int j ;
  int k , kk , iTmp ;
  boolean bExtraValve = false ;
  boolean bDefault = true ;
  long lTmp ; 
  String MyCheck , MyColor , MyNum , MyCheck2 ;
  String pinname ;
  byte mac[6];
  String message ;

//  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
          LoadParamsFromEEPROM(true);
//          Serial.println("Load from EEPROM");
        break;
        case 2: // Save values
          LoadParamsFromEEPROM(false);
//          Serial.println("Save to EEPROM");
        break;
        case 3: // 
        break;
        case 4: // 
        break;
        case 5: // manual get weather
          shams.bDoGetWeather = true ;
        break;
        case 8: //  Cold Reboot
          ESP.reset() ;
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 42:
        break;
        case 667: // wipe the memory to factory default
          BackInTheBoxMemory();
        break;
        case 69: // wipe the memory to factory default
          BackInTheBoxMemory();
          LoadParamsFromEEPROM(false);
        break;
        case 665:
          sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server  once and hour  
        break;
        case 668:
        break;
      }  
    }

    i = String(server.argName(j)).indexOf("hr"  );
    if (i != -1) { //
      if ( String(server.arg(j)).toInt()  == 0 ) {
        for ( k = 0 ; k < 24 ; k++) { // handle all the valve control commands for any and all valves
          shas.bBoostTimes[k] = false ;
        }
      } else {
        for ( k = 24 ; k < 48 ; k++) { // handle all the valve control commands for any and all valves
          shas.bBoostTimes[k] = false ;
        }
      }
    }
    for ( k = 0 ; k < 48 ; k++) { // handle all the valve control commands for any and all valves
      MyNum = String(k) ;
      if ( k < 10 ) {
        MyNum = "0" + MyNum ;
      }
      i = String(server.argName(j)).indexOf("h" + MyNum  );
      if (i != -1) { //
        shas.bBoostTimes[k] = true ;
      }
    }
    
    i = String(server.argName(j)).indexOf("reboot");
    if (i != -1){  // 
      if (( lRebootCode == String(server.arg(j)).toInt() ) && (lRebootCode>0 )){  // stop the phone browser being a dick and retry resetting !!!!
        ESP.restart() ;        
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
     
    i = String(server.argName(j)).indexOf("ndadd");
    if (i != -1){  // 
      ghks.lNodeAddress = String(server.arg(j)).toInt() ;
      ghks.lNodeAddress = constrain(ghks.lNodeAddress,0,32768);
    }        
    i = String(server.argName(j)).indexOf("tzone");
    if (i != -1){  // 
      ghks.fTimeZone = String(server.arg(j)).toFloat() ;
      ghks.fTimeZone = constrain(ghks.fTimeZone,-12,12);
      bDoTimeUpdate = true ; // trigger and update to fix the time
    }        

    i = String(server.argName(j)).indexOf("pfrq");
    if (i != -1){  // 
      ghks.PingFreq = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("pmax");
    if (i != -1){  // 
      ghks.PingMax = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("serb");
    if (i != -1){  // 
      ghks.SelfReBoot = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("imod");
    if (i != -1){  // 
      shas.iMode = String(server.arg(j)).toInt() ;
      shas.iMode = constrain(shas.iMode,0,3);
    }
    i = String(server.argName(j)).indexOf("jmod");
    if (i != -1){  // 
      shas.iBoostMode  = String(server.arg(j)).toInt() ;
      shas.iBoostMode  = constrain(shas.iBoostMode,0,7);
    }
    
    i = String(server.argName(j)).indexOf("mylat");    //lat  
    if (i != -1){  // have a request to set the latitude
      ghks.latitude = String(server.arg(j)).toFloat() ;
      if (( ghks.latitude < -90) || ( ghks.latitude > 90 )){
        ghks.latitude = -34.051219 ;
      }
    }        
    i = String(server.argName(j)).indexOf("mylon");    // long
    if (i != -1){  // have a request to set the logitude
      ghks.longitude = String(server.arg(j)).toFloat() ;
      if (( ghks.longitude < -180) || ( ghks.longitude > 180 )){
        ghks.longitude = 142.013618 ;
      }
    }        

    i = String(server.argName(j)).indexOf("alut1");
    if (i != -1){   
      shas.fTankUnder1TempAlarm = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("alut2");
    if (i != -1){   
      shas.fTankUnder2TempAlarm = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("alota");
    if (i != -1){   
      shas.fTankOverTempAlarm = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("alrot");
    if (i != -1){   
      shas.fRoofOverTempAlarm  = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("alrut");
    if (i != -1){   
      shas.fRoofUnderTempAlarm = String(server.arg(j)).toFloat() ;
    }        

    
    i = String(server.argName(j)).indexOf("clmx");
    if (i != -1){  
      shas.fCollectorMaxTemp = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("clmn");
    if (i != -1){  
      shas.fCollectorMinTemp = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("sdmx");
    if (i != -1){  
      shas.fSolarTempDiffMax = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("sdmn");
    if (i != -1){  
      shas.fSolarTempDiffMin = String(server.arg(j)).toFloat() ;
    }   
    i = String(server.argName(j)).indexOf("tbtp");
    if (i != -1){  
      shas.fTopBoostTemp = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("bbtp");
    if (i != -1){  
      shas.fBottomBoostTemp = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("tdtp");
    if (i != -1){  
      shas.fTopBoostDiffTemp = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("bdtp");
    if (i != -1){  
      shas.fBottomBoostDiffTemp = String(server.arg(j)).toFloat() ;
    }        
    
    i = String(server.argName(j)).indexOf("owper");
    if (i != -1){  
      shas.iWeatherBoostMinCloud[0] = String(server.arg(j)).toInt() ;
    }        
    i = String(server.argName(j)).indexOf("owmwt");
    if (i != -1){  
      shas.fWeatherBoostMinWaterTemp[0] = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("owmat");
    if (i != -1){  
      shas.fWeatherBoostMinAirTemp[0] = String(server.arg(j)).toFloat() ;
    }        
    i = String(server.argName(j)).indexOf("owdlt");
    if (i != -1){  
      shas.fWeatherBoostTempDiffMin = String(server.arg(j)).toFloat() ;
    }        
    
         
    
    i = String(server.argName(j)).indexOf("disop");
    if (i != -1){  // 
      ghks.lDisplayOptions = String(server.arg(j)).toInt() ;
      ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,255);
    }  
    i = String(server.argName(j)).indexOf("netop");
    if (i != -1){  // 
      ghks.lNetworkOptions = String(server.arg(j)).toInt() ;
      ghks.lNetworkOptions = constrain(ghks.lNetworkOptions,0,255);
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
    i = String(server.argName(j)).indexOf("cname");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( ghks.NodeName , sizeof(ghks.NodeName)) ;
    }

    
    i = String(server.argName(j)).indexOf("owapi");
    if (i != -1){  
     String(server.arg(j)).toCharArray( ghks.apikey , sizeof(ghks.apikey)) ;
    }
    i = String(server.argName(j)).indexOf("owser");
    if (i != -1){  
     String(server.arg(j)).toCharArray( ghks.servername , sizeof(ghks.servername)) ;
    }
    
    

    
    i = String(server.argName(j)).indexOf("rpcip");
    if (i != -1){  // have a request to request an IP address
      ghks.RCIP[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.RCIP[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.RCIP[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.RCIP[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("staip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPStatic[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPStatic[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPStatic[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPStatic[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("gatip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPGateway[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPGateway[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPGateway[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPGateway[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("mskip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPMask[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPMask[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPMask[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPMask[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("dnsip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPDNS[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPDNS[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPDNS[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPDNS[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("ipping");
    if (i != -1){  // have a request to request an IP address
      ghks.IPPing[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPPing[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPPing[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPPing[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    
    i = String(server.argName(j)).indexOf("atoff");
    if (i != -1){  // have a request to request a time update
      tm.Year = (String(server.arg(j)).substring(0,4).toInt()-1970) ;
      tm.Month =(String(server.arg(j)).substring(5,7).toInt()) ;
      tm.Day = (String(server.arg(j)).substring(8,10).toInt()) ;
      tm.Hour =(String(server.arg(j)).substring(11,13).toInt()) ;
      tm.Minute = (String(server.arg(j)).substring(14,16).toInt()) ;
      tm.Second = 0 ;
      ghks.AutoOff_t = makeTime(tm);
    }  
    i = String(server.argName(j)).indexOf("nssid");
    if (i != -1){                                    // SSID
 //    Serial.println("SookyLala 1 ") ;
     String(server.arg(j)).toCharArray( ghks.nssid , sizeof(ghks.nssid)) ;
    }
    
    i = String(server.argName(j)).indexOf("npass");
    if (i != -1){                                    // Password
     String(server.arg(j)).toCharArray( ghks.npassword , sizeof(ghks.npassword)) ;
    }
    
    i = String(server.argName(j)).indexOf("cpass");
    if (i != -1){                                    // Password
     String(server.arg(j)).toCharArray( ghks.cpassword , sizeof(ghks.cpassword)) ;
    }
    
    i = String(server.argName(j)).indexOf("timsv");
    if (i != -1){                                    // timesvr
     String(server.arg(j)).toCharArray( ghks.timeServer , sizeof(ghks.timeServer)) ;
    }
    for (k = 0; k < MAX_RELAY ; k++) {
      i = String(server.argName(j)).indexOf("actv"+String(k));
      if (i != -1){                                    // activate relays
        if (shams.TTG[k] == 0 ){
          shams.TTG[k] = 5 ; // turn on for 5 minutes
        }else{
          shams.TTG[k] = 1 ; // turn off smhortly          
        }
      }        
      i = String(server.argName(j)).indexOf("rbrp"+String(k));  // GPIO pin for relay
      if (i != -1){                                   
        shas.relayPort[k] = String(server.arg(j)).toInt() ; // turn on for 5 minutes
        if ((shas.relayPort[k] <0 )|| (shas.relayPort[k]>16)){
          shas.relayPort[k] = 12 + k ;
        }
      }        
      i = String(server.argName(j)).indexOf("atrs"+String(k));  // GPIO pin for relay
      if (i != -1){                                   
        shas.ActiveValue[k] = String(server.arg(j)).toInt() ; // turn on for 5 minutes
        if ((shas.ActiveValue[k] != 0 ) && (shas.ActiveValue[k] != 1)){
          shas.ActiveValue[k] = 0 ;
        }
      }    
    }
    for (k = 0 ; k < MAX_EMAIL_ALARMS ; k++ ){
      i = String(server.argName(j)).indexOf("etst"+String(k));  // reset email flag
      if (i != -1){                                   
        shas.bEmails[k] = false ;
      }      
      i = String(server.argName(j)).indexOf("emal"+String(k));  // set email flag if the 'on' tag exists
      if (i != -1){                                   
        shas.bEmails[k] = true ;
      }    

      i = String(server.argName(j)).indexOf("atst"+String(k));  // reset email flag
      if (i != -1){                                   
        if ( shams.bAlarm[k] == false ) {
          shams.bAlarm[k] = true ;
        }else{
          shams.bAlarm[k] = false ;
        }
      }
    }
    for (k = 0 ; k < MAX_TEMP_SENSOR-1 ; k++ ){
      i = String(server.argName(j)).indexOf("snmp"+String(k));  // reset email flag
      if (i != -1){                                   
        shas.sensor[k] = String(server.arg(j)).toInt();
      }
    }
  }

  SendHTTPHeader();   //  ################### START OF THE RESPONSE  ######

  if ( bSaveReq != 0 ){
    server.sendContent(F("<blink>"));      
  }   
  server.sendContent(F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>")) ;     
  if ( bSaveReq != 0 ){
    server.sendContent(F("</blink><font color='red'><b>Changes Have been made to settings.<br>Make sure you save if you want to keep them</b><br></font><br>")) ;     
  }
    
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
  if (ghks.fTimeZone > 0 ) {
    server.sendContent("<b>"+ String(buff) + " UTC +" + String(ghks.fTimeZone,1) ) ;   
  }else{
    server.sendContent("<b>"+ String(buff) + " UTC " + String(ghks.fTimeZone,1) ) ;       
  }
  if ( year() < 2000 ) {
    server.sendContent(F("  --- CLOCK NOT SET ---")) ;
  }
  server.sendContent(F("</b><br>")) ;  
  if ( ghks.AutoOff_t > now() )  {
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(ghks.AutoOff_t), month(ghks.AutoOff_t), day(ghks.AutoOff_t) , hour(ghks.AutoOff_t), minute(ghks.AutoOff_t), second(ghks.AutoOff_t));
    server.sendContent(F("<b><font color=red>Automation OFFLINE Untill ")) ;  
    server.sendContent(String(buff)) ; 
    server.sendContent(F("</font></b><br>")) ; 
  }else{
    if ( year() > 2000 ) {
      server.sendContent(F("<b><font color=green>Automation ONLINE</font></b><br>")) ;  
    }else{
      server.sendContent(F("<b><font color=green>Automation OFFLINE Invalid time</font></b><br>")) ;        
    }
  }

  if (String(server.uri()).indexOf("stime")>0) {  // ################   SETUP TIME    #######################################
    bDefault = false ;
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(), month(), day() , hour(), minute());
    server.sendContent("<br><br><form method=post action=" + server.uri() + "><br>Set Current Time: <input type='text' name='stime' value='"+ String(buff) + "' size=12>");
    server.sendContent(F("<input type='submit' value='SET'><br><br></form>"));
  }

    
  if (String(server.uri()).indexOf("setup")>0) {  // ################  SETUP OF THE NODE #####################################
    bDefault = false ;
    server.sendContent("<form method=post action=" + server.uri() + "><table border=1 title='Node Settings'>");
    server.sendContent(F("<tr><th>Parameter</th><th>Value</th><th><input type='submit' value='SET'></th></tr>"));
  
    server.sendContent(F("<tr><td>Controler Name</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='cname' value='"+String(ghks.NodeName)+"' maxlength=15 size=12></td><td></td></tr>");
  
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(ghks.AutoOff_t), month(ghks.AutoOff_t), day(ghks.AutoOff_t) , hour(ghks.AutoOff_t), minute(ghks.AutoOff_t));
    if (ghks.AutoOff_t > now()){
      MyColor =  F("bgcolor=red") ;
    }else{
      MyColor =  "" ;
    }
    server.sendContent("<tr><td "+String(MyColor)+">Auto Off Until</td><td align=center>") ; 
    server.sendContent("<input type='text' name='atoff' value='"+ String(buff) + "' size=12></td><td>(yyyy/mm/dd)</td></tr>");
  
    server.sendContent(F("<tr><td>Node Address</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='ndadd' value='" + String(ghks.lNodeAddress) + "' size=12></td><td>"+String(ghks.lNodeAddress & 0xff)+"</td></tr>");
  
    server.sendContent(F("<tr><td>Time Zone</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='tzone' value='" + String(ghks.fTimeZone,1) + "' size=12></td><td>(Hours)</td></tr>");

    server.sendContent(F("<tr><td>Display Options</td><td align=center>")) ; 
    server.sendContent(F("<select name='disop'>")) ;
    if (ghks.lDisplayOptions == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 - Normal")); 
      server.sendContent(F("<option value='1'>1 - Invert")); 
    }else{
      server.sendContent(F("<option value='0'>0 - Normal")); 
      server.sendContent(F("<option value='1' SELECTED>1 - Invert")); 
    }
    server.sendContent(F("</select></td><td></td></tr>"));

    server.sendContent(F("</form>"));

    server.sendContent("<form method=post action=" + server.uri() + "><tr><td></td><td></td><td></td></tr>") ; 
  
    server.sendContent(F("<tr><td>Local UDP Port NTP</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='lpntp' value='" + String(ghks.localPort) + "' size=12></td><td><input type='submit' value='SET'></td></tr>");
  
    server.sendContent(F("<tr><td>Local UDP Port Control</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='lpctr' value='" + String(ghks.localPortCtrl) + "' size=12></td><td></td></tr>");
  
    server.sendContent(F("<tr><td>Remote UDP Port Control</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='rpctr' value='" + String(ghks.RemotePortCtrl) + "' size=12></td><td></td></tr>");

    server.sendContent(F("<tr><td>Network SSID</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='nssid' value='" + String(ghks.nssid) + "' maxlength=15 size=12></td><td></td></tr>");

    server.sendContent(F("<tr><td>Network Password</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='npass' value='" + String(ghks.npassword) + "' maxlength=15 size=12></td><td></td></tr>");

    server.sendContent(F("<tr><td>Configure Password</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='cpass' value='" + String(ghks.cpassword) + "' maxlength=15 size=12></td><td></td></tr>");

    server.sendContent(F("<tr><td>Time Server</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='timsv' value='" + String(ghks.timeServer) + "' maxlength=23 size=12></td><td></td></tr>");
 
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.RCIP[0],ghks.RCIP[1],ghks.RCIP[2],ghks.RCIP[3]);
    server.sendContent(F("<tr><td>Remote IP Address Control</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='rpcip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr></form>");
    
    server.sendContent("<form method=post action=" + server.uri() + "><tr><td></td><td></td><td></td></tr>") ; 

    server.sendContent(F("<tr><td>Network Options</td><td align=center>")) ; 
    server.sendContent(F("<select name='netop'>")) ;
    if (ghks.lNetworkOptions == 0 ){
      server.sendContent(F("<option value='0' SELECTED>0 - DHCP")); 
      server.sendContent(F("<option value='1'>1 - Static")); 
    }else{
      server.sendContent(F("<option value='0'>0 - DHCP")); 
      server.sendContent(F("<option value='1' SELECTED>1 - Static IP")); 
    }
    server.sendContent(F("</select></td><td><input type='submit' value='SET'></td></tr>"));
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPStatic[0],ghks.IPStatic[1],ghks.IPStatic[2],ghks.IPStatic[3]);
    server.sendContent(F("<tr><td>Static IP Address</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='staip' value='" + String(buff) + "' maxlength=20 size=12></td><td></td></tr>");

    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPGateway[0],ghks.IPGateway[1],ghks.IPGateway[2],ghks.IPGateway[3]);
    server.sendContent(F("<tr><td>Gateway IP Address</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='gatip' value='" + String(buff) + "' maxlength=20 size=12></td><td></td></tr>");
  
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPMask[0],ghks.IPMask[1],ghks.IPMask[2],ghks.IPMask[3]);
    server.sendContent(F("<tr><td>IP Mask</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='mskip' value='" + String(buff) + "' maxlength=20 size=12></td><td></td></tr>");

    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPDNS[0],ghks.IPDNS[1],ghks.IPDNS[2],ghks.IPDNS[3]);
    server.sendContent(F("<tr><td>DNS IP Address</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='dnsip' value='" + String(buff) + "' maxlength=20 size=12></td><td></td></tr>");

    server.sendContent("<tr><td>Last Scan Speed</td><td align=center>" + String(lScanLast) + "</td><td>(per second)</td></tr>" ) ;    
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

  if (String(server.uri()).indexOf("settings")>0) {  // ################   SETTINGS    #######################################
    bDefault = false ;
    message = F("<table border=1 title='Solar Water Heater Main SCADA'>") ;
    message += F("<tr><th>Relay Operating Parameters</th><th>Value</th><th>Units</th><th>.</th></tr>") ;          
    message += "<tr><form method=post action=" + server.uri() + "><td>Max Collector Temp (0-Pump)</td><td align=center><input type='text' name='clmx' value='"+String(shas.fCollectorMaxTemp)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Min Collector Temp (0-Pump)</td><td align=center><input type='text' name='clmn' value='"+String(shas.fCollectorMinTemp)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Solar Temp Diff Max (0-Pump)</td><td align=center><input type='text' name='sdmx' value='"+String(shas.fSolarTempDiffMax)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Solar Temp Diff Min (0-Pump)</td><td align=center><input type='text' name='sdmn' value='"+String(shas.fSolarTempDiffMin)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Top Boost Temp (1-Boost)</td><td align=center><input type='text' name='tbtp' value='"+String(shas.fTopBoostTemp)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Top Boost Diff Temp (1-Boost)</td><td align=center><input type='text' name='tdtp' value='"+String(shas.fTopBoostDiffTemp)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Bottom Boost Temp (2-Boost)</td><td align=center><input type='text' name='bbtp' value='"+String(shas.fBottomBoostTemp)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Bottom Boost Diff Temp (2-Boost)</td><td align=center><input type='text' name='bdtp' value='"+String(shas.fBottomBoostDiffTemp)+"' size=30></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    server.sendContent(message) ;
    message = "" ;
    
    message += "<tr><form method=post action=" + server.uri() + "><td>Pump Operating Mode</td><td align=center><select name='imod'>" ;
    for ( i = 0 ; i < MAX_MODES ; i++ ){
        if (shas.iMode == i ){
          MyCheck = F(" SELECTED ");
        }else{
          MyCheck = "";            
        }
        switch(i){
          case 0: pinname = F("Temperature Difference (T1-T3)") ; break;
          case 1: pinname = F("Time of Day - Sunrise To Sunset") ; break;
        }
        message += "<option value="+String(i)+ MyCheck +">" + pinname ;                
    }
    message += "</select></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;

    message += "<tr><form method=post action=" + server.uri() + "><td>Boost Operating Mode</td><td align=center><select name='jmod'>" ;
    for ( i = 0 ; i < MAX_BMODES ; i++ ){
        if (shas.iMode == i ){
          MyCheck = F(" SELECTED ");
        }else{
          MyCheck = "";            
        }
        switch(i){
          case 0: pinname = F("Temperature Only") ; break;
          case 1: pinname = F("Time of Day") ; break;
          case 2: pinname = F("Time of Day + Air Temperature") ; break;
          case 3: pinname = F("Cloud") ; break;
          case 4: pinname = F("Cloud + Temperature") ; break;
        }
        message += "<option value="+String(i)+ MyCheck +">" + pinname ;                
    }
    message += "</select></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    
    message += "<tr><td colspan=4>.</td></tr>" ;
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPPing[0],ghks.IPPing[1],ghks.IPPing[2],ghks.IPPing[3]);
    message += "<tr><form method=post action=" + server.uri() + "><td>Ping Address</td><td align=center>" ; 
    message += "<input type='text' name='ipping' value='" + String(buff) + "' maxlength=16 size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>";
    message += "<tr><form method=post action=" + server.uri() + "><td>MaxPing </td><td align=center><input type='text' name='pmax' value='"+String(ghks.PingMax)+"' size=30></td><td>(ms)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Ping Period </td><td align=center><input type='text' name='pfrq' value='"+String(ghks.PingFreq)+"' size=30></td><td>(min)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    
    message += "<tr><form method=post action=" + server.uri() + "><td>Self Reboot Period </td><td align=center><input type='text' name='serb' value='"+String(ghks.SelfReBoot)+"' size=30></td><td>(min)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    server.sendContent(message) ;
    message = "" ;
    
    message += "<tr><td colspan=4>.</td></tr>" ;
    message += "<form method=post action=" + server.uri() + "><tr><td>Latitude +N -S</td><td align=center><input type='text' name='mylat' value='" ; 
    message += String(ghks.latitude,8);
    message += "' size=30></td><td>.</td><td><input type='submit' value='SET'></td></tr></form>" ; 

    message += "<form method=post action=" + server.uri() + "><tr><td>Longitude +E -W</td><td align=center><input type='text' name='mylon' value='" ; 
    message += String(ghks.longitude,8);
    message += "' size=30></td><td>.</td><td><input type='submit' value='SET'></td></tr></form>" ; 
    message += "<tr><td colspan=4>.</td></tr>" ;
    server.sendContent(message) ;
    message = "";

    message += "<tr><form method=post action=" + server.uri() + "><td>Open Weather API KEY</td><td align=center><input type='text' name='owapi' value='"+String(ghks.apikey)+"' size=30 maxlength=40></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Open Weather Site or IP</td><td align=center><input type='text' name='owser' value='"+String(ghks.servername)+"' size=30 maxlength=32></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;

    message += "<tr><form method=post action=" + server.uri() + "><td>Max Predicted Cloud Boost Percetage</td><td align=center><input type='text' name='owper' value='"+String(shas.iWeatherBoostMinCloud[0])+"' size=30 maxlength=32></td><td>(%)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Min Water Temp Cloud Boost</td><td align=center><input type='text' name='owmwt' value='"+String(shas.fWeatherBoostMinWaterTemp[0])+"' size=30 maxlength=32></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Min Predicted Air Boost Temp</td><td align=center><input type='text' name='owmat' value='"+String(shas.fWeatherBoostMinAirTemp[0])+"' size=30 maxlength=32></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>Predicted Boost Temps delta T</td><td align=center><input type='text' name='owdlt' value='"+String(shas.fWeatherBoostTempDiffMin)+"' size=30 maxlength=32></td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
   
     
    message += F("</table>");
    server.sendContent(message) ;    

    message = "<br><b>Alarms</b>" ;
    message += F("<table border=1 title='Email Alarms'><tr><th>Alarm</th><th>Value</th><th>Email</th><th>.</th></tr>") ;
    for (i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ){
      if ( shams.bAlarm[i] ){
        MyColor = "bgcolor='Yellow'" ;
      }else{
        MyColor = "" ;        //bgcolor='red'
      }
      message += "<tr><form method=post action=" + server.uri() + "><td " + MyColor + ">" + GetAlarmString(i) + "</td><td>"+GetAlarmValue(i)+"</td>";
      if ( ( shas.bEmails[i] ) != 0 ){
        MyCheck = F("CHECKED")  ;    
      }else{
        MyCheck = F("")  ;    
      }
      message += "<td align=center><input type='hidden' name='etst"+String(i) +"' value='true'><input type='checkbox' name='emal" + String(i) + "' " + String(MyCheck)+ "></td>" ;
      message += "</td><td><input type='submit' value='SET'></td></form></tr>" ;  
    }
    message += F("</table>");
    server.sendContent(message);
    
    message = "<br>Virtual Output to Relay Setup" ;
    message += F("<table border=1 title='Relay Setup'>") ;
    message += F("<tr><th>Output Relay</th><th>Function</th><th>CPU Pin</th><th>Active State</th><th>.</th></tr>") ;
    for (i = 0; i < MAX_RELAY ; i++) {
      if ( shams.bRelayState[i] ==  (bool)shas.ActiveValue[i] ){
        MyColor = "bgcolor='Yellow'" ;
      }else{
        MyColor = "" ;         //bgcolor='red'
      }
      MyCheck = RelayDescription(i) ;      
      message +="<tr><form method=get action=" + server.uri() + "><td align=center " + MyColor + ">" + String(i+1) + "</td><td>"+MyCheck+"</td><td align=center><select name='rbrp"+String(i)+"'>";
      for (k = -1; k < 17; k++) {
        if (shas.relayPort[i] == k ){
          MyCheck = F(" SELECTED ");
        }else{
          MyCheck = "";            
        }
        switch(k){
          case -1: pinname = F("--- UNUSED ---") ; break;
          case 0: pinname = F("GPIO 0 - D3") ; break;
          case 1: pinname = F("GPIO 1 - D1 TXD0") ; break;
          case 2: pinname = F("GPIO 2 - D9 BUILTIN LED") ; break;
          case 3: pinname = F("GPIO 3 - D0 - RXD0") ; break;
          case 4: pinname = F("GPIO 4 - I2C SDA - Avoid") ; break;
          case 5: pinname = F("GPIO 5 - I2C SCL - Avoid") ; break;
          case 6: pinname = F("GPIO 6 - SDCLK - NA Dont Use") ; break;
          case 7: pinname = F("GPIO 7 - SDD0 - NA Dont Use") ; break;
          case 8: pinname = F("GPIO 8 - SDD1 - NA Dont Use") ; break;
          case 9: pinname = F("GPIO 9 - SDD2 - NA ? ") ; break;
          case 10: pinname = F("GPIO 10 - SDD3 - NA ? ") ; break;
          case 11: pinname = F("GPIO 11 - SDCMD - NA Dont Use") ; break;
          case 12: pinname = F("GPIO 12 - D12 - ") ; break;
          case 13: pinname = F("GPIO 13 - D11 - RXD2") ; break;
          case 14: pinname = F("GPIO 14 - D13") ; break;
          case 15: pinname = F("GPIO 15 - D10 -  TXD2") ; break;
          case 16: pinname = F("GPIO 16 - D2 -  Wake") ; break;
        }
        message += "<option value="+String(k)+ MyCheck +">" + pinname ;          
      }
      message += "</select></td>" ;
      
      message += "<td align=center><select name='atrs"+String(i)+"'>" ; 
      if ( shas.ActiveValue[i] == 0 ){
        message += F("<option value='0' SELECTED>0 LOW"); 
        message += F("<option value='1'>1 HGH"); 
      }else{
        message += F("<option value='0'>0 LOW"); 
        message += F("<option value='1' SELECTED>1 HIGH"); 
      }
      message += F("</select></td><td><input type='submit' value='SET'></td></form></tr>");
    }
    message += F("</table>");
    server.sendContent(message) ;    

    message = "<br>Temperature Sender Setup" ;
    message += F("<table border=1 title='Temperature Sender Setup'>") ;
    message += "<tr><th>Temp Sensor No</th><th>Address</th><th>Mapped to</th><th>Description</th><th>Current Temp</th><th>(C)</th>.<th></th></tr>" ;
    for ( i = 0 ; i < (MAX_TEMP_SENSOR-1) ; i++ ){
      snprintf(buff, BUFF_MAX, "%02X%02X%02X%02X%02X%02X%02X%02X\0",Thermometer[shas.sensor[i]][0],Thermometer[shas.sensor[i]][1],Thermometer[shas.sensor[i]][2],Thermometer[shas.sensor[i]][3],Thermometer[shas.sensor[i]][4],Thermometer[shas.sensor[i]][5],Thermometer[shas.sensor[i]][6],Thermometer[shas.sensor[i]][7]  );         
      switch (i){
        case 0: MyCheck =  "<b>T2</b> Tank Top " ;  break; 
        case 1: MyCheck =  "<b>T3</b> Tank Bottom"  ;    break; 
        case 2: MyCheck =  "<b>T4</b> Air Temp"  ;    break; 
        case 3: MyCheck =  "<b>T5</b> Spare Temp"  ;    break; 
      }
      message += "<tr><form method=get action=" + server.uri() + "><td align=center>"+String(i)+"</td><td>"+String(buff)+"</td><td align=center><select name='snmp"+String(i)+"'>" ;
      for ( k = 0 ; k < (MAX_TEMP_SENSOR-1) ; k++ ){
        message += "<option value='"+String(k)+"'";
        if (shas.sensor[i] == k ) {
          message += " SELECTED " ;
        }
        message += ">"+String(k) ; 
      }
      message += "</select></td><td>"+MyCheck+"</td><td align=center>"+String(shams.fTemp[i],1)+"</td><td>(C)</td><td><input type='submit' value='SET'></td></form></tr>" ;
    }    
    message += "<tr><td align=center>4</td><td>--- AO ---</td><td align=center>X</td><td><b>T1</b> Roof Temp</td><td align=center>"+String(shams.fTemp[4])+"</td><td>(C)</td></tr>" ;

    message += F("</table><br>");
    server.sendContent(message) ;    

    message = F("<br>Booster Time Control<br><table border=1 title='Booster Time Control'>");
    message += F("<tr><th></th><th colspan=12>Boost Hours</th><th>.</th></tr>");
    server.sendContent(message) ; // End of Table Header

    for ( j = 0 ; j < 2 ; j++ ) {
      message = "<form method=post action='" + server.uri() + "'>" ;
      message += "<tr><th><input type='hidden' name='hr' value='" + String(j) + "'>.</th>";
      for ( i = 0 ; i < 12 ; i++ ) {
        k = i + (j * 12 ) ;
        if ( k < 10 ) {
          message += "<th>0" + String(k) + "</th>" ;
        } else {
          message += "<th>" + String(k) + "</th>" ;
        }
      }
      message += F("<th><input type='submit' value='SET'>.</th></tr>");
      server.sendContent(message) ; // End of Table Header
      message = F("<tr><td>00</td>") ;
      for ( i = (j * 24) ; i < (24 + (j * 24)) ; i += 2 ) {
        MyNum = String(i) ;
        if ( i < 10 ) {
          MyNum = "0" + MyNum ;
        }
        if ( shas.bBoostTimes[i] ) {
          MyCheck = F("CHECKED") ;
          MyColor =  F("bgcolor=green") ;  // check if no start times
        } else {
          MyCheck = F("") ;
          MyColor = F("") ;
        }
        message += "<td " + String(MyColor) + "><input type='checkbox' name='h" + MyNum + "' " + String(MyCheck) + "></td>" ;
      }
      message += F("</td><td></tr>");
      server.sendContent(message) ;

      message = F("<tr><td>30</td>");
      for ( i = 1 + (j * 24) ; i < ( 24 + (j * 24)) ; i += 2 ) {
        MyNum = String(i) ;
        if ( i < 10 ) {
          MyNum = "0" + MyNum ;
        }
        if ( shas.bBoostTimes[i] ) {
          MyCheck = F("CHECKED") ;
          MyColor =  F("bgcolor=green") ;  // check if no start times
        } else {
          MyCheck = F("") ;
          MyColor = F("") ;
        }
        message += "<td " + String(MyColor) + "><input type='checkbox' name='h" + MyNum + "' " + String(MyCheck) + "></td>" ;
      }
      message += F("</td><td></tr></form>");
      server.sendContent(message) ; // End of Table Header
    }
    server.sendContent(F("</table><br>"));


  }
  
  
  if (bDefault) {     // #####################################   default control   ##############################################
    server.sendContent(F("<br><b>Solar Water Heater Main SCADA</b>"));
/*
    sensors.requestTemperatures();  
    for ( i = 0 ; i < 4 ; i++ ){
      shams.fTemp[i] = sensors.getTempC(Thermometer[shas.sensor[i]]) ;
    }
*/    
    message = F("</table><br><table border=1 title='Temperatures'>");
    message += F("<tr><th><b>Location</th><th align=center><b>Address</b></th><th align=center><b>Current Value</b></th><th>Units</th></tr>") ;
    message += "<tr><td><b>T1</b> Roof Temp</td><td align=center> - Analog A0 - </td><td align=center>" + String(shams.fTemp[4],1) + "</td><td align=center>(C)</td></tr>" ;
    
    for ( i = 0 ; i < 4 ; i++ ){
      MyColor = "" ; 
      snprintf(buff, BUFF_MAX, "%02X%02X%02X%02X%02X%02X%02X%02X\0",Thermometer[shas.sensor[i]][0],Thermometer[shas.sensor[i]][1],Thermometer[shas.sensor[i]][2],Thermometer[shas.sensor[i]][3],Thermometer[shas.sensor[i]][4],Thermometer[shas.sensor[i]][5],Thermometer[shas.sensor[i]][6],Thermometer[shas.sensor[i]][7]  );   
      switch (i){
        case 0: MyCheck =  "<b>T2 Tank Top </b>"  ; MyColor = "<b>" ;  break; 
        case 1: MyCheck =  "<b>T3</b> Tank Bottom"  ;    break; 
        case 2: MyCheck =  "<b>T4</b> Air Temp"  ;    break; 
        case 3: MyCheck =  "<b>T5</b> Spare Temp"  ;    break; 
      }
      message += "<tr><td>" + MyCheck + "</td><td align=center>" + String(buff) + "</td><td align=center>" + MyColor + String(shams.fTemp[i],1) + "</td><td align=center>(C)</td></tr>"  ;
    }    
            
    message += F("<tr><td>Sunrise - State - Sunset</td><td colspan=2 align=center>");
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(SolarApp.sunrise), MinSolarTime(SolarApp.sunrise));
    message += String(buff) ; 
    if ( SolarApp.iDayNight == 1 ){
      message += F(" - DAY - ");
    }else{
      message += F(" - NIGHT - ");          
    }
    snprintf(buff, BUFF_MAX, "%02d:%02d", HrsSolarTime(SolarApp.sunset), MinSolarTime(SolarApp.sunset));        
    message += String(buff) ; 
    message += F("</td><td>(hh:mm)</td></tr>");
    message += F("</table><br><b>Outputs</b>");
    server.sendContent(message) ;
    
    message = F("<table><br><table border=1 title='Relay Outputs'>");
    message += "<tr><th>Relay No</th><th>Function</th><th>GPIO Pin</th><th>Manual</th><th>Timer(s)</th></tr>"  ;
    for ( i = 0 ; i < MAX_RELAY ; i++ ){
      if ( shams.bRelayState[i] ==  (bool)shas.ActiveValue[i] ){
        MyColor = "bgcolor='Yellow'" ;
      }else{
        MyColor = "" ;                     // bgcolor='red'
      }
      MyCheck = RelayDescription(i) ;
      message += "<tr><form method=get action=" + server.uri() + "><td align=center "+String(MyColor)+">"+String(i)+"</td><td "+String(MyColor)+">" + String(MyCheck) + "</td><td align=center>"+String(shas.relayPort[i])+"</td><td><input type='submit' value='Activate'><input type='hidden' name='actv"+String(i)+"' value='true'></td><td align=center>"+String(shams.TTG[i])+"</td></form></tr>" ;
    }
    message += F("</table>");
    server.sendContent(message) ;

    message = F("<br><b>Alarms</b><br><table border=1 title='Alarms'>");
    message += "<tr><th>Alarm</th><th>Email</th><th>Test</th></tr>" ;
    for ( i = 0 ; i < MAX_EMAIL_ALARMS ; i++ ){
      if ( shams.bAlarm[i]  ) {
        MyColor = "bgcolor='Yellow'" ;
        MyCheck = "CLEAR ALARM" ;
      }else{
        MyColor = "" ;         // bgcolor='red'
        MyCheck = "TEST ALARM" ;
      }
      if ( shas.bEmails[i] == true ){
        MyCheck2 = "- YES -" ;
        MyNum = "bgcolor='ForestGreen'" ;
      }else{
        MyCheck2 = "- NO -" ;        
        MyNum = "bgcolor='FireBrick'" ;
      }
      message += "<tr><form method=get action=" + server.uri() + "><input type='hidden' name='atst"+String(i) +"' value='true'><td "+MyColor+">"+GetAlarmString(i)+"</td><td "+MyNum+">" + MyCheck2+ "</td><td><input type='submit' value='"+MyCheck+"'></td></form></tr>" ;
    }
    message += F("</table>");
    server.sendContent(message) ; 
  }
  SendHTTPPageFooter();
}

String GetAlarmString(int iAlarmNo){
      switch (iAlarmNo){
        case 0: return("Water Luke Warm (T2)")  ;  break; 
        case 1: return("Water Cold (T2)") ;    break; 
        case 2: return("Water Over Temp (T2)")  ;    break; 
        case 3: return("Roof Collector Over Temp (T1)")  ;    break; 
        case 4: return("Roof Collector Under Temp (T1)")  ;    break; 
        case 5: return("Boost Element 1 Energised ")  ;    break; 
        case 6: return("Boost Element 2 Energised ")  ;    break; 
        case 7: return("Controler Rebooted ")  ;    break; 
        case 8: return("Sensor Failed ")  ;    break; 
        case 9: return("Spare ")  ;    break; 
      }  
}

String GetAlarmValue(int iAlarmNo){
      switch (iAlarmNo){
        case 0: return("<input type='text' name='alut1' value='"+String(shas.fTankUnder1TempAlarm,1)+"' size=10>")  ;  break; 
        case 1: return("<input type='text' name='alut2' value='"+String(shas.fTankUnder2TempAlarm,1)+"' size=10>") ;    break; 
        case 2: return("<input type='text' name='alota' value='"+String(shas.fTankOverTempAlarm,1)+"' size=10>")  ;    break; 
        case 3: return("<input type='text' name='alrot' value='"+String(shas.fRoofOverTempAlarm,1)+"' size=10>")  ;    break; 
        case 4: return("<input type='text' name='alrut' value='"+String(shas.fRoofUnderTempAlarm,1)+"' size=10>")  ;    break; 
        case 5: return(String(shas.fTopBoostTemp,1))  ;    break; 
        case 6: return(String(shas.fBottomBoostTemp,1))  ;    break; 
        case 7: return("--- NA ---")  ;    break; 
        case 8: return("--- NA ---")  ;    break; 
        case 9: return("--- NA ---")  ;    break; 
      }  
}


String RelayDescription(int i){
    switch (i){
      case 0: return("Pump Relay (T1-T3)")  ;  break; 
      case 1: return("Boost Element 1 (T2)")  ;    break; 
      case 2: return("Boost Element 2 (T3)")  ;    break; 
      case 3: return("Pump (T1-T3) Only")  ;    break; 
      case 4: return("Boost Weather Only")  ;    break; 
      case 5: return("Multi Boost ")  ;    break; 
      case 6: return("Spare ")  ;    break; 
      case 7: return("Spare ")  ;    break; 
    }
  
}


