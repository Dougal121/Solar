#define MESSAGE_MAX 1024 


void IndicateReboot(void){
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);  
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 20, "AUTO");
    display.drawString(63, 36, "REBOOT");
    display.setFont(ArialMT_Plain_10);
    display.setColor(INVERSE);
    display.fillRect(0, 0, 128, 11);
    display.fillRect(0, 53, 128, 11);
    display.display();    
    ESP.restart() ;
}


void SendEmailToClient(int iMessageID){
char csTemp[MESSAGE_MAX] ;
int i ,j ;
String strMyTmp ;
IPAddress MyIP;
bool bRoach = false ;
//  Serial.println("Send Email Called");

  MyIP =  WiFi.localIP() ;                  
  if (WiFi.isConnected() && (MyIP[0] != 0 )&& (MyIP[3] != 0 ) && bPower )  {
    if (( String(SMTP.FROM).length() >5 )&&( String(SMTP.TO).length() >5 ) && ( String(SMTP.server).indexOf('.') > 0 ) && ( String(SMTP.TO).indexOf('.')>0  )){
//      Serial.println( "Going into the send - MessageID " + String(iMessageID) );
      snprintf(csTemp,MESSAGE_MAX,"\0"  )  ;  
      switch(iMessageID){
        case -3:  // About to /reboot
            snprintf(csTemp,MESSAGE_MAX,"%s - One minute to Self reboot \0",ghks.NodeName  )  ;  
        break;   
        case -2:  // bootup/reboot
            snprintf(csTemp,MESSAGE_MAX,"%s - Booted up or Rebooted 3 minutes ago\0",ghks.NodeName  )  ;  
        break;   
        case -1:  // test
            snprintf(csTemp,MESSAGE_MAX,"%s - Test Email \0",ghks.NodeName  )  ;  
        break;   
        case 2:  // got to windy 
            snprintf(csTemp,MESSAGE_MAX,"%s - Trackers Paking Due to High Winds %f \0",ghks.NodeName , tv.fWindSpeedVel  )  ;  
        break ; 
        case 3:  // wind gone going back online
            snprintf(csTemp,MESSAGE_MAX,"%s - Trackers Resuming Normal Operation %f \0",ghks.NodeName , tv.fWindSpeedVel )  ;  
        break ;
        case 4:  // main soleniod CB trip
            snprintf(csTemp,MESSAGE_MAX,"%s - Main Solenoid Circuit Breaker Trip \0",ghks.NodeName  )  ;  
        break ;
        default: snprintf(csTemp,MESSAGE_MAX,"%s \0",SMTP.subject ) ; 
        break ;
      }
      if ((iMessageID >= 20) and (iMessageID<(20+ADC_MAX_ALARM))){
          j = constrain(adcs.alarm[iMessageID - 20].ADC_Channel,0,ADC_MAX_CHAN) ;    
          snprintf(csTemp,MESSAGE_MAX,"%s - %f (%s) greater then Alarm 1 \0",ghks.NodeName,adcs.chan[j].ADC_Value, adcs.chan[j].ADC_Unit )  ;          
      }
      if ((iMessageID >= 40) and (iMessageID<(40+ADC_MAX_ALARM))){
            snprintf(csTemp,MESSAGE_MAX,"%s - %f (%s) less then Alarm 1 \0",ghks.NodeName,adcs.chan[j].ADC_Value, adcs.chan[j].ADC_Unit )  ;  
      }
      // Set the session config      
      session.server.host_name = String(SMTP.server) ;     //  "smtp.office365.com"; // for outlook.com
      session.server.port = SMTP.port ;
      if ( SMTP.user[0] == 0 ){
        session.login.email = "" ;                       // set to empty for no SMTP Authentication
        session.login.password = "" ;                    // set to empty for no SMTP Authentication
        session.login.user_domain = "" ;
      }else{
        session.login.email = SMTP.user ;                  // Get settings from memory
        session.login.password = SMTP.password ;           // 
        session.login.user_domain = "";                    // client domain or ip e.g. mydomain.com << currently no setting for this
      }
    
      // Set the NTP config time
      session.time.ntp_server = String(ghks.timeServer[24]) ; // "pool.ntp.org,time.nist.gov";
      session.time.gmt_offset = (int)ghks.fTimeZone ;
      session.time.day_light_offset = 0;
//      session.time.ntp_server = "" ;
    
      // Declare the SMTP_Message class variable to handle to message being transport
      SMTP_Message message;
      
      // Set the message headers
      if ( String(SMTP.FROM).length() < 6 ){
        message.sender.name = "Doby The House Elf";
        message.sender.email = "doby@house.elf";
      }else{
        message.sender.name = ghks.NodeName ;
        message.sender.email = String(SMTP.FROM) ;
      }

//      if ( iMessageID == -1 ){
//        snprintf(buff, BUFF_MAX, "%02d:%02d:%02d\0", hour(), minute(), second());
//        message.subject = "Test Email Sent "+ String(buff);
//      }else{
        message.subject = String(csTemp);      
        strcat(csTemp,"\r\n");
//      }
      message.addRecipient("", String(SMTP.TO));
    
      if ( String(SMTP.CC).length() > 5 ){
        message.addCc(String(SMTP.CC));
      }
      if ( String(SMTP.BCC).length() > 5 ){
        message.addBcc(String(SMTP.BCC ));
      }     

      if (iMessageID == 666 ) {
        strcat(csTemp,strBusResults.c_str() ) ;  // if a buss alarm add this in
        strBusResults = "" ; // clear after sending
      }
      
      snprintf(buff, BUFF_MAX, "\r\nWind Speed   %.2f   Y(N/S) %.2f \r\n\0", tv.fWindSpeedVel );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "RTC Temperature  %.1f   Y(N/S) %.2f \r\n\0", rtc_temp );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "Air Temperature  %.1f   Y(N/S) %.2f \r\n\0", tv.gT );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "Air Pressure     %.2f   Y(N/S) %.2f \r\n\0", tv.Pr );
      strcat(csTemp,buff) ;

