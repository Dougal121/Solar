void BackInTheBoxMemory(){
  uint8_t i , j ;

  ghks.lVersion = LVER ;
  ghks.lDisplayOptions = 1 ;
  ghks.lNetworkOptions = 0 ; 
  for ( i = 0 ; i < sizeof(ghks.npassword) ; i++ ){     
    ghks.npassword[i] = 0 ;  
  }
  for ( i = 0 ; i < sizeof(ghks.nssid) ; i++ ){     
    ghks.nssid[i] = 0 ;  
  }
  for ( i = 0 ; i < sizeof(ghks.cpassword) ; i++ ){     
    ghks.cpassword[i] = 0 ;  
  }

  sprintf(ghks.nssid,"************\0");  // put your default credentials in here if you wish
  sprintf(ghks.npassword,"********\0");  // put your default credentials in here if you wish
  

  
  sprintf(ghks.NodeName,"ESP32\0") ;
  sprintf(ghks.timeServer, "au.pool.ntp.org\0") ;

  ghks.localPort = 2390;
  ghks.localPortCtrl = 8666;  
  ghks.RemotePortCtrl = 8664;

  ghks.MyIP[0] = 0 ;
  ghks.MyIP[1] = 0 ; 
  ghks.MyIP[2] = 0 ;
  ghks.MyIP[3] = 0 ;

  ghks.IPStatic[0] = 192 ;
  ghks.IPStatic[1] = 168 ;
  ghks.IPStatic[2] = 2 ;
  ghks.IPStatic[3] = 234 ;

  ghks.IPGateway[0] = 192 ;
  ghks.IPGateway[1] = 168 ;
  ghks.IPGateway[2] = 1 ;
  ghks.IPGateway[3] = 1 ;

  ghks.IPDNS = ghks.IPGateway ;

  ghks.IPMask[0] = 255 ;
  ghks.IPMask[1] = 255 ;
  ghks.IPMask[2] = 255 ;
  ghks.IPMask[3] = 0 ;
    
  tv.xzH = 4.0;             // hysterisis NS
  tv.yzH = 4.0;             //    ""      EW

  tv.dxPark = 34.0 ;        // NS  
  tv.dxParkWind = 34.0 ;
  tv.dyPark = -45.0 ;       // EW
  tv.dxParkWind = -45.0 ; 
  
  tv.xzOffset = 0.0 ;       // NS
  tv.yzOffset = 0.0 ;       // EW

  tv.xzTarget = 0.0;        // NS
  tv.yzTarget = 0.0;        // EW

  tv.xMinVal = 0.0 ;        // NS
  tv.xMaxVal = 45.0 ;

  tv.yMinVal = -65.0 ;      // EW
  tv.yMaxVal = 45.0 ;

  tv.iTrackMode = 0 ;
  tv.iUseGPS = 0 ;
    
  tv.latitude = -34.051219 ;
  tv.longitude = 142.013618 ;
//  tv.latitude = 47.8289 ;
//  tv.longitude = 20.1173 ;

  ghks.fTimeZone = 10.0 ;  
  tv.xMul = 1.0 ;  
  tv.yMul = 1.0 ;  
  tv.zMul = 1.0 ;  
  tv.iXYS = 0 ;
  tv.iNightShutdown = 1 ;
  tv.iMultiDrive = 0 ;
  tv.iUseGPS = 0 ;
  tv.iMaxWindSpeed = 0 ;    // disable
  tv.iMaxWindTime = 15 ;    // 15 seconds of max speed parks the arrray
  tv.iMinWindTime = 180 ;   // 180 secodns to resume tracking 
  tv.iMountType = 0 ;       // equitorial
  tv.iOutputType = 0 ;      // Output circuit type 
  tv.iTimeSource = 0 ;
  tv.fWindSpeedCal = 1.0 ;
  tv.iWindInputSource = 0 ;
  ghks.MyIPC = IPAddress(192,168,5 +(chipid & 0x7f ) ,1);
  tv.xMaxMotorSpeed = MAX_MOTOR_PWM ;
  tv.yMaxMotorSpeed = MAX_MOTOR_PWM ;
/*  tv.RELAY_XZ_PWM = 14 ; // defaults for a relay sheild if your that way inclined
  tv.RELAY_YZ_PWM = 27 ;
  tv.RELAY_XZ_DIR = 16 ;
  tv.RELAY_YZ_DIR = 17 ;*/
  tv.RELAY_XZ_PWM = 12 ; // defaults for a relay sheild if your that way inclined
  tv.RELAY_YZ_PWM = 13 ;
  tv.RELAY_XZ_DIR = 14 ;
  tv.RELAY_YZ_DIR = 15 ;
  sprintf(tv.trackername,"Most Excellent\0");
  sprintf(ghks.cssid , "Configure_%X\0",chipid) ;
  bSaveReq = 1 ;

/*  sprintf(ghks.ADC_Unit,"km/h\0");
  ghks.ADC_Cal_Ofs = 0 ;
  ghks.ADC_Cal_Mul = 1.0 ;  */
  ghks.cpufreq = 240 ;
  ghks.displaytimer = 120 ;


  clearChartData();
  ResetSMTPInfo();
  ResetADCCalInfo();  
  clearDataLog();
  WriteDataLogsToEEPROM();
}


