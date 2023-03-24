void handleSensor() {
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


  server.sendContent(F("<a href='/sensor'>Update</a><br><br>")) ;         
  server.sendContent(F("<br><b>Sensor Calibration System</b><table border=1 title='Magnetometer Sensor Calibration'><tr><th rowspan=2> Parameter</th><th colspan=3>Value</th></tr>")) ; 
  server.sendContent(F("<tr><th>X (NS/Alt)</th><th>Y (EW/Az)</th><th>Z</th><th>.</th></tr>")) ; 

  server.sendContent(F("<tr><td>LSM303 Device Type</td><td colspan=3 align=center>")) ;
  i = compass.getDeviceType() ;
  server.sendContent(String(i)) ; 
  switch(i){
    case 0:
      server.sendContent(String(" - DLH")) ; 
    break;
    case 1:
      server.sendContent(String(" - DLM")) ; 
    break;
    case 2:
      server.sendContent(String(" - DLHC")) ; 
    break;
    case 3:
      server.sendContent(String(" - D")) ; 
    break;
    default:
      server.sendContent(String(" - AUTO")) ; 
    break;
  }
  server.sendContent(F("</td><td align=center></td>.</tr>"));
  
  
  server.sendContent(F("<tr><td>Acceleration RAW</td><td align=center>")) ;
  server.sendContent(String(compass.a.x)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.a.y)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.a.z)) ; 
  server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

  server.sendContent(F("<tr><td>Acceleration SI</td><td align=center>")) ;  //   +/- 2g  16Bit 2s complemet  -32767 - 0 - 32767 
  server.sendContent(String(compass.a.x/16383.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.a.y/16383.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.a.z/16383.0,3)) ; 
  server.sendContent(F("</td><td align=center>(g)</td></tr>")) ;

  server.sendContent(F("<tr><td colspan=5>.</td></tr>")) ;

  server.sendContent(F("<tr><td>Magnetometer RAW</td><td align=center>")) ;
  server.sendContent(String(xMag)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(yMag)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(zMag)) ; 
  server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

  server.sendContent(F("<tr><td>Magnetometer SI</td><td align=center>")) ;
  server.sendContent(String(xMag/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(yMag/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(zMag/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>(Gauss)</td></tr>")) ;
  
  server.sendContent(F("<tr><td>Running Min Value</td><td align=center>")) ;
  server.sendContent(String(running_min.x/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(running_min.y/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(running_min.z/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>(Gauss)</td></tr>")) ;

  server.sendContent(F("<tr><td>Running Max Value</td><td align=center>")) ;
  server.sendContent(String(running_max.x/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(running_max.y/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(running_max.z/32767.0*4.0,3)) ; 
  server.sendContent(F("</td><td align=center>(Gauss)</td></tr>")) ;

  server.sendContent(F("<tr><td>Current Cal Min</td><td align=center>")) ;
  server.sendContent(String(compass.m_min.x)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.m_min.y)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.m_min.z)) ; 
  server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

  server.sendContent(F("<tr><td>Current Cal Max</td><td align=center>")) ;
  server.sendContent(String(compass.m_max.x)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.m_max.y)) ; 
  server.sendContent(F("</td><td align=center>")) ;
  server.sendContent(String(compass.m_max.z)) ; 
  server.sendContent(F("</td><td align=center>(+/-32767)</td></tr>")) ;

  if (bMagCal){
    MyColor =  F("STOP") ;
  }else{
    MyColor =  F("START") ;
  }
  server.sendContent("<tr><td>.</td><td><form method=get action=" + server.uri() + "><input type='hidden' name='command' value='5'><input type='submit' value='RESET'></form></td>") ;
  server.sendContent("<td><form method=get action=" + server.uri() + "><input type='hidden' name='command' value='4'><input type='submit' value='"+MyColor+"'></form></td>") ;
  server.sendContent("<td><form method=get action=" + server.uri() + "><input type='hidden' name='command' value='3'><input type='submit' value='STORE'></form></td></tr>") ;

  server.sendContent(F("<tr><td>Heading (Az)</td><td colspan=3 align=center>")) ;
  server.sendContent(String(tv.heading,1)) ;     
  server.sendContent(F("</td><td>(Deg)</td></tr>")) ;

  server.sendContent(F("</table><br>")) ;
    
  SendHTTPPageFooter();
}

