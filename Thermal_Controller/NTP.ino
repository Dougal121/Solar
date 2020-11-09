

unsigned long processNTPpacket(void){
    ntpudp.read(packetBuffer, NTP_PACKET_SIZE);                                         // the timestamp starts at byte 40 of the received packet and is four bytes, or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);                  // combine the four bytes (two words) into a long integer
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;                             // this is NTP time (seconds since Jan 1 1900):
    const unsigned long seventyYears = 2208988800UL;                                    // now convert NTP time into everyday time:     Unix time starts on Jan 1 1970. In seconds, that's 2208988800:   
    unsigned long epoch = secsSince1900 - seventyYears + long(SECS_PER_HOUR * ghks.fTimeZone );   // subtract seventy years:
    setTime((time_t)epoch);                                                             // update the clock
    Serial.print(F("Unix time = "));
    Serial.println(epoch);                                                              // print Unix time:
    rtc_hour = hour() ;                                                                 // stop this happining again now it's updated
}

unsigned long sendNTPpacket(char* address){
byte packetBuffer[ NTP_PACKET_SIZE];     //buffer to hold incoming and outgoing packets  
IPAddress ntpIP ;
  if (WiFi.isConnected())  {
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
  
    WiFi.hostByName(address, ntpIP);
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u\0", ntpIP[0], ntpIP[1], ntpIP[2], ntpIP[3]);
    Serial.print("NTP IP address: "+String(address)+" ->  ");
    Serial.println(buff);
  
    if  (( ntpIP[0] == 0 ) &&  ( ntpIP[1] == 0 ) && ( ntpIP[2] == 0 ) && ( ntpIP[3] == 0 )){
      Serial.println("No DNS - no point sending NTP to 0.0.0.0 ");      
    }else{
      // all NTP fields have been given values, now
      // you can send a packet requesting a timestamp:
      ntpudp.beginPacket(buff, 123); //NTP requests are to port 123
      ntpudp.write(packetBuffer, NTP_PACKET_SIZE);
      ntpudp.endPacket();
    }
  }else{
    Serial.println("No WiFi - no point trying to send NTP packet...");
  }  
}