void LoadParamsFromEEPROM(bool bLoad){
int eeAddress ;  
  if ( bLoad ) {
    eeAddress = 0 ;
    EEPROM.get(eeAddress,ghks);
    eeAddress += sizeof(ghks) ;
    Serial.println("read - ghks structure size " +String(eeAddress));   
    eeAddress = TV_BASE ;  // 450
    
    EEPROM.get(eeAddress,tv) ; 
    eeAddress += sizeof(tv) ;
    EEPROM.get(eeAddress,SMTP);
    eeAddress += sizeof(SMTP) ;
    EEPROM.get(eeAddress,adcs);
    eeAddress += sizeof(adcs) ;
    EEPROM.get(eeAddress,chart);
    eeAddress += sizeof(chart) ;
    
    Serial.println("Final Load EEPROM adress " +String(eeAddress));   
    
    tv.xzH = constrain(tv.xzH,ANG_ABS_MIN_HYS_NS,ANG_ABS_MAX_HYS_NS);          // NS Bullshit detectors and correctors 
    tv.yzH = constrain(tv.yzH,ANG_ABS_MIN_HYS_EW,ANG_ABS_MAX_HYS_EW);          // EW
 
    tv.xzOffset = constrain(tv.xzOffset,ANG_ABS_MIN_OFS_NS,ANG_ABS_MAX_OFS_NS);  // NS
    tv.yzOffset = constrain(tv.yzOffset,ANG_ABS_MIN_OFS_EW,ANG_ABS_MAX_OFS_EW);  // EW
  
    tv.xMinVal = constrain(tv.xMinVal,ANG_ABS_MIN_NS,ANG_ABS_MAX_NS);   // NS - Alt
    tv.xMaxVal = constrain(tv.xMaxVal,ANG_ABS_MIN_NS,ANG_ABS_MAX_NS);

    tv.dxPark = constrain(tv.dxPark,ANG_ABS_MIN_NS,ANG_ABS_MAX_NS);      // NS
    tv.xzTarget = constrain(tv.xzTarget,ANG_ABS_MIN_NS,ANG_ABS_MAX_NS);  // NS

    if ( tv.iMountType == 0 ) {
      tv.yMinVal = constrain(tv.yMinVal,ANG_ABS_MIN_EW,ANG_ABS_MAX_EW);          // EW 
      tv.yMaxVal = constrain(tv.yMaxVal,ANG_ABS_MIN_EW,ANG_ABS_MAX_EW);
      tv.dyPark = constrain(tv.dyPark,ANG_ABS_MIN_EW,ANG_ABS_MAX_EW);            // EW    
      tv.yzTarget = constrain(tv.yzTarget,ANG_ABS_MIN_EW,ANG_ABS_MAX_EW);        // EW
      tv.dyParkWind = constrain(tv.dyParkWind ,ANG_ABS_MIN_EW ,ANG_ABS_MAX_EW ); // EW      
   }else{
      tv.yMinVal = constrain(tv.yMinVal,ANG_ABS_MIN_AZ,ANG_ABS_MAX_AZ);          // Az
      tv.yMaxVal = constrain(tv.yMaxVal,ANG_ABS_MIN_AZ,ANG_ABS_MAX_AZ);      
      tv.dyPark = constrain(tv.dyPark,ANG_ABS_MIN_AZ,ANG_ABS_MAX_AZ);            // Az    
      tv.yzTarget = constrain(tv.yzTarget,ANG_ABS_MIN_AZ,ANG_ABS_MAX_AZ);        // Az
      tv.dyParkWind = constrain(tv.dyParkWind ,ANG_ABS_MIN_AZ ,ANG_ABS_MAX_AZ ); // Az      
    }
  
    tv.iTrackMode = constrain(tv.iTrackMode,-1,5);
      
    tv.latitude = constrain(tv.latitude,-90.0,90.0);
    tv.longitude = constrain(tv.longitude,-180.0,180.0);
    ghks.fTimeZone = constrain(ghks.fTimeZone,-12,+12);  
    tv.xMul = constrain(tv.xMul,-10,10);  
    tv.yMul = constrain(tv.yMul,-10,10);  
    tv.zMul = constrain(tv.zMul,-10,10);  
    tv.iXYS = constrain(tv.iXYS,0,1);
    if ( tv.xMul == 0.0 )  // zero is rubbish value so take 1.0 as the default
      tv.xMul = 1.0 ;
    if ( tv.yMul == 0.0 )
      tv.yMul = 1.0 ;
    if ( tv.zMul == 0.0 )
      tv.zMul = 1.0 ;

    tv.xMaxMotorSpeed = constrain(tv.xMaxMotorSpeed,0,MAX_MOTOR_PWM);  
    tv.yMaxMotorSpeed = constrain(tv.yMaxMotorSpeed,0,MAX_MOTOR_PWM);  
    if (tv.xMaxMotorSpeed < 10 ) {
      tv.xMaxMotorSpeed = MAX_MOTOR_PWM ;
    }
    if (tv.yMaxMotorSpeed < 10 ) {
      tv.yMaxMotorSpeed = MAX_MOTOR_PWM ;
    }
    tv.iMountType = constrain(tv.iMountType,0,1);
    tv.iOutputType = constrain(tv.iOutputType,0,3);
    tv.iNightShutdown = constrain(tv.iNightShutdown,0,1);
    tv.iMultiDrive = constrain(tv.iMultiDrive,0,1);
    for (int j=0; j<ADC_MAX_ALARM; j++){
      adcs.alarm[j].ADC_Trigger = 0 ; 
      adcs.alarm[j].ADC_bSentADCAlarmEmail = false ;      // high low etc  
    }
  
  }else{
    eeAddress = 0 ;
    EEPROM.put(eeAddress,ghks);
    eeAddress += sizeof(ghks) ;
    Serial.println("write - ghks structure size " +String(eeAddress));   

    eeAddress = TV_BASE ;        // 450
    EEPROM.put(eeAddress,tv);
    eeAddress += sizeof(tv) ;
    EEPROM.put(eeAddress,SMTP);
    eeAddress += sizeof(SMTP) ;
    EEPROM.put(eeAddress,adcs);
    eeAddress += sizeof(adcs) ;
    EEPROM.put(eeAddress,chart);
    eeAddress += sizeof(chart) ;
    
    Serial.println("Final Save EEPROM adress " +String(eeAddress));   
    EEPROM.commit();                                                       // save changes in one go ???
  }
  bSaveReq = 0 ;  // reset the flag
}





