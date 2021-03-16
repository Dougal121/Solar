#define MESSAGE_MAX 512 

void SendEmailToClient(int iMessageID){
char csTemp[MESSAGE_MAX] ;
int i ;
IPAddress MyIP;

  MyIP =  WiFi.localIP() ;                  
  if (WiFi.isConnected() && (MyIP[0] != 0 )&& (MyIP[4] != 0 ))  {
    if (( String(SMTP.FROM).length() >5 )&&( String(SMTP.TO).length() >5 )){
      switch(iMessageID){
          case 0: snprintf(csTemp,MESSAGE_MAX,"%s - Water Luke Warm %3.1f (C)\0",ghks.NodeName ,shams.fTemp[0] )  ;  break; 
          case 1: snprintf(csTemp,MESSAGE_MAX,"%s - Water Cold %3.1f (C)\0",ghks.NodeName ,shams.fTemp[0] )  ;  break; 
          case 2: snprintf(csTemp,MESSAGE_MAX,"%s - Water Over Temp %3.1f (C)\0",ghks.NodeName ,shams.fTemp[0] )  ;  break; 
          case 3: snprintf(csTemp,MESSAGE_MAX,"%s - Roof Collector Over Temp %3.1f\0",ghks.NodeName ,shams.fTemp[4] )  ;  break; 
          case 4: snprintf(csTemp,MESSAGE_MAX,"%s - Roof Collector Under Temp %3.1f\0",ghks.NodeName ,shams.fTemp[4] )  ;  break; 
          case 5: snprintf(csTemp,MESSAGE_MAX,"%s - Boost Element 1 Energised %3.1f\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
          case 6: snprintf(csTemp,MESSAGE_MAX,"%s - Boost Element 2 Energised %3.1f\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
          case 7: snprintf(csTemp,MESSAGE_MAX,"%s - Controler Rebooted \0",ghks.NodeName )  ;  break; 
          case 8: snprintf(csTemp,MESSAGE_MAX,"%s - Sensor Failed \0",ghks.NodeName )  ;  break; 
          case 9: snprintf(csTemp,MESSAGE_MAX,"%s - Spare \0",ghks.NodeName )  ;  break; 
          case -1: snprintf(csTemp,MESSAGE_MAX,"%s - Test Email \0",ghks.NodeName )  ;  break; 
          default: snprintf(csTemp,MESSAGE_MAX,"%s \0",SMTP.subject ) ; break ;
      }
  
      if ( String(SMTP.FROM).length() < 6 ){
        WDmail.setSubject("doby@house.elf", csTemp);      
      }else{
        WDmail.setSubject( SMTP.FROM , csTemp );
      }
      WDmail.addTo( SMTP.TO );
      if ( String(SMTP.CC).length() > 5 ){
        WDmail.addCC(SMTP.CC);
      }
      if ( String(SMTP.BCC).length() > 5 ){
        WDmail.addBCC(SMTP.BCC );
      }
      
  //    snprintf(csTemp,MESSAGE_MAX, "%s\r\n\r\n", ghks.NodeName);
      snprintf(buff,BUFF_MAX,"\r\nOperating Mode %d\r\n\r\n", shas.iMode ) ;
      strcat(csTemp,buff ) ;
      
      for ( i = 0 ; i < MAX_TEMP_SENSOR ; i++){
        switch(i){
            case 4: snprintf(buff,BUFF_MAX,"Roof %3.1f (C) \r\n" ,shams.fTemp[i] ) ; strcat(csTemp,buff) ;  break; 
            case 0: snprintf(buff,BUFF_MAX,"Tank Top %3.1f (C) \r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
            case 1: snprintf(buff,BUFF_MAX,"Tank Bottom %3.1f (C) \r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
            case 2: snprintf(buff,BUFF_MAX,"Air %3.1f (C) \r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
            case 3: snprintf(buff,BUFF_MAX,"Spare %3.1f (C) \r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
        }
      }
      strcat(csTemp,"\r\n") ;
      for (i = 0; i < MAX_RELAY ; i++) {
        if ( shams.bRelayState[i] ==  shas.ActiveValue[i] ){
          snprintf(buff,BUFF_MAX,"Relay %d Active - %s \r\n" , i , RelayDescription(i).c_str() )  ; strcat(csTemp,buff) ;
        }else{
          snprintf(buff,BUFF_MAX,"Relay %d Inactive - %s \r\n" , i , RelayDescription(i).c_str() )  ; strcat(csTemp,buff) ;
        }
      }
      
      snprintf(buff, BUFF_MAX, "\r\n %d/%02d/%02d %02d:%02d:%02d \r\nNode ID %08X\r\nIP %03u.%03u.%03u.%03u\r\n\r\n", year(), month(), day() , hour(), minute(), second(),ESP.getChipId(), MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
      strcat(csTemp,buff) ;
      WDmail.setBody(csTemp);
      WDmail.enableDebugMode();
  
      Serial.println("Server: " + String(SMTP.server));
      Serial.println("Port: " + String(SMTP.port));
      Serial.println("User: " + String(SMTP.user));
      Serial.println("Password: " + String(SMTP.password));
      if ( SMTP.user[0] == 0 ){
        if (WDmail.send(SMTP.server , SMTP.port , NULL , NULL ) == 0){
          Serial.println("Mail send OK");
        }
      }else{
        if (WDmail.send(SMTP.server , SMTP.port , SMTP.user , SMTP.password ) == 0){
          Serial.println("Mail send OK");
        }    
      }
    }else{
      Serial.println("Mail not set up proper like dude...");
    }
  }
}

void DisplayEmailSetup() {
  long  i = 0 ;
  int ii  ;
  int j ;
  int k , kk  ;
  String message ;
  String  MyCheck ;
  
  SendHTTPHeader();   //  ################### START OF THE RESPONSE  ######
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 11:  // load values
          bSendTestEmail = true ;
        break;
      }
    }    
    i = String(server.argName(j)).indexOf("smse");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.server , sizeof(SMTP.server)) ;
    }
    i = String(server.argName(j)).indexOf("smus");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.user , sizeof(SMTP.user)) ;
    }
    i = String(server.argName(j)).indexOf("smpa");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.password , sizeof(SMTP.password)) ;
    }
    i = String(server.argName(j)).indexOf("smfr");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.FROM , sizeof(SMTP.FROM)) ;
    }
    i = String(server.argName(j)).indexOf("smto");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.TO , sizeof(SMTP.TO)) ;
    }
    i = String(server.argName(j)).indexOf("smcc");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.CC , sizeof(SMTP.CC)) ;
    }
    i = String(server.argName(j)).indexOf("smbc");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( SMTP.BCC , sizeof(SMTP.BCC)) ;
    }
    i = String(server.argName(j)).indexOf("smmb");
    if (i != -1){  
     String(server.arg(j)).toCharArray( SMTP.message , sizeof(SMTP.message)) ;
    }
    i = String(server.argName(j)).indexOf("smsj");
    if (i != -1){  
     String(server.arg(j)).toCharArray( SMTP.subject , sizeof(SMTP.subject)) ;
    }
    i = String(server.argName(j)).indexOf("smbz");
    if (i != -1){  //  
        SMTP.bSecure = false ;        
    }
    i = String(server.argName(j)).indexOf("smbs");
    if (i != -1){  //  
      if ( String(server.arg(j)).length() == 2 ){ // only put back what we find
        SMTP.bSecure = true ;        
      }
    }  
    i = String(server.argName(j)).indexOf("smpo");
    if (i != -1){  // 
      SMTP.port = String(server.arg(j)).toInt() ;
      SMTP.port = constrain(SMTP.port,1,65535);
    }
    
  }


    message = "";
    message = F("<table border=1 title='Email setup for SCADA'>") ;
    message += F("<tr><th>Email Parameter</th><th>Value</th><th>Units</th><th>.</th></tr>") ;              
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Port</td><td align=center title='Popular Values 25 , 465 , 2525 , 587'><input type='text' name='smpo' value='"+String(SMTP.port)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Server</td><td align=center><input type='text' name='smse' value='"+String(SMTP.server)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP User</td><td align=center><input type='text' name='smus' value='"+String(SMTP.user)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Password</td><td align=center><input type='text' name='smpa' value='"+String(SMTP.password)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP FROM</td><td align=center><input type='text' name='smfr' value='"+String(SMTP.FROM)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP TO</td><td align=center><input type='text' name='smto' value='"+String(SMTP.TO)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP CC</td><td align=center><input type='text' name='smcc' value='"+String(SMTP.CC)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP BCC</td><td align=center><input type='text' name='smbc' value='"+String(SMTP.BCC)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Subject</td><td align=center><input type='text' name='smsj' value='"+String(SMTP.subject)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Message</td><td align=center><input type='text' name='smmb' value='"+String(SMTP.message)+"' size=30></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;

    if ( ( SMTP.bSecure ) != 0 ){
      MyCheck = F("CHECKED")  ;    
    }else{
      MyCheck = F("")  ;    
    }
    message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Secure<input type='hidden' name='smbz' value='0'></td><td align=center><input type='checkbox' name='smbs' " + String(MyCheck)+ "></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
    message += "<tr><td colspan=4>.</td></tr>" ;
    server.sendContent(message) ;
    message = "<tr><td>Last Email Status</td><td>"+String(lRet_Email)+"</td><td colspan=2>.</td></tr>" ;
    message += "<tr><form method=post action=" + server.uri() + "><td colspan=4 align='center'><input type='hidden' name='command' value='11'><input type='submit' value='### SEND TEST EMAIL ###'></td></form></tr></table>" ;
    server.sendContent(message) ;
  
    SendHTTPPageFooter();
}

