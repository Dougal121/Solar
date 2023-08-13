
void adcLocalMap() {
  int i , ii , x , iTmp ;
  uint8_t j , k , kk ;
  uint8_t BoardBit ;
  int chan = 0 ;
  String message ;  
  String bgcolor ;  
  String pinname ;
  String MyColor ;
  String MyCheck ;
  
//  SerialOutParams();
  for ( j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
          LoadParamsFromEEPROM(true);
          Serial.println("Load from EEPROM");
        break;
        case 2: // Save values
          LoadParamsFromEEPROM(false);
          Serial.println("Save to EEPROM");
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 11:  // load values
          bSendTestEmail = true ;
        break;
        case 121:
          ResetSMTPInfo();
        break;        
        case 122:
          ResetADCCalInfo();
        break;   
        
        case 667: // wipe the memory to factory default
          BackInTheBoxMemory();
        break;
        case 665:
          sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server  once and hour  
        break;
      }  
    }

    i = String(server.argName(j)).indexOf("chan");
    if (i != -1){  
      chan = String(server.arg(j)).toInt() ;
      chan = constrain(chan,0,ADC_MAX_CHAN - 1) ;
    }    
    i = String(server.argName(j)).indexOf("adp");
    if (i != -1){  
     adcs.chan[chan].ADC_Input_PIN = String(server.arg(j)).toInt() ;
    }    
    i = String(server.argName(j)).indexOf("adm");
    if (i != -1){ 
      adcs.chan[chan].ADC_Cal_Mul = String(server.arg(j)).toFloat() ;
    }      
    i = String(server.argName(j)).indexOf("ado");
    if (i != -1){ 
      adcs.chan[chan].ADC_Cal_Ofs = String(server.arg(j)).toFloat() ;
    }          
    i = String(server.argName(j)).indexOf("adu");
    if (i != -1){  
     String(server.arg(j)).toCharArray(adcs.chan[chan].ADC_Unit , sizeof(adcs.chan[chan].ADC_Unit)) ;
    }     
    i = String(server.argName(j)).indexOf("adt");
    if (i != -1){  
     String(server.arg(j)).toCharArray(adcs.chan[chan].ADC_Description , sizeof(adcs.chan[chan].ADC_Description)) ;
    }     



    i = String(server.argName(j)).indexOf("aln");  // number
    if (i != -1){ 
      chan = String(server.arg(j)).toInt()  ;  
      chan = constrain(chan,0,ADC_MAX_ALARM - 1) ;
    }
    i = String(server.argName(j)).indexOf("alc");    // cahnnel
    if (i != -1){ 
      adcs.alarm[chan].ADC_Channel = constrain(String(server.arg(j)).toInt(),0,10)  ;   
    }
    i = String(server.argName(j)).indexOf("ale");   // enabled
    if (i != -1){ 
      adcs.alarm[chan].ADC_Mode = String(server.arg(j)).toInt() ;
      /*
      switch ( String(server.arg(j)).toInt()  ){
        case 0:
          adcs.alarm[chan].ADC_Mode &=  0xf9 ;      
        break;
        case 1:
          adcs.alarm[chan].ADC_Mode &=  0xf9 ;      
          adcs.alarm[chan].ADC_Mode |=  0x02 ;      
        break;
        case 2:
          adcs.alarm[chan].ADC_Mode &=  0xf9 ;      
          adcs.alarm[chan].ADC_Mode |=  0x04 ;      
        break;
        case 3:
          adcs.alarm[chan].ADC_Mode |=  0x06 ;      
        break;
      } 
       */
    }
  
    i = String(server.argName(j)).indexOf("ala");   // active
    if (i != -1){ 
      if ( String(server.arg(j)).toInt() == 0x00 ){
        adcs.alarm[chan].ADC_Mode &=  0xfe ;      
      }else{
        adcs.alarm[chan].ADC_Mode |=  0x01 ;              
      }
    } 
    i = String(server.argName(j)).indexOf("ald");   // delay
    if (i != -1){ 
      adcs.alarm[chan].ADC_Delay = String(server.arg(j)).toInt() ;
    }      
    i = String(server.argName(j)).indexOf("all");  // level
    if (i != -1){ 
      adcs.alarm[chan].ADC_Value = String(server.arg(j)).toInt() ;
    }      

    i = String(server.argName(j)).indexOf("alx");   // xecute
    if (i != -1){ 
      adcs.alarm[chan].ADC_Do = 0x03 & String(server.arg(j)).toInt() ;
    } 
    


  }
  
  //  ######################  START RESPONCE  ########################
  SendHTTPHeader();
  
  message = F("<br><center><b>ADC Setup</b><br><table border=1 title='ADC Setup'>\r\n");
  message += F("<tr><th>Chanel</th><th>Name</th><th>PIN</th><th>Mult</th><th>Ofs</th><th>Units</th><th>Raw</th><th>Values</th><th colspan=2></th></tr>\r\n");  
  for ( j=0; j<ADC_MAX_CHAN; j++){
    message += "<form method=get action=" + server.uri() + "><input type='hidden' name='chan' value='"+String(j)+"'><tr><td align=center>"+String(j)+"</td> <td><input title='Description' type='text' name='adt' value='"+String(adcs.chan[j].ADC_Description)+"' size=5></td> <td align=center><select name='adp'>" ; 
    for (ii = MinPinPort; ii < MaxPinPort; ii++) {
      if (adcs.chan[j].ADC_Input_PIN == ii ){
        MyColor = F(" SELECTED ");
      }else{
        MyColor = "";            
      }
  //    iTmp = 0 ;
      pinname = strPINName(ii,&iTmp,0);
      if ( iTmp == 0 ) {
        message +=  "<option value="+String(ii)+ MyColor +">" + pinname  ;          
      }
    }
    message += "</select></td><td><input title='The number of S.I. units per input Volt x 3.2' type='text' name='adm' value='"+String(adcs.chan[j].ADC_Cal_Mul)+"' size=5></td><td><input title='The easy way to do this is use the zero cal button below' type='text' name='ado' value='"+String(adcs.chan[j].ADC_Cal_Ofs)+"' size=5></td><td><input type='text' name='adu' value='"+String(adcs.chan[j].ADC_Unit)+"' size=5 maxlength=5></td><td>"+String(adcs.chan[j].ADC_RAW)+"</td><td><b> "+String(adcs.chan[j].ADC_Value,1) +"</b></td>" ;
    message += "<td><input type='submit' value='SET'></td></form><form method=get action=" + server.uri() + "><td><input type='hidden' name='ado' value='"+String(-1.0*adcs.chan[j].ADC_RAW)+"'><input title='Setup the transducer for Zero physical input quantity then press this button to calibrate the zero point on computer' type='submit' value='DO ZERO CAL'></td><input type='hidden' name='chan' value='"+String(j)+"'></form></tr>\r\n" ;
    server.sendContent(message) ;
    message = "" ;
  }
  message += F("</table>\r\n<br><br>");
  server.sendContent(message);

  message = F("<br><center><b>ADC Alarms</b><br><table border=1 title='ADC Alarms'>\r\n");
  message += F("<tr><th>Alarm No</th><th>Chanel</th><th>Alarm</th><th>State</th><th>Level</th><th>Delay(s)</th><th>Action</th><th></th></tr>\r\n");  
  for ( j=0; j<ADC_MAX_ALARM; j++){
    message += "<form method=get action=" + server.uri() + "><input type='hidden' name='aln' value='"+String(j)+"'><tr><td align=center>"+String(j)+"</td>" ;

//    message += "<input type='text' name='alc' value='"+String(adcs.alarm[j].ADC_Channel)+"'></td>" ;
    message += "<td><select name='alc'>" ;
    for ( k = 0 ; k < ADC_MAX_CHAN + 5 ; k++ ){ 
      if ( adcs.alarm[j].ADC_Channel == k ){
        MyCheck = "SELECTED" ;
      }else{
        MyCheck = "" ;
      }
      message += "<option value='"+String(k)+"' " + MyCheck +">" ;
      switch ( k ){
        case 6:      // x axis
          message += F("6 X-Axis") ;  break ;
        case 7:      // y asxis   
          message += F("7 Y-Axis") ;  break ;
        case 8:      // temp 1
          message += F("8 Temp BMP") ;  break ;
        case 9:      // temp 2
          message += F("9 Temp RTC") ;  break ;
        case 10:     // pressure
          message += F("10 Pressure") ;  break ;
        default:
          message += String(k)+ " " +String(adcs.chan[k].ADC_Description)  ;
      }
    }
    message += "</select></td>" ;
    
    message += "<td><select name='ale'>" ;
    for ( k = 0 ; k < 4 ; k++ ){ 
      if (( adcs.alarm[j].ADC_Mode & 0x03 ) == k ){
        MyCheck = "SELECTED" ;
      }else{
        MyCheck = "" ;
      }
      message += "<option value='"+String(k)+"' " + MyCheck +">" ;
      switch ( k ){
        case 0: message += F("Disabled") ;  break ;
        case 1: message += F("On Master Enabled") ; break ;
        case 2: message += F("Off Master Enabled") ; break ;
        case 3: message += F("Always Enabled") ; break ;
      }
    }
    message += "</select></td>" ;
    message += "<td><select name='ala'>" ;
    switch ( adcs.alarm[j].ADC_Mode & 0x01 ){
      case 0:   
        message += F("<option value='0' SELECTED>Less Than") ;
        message += F("<option value='1' >Greater Than") ;
      break;
      case 1:   
        message += F("<option value='0'>Less Than") ;
        message += F("<option value='1' SELECTED>Greater Than") ;
      break;
    }
    message += "</select></td><td><input type='text' name='all' value='"+String(adcs.alarm[j].ADC_Value)+"' size=6></td>" ;
    message += "<td><input type='text' name='ald' value='"+String(adcs.alarm[j].ADC_Delay)+"' size=6></td>" ;

    message += "<td><select name='alx'>" ;
    for ( k = 0 ; k < 4 ; k++ ) {
      if (( adcs.alarm[j].ADC_Do & 0x03 ) == k ){
        MyCheck = "SELECTED" ;
      }else{
        MyCheck = "" ;
      }
      message += "<option value='"+String(k)+"' " + MyCheck +">" ;
      switch ( k ){
        case 0: message += F("Email") ;  break ;
        case 1: message += F("Wind Park") ; break ;
        case 2: message += F("Night Park") ; break ;
        case 3: message += F("Resume Tracking") ; break ;
      }
    }
    message += "</select></td>";
    
    message += "<td><input type='submit' value='SET'></td></form></tr>\r\n" ;
    server.sendContent(message) ;
    message = "" ;
  }
  message += F("</table>");
  message += F("<a href='/adc?command=122'>Reset ADC Data</a><br>\r\n");
  server.sendContent(message);
  
  SendHTTPPageFooter();   
  
}

