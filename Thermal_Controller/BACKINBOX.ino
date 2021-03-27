void BackInTheBoxMemory(){
  uint8_t i , j ;

  sprintf(ghks.nssid,"************\0");  // put your default credentials in here if you wish
  sprintf(ghks.npassword,"********\0");  // put your default credentials in here if you wish
  sprintf(ghks.NodeName,"Solar Water Heater SCADA\0") ;
  sprintf(ghks.servername,"api.openweathermap.org\0\0\0" ) ;     // remote server we will connect to
  sprintf(ghks.apikey , "\0" ) ; // 



  sprintf(ghks.cpassword,"\0");
  
  ghks.fTimeZone = 10.0 ;
  ghks.lNodeAddress = ESP.getChipId() & 0xff ;
  sprintf(ghks.timeServer ,"au.pool.ntp.org\0"); 
  ghks.AutoOff_t = 0 ;
  ghks.localPortCtrl = 8088 ;
  ghks.RemotePortCtrl= 8088 ;
  ghks.lVersion = MYVER ;
  
/*  ghks.RCIP[0] = 192 ;
  ghks.RCIP[1] = 168 ; 
  ghks.RCIP[2] = 2 ;
  ghks.RCIP[3] = 255 ;*/
  sprintf(ghks.RCIP ,"192.168.2.255\0"); 
  
  ghks.lNetworkOptions = 0 ;     // DHCP 
  ghks.IPStatic[0] = 192 ;
  ghks.IPStatic[1] = 168 ;
  ghks.IPStatic[2] = 0 ;
  ghks.IPStatic[3] = 123 ;

  ghks.IPGateway[0] = 192 ;
  ghks.IPGateway[1] = 168 ;
  ghks.IPGateway[2] = 0 ;
  ghks.IPGateway[3] = 1 ;

  ghks.IPDNS = ghks.IPGateway ;

  ghks.IPMask[0] = 255 ;
  ghks.IPMask[1] = 255 ;
  ghks.IPMask[2] = 255 ;
  ghks.IPMask[3] = 0 ;

  ghks.IPPing[0] = 192 ;
  ghks.IPPing[1] = 168 ;
  ghks.IPPing[2] = 1 ;
  ghks.IPPing[3] = 1 ;

  ghks.PingMax = 1500 ;
  ghks.PingFreq = 10 ; 
  ghks.SelfReBoot = 0 ; 

  shas.fCollectorMinTemp = 2 ;
  shas.fCollectorMaxTemp = 98 ;
  shas.fSolarTempDiffMax = 10;
  shas.fSolarTempDiffMin = 6 ;
  shas.fTopBoostTemp = 30 ;
  shas.fBottomBoostTemp = 25 ; 
  shas.fTopBoostDiffTemp = 2.0 ;
  shas.fBottomBoostDiffTemp = 2.0 ;
  shas.fWeatherBoostTempDiffMin = 2.0 ;
  
  shas.fTankOverTempAlarm = 99 ;
  shas.fTankUnder1TempAlarm = 42.0 ;
  shas.fTankUnder2TempAlarm = 33.0 ;
  shas.fRoofOverTempAlarm = 105.0 ;
  shas.fRoofUnderTempAlarm = -1.0 ;

  shas.Boost_wdays = 0;
  for ( i = 0 ; i < 3 ; i++ ) {
    shas.sensor[i] = i ;
    shas.iWeatherBoostMinCloud[i]= 75 ;
    shas.fWeatherBoostMinWaterTemp[i] = 40 ;
    shas.fWeatherBoostMinAirTemp[i] = 35 ;
  }
  for ( i = 0 ; i < 48 ; i++ ) {
    shas.bBoostTimes[i] = false ;
  }
  
  shas.sensor[3] = 4 ;

  shas.relayPort[0] =  4 ; // 12
  shas.relayPort[1] =  5 ; // 13
  shas.relayPort[2] = -1 ; // 14
  shas.relayPort[3] = -1 ; // 15
  shas.relayPort[4] = -1 ;
  shas.relayPort[5] = -1 ;
  shas.relayPort[6] = -1 ;
  shas.relayPort[7] = -1 ;

  shas.fAnalogMult = 1.0 ;
  shas.fAnalogAdd = 0.0 ;
  shas.iMode = 0 ;
  shas.iBoostMode = 0 ;
  shas.Boost_wdays = 0 ;

  ghks.latitude = -34.051219 ;
  ghks.longitude = 142.013618 ;
  
  SMTP.port = 25 ;                // 25 , 465 , 2525 , 587
  sprintf(SMTP.server,"\0") ;   
  sprintf(SMTP.user,"\0") ;
  sprintf(SMTP.password,"\0") ;
  sprintf(SMTP.FROM,"\0") ;
  sprintf(SMTP.TO,"\0") ;
  sprintf(SMTP.CC,"\0") ;
  sprintf(SMTP.BCC,"\0") ;
  sprintf(SMTP.message,"Test\0") ;
  sprintf(SMTP.subject,"Node %08X\0",ESP.getChipId()) ;
  SMTP.bSecure = false ;

 

}


void LoadParamsFromEEPROM(bool bLoad){
long lTmp ;  
int i ;
int j ;
int bofs ,ofs ;
int eeAddress ;

  if ( bLoad ) {
    EEPROM.get(0,ghks);
    eeAddress = sizeof(ghks) ;
    Serial.println("read - ghks structure size " +String(eeAddress));   

    ghks.lNodeAddress = constrain(ghks.lNodeAddress,0,32768);
    ghks.fTimeZone = constrain(ghks.fTimeZone,-12,12);
    ghks.localPort = constrain(ghks.localPort,1,65535);
    ghks.localPortCtrl = constrain(ghks.localPortCtrl,1,65535);
    ghks.RemotePortCtrl = constrain(ghks.RemotePortCtrl,1,65535);
    if ( year(ghks.AutoOff_t) < 2000 ){
       ghks.AutoOff_t = now();
    }
    ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,1);

    eeAddress = PROG_BASE ;  
    EEPROM.get(eeAddress,shas);
    eeAddress += sizeof(shas) ;

    EEPROM.get(eeAddress,SMTP);
    eeAddress += sizeof(SMTP) ;
    
    Serial.println("Final VPFF EEPROM adress " +String(eeAddress));   
    
  }else{
    ghks.lVersion  = MYVER ;
    EEPROM.put(0,ghks);
    eeAddress = sizeof(ghks) ;
    Serial.println("write - ghks structure size " +String(eeAddress));   

    eeAddress = PROG_BASE ;
    EEPROM.put(eeAddress,shas);
    eeAddress += sizeof(shas) ;
    
    EEPROM.put(eeAddress,SMTP);
    eeAddress += sizeof(SMTP) ;

    Serial.println("Final EEPROM Save adress " +String(eeAddress));   

    EEPROM.commit();                                                       // save changes in one go ???
    bSaveReq = 0 ;
  }
}


