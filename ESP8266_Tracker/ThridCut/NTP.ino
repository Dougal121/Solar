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