String strPINName(int iPin,int *iTmp,int iPinType)
{
  *iTmp = 0 ;
  String pinname = "" ;
  switch(iPinType){
    case 1:   // digital pins
#if defined(ESP32)
      switch(iPin){
        case 0: pinname = F("GPIO 0 DIO") ; break;
        case 5: pinname = F("GPIO 5 DIO") ; break;
        case 12: pinname = F("GPIO 12 DIO") ; break;
        case 13: pinname = F("GPIO 13 DIO") ; break;
        case 14: pinname = F("GPIO 14 DIO") ; break;
        case 16: pinname = F("GPIO 16 DIO") ; break;
        case 17: pinname = F("GPIO 17 DIO") ; break;
        case 18: pinname = F("GPIO 18 DIO") ; break;
        case 19: pinname = F("GPIO 19 DIO") ; break;
        case 23: pinname = F("GPIO 23 DIO") ; break;
        case 25: pinname = F("GPIO 25 DIO") ; break;
        case 26: pinname = F("GPIO 26 DIO") ; break;
        case 27: pinname = F("GPIO 27 DIO") ; break;
        case 2: pinname = F("GPIO 2 ADC") ; break;
        case 4: pinname = F("GPIO 4 ADC") ; break;
        case 34: pinname = F("GPIO 34 ADC") ; break;
        case 35: pinname = F("GPIO 35 ADC") ; break;
        case 36: pinname = F("GPIO 36 ADC") ; break;
        case 39: pinname = F("GPIO 39 ADC") ; break;
        default: pinname = F("- UNKNOWN-") ; *iTmp = 1 ; break;
      }
#elif defined(ESP8266)
      switch(iPin){
        case 0: pinname = F("GPIO 0 - D3") ; break;
        case 1: pinname = F("GPIO 1 - D1 TXD0") ; break;
        case 2: pinname = F("GPIO 2 - D9") ; break;
        case 3: pinname = F("GPIO 3 - D0 - RXD0") ; break;
        case 4: pinname = F("GPIO 4 - I2C SDA - Dont Use") ; break;
        case 5: pinname = F("GPIO 5 - I2C SCL - Dont Use") ; break;
        case 6: pinname = F("GPIO 6 - SDCLK - NA Dont Use") ; break;
        case 7: pinname = F("GPIO 7 - SDD0 - NA Dont Use") ; break;
        case 8: pinname = F("GPIO 8 - SDD1 - NA Dont Use") ; break;
        case 9: pinname = F("GPIO 9 - SDD2 - NA ? ") ; break;
        case 10: pinname = F("GPIO 10 - SDD3 - NA ? ") ; break;
        case 11: pinname = F("GPIO 11 - SDCMD - NA Dont Use") ; break;
        case 12: pinname = F("GPIO 12 - D12 - ") ; break;
        case 13: pinname = F("GPIO 13 - D11 - RXD2") ; break;
        case 14: pinname = F("GPIO 14 - D13") ; break;
        case 15: pinname = F("GPIO 15 - D10 -  TXD2") ; break;
        case 16: pinname = F("GPIO 16 - D2 -  Wake") ; break;
        default: pinname = F("- UNUSED -") ; *iTmp = 1  ; break;
      }
#endif      
    break;
    default:  // analog pins
#if defined(ESP32)
      switch(iPin){
        case 2: pinname = F("GPIO 2 ADC") ; break;
        case 4: pinname = F("GPIO 4 ADC") ; break;
        case 34: pinname = F("GPIO 34 ADC") ; break;
        case 35: pinname = F("GPIO 35 ADC") ; break;
        case 36: pinname = F("GPIO 36 ADC") ; break;
        case 39: pinname = F("GPIO 39 ADC") ; break;
        default: pinname = F("- UNKNOWN-") ; *iTmp = 1 ; break;
      }
#elif defined(ESP8266)
      switch(iPin){
        case 0: pinname = F("ADC") ; break;
        default: pinname = F("- UNKNOWN-") ; *iTmp = 1 ; break;
      }  
#endif      
    break;
  }
  return(pinname);
}