//      snprintf(buff, BUFF_MAX, "ADC Input RAW %d   Scaled  %.2f  (%s) \r\n\0", ADC_Raw , ADC_Value , ghks.ADC_Unit );
//      strcat(csTemp,buff) ;

      snprintf(buff, BUFF_MAX, "\r\nTarget Angle Y(E/W)     %.2f   Y(N/S) %.2f \r\n\0",tv.yzTarget, tv.xzTarget );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "Current Sensor Angle Y(E/W) %.2f   Y(N/S) %.2f \r\n\0",tv.yzAng, tv.xzAng );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "Target minus Current Y(E/W) %.2f   Y(N/S) %.2f \r\n\0",(tv.yzTarget-tv.yzAng), (tv.xzTarget-tv.xzAng) );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "Azomouth / Elevation        %.2f          %.2f \r\n\0",tv.solar_az_deg, tv.solar_el_deg );
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "Hour Angle / Elevation      %.2f          %.2f \r\n\0",tv.ha, tv.sunX );
      strcat(csTemp,buff) ;

      
      snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d \r\n\0", year(), month(), day() , hour(), minute(), second());
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX,"Node ID %08X\r\nIP %03u.%03u.%03u.%03u \r\n\0",chipid, MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
      strcat(csTemp,buff) ;
      snprintf(buff, BUFF_MAX, "CPU Uptime %d:%02d:%02d (Days:Hrs:Min)\r\n\r\n\0",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
      strcat(csTemp,buff) ;
      
//      Serial.println("Setting Body >" + String(csTemp) + "<");
      message.text.content = csTemp;

      smtp.connect(&session /* session credentials */);    
      if (!MailClient.sendMail(&smtp, &message)){  // Start sending Email and close the session
        Serial.println("Error sending Email, " + smtp.errorReason());
      }else    {
        Serial.println("Email Sent OK");        
      }
  
  //    Serial.println("Server: " + String(SMTP.server));
  //    Serial.println("Port: " + String(SMTP.port));
  //    Serial.println("User: " + String(SMTP.user));
  //    Serial.println("Password: " + String(SMTP.password));
  
      
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
  String MyCheck ;

//  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
          tv.iDoSave = 3 ;
        break;
        case 2: // Save values
          tv.iDoSave = 2 ;
          bSendSaveConfirm = true ;
        break;        
        case 11:  // send a test email straight away
          bSendTestEmail = true ;
        break;
        case 121:
          ResetSMTPInfo();
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
    if (i != -1){ 
      SMTP.port = String(server.arg(j)).toInt() ;
    }       
    i = String(server.argName(j)).indexOf("lotk");
    if (i != -1){ 
      SMTP.LowTankQty= String(server.arg(j)).toFloat() ;
    }       

    i = String(server.argName(j)).indexOf("smuz");
    if (i != -1){  //  
        SMTP.bUseEmail = false ;        
    }
    i = String(server.argName(j)).indexOf("smuy");
    if (i != -1){  //  
        SMTP.bSpare = false ;
    }
    i = String(server.argName(j)).indexOf("smue");
    if (i != -1){  //  
      if ( String(server.arg(j)).length() == 2 ){ // only put back what we find
        SMTP.bUseEmail = true ;        
      }
    }     
    i = String(server.argName(j)).indexOf("smup");
    if (i != -1){  //  
      if ( String(server.arg(j)).length() == 2 ){ // only put back what we find
        SMTP.bSpare = true ;        
      }
    }     

/*
    i = String(server.argName(j)).indexOf("allu");
    if (i != -1){  
     String(server.arg(j)).toCharArray(ghks.ADC_Unit , sizeof(ghks.ADC_Unit)) ;
    }    
    i = String(server.argName(j)).indexOf("all1");
    if (i != -1){ 
      ghks.ADC_Alarm1 = String(server.arg(j)).toFloat() ;
    }          
    i = String(server.argName(j)).indexOf("all2");
    if (i != -1){ 
      ghks.ADC_Alarm2 = String(server.arg(j)).toFloat() ;
    }      
    i = String(server.argName(j)).indexOf("adcm");
    if (i != -1){ 
      ghks.ADC_Cal_Mul = String(server.arg(j)).toFloat() ;
    }      
    i = String(server.argName(j)).indexOf("adco");
    if (i != -1){ 
      ghks.ADC_Cal_Ofs = String(server.arg(j)).toFloat() ;
    }      
    i = String(server.argName(j)).indexOf("aldl");
    if (i != -1){ 
      ghks.ADC_Alarm_Delay = String(server.arg(j)).toInt() ;
    }      
    
    i = String(server.argName(j)).indexOf("almmc");
    if (i != -1){ 
      if ( String(server.arg(j)).toInt() == 0x00 ){
        ghks.ADC_Alarm_Mode &=  0x7f ;      
      }else{
        ghks.ADC_Alarm_Mode |=  0x80 ;              
      }
    }  

    i = String(server.argName(j)).indexOf("alme2");
    if (i != -1){ 
      switch ( String(server.arg(j)).toInt()  ){
        case 0:
          ghks.ADC_Alarm_Mode &=  0xcf ;      
        break;
        case 1:
          ghks.ADC_Alarm_Mode &=  0xcf ;      
          ghks.ADC_Alarm_Mode |=  0x10 ;      
        break;
        case 2:
          ghks.ADC_Alarm_Mode &=  0xcf ;      
          ghks.ADC_Alarm_Mode |=  0x20 ;      
        break;
        case 3:
          ghks.ADC_Alarm_Mode |=  0x30 ;      
        break;
      }  
    }

    i = String(server.argName(j)).indexOf("alme1");
    if (i != -1){ 
      switch ( String(server.arg(j)).toInt()  ){
        case 0:
          ghks.ADC_Alarm_Mode &=  0xf9 ;      
        break;
        case 1:
          ghks.ADC_Alarm_Mode &=  0xf9 ;      
          ghks.ADC_Alarm_Mode |=  0x02 ;      
        break;
        case 2:
          ghks.ADC_Alarm_Mode &=  0xf9 ;      
          ghks.ADC_Alarm_Mode |=  0x04 ;      
        break;
        case 3:
          ghks.ADC_Alarm_Mode |=  0x06 ;      
        break;
      }  
    }

    i = String(server.argName(j)).indexOf("alma2");
    if (i != -1){ 
      if ( String(server.arg(j)).toInt() == 0x00 ){
        ghks.ADC_Alarm_Mode &=  0xf7 ;      
      }else{
        ghks.ADC_Alarm_Mode |=  0x08 ;              
      }
    }  
    i = String(server.argName(j)).indexOf("alma1");
    if (i != -1){ 
      if ( String(server.arg(j)).toInt() == 0x00 ){
        ghks.ADC_Alarm_Mode &=  0xfe ;      
      }else{
        ghks.ADC_Alarm_Mode |=  0x01 ;              
      }
    }  
*/        
    
  }

  SendHTTPHeader();   //  ################### START OF THE RESPONSE  ######
        
  message += F("<table border=1 title='Email setup for SCADA'>") ;
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
  server.sendContent(message) ;
  message = "" ;
  
  if ( ( SMTP.bSecure ) != 0 ){
    MyCheck = F("CHECKED")  ;    
  }else{
    MyCheck = F("")  ;    
  }
  message += "<tr><form method=post action=" + server.uri() + "><td>SMTP Secure<input type='hidden' name='smbz' value='0'></td><td align=center><input type='checkbox' name='smbs' " + String(MyCheck)+ "></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
  message += "<tr><td>Last Email Status</td><td>"+String(lRet_Email)+"</td><td colspan=2>.</td></tr>" ;
  message += "<tr></td><td colspan=4>.</td></tr>" ;
