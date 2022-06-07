#include <ESP8266HTTPClient.h>
HTTPClient http;
char nURL[64] = "http://192.168.2.24:86/default.asp" ;  // url so end data to
int  httpCode = 0 ; 


unsigned long sendCTRLpacket(){
int j ;  
byte packetBuffer[64];           //buffer to hold outgoing packets  
tp_t  tp ;  

  Serial.println("sending CTRL packet...");                    
  memset(packetBuffer, 0, sizeof(packetBuffer));    // set all bytes in the buffer to 0

  tp.fWindSpeedVel = tv.fWindSpeedVel ;
  tp.dtNow = now() ;

  if  (( ghks.RCIP[0] == 0 ) &&  ( ghks.RCIP[1] == 0 ) && ( ghks.RCIP[2] == 0 ) && ( ghks.RCIP[3] == 0 ) && ( ghks.RemotePortCtrl != 0 )){
    Serial.println("No DNS - no point sending NTP to 0.0.0.0 ");      
  }else{
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u\0",ghks.RCIP[0], ghks.RCIP[1], ghks.RCIP[2], ghks.RCIP[3]);
    Serial.print("Sending CTRL packet - IP address: "+String(ghks.RemotePortCtrl)+" ->  ");
    Serial.println(buff);
    ctrludp.beginPacket( buff, ghks.RemotePortCtrl);                                 // Send control data to the remote port - Broadcast ???
    ctrludp.write((byte *)&tp, sizeof(tp));
    ctrludp.endPacket();
  }
}


unsigned long processCtrlUDPpacket(long lSize){                     // should only get one per minute
int i , j ;  
unsigned long highWord ;
unsigned long lowWord ;
unsigned long lAddress ;
byte packetBuffer[64];                                              // buffer to hold incomming packets  
tp_t  tp ;  

  Serial.println(F("Process Ctrl Packet "));
  memset(packetBuffer, 0, sizeof(packetBuffer));
  ctrludp.read((byte *)&tp, sizeof(tp));                                     // read first bit packet into the buffer

  if ( year(tp.dtNow) > MINYEAR ) {                                 // if the time not good then assume the rest is crap
    if ( tv.iWindInputSource == 1 ){
      tv.fWindSpeedVel = tp.fWindSpeedVel ;
    }
    if ( tv.iTimeSource == 2 ){                                     // Use the network broadcast timeC
      setTime((time_t)tp.dtNow) ;                                   // sync up the clocks
      if (( hasRTC ) && ( minute(tp.dtNow) == 0 )) {                // update every hour
        tv.tn.year = year();                                                              // record the last NTP time set
        tv.tn.mon = month() ;
        tv.tn.mday = day();
        tv.tn.hour = hour();
        tv.tn.min = minute();
        tv.tn.sec = second();
        DS3231_set(tv.tn);                                       // set the RTC if we have one and time looks non bullshit      
      }
    }    
  }

  while (ctrludp.available()){                                      // clean out the rest of the packet and dump overboard
    ctrludp.read(packetBuffer, sizeof(packetBuffer));  
  }
}




//   ######################################  SendData  ##########################################
int SendData(void){
int i , j , k  ;
String myurl ;
boolean bOK = true ;

  Serial.println("Send Data");
  Serial.println("Send Data - part 1 - ");
  j = 0 ;
  if ( year() > MINYEAR ) {  // check if transmitted
    myurl = String(nURL) + "?CID="+String(ESP.getChipId())+"&RSSI="+String(WiFi.RSSI()) ;
    myurl += "&Y="+String(year()) ;
    myurl += "&M="+String(month()) ;
    myurl += "&D="+String(day()) ;
    myurl += "&H="+String(hour()) ;
    myurl += "&N="+String(minute()) ;
    myurl += "&S="+String(second()) ;
/*         
    for ( i = 0 ; i < MAX_DP ; i++ ){
      myurl += "&DV" + String(i) + "=" + String(DR[j].Data[i])  ;
    }
*/
//      Serial.println(myurl);
    http.begin(myurl);

    httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode != HTTP_CODE_OK) {
        bOK = false ;
      }else{
//        lLastGoodUpload = now() ;
      }
    }
    http.end();
    return(bOK);
  }  
}