void ProcessADC(){
    tv.iWindInputSource = -1 ;
    tv.fWindSpeedVel = 0 ;
    tv.iMaxWindSpeed = 0 ;      //  max speeed  - set to zero to disable
    tv.iMaxWindTime = 0 ; 
    tv.iMinWindTime = 0 ;     
    snprintf(tv.Wind_Unit, sizeof(tv.Wind_Unit),"\0");
    for ( int i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
      if (( adcs.chan[i].ADC_Input_PIN > 0 ) && (adcs.chan[i].ADC_Input_PIN<40)) {
        adcs.chan[i].ADC_RAW = analogRead(adcs.chan[i].ADC_Input_PIN) ;
        adcs.chan[i].ADC_Value = ((adcs.chan[i].ADC_Cal_Mul * (( 1.0 * adcs.chan[i].ADC_RAW ) + adcs.chan[i].ADC_Cal_Ofs ) / 1023 ) )  ;
        for ( int j = 0 ; j < ADC_MAX_ALARM ; j++ ) {
          if ((adcs.alarm[j].ADC_Channel == i )&&(adcs.alarm[j].ADC_Do == 1 )){
            tv.fWindSpeedVel = adcs.chan[i].ADC_Value ;
            tv.iWindInputSource = i ;
            tv.iMaxWindSpeed = adcs.alarm[j].ADC_Value ;   //  max speed  
            tv.iMaxWindTime = adcs.alarm[j].ADC_Delay ;   //  delay time
            snprintf(tv.Wind_Unit, sizeof(tv.Wind_Unit),"%s\0",adcs.chan[i].ADC_Unit);
          }
          if ((adcs.alarm[j].ADC_Channel == i )&&(adcs.alarm[j].ADC_Do == 3 )){
            tv.iMinWindTime = adcs.alarm[j].ADC_Delay ;   //  delay time 
            tv.iMinWindSpeed = adcs.alarm[j].ADC_Value ;
          }
        }
      }else{
        adcs.chan[i].ADC_RAW = 0 ;
        adcs.chan[i].ADC_Value = 0 ; 
      }
    }  
    if (( tv.iWindInputSource == -1 ) && ( bWindPark )) {
      bWindPark = false ;
    }
}
float AlarmItemValue(int iAlarmNo){
  float fRet = 0 ;
  int j = constrain(adcs.alarm[iAlarmNo].ADC_Channel,0,ADC_MAX_CHAN+5) ;
  switch(j){
    case 6:
      fRet = tv.xzAng ;
    case 7:
      fRet = tv.yzAng ;
    case 8:
      fRet = tv.gT ;
    case 9:
      fRet = rtc_temp ;
    case 10:
      fRet = tv.Pr ;
    default:
      fRet = adcs.chan[j].ADC_Value ;
    break;
  }
  return(fRet);
}
void ProcessAlarms(){
int j ;  
int iMailMsg = 0 ;
bool bTrigger ;
float fValue = 0 ; 
    for (int i=0; i<ADC_MAX_ALARM; i++){  // do for all the alarms
      
      fValue = AlarmItemValue(i);
      if (( adcs.alarm[i].ADC_Mode & 0x80 ) != 0 ) {
        bTrigger = false ;
        if ((( adcs.alarm[i].ADC_Mode & 0x06 ) == 0x06  ) || ( (( adcs.alarm[i].ADC_Mode & 0x02 ) == 0x02 ) && bValveActive ) || ( (( adcs.alarm[i].ADC_Mode & 0x04 ) == 0x04 ) && !bValveActive ))  {    // alarm 1 on 
          if (( adcs.alarm[i].ADC_Mode & 0x01 ) != 0 ){ // looking for a high alarm else jump down for a low on
            if ( fValue > adcs.alarm[i].ADC_Value) {     // high alarm test 
              bTrigger = true ;  
              if (( adcs.alarm[i].ADC_Mode & 0x06 ) == 0x06  ){  // this is the always case
                  iMailMsg = 5 ;              
              }else{
                if ( bValveActive ){
                  iMailMsg = 9 ;                 
                }else{
                  iMailMsg = 13 ;
                }
              }
            }          
          }else{
            if ( fValue < adcs.alarm[i].ADC_Value ) { // low alarm test
              bTrigger = true ;
              if (( adcs.alarm[i].ADC_Mode & 0x06 ) == 0x06  ){   // this is the always case
                  iMailMsg = 7 ;                                
              }else{
                if ( bValveActive ){
                  iMailMsg = 11 ;                  
                }else{
                  iMailMsg = 15 ;                  
                }
              }
            }          
          }
        }
        
        if ( bTrigger ) {
          if (!adcs.alarm[i].ADC_bSentADCAlarmEmail) {
            adcs.alarm[i].ADC_Trigger++ ;           
          }               
        }else{
          adcs.alarm[i].ADC_Trigger = 0 ;      
          iMailMsg = 0 ;
        }
        if (adcs.alarm[i].ADC_Trigger > adcs.alarm[i].ADC_Delay) {
          if ( !adcs.alarm[i].ADC_bSentADCAlarmEmail ){
            if ( iMailMsg != 0 ){
              if ( adcs.alarm[i].ADC_Do == 0 ) {  // sens an email to user
                SendEmailToClient(iMailMsg) ;
              }
              if ( adcs.alarm[i].ADC_Do == 1 ){   // wind park
                bWindPark = true ;
              }
              if ( adcs.alarm[i].ADC_Do == 3 ){   // resume tracking
                bWindPark = false ;
              }
            }
            adcs.alarm[i].ADC_bSentADCAlarmEmail = true ;
          }  
        }
      }else{
        adcs.alarm[i].ADC_Trigger = 0 ;
        adcs.alarm[i].ADC_bSentADCAlarmEmail = false ;
        iMailMsg = 0 ;
      }
    }  
}


