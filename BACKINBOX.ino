void BackIntheBoxMemory(){
  uint8_t i , j ;
//  sprintf(nssid,"WLAN-PLUMMER\0") ;
//  sprintf(npassword,"cheegh5S\0") ;

  sprintf(nssid,"TP-LINK_52FC8C\0");
  sprintf(npassword,"0052FC8C\0");
  
  sprintf(NodeName,"North Tower\0") ;
  lTimeZone = 10 ;

  RCIP[0] = 192 ;
  RCIP[1] = 168 ; 
  RCIP[2] = 2 ;
  RCIP[3] = 255 ;

  xzH = 4.0;  // hysterisis NS
  yzH = 4.0;  //    ""      EW

  dxPark = 34.0 ;    // NS  
  dyPark = -45.0 ;    // EW
  
  xzOffset = 0.0 ;  // NS
  yzOffset = 0.0 ;  // EW

  xzTarget = 0.0;  // NS
  yzTarget = 0.0;  // EW

  xMinVal = 0.0 ;   // NS
  xMaxVal = 45.0 ;

  yMinVal = -65.0 ;  // EW
  yMaxVal = 45.0 ;

  iTrackMode = 0 ;
  iUseGPS = 0 ;
    
  latitude = -34.051219 ;
  longitude = 142.013618 ;
  timezone = 10 ;  
  xMul = 1.0 ;  
  yMul = 1.0 ;  
  zMul = 1.0 ;  
  iXYS = 0 ;
  iNightShutdown = 1 ;
  iMultiDrive = 0 ;
  MyIP = IPAddress(192,168,5 +(ESP.getChipId() & 0x7f ) ,1);
  for ( i = 0 ; i < sizeof(npassword) ; i++ ){     
    npassword[i] = 0 ;  
  }
  for ( i = 0 ; i < sizeof(nssid) ; i++ ){     
    nssid[i] = 0 ;  
  }
  sprintf(trackername,"Most Excellent\0");
  sprintf(nssid , "Configure_%X\0",ESP.getChipId()) ;
//  sprintf(npassword, "\0");
}




