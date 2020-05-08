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
  server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String(ESP.getChipId(), HEX) + "</td><td align=center>"+String(ESP.getChipId())+"</td></tr>" ) ; 
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

  server.sendContent("<tr><td>ESP Core Version</td><td align=center>" + String(ESP.getCoreVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>ESP Full Version</td><td align=center>" + String(ESP.getFullVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>SDK Version</td><td align=center>" + String(ESP.getSdkVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>CPU Volts</td><td align=center>" + String(ESP.getVcc()) + "</td><td>(V)</td></tr>" ) ;    
  server.sendContent("<tr><td>CPU Frequecy</td><td align=center>" + String(ESP.getCpuFreqMHz()) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>Get Rest Reason</td><td align=center>" + String(ESP.getResetReason()) + "</td><td></td></tr>" ) ;    
  server.sendContent("<tr><td>Get Reset Into</td><td align=center>" + String(ESP.getResetInfo()) + "</td><td></td></tr>" ) ;    
  server.sendContent("<tr><td>Get Sketch Size</td><td align=center>" + String(ESP.getSketchSize()) + "</td><td>(Bytes)</td></tr>" ) ;    
  server.sendContent("<tr><td>Free Sketch Space</td><td align=center>" + String(ESP.getFreeSketchSpace()) + "</td><td>(Bytes)</td></tr>" ) ;    

  server.sendContent(F("</table><br>"));    
  SendHTTPPageFooter();
}

