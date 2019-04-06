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
    oldyear = year() ;  
    ntpudp.read(packetBuffer, NTP_PACKET_SIZE);                                         // the timestamp starts at byte 40 of the received packet and is four bytes, or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);                  // combine the four bytes (two words) into a long integer
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;                             // this is NTP time (seconds since Jan 1 1900):
    const unsigned long seventyYears = 2208988800UL;                                    // now convert NTP time into everyday time:     Unix time starts on Jan 1 1970. In seconds, that's 2208988800:   
    unsigned long epoch = secsSince1900 - seventyYears + (timezone * SECS_PER_HOUR);   // subtract seventy years:
    setTime((time_t)epoch);                                                             // update the clock
    Serial.print(F("Unix time = "));
    Serial.println(epoch);                                                              // print Unix time:
    tn.year = year();   // record the last NTP time set
    tn.mon = month() ;
    tn.mday = day();
    tn.hour = hour();
    tn.min = minute();
    tn.sec = second();
    if (( !hasRTC ) && ( oldyear < 2000 )){
      tb = tn ;
    }
}

int SetTimeFromGPS(){
  byte hundredths ;
  time_t chiptime ;
  
  gps.crack_datetime((int *)&tg.year,(byte *)&tg.mon,(byte *) &tg.mday,(byte *) &tg.hour,(byte *) &tg.min,(byte *) &tg.sec , &hundredths, &fixage);
  setTime((int)tg.hour,(int)tg.min,(int)tg.sec,(int)tg.mday,(int)tg.mon,(int)tg.year ) ; // set the internal RTC from last GPS time  
  chiptime = now() ;                          // get it back again
  chiptime += (( timezone * SECS_PER_HOUR ) + ( fixage / 1000 )) ; // add the offset plus the fix age
  setTime(chiptime);                         // set it again  
   if (hasRTC) {
      tg.year = year();
      tg.mon = month() ;
      tg.mday = day();
      tg.hour = hour() ;
      tg.min = minute();
      tg.sec = second();
      DS3231_set(tg);                          //should also update this
    }
  return(0);
}


