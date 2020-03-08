unsigned long sendNTPpacket(char* address){
byte packetBuffer[ NTP_PACKET_SIZE];     //buffer to hold incoming and outgoing packets  
  Serial.println("sending NTP packet...");
                    
  memset(packetBuffer, 0, NTP_PACKET_SIZE);    // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  ntpudp.beginPacket(address, 123); //NTP requests are to port 123
  ntpudp.write(packetBuffer, NTP_PACKET_SIZE);
  ntpudp.endPacket();
}



unsigned long processNTPpacket(void){
int oldyear ;
unsigned long lNowTime;
    oldyear = year() ;  
    ntpudp.read(packetBuffer, NTP_PACKET_SIZE);                                         // the timestamp starts at byte 40 of the received packet and is four bytes, or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);                  // combine the four bytes (two words) into a long integer
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;                             // this is NTP time (seconds since Jan 1 1900):
    const unsigned long seventyYears = 2208988800UL;                                    // now convert NTP time into everyday time:     Unix time starts on Jan 1 1970. In seconds, that's 2208988800:   
    unsigned long epoch = secsSince1900 - seventyYears + (ghks.fTimeZone * SECS_PER_HOUR);   // subtract seventy years:
    lNowTime = (unsigned long) now();
    Serial.print(F("Network Unix time = "));
    Serial.println(epoch);                                                              // print network Unix time:
    Serial.print(F("Arduino Unix time = "));
    Serial.println(lNowTime);                                                           // print arduino Unix time:
    if (( abs(lNowTime-epoch) < 1200 ) || ( bNTPFirst )) {                              // only updae if within 20 min of our current lock    
      bNTPFirst = false ;
      setTime((time_t)epoch);                                                           // update the clock
      tv.tn.year = year();                                                              // record the last NTP time set
      tv.tn.mon = month() ;
      tv.tn.mday = day();
      tv.tn.hour = hour();
      tv.tn.min = minute();
      tv.tn.sec = second();
      if (( !hasRTC ) && ( oldyear < 2000 )){
        tv.tb = tv.tn ;
      }
      if (( hasRTC ) && ( year() > MINYEAR )) {
          DS3231_set(tv.tn);                       // set the RTC if we have one and time looks non bullshit      
      }
    }
}

int SetTimeFromGPS(){
  byte hundredths ;
  time_t chiptime ;
  
  gps.crack_datetime((int *)&tv.tg.year,(byte *)&tv.tg.mon,(byte *) &tv.tg.mday,(byte *) &tv.tg.hour,(byte *) &tv.tg.min,(byte *) &tv.tg.sec , &hundredths, &tv.fixage);
  setTime((int)tv.tg.hour,(int)tv.tg.min,(int)tv.tg.sec,(int)tv.tg.mday,(int)tv.tg.mon,(int)tv.tg.year ) ; // set the internal RTC from last GPS time  
  chiptime = now() ;                          // get it back again
  chiptime += (( ghks.fTimeZone * SECS_PER_HOUR ) + ( tv.fixage / 1000 )) ; // add the offset plus the fix age
  setTime(chiptime);                         // set it again  
  if ( hasRTC ) {
      tv.tg.year = year();
      tv.tg.mon = month() ;
      tv.tg.mday = day();
      tv.tg.hour = hour() ;
      tv.tg.min = minute();
      tv.tg.sec = second();
      DS3231_set(tv.tg);                          //should also update this
  }
  return(0);
}



void handleTime() {
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
          BackIntheBoxMemory();
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
        tv.tc.sec = second();     
        tv.tc.min = minute();     
        tv.tc.hour = hour();   
        tv.tc.wday = dayOfWeek(makeTime(tm));            
        tv.tc.mday = day();  
        tv.tc.mon = month();   
        tv.tc.year = year();       
        DS3231_set(tv.tc);                       // set the RTC as well
        rtc_status = DS3231_get_sreg();       // get the status
        DS3231_set_sreg(rtc_status & 0x7f ) ; // clear the clock fail bit when you set the time
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

  snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", year(), month(), day() , hour(), minute());
  server.sendContent("<br><br><form method=get action=" + server.uri() + "><br>Set Current Time: <input type='text' name='stime' value='"+ String(buff) + "' size=12>");
  server.sendContent(F("<input type='submit' value='SET'><br><br></form>"));

  SendHTTPPageFooter();
}



