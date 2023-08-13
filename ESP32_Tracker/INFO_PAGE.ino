void handleInfo(){
  byte mac[6];

/*
  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
  }
*/
  
  SendHTTPHeader();
  ghks.MyIP =  WiFi.localIP() ;
  server.sendContent(F("<br><center><b>Node Info</b><br>"));
  server.sendContent(F("<table border=1 title='Device Info'>"));
  server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String((long)chipid, HEX) + "</td><td align=center>"+String((long)chipid)+"</td></tr>" ) ; 
  ghks.MyIP =  WiFi.localIP() ;
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.MyIP[0],ghks.MyIP[1],ghks.MyIP[2],ghks.MyIP[3]);
  server.sendContent("<tr><td>Network Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
  ghks.MyIPC = WiFi.softAPIP();  // get back the address to verify what happened  
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);
  server.sendContent("<tr><td>Config Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Time Server</td><td align=center>" + String(ghks.timeServer) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Network SSID</td><td align=center>" + String(ghks.nssid) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Network Password</td><td align=center>" + String(ghks.npassword) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Configure SSID</td><td align=center>" + String(ghks.cssid) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Configure Password</td><td align=center>" + String(ghks.cpassword) + "</td><td>.</td></tr>" ) ; 
  
  server.sendContent("<tr><td>WiFi RSSI</td><td align=center>" + String(WiFi.RSSI()) + "</td><td>(dBm)</td></tr>" ) ; 
  WiFi.macAddress(mac);      
  snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  server.sendContent("<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>" ) ; 
 
  server.sendContent("<tr><td>Last Scan Speed</td><td align=center>" + String(lScanLast) + "</td><td>(per second)</td></tr>" ) ;    

  server.sendContent("<tr><td>ESP Chp Revision</td><td align=center>" + String(ESP.getChipRevision()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>SDK Version</td><td align=center>" + String(ESP.getSdkVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>CPU Frequecy</td><td align=center>" + String(ESP.getCpuFreqMHz()) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>XTAL Frequecy</td><td align=center>" + String(getXtalFrequencyMhz()) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>APB Frequecy</td><td align=center>" + String(getApbFrequency()/1000000) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>Magnetic Sensor Value</td><td align=center>" + String(magval) + "</td><td>(?)</td></tr>" ) ;    
  snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
  server.sendContent("<tr><td>Computer Uptime</td><td align=center>"+String(buff)+"</td><td>(day:hr:min)</td></tr>" ) ;

  server.sendContent("<tr><td>ghks Structure Size</td><td align=center>"+String(sizeof(ghks))+"</td><td>(bytes)</td></tr>" ) ;
  server.sendContent("<tr><td>tv Structure Size</td><td align=center>"+String(sizeof(tv))+"</td><td>(bytes)</td></tr>" ) ;
  server.sendContent("<tr><td>SMTP Structure Size</td><td align=center>"+String(sizeof(SMTP))+"</td><td>(bytes)</td></tr>" ) ;
  server.sendContent("<tr><td>adcs Structure Size</td><td align=center>"+String(sizeof(adcs))+"</td><td>(bytes)</td></tr>" ) ;

  server.sendContent(F("</table><br>"));    
  SendHTTPPageFooter();
}



void handleSetup() {
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
  String strOption ;
  String strSelected ;
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
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 667: // wipe the memory to factory default
          BackInTheBoxMemory();
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
    
    i = String(server.argName(j)).indexOf("netop");
    if (i != -1){  // 
      ghks.lNetworkOptions = String(server.arg(j)).toInt() ;
      ghks.lNetworkOptions = constrain(ghks.lNetworkOptions,0,255);
    }
/*    i = String(server.argName(j)).indexOf("naddr");   // ip address setup
    if (i != -1){ 
      ghks.MyIP[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.MyIP[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.MyIP[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.MyIP[3] =String(server.arg(j)).substring(12,15).toInt() ;
    } */              
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

    i = String(server.argName(j)).indexOf("tzone");
    if (i != -1){  // 
      ghks.fTimeZone = String(server.arg(j)).toFloat() ;
      ghks.fTimeZone = constrain(ghks.fTimeZone,-12,12);
      bDoTimeUpdate = true ;                                       // trigger and update to fix the time
    }

    i = String(server.argName(j)).indexOf("tname");
    if (i != -1){  // have a request to Update Node name
     String(server.arg(j)).toCharArray( ghks.NodeName , sizeof(ghks.NodeName)) ;
    }
        
    i = String(server.argName(j)).indexOf("timsv");
    if (i != -1){                                                  // timesvr
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
    
    i = String(server.argName(j)).indexOf("disop");
    if (i != -1){  // 
      ghks.lDisplayOptions = String(server.arg(j)).toInt() ;
      ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,255);
    }  
    i = String(server.argName(j)).indexOf("disro");
    if (i != -1){  // 
      ii =(( 0x01 & String(server.arg(j)).toInt()) << 1 ) ;
      ghks.lDisplayOptions = ( ghks.lDisplayOptions & 0xFD ) | ii ;
      ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,255);
    }  
    i = String(server.argName(j)).indexOf("cpufr");
    if (i != -1){  // 
      ghks.cpufreq = String(server.arg(j)).toInt() ;
    }  
    i = String(server.argName(j)).indexOf("magsn");
    if (i != -1){  // 
      ghks.magsens = String(server.arg(j)).toInt() ;
    }  

    i = String(server.argName(j)).indexOf("netop");
    if (i != -1){  // 
      ghks.lNetworkOptions = String(server.arg(j)).toInt() ;
      ghks.lNetworkOptions = constrain(ghks.lNetworkOptions,0,255);
    }    
    i = String(server.argName(j)).indexOf("dotmr");
    if (i != -1){  // 
      ghks.displaytimer = String(server.arg(j)).toInt() ;
      ghks.displaytimer = constrain(ghks.displaytimer,0,255);
    }      

    i = String(server.argName(j)).indexOf("srbt");
    if (i != -1){  // 
      ghks.SelfReBoot =  String(server.arg(j)).toInt()  ;
      if ((ghks.SelfReBoot < MIN_REBOOT )){
        if (ghks.SelfReBoot <= 0 ) {
          ghks.SelfReBoot = 0 ;                  
        }else{
          ghks.SelfReBoot = MIN_REBOOT ;
        }
      }
    }  
    i = String(server.argName(j)).indexOf("lrtd");
    if (i != -1){  // 
      lTmp =  String(server.arg(j)).toInt()  ;
      ghks.lRebootTimeDay = lTmp & 0xfff ;
    }        
    for ( k = 0 ; k < 8 ; k++){  // handle all the valve control commands for any and all valves
      i = String(server.argName(j)).indexOf( "dw" + String(k) );
      if (i != -1){  // 
        ghks.lRebootTimeDay |= ( 0x1000 << k ) ;
      }              
    }  
    
    i = String(server.argName(j)).indexOf("rbrt");
    if (i != -1){  // 
      ghks.MinRecycleTime = String(server.arg(j)).toInt() ;
      ghks.MinRecycleTime = constrain(ghks.MinRecycleTime,0,1440);  // a days worth of minutes
    }    
    i = String(server.argName(j)).indexOf("rbpd");
    if (i != -1){  // 
      ghks.RebootInterval = String(server.arg(j)).toInt() ;
      ghks.RebootInterval = constrain(ghks.RebootInterval,0,10080);  // a weeks worth of minutes
    }    
  }          

  SendHTTPHeader();
  
  bDefault = false ;
  message = "<table border=1 title='WiFi Node Settings'>";
  message += F("<tr><th>Parameter</th><th>Value</th><th></th></tr>\r\n");

  snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(AutoOff_t), month(AutoOff_t), day(AutoOff_t) , hour(AutoOff_t), minute(AutoOff_t));
  if (AutoOff_t > now()){
    MyColor =  F("bgcolor=red") ;
  }else{
    MyColor =  "" ;
  }
  message += F("<tr><td>Time Zone</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='tzone' value='" + String(ghks.fTimeZone) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

  message += F("<tr><td>Local UDP Port NTP</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='lpntp' value='" + String(ghks.localPort) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

  message += F("<tr><td>Local UDP Port Control</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='lpctr' value='" + String(ghks.localPortCtrl) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

  message += F("<tr><td>Remote UDP Port Control</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='rpctr' value='" + String(ghks.RemotePortCtrl) + "' size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";
  
  message += F("<tr><td>Network SSID</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='nssid' value='" + String(ghks.nssid) + "' maxlength="+String(sizeof(ghks.nssid))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";
  message += F("<tr><td>Network Password</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='npass' value='" + String(ghks.npassword) + "' maxlength="+String(sizeof(ghks.npassword))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

  message += F("<tr><td>Config SSID</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='cssid' value='" + String(ghks.cssid) + "' maxlength="+String(sizeof(ghks.cssid))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";
  message += F("<tr><td>Config Password</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='cpass' value='" + String(ghks.cpassword) + "' maxlength="+String(sizeof(ghks.cpassword))+" size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

  message += F("<tr><td>Time Server</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='timsv' value='" + String(ghks.timeServer) + "' maxlength=23 size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

  message += F("<tr><td>Node Name</td><td align=center>") ; 
  message += "<form method=get action=" + server.uri() + "><input type='text' name='tname' value='" + String(ghks.NodeName) + "' maxlength=23 size=16></td><td><input type='submit' value='SET'></form></td></tr>\r\n";

 

//    message += "<tr><td>ESP ID</td><td align=center>0x" + String(ESP.getChipId(), HEX) + "</td><td align=center>"+String(ESP.getChipId())+"</td></tr>\r\n"  ; 
  WiFi.macAddress(mac);      
  snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  message += "<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>\r\n"  ; 
  message += "<tr><td>WiFi RSSI</td><td align=center>" + String(WiFi.RSSI()) + "</td><td>(dBm)</td></tr>\r\n"  ; 

  server.sendContent(message) ;
  message = "" ; 
  message += "<form method=postaction=" + server.uri() + "><tr><td align=center colspan=3>Network</td></tr>\r\n" ; 

  message += F("<tr><td>Network Options</td><td align=center>") ; 
  message += F("<select name='netop'>") ;
  if ((ghks.lNetworkOptions & 0x01 ) == 0 ){
    message += F("<option value='0' SELECTED>0 - DHCP"); 
    message += F("<option value='1'>1 - Static"); 
  }else{
    message += F("<option value='0'>0 - DHCP"); 
    message += F("<option value='1' SELECTED>1 - Static IP"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr>\r\n");
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPStatic[0],ghks.IPStatic[1],ghks.IPStatic[2],ghks.IPStatic[3]);
  message += F("<tr><td>Static IP Address</td><td align=center>") ; 
  message += "<input type='text' name='staip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>\r\n";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPGateway[0],ghks.IPGateway[1],ghks.IPGateway[2],ghks.IPGateway[3]);
  message += F("<tr><td>Gateway IP Address</td><td align=center>") ; 
  message += "<input type='text' name='gatip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>\r\n";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPMask[0],ghks.IPMask[1],ghks.IPMask[2],ghks.IPMask[3]);
  message += F("<tr><td>IP Mask</td><td align=center>") ; 
  message += "<input type='text' name='mskip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>\r\n";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPDNS[0],ghks.IPDNS[1],ghks.IPDNS[2],ghks.IPDNS[3]);
  message += F("<tr><td>DNS IP Address</td><td align=center>") ; 
  message += "<input type='text' name='dnsip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr></form>\r\n";

  server.sendContent(message) ;
  message = "" ; 
  message += "<form method=postaction=" + server.uri() + "><tr><td align=center colspan=3>Misc Options</td></tr>\r\n" ; 

  message += F("<tr><td>OLED Display Option</td><td align=center>") ; 
  message += F("<select name='disop'>") ;
  if ((ghks.lDisplayOptions & 0x01 ) == 0 ){
    message += F("<option value='0' SELECTED>0 - Normal"); 
    message += F("<option value='1'>1 - Invert"); 
  }else{
    message += F("<option value='0'>0 - Normal"); 
    message += F("<option value='1' SELECTED>1 - Invert"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr>\r\n");
  
  message += F("<tr><td>Display Power Option</td><td align=center>") ; 
  message += F("<select name='disro'>") ;
  if (((ghks.lDisplayOptions & 0x02 ) >> 1 ) == 0 ){
    message += F("<option value='0' SELECTED>0 - Always On"); 
    message += F("<option value='1'>1 - Go off with WiFi"); 
  }else{
    message += F("<option value='0'>0 - Always On"); 
    message += F("<option value='1' SELECTED>1 - Go off with WiFi"); 
  }
  message += F("</select></td><td></td></tr>\r\n");

  message += F("<tr><td>Turn Wifi and Display off after</td><td align=center>") ; 
  message += "<input type='text' name='dotmr' value='" + String(ghks.displaytimer) + "' size=4 maxlength=3></td><td>(min) 0 - disables</td></tr>\r\n";

  message += F("<tr><td>Mag Switch Level</td><td align=center>") ; 
  message += "<input type='text' name='magsn' value='" + String(ghks.magsens) + "' size=4 maxlength=5></td><td>(?)</td></tr>\r\n";

  message += F("<tr><td>CPU Frequency</td><td align=center>") ; 
  message += F("<select name='cpufr'>") ;
  for ( j = 0 ; j <= 4 ; j++ ) {
    strOption = CPUSpeedTextLUT(j,&ii);
    if ( ghks.cpufreq == ii ){
      strSelected = "SELECTED" ;
    }else{
      strSelected = "" ;
    }
    message += "<option value='"+String(ii)+"' " + strSelected + ">" + strOption + "\r\n"; 
  }
  message += F("</select></td><td>(MHz)</td></tr></form>\r\n");
  message += "<tr><td>Last Scan Speed</td><td align=center>" + String(lScanCtr) + "</td><td>(per second)</td></tr>\r\n" ;    

  message += F("<tr><td align=center colspan=3>Self Reboot Options</td></tr>\r\n") ;

  message += F("<tr><td>Self Reboot Timer</td><td align=center>") ; 
  message += "<form method=post action=" + server.uri() + "><input type='text' name='srbt' value='" + String( ghks.SelfReBoot) + "' size=8 maxlength=8></td><td>(min)</td></tr>\r\n";

  message += F("<tr><td>Reboot Time of Day</td><td align=center>") ; 
  message += "<input type='text' name='lrtd' value='" + String( ghks.lRebootTimeDay & 0xfff ) + "' size=8 maxlength=8></td><td>(HHMM)</td></tr>\r\n";
  message += "<tr><td>Reboot Days</td><td align=center>";
  for (k = 0 ; k < 8 ; k++){      
    MyColor =  ""   ;  
    if ( ( ghks.lRebootTimeDay & (0x1000 << k)) != 0 ){
      MyCheck = F("CHECKED")  ;  
    }else{
      MyCheck = "" ;      
    }
    message += String(dayarray[k])+ "<input type='checkbox' name='dw" + String(k)+"' "+String(MyCheck)+ ">";    
  }
  message+="</td><td><input type='submit' value='SET'></td></tr>";  
  message += "<tr><td>Reboot Period </td><td align=center><input type='text' name='rbpd' value='"+String(ghks.RebootInterval)+"' size=12></td><td>(min)</td></tr>" ;
  message += "<tr><td>Min Recycle Time </td><td align=center><input type='text' name='rbrt' value='"+String(ghks.MinRecycleTime)+"' size=12></td><td>(min)</td></tr>" ;
  
  if( hasRTC ){
    rtc_status = DS3231_get_sreg();
    if (( rtc_status & 0x80 ) != 0 ){
      message += F("<tr><td>RTC Battery</td><td align=center bgcolor='red'>DEPLETED</td>");
      message += "<td><form method=get action=" + server.uri() + "><input type='hidden' name='rtcbf' value='1'><input type='submit' value='RESET'></form></td>";
      message += F("</tr>") ;            
    }else{
      message += F("<tr><td>RTC Battery</td><td align=center bgcolor='green'>-- OK --</td><td></td></tr>\r\n");
    }
    message += "<tr><td>RTC Temperature</td><td align=center>"+String(rtc_temp,1)+"</td><td>(C)</td></tr>\r\n" ;                    
  }
  message += F("</form></table>\r\n");
  
  server.sendContent(message) ;
  message = "" ; 

  SendHTTPPageFooter();
}



String CPUSpeedTextLUT(int Option , int *speedvalue){
String tmpStr = "" ;

  switch(Option){
    case 0:
      tmpStr = "13 Mhz - Glacial - Danger Will Robinson" ;
      *speedvalue = 13 ;
    break;    
    case 1:
      tmpStr = "26 Mhz - Sloooow - Danger Will Robinson" ;
      *speedvalue = 26 ;
    break;    
    case 2:
      tmpStr = "80 Mhz - Slow - Low Power" ;
      *speedvalue = 80 ;
    break;    
    case 3:
      tmpStr = "160MHz" ;
      *speedvalue = 160 ;     
    break;  
    case 4:  
    default:
      tmpStr = "240MHz - Fast - High Power" ;
      *speedvalue = 240 ;
    break;    
  }
  return(tmpStr);  
}


