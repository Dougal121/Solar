bool StartWiFi(void){
  int i , k , j = 0; 
  
  Serial.setDebugOutput(true);  
  snprintf(buff, BUFF_MAX,"%08X\0",chipid);
  Serial.println("Chip ID " + String(buff));
    
  WiFi.disconnect();
  Serial.println("Configuring soft access point...");
  WiFi.mode(WIFI_AP_STA);  // we are having our cake and eating it eee har
  sprintf(cssid,"Configure_%08X\0",chipid);
  if ( cssid[0] == 0 || cssid[1] == 0 ){   // pick a default setup ssid if none
    sprintf(ghks.cpassword,"\0");
  }
  ghks.MyIPC = IPAddress (192, 168, 5 +(chipid & 0x7f ) , 1);
  Serial.print("Asking for Soft AP on address: ");
  snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);      
  Serial.println(buff);
  WiFi.softAPConfig(ghks.MyIPC,ghks.MyIPC,IPAddress (255, 255, 255 , 0));  
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
  ghks.MyIPC = WiFi.softAPIP();  // get back the address to verify what happened
  Serial.print("Soft AP IP Started on address: ");
  snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);      
  Serial.println(buff);
  
  bConfig = false ;   // are we in factory configuratin mode
  display.display();
  if ( ghks.lNetworkOptions != 0 ) {
    WiFi.config(ghks.IPStatic,ghks.IPGateway,ghks.IPMask,ghks.IPDNS ); 
  }  
  if ( ghks.npassword[0] == 0 ){
    WiFi.begin((char*)ghks.nssid);                            // connect to unencrypted access point      
  }else{
    WiFi.begin((char*)ghks.nssid, (char*)ghks.npassword);     // connect to access point with encryption
  }
  while (( WiFi.status() != WL_CONNECTED ) && ( j < MAX_WIFI_TRIES )) {
    j = j + 1 ;
    delay(500);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "Chip ID " + String((uint32_t)chipid, HEX) );
    display.drawString(0, 9, String("SSID:") );
    display.drawString(0, 18, String("Password:") );
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128 , 0, String(WiFi.RSSI()));
    display.drawString(128, 9, String(ghks.nssid) );
    display.drawString(128, 18, String(ghks.npassword) );
    display.drawString(j*4, 27 , String(">") );
    display.drawString(0, 36 , String(1.0*j/2) + String(" (s)" ));   
    snprintf(buff, BUFF_MAX, ">>  IP %03u.%03u.%03u.%03u <<", ghks.MyIPC[0],ghks.MyIPC[1],ghks.MyIPC[2],ghks.MyIPC[3]);            
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(63 , 54 ,  String(buff) );
    display.display();     
    digitalWrite(ESP32_BUILTIN_LED,!digitalRead(ESP32_BUILTIN_LED));
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
     ghks.MyIP =  WiFi.localIP() ;
//     Serial.println(MyIP) ;
     snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.MyIP[0],ghks.MyIP[1],ghks.MyIP[2],ghks.MyIP[3]);            
     Serial.println(buff);
     display.drawString(0 , 53 ,  String(buff) );
//     display.drawString(0, 53, "IP "+String(MyIP) );
     display.display();
  }
  return(true);
}


bool StopWiFi(void){
  WiFi.disconnect(true);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi OFF");  
}
