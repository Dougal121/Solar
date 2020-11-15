#define MESSAGE_MAX 256 

void SendEmailToClient(int iMessageID){
char csTemp[MESSAGE_MAX] ;
int i ;

  if (( String(SMTP.FROM).length() >5 )&&( String(SMTP.TO).length() >5 )){
    switch(iMessageID){
        case 0: snprintf(csTemp,MESSAGE_MAX,"%s - Water Luke Warm %3.1f (C)\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
        case 1: snprintf(csTemp,MESSAGE_MAX,"%s - Water Cold %3.1f (C)\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
        case 2: snprintf(csTemp,MESSAGE_MAX,"%s - Water Over Temp %3.1f (C)\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
        case 3: snprintf(csTemp,MESSAGE_MAX,"%s - Roof Collector Over Temp %3.1f\0",ghks.NodeName ,shams.fTemp[0] )  ;  break; 
        case 4: snprintf(csTemp,MESSAGE_MAX,"%s - Roof Collector Under Temp %3.1f\0",ghks.NodeName ,shams.fTemp[0] )  ;  break; 
        case 5: snprintf(csTemp,MESSAGE_MAX,"%s - Boost Element 1 Energised %3.1f\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
        case 6: snprintf(csTemp,MESSAGE_MAX,"%s - Boost Element 2 Energised %3.1f\0",ghks.NodeName ,shams.fTemp[1] )  ;  break; 
        case 7: snprintf(csTemp,MESSAGE_MAX,"%s - Controler Rebooted \0",ghks.NodeName )  ;  break; 
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
    
    snprintf(csTemp,MESSAGE_MAX, "%s\r\n\r\n", ghks.NodeName);
    for ( i = 0 ; i < MAX_TEMP_SENSOR ; i++){
      switch(iMessageID){
          case 0: snprintf(buff,BUFF_MAX,"Roof %3.1f (C)\r\n" ,shams.fTemp[i] ) ; strcat(csTemp,buff) ;  break; 
          case 1: snprintf(buff,BUFF_MAX,"Tank Top %3.1f (C)\r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
          case 2: snprintf(buff,BUFF_MAX,"Tank Bottom %3.1f (C)\r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
          case 3: snprintf(buff,BUFF_MAX,"Air %3.1f\r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
          case 4: snprintf(buff,BUFF_MAX,"Spare %3.1f\r\n" ,shams.fTemp[i] )  ; strcat(csTemp,buff) ;  break; 
      }
    }

    snprintf(buff, BUFF_MAX, "\r\n %d/%02d/%02d %02d:%02d:%02d \r\nNode ID %08X\r\nIP %03u.%03u.%03u.%03u\r\n\r\n", year(), month(), day() , hour(), minute(), second(),ESP.getChipId(), MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
    strcat(csTemp,buff) ;
    WDmail.setBody(csTemp);

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

