#include <ESP8266HTTPClient.h>
HTTPClient http;
char nURL[64] = "http://192.168.2.24:86/default.asp" ;  // url so end data to
int  httpCode = 0 ; 


unsigned long sendCTRLpacket(IPAddress address){
int j ;  
byte packetBuffer[64];           //buffer to hold outgoing packets  
  Serial.println("sending CTRL packet...");
                    
  memset(packetBuffer, 0, sizeof(packetBuffer));    // set all bytes in the buffer to 0

  ctrludp.beginPacket(address, ghks.RemotePortCtrl);      // Send control data to the remote port - Broadcast ???
  ctrludp.write(packetBuffer, sizeof(packetBuffer));
  ctrludp.endPacket();
}


unsigned long processCtrlUDPpacket(long lSize){
int i , j ;  
unsigned long highWord ;
unsigned long lowWord ;
unsigned long lAddress ;
byte packetBuffer[64];           //buffer to hold incomming packets  

  Serial.println(F("Process Ctrl Packet "));
  memset(packetBuffer, 0, sizeof(packetBuffer));
  ctrludp.read(packetBuffer, sizeof(packetBuffer)); // read the packet into the buffer
/*
  highWord = word(packetBuffer[0], packetBuffer[1]);
  lowWord = word(packetBuffer[2], packetBuffer[3]);
  lAddress = highWord << 16 | lowWord;


  if (( ghks.lNodeAddress == lAddress) || ( lAddress == 0xffffffff )){ // is our address or a braodcast
      switch(packetBuffer[8] ){ // command byte
        case 52:
          j = packetBuffer[9] ; // number of valves to be read
          for (i = 0 ; i < MAX_VALVE && ( i < j ) ; i++ ){
            if (( packetBuffer[14+(i*6)] == ghks.lNodeAddress ) && ( packetBuffer[15+(i*6)] == i )){
              lowWord = word(packetBuffer[10+(i*6)], packetBuffer[11+(i*6)]);  // recieve all the ttgs for the valves
              vvalve[i].lTTG = lowWord ;
              highWord = word(packetBuffer[12+(i*6)], packetBuffer[13+(i*6)]);  // recieve all the ttgs for the valves
              vvalve[i].lATTG = highWord ;
            }
          }
        break;
      }
  }
*/  
  while (ctrludp.available()){  // clean out the rest of the packet and dump overboard
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