void ResetADCCalInfo(){
  int i ,j ; 
/*  sprintf(ghks.ADC_Unit,"kPa\0") ;
  ghks.ADC_Cal_Ofs = -170.0 ;
  ghks.ADC_Cal_Mul = 640.0 ;
  adcs.alarm[i].ADC_Mode = 0 ;               // high low etc
  adcs.alarm[i].ADC_alarm1 = 0 ;
  adcs.alarm[i].ADC_alarm2 = 0 ;                   // 
  ghks.ADC_Input_PIN1 = 25 ;
  ghks.ADC_Input_PIN2 = 26 ;  
  adcs.alarm[i].ADC_alarm_Delay = 60 ;  */

  for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
    sprintf(adcs.chan[i].ADC_Description,"Chan %02d",i);   
    adcs.chan[i].ADC_RAW = 0 ;
    adcs.chan[i].ADC_Value = 0 ;
    adcs.chan[i].ADC_Cal_Mul = 1.0 ;
    adcs.chan[i].ADC_Cal_Ofs = 0.0 ;
    sprintf(adcs.chan[i].ADC_Unit , "V\0") ;      // units for display
//    adcs.chan[i].ADC_Mode[j] = 0 ;
//    adcs.chan[i].ADC_Alarm[j] = 0 ;
//    adcs.chan[i].ADC_Alarm_Delay[j] = 60 ;
    switch(i){
      case 0:
        adcs.chan[i].ADC_Input_PIN = 2 ;
      break;
      case 1:
        adcs.chan[i].ADC_Input_PIN = 4 ;
      break;
      case 2:
        adcs.chan[i].ADC_Input_PIN = 35 ;
      break;
      case 3:
        adcs.chan[i].ADC_Input_PIN = 34 ;
      break;
      case 4:
        adcs.chan[i].ADC_Input_PIN = 36 ;
      break;
      default:  // 39 etc
        adcs.chan[i].ADC_Input_PIN = 39 ;
      break;
    }
  }  
  for ( j=0; j<ADC_MAX_ALARM; j++){
    adcs.alarm[j].ADC_Channel = (j/4); 
    adcs.alarm[j].ADC_Mode = 0 ;      // high low etc  
    adcs.alarm[j].ADC_Value = 0 ;     //
    adcs.alarm[j].ADC_Delay = 60 ;     // trigger to alarm in seconds
    adcs.alarm[j].ADC_Action = 0 ;    // high low etc  
    adcs.alarm[j].ADC_Do = 0 ;        // email, park ... etc    
    adcs.alarm[j].ADC_Trigger = 0 ;
    adcs.alarm[j].ADC_bSentADCAlarmEmail = false ;    
  }
  Serial.println(F("*** ResetADCInfo Called ***"));  
}