/*  
  message += "<tr><form method=post action=" + server.uri() + "><td>ADC Multiplier</td><td align=center><input type='text' name='adcm' value='"+String(ghks.ADC_Cal_Mul)+"' size=30></td><td></td><td><input type='submit' value='SET'></td></form></tr>" ;
  message += "<tr><form method=post action=" + server.uri() + "><td>ADC Offset</td><td align=center><input type='text' name='adco' value='"+String(ghks.ADC_Cal_Ofs)+"' size=30></td><td></td><td><input type='submit' value='SET'></td></form></tr>" ;
  message += "<tr><form method=post action=" + server.uri() + "><td>ADC SI Units</td><td align=center><input type='text' name='allu' value='"+String(ghks.ADC_Unit)+"' size=30 maxlength=5></td><td></td><td><input type='submit' value='SET'></td></form></tr>" ;
  message += "<tr><form method=post action=" + server.uri() + "><td>ADC RAW <b> "+String(ADC_Raw)+" </td><td>ADC Scaled Value <b> "+String(ADC_Value,1)+" </td><td colspan=2><input type='hidden' name='adco' value='"+String(-1.0*ADC_Raw)+"'><input title='Setup the transducer for Zero physical input quantity then press this button to calibrate the zero point on computer' type='submit' value='DO ZERO CAL'></td></tr>" ;
  message += "<tr><form method=post action=" + server.uri() + "><td>Master Alarm Enable</td><td> <select name='almmc'>" ;
  switch ( ghks.ADC_Alarm_Mode & 0x80 ){
    case 0:   
      message += F("<option value='0' SELECTED>Disabled") ;
      message += F("<option value='1' >Enabled") ;
    break;
    case 128:   
      message += F("<option value='0'>Disabled") ;
      message += F("<option value='1' SELECTED>Enabled") ;
    break;
  }
  message += "</td><td></td><td><input type='submit' value='SET'></td></form></tr>" ;
  server.sendContent(message) ;
  message = "" ;
    
  message += "<tr><form method=post action=" + server.uri() + "><td colspan=3>Alarm 1 <select name='alme1'>" ;
  for ( j = 0 ; j < 4 ; j=j+3 ){ 
    if ((( ghks.ADC_Alarm_Mode & 0x06 )>>1 ) == j ){
      MyCheck = "SELECTED" ;
    }else{
      MyCheck = "" ;
    }
    message += "<option value='"+String(j)+"' " + MyCheck +">" ;
    switch ( j ){
      case 0: message += F("Disabled") ;  break ;
      case 1: message += F("On Master Enabled") ; break ;
      case 2: message += F("Off Master Enabled") ; break ;
      case 3: message += F("Always Enabled") ; break ;
    }
  }
  message += "</select>" ;
  message += " Active State <select name='alma1'>" ;
  switch ( ghks.ADC_Alarm_Mode & 0x01 ){
    case 0:   
      message += F("<option value='0' SELECTED>Less Than") ;
      message += F("<option value='1' >Greater Than") ;
    break;
    case 1:   
      message += F("<option value='0'>Less Than") ;
      message += F("<option value='1' SELECTED>Greater Than") ;
    break;
  }
  message += "</select> <input type='text' name='all1' value='"+String(ghks.ADC_Alarm1)+"' size=6> ("+String(ghks.ADC_Unit)+")</td><td><input type='submit' value='SET'></td></form></tr>" ;
  
  message += "<tr><form method=post action=" + server.uri() + "><td colspan=3>Alarm 2 <select name='alme2'>" ;
  for ( j = 0 ; j < 4 ; j=j+3 ){ 
    if ((( ghks.ADC_Alarm_Mode & 0x30 )>>4 ) == j ){
      MyCheck = "SELECTED" ;
    }else{
      MyCheck = "" ;
    }
    message += "<option value='"+String(j)+"' " + MyCheck +">" ;
    switch ( j ){
      case 0: message += F("Disabled") ;  break ;
      case 1: message += F("On Master Enabled") ; break ;
      case 2: message += F("Off Master Enabled") ; break ;
      case 3: message += F("Always Enabled") ; break ;
    }
  }
  message += "</select>" ;
  message += " Active State <select name='alma2'>" ;
  switch ( ghks.ADC_Alarm_Mode & 0x08 ){
    case 0:   
      message += F("<option value='0' SELECTED>Less Than") ;
      message += F("<option value='1' >Greater Than") ;
    break;
    case 8:   
      message += F("<option value='0'>Less Than") ;
      message += F("<option value='1' SELECTED>Greater Than") ;
    break;
  }
  message += "</select> <input type='text' name='all2' value='"+String(ghks.ADC_Alarm2)+"' size=6> ("+String(ghks.ADC_Unit)+")</td><td><input type='submit' value='SET'></td></form></tr>" ;
  
  message += "<tr><form method=post action=" + server.uri() + "><td>Alarm Delay</td><td align=center><input type='text' name='aldl' value='"+String(ghks.ADC_Alarm_Delay)+"' size=30></td><td>(s)</td><td><input type='submit' value='SET'></td></form></tr>" ;
  message += "<tr><form method=post action=" + server.uri() + "><td title='-1 to disable alarm emails'>Tank Low Alarm Volume</td><td align=center><input type='text' name='lotk' value='"+String(SMTP.LowTankQty)+"' size=30></td><td>(L)</td><td><input type='submit' value='SET'></td></form></tr>" ;
*/  
  if ( ( SMTP.bSpare ) != 0 ){
    MyCheck = F("CHECKED")  ;    
  }else{
    MyCheck = F("")  ;    
  }
  message += "<tr><form method=post action=" + server.uri() + "><td title='SPAREemails'><input type='hidden' name='smuy' value='0'>SPARE Emails</td><td align=center><input type='checkbox' name='smup' " + String(MyCheck)+ "></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
  if ( ( SMTP.bUseEmail ) != 0 ){
    MyCheck = F("CHECKED")  ;    
  }else{
    MyCheck = F("")  ;    
  }
  message += "<tr><form method=post action=" + server.uri() + "><td><input type='hidden' name='smuz' value='0'>Use Email Alarm Messaging</td><td align=center><input type='checkbox' name='smue' " + String(MyCheck)+ "></td><td>.</td><td><input type='submit' value='SET'></td></form></tr>" ;
  message += "<tr><form method=post action=" + server.uri() + "><td colspan=4 align='center'><input type='hidden' name='command' value='11'><input type='submit' value='### SEND TEST EMAIL ###'></td></form></tr>" ;
  message += "<tr><td colspan=4 align='center'><form method=post action=" + server.uri() + "><input type='hidden' name='command' value='121'><input type='submit' value='Clear / RESET EMAIL Settings'></form></td></tr>" ; 
  message += "</table>" ;
  
  server.sendContent(message) ;
  message = "" ;
  SendHTTPPageFooter();
}



void ResetSMTPInfo(){
  SMTP.port = 25 ;                // 25 , 465 , 2525 , 587
//  sprintf(SMTP.server,"203.36.137.241\0") ;   // smtp.telstrabusiness.com 
  sprintf(SMTP.server,"121.200.0.25\0") ;   // mail.aussiebroadband.com.au
  sprintf(SMTP.user,"\0") ;
  sprintf(SMTP.password,"\0") ;
  sprintf(SMTP.FROM,"dougal@plummer.com.au\0") ;
  sprintf(SMTP.FROM,"\0") ;
  sprintf(SMTP.TO,"dougal@plummersoftware.onmicrosoft.com\0") ;
  sprintf(SMTP.TO,"\0") ;
  sprintf(SMTP.CC,"\0") ;
  sprintf(SMTP.BCC,"\0") ;
  sprintf(SMTP.message,"Test Email\0") ;
  sprintf(SMTP.subject,"Node %08X\0",chipid) ;
  SMTP.bSecure = false ;
  SMTP.bUseEmail = false ;
  SMTP.LowTankQty = 50.0 ;   
}



//   MailClient.networkReconnect(true);

  /** Enable the debug via Serial port
   * 0 for no debugging
   * 1 for basic level debugging
   *
   * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
   */
//  smtp.debug(1);  // 0 no debig 1 deug

  /* Set the callback function to get the sending results */
//  smtp.callback(smtpCallback);

  /* Declare the session config data */
//  ESP_Mail_Session session; 
  /* Set the session config */
//  session.server.host_name = SMTP_HOST;
//  session.server.port = SMTP_PORT;
//  session.login.email = AUTHOR_EMAIL;
//  session.login.password = AUTHOR_PASSWORD;
//  session.login.user_domain = F("mydomain.net");

  /* Set the NTP config time */
//  session.time.ntp_server = F("pool.ntp.org,time.nist.gov");
//  session.time.gmt_offset = 3;
//  session.time.day_light_offset = 0;  


