int i2cBusCheck() {
  uint8_t i, address, error ;
  uint8_t first = 0x03 ; 
  uint8_t last = 0x77 ;
  int errcnt = 0 ; 
  
  strBusResults = "" ;
  for (address = 0; address <= 119; address++) {
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (( SMTP.iBusState[address/16] & ( 0x0001 << (address % 16 ))) != 0 ) {
        if (( error != 0 ) && (errcnt < 10 )){
          strBusResults += "0x" + String(address,HEX) + String(" ") + DeviceName(address) + String(" Faulted\r\n");          
          errcnt++ ;
        }        
      }else{
        if (( error == 0 ) && (errcnt < 10 )){
          strBusResults += "0x" + String(address,HEX) + String(" ") + DeviceName(address) + String(" Materialized\r\n");          
          errcnt++ ;
        }
      }
    }
  }  

  if ( errcnt == 0 ){
    bBusGood = true ; 
  }
  return(errcnt);
}


String DeviceName(uint8_t address){
  if ((address >= 0x20 )&&(address<=0x2F)){
    return(String("MCP23017 Port Expander chip"));  
  }else{
    switch (address){
      case 0x38:
      case 0x39:
      case 0x3a:
      case 0x3b:
      case 0x3e:
      case 0x3f:
        return(String("PCF8570 Port Expander chip"));  
      break;
      case 0x3c:
      case 0x3d:
        return(String("OLED Display"));
      break;
      case 0x57:
        return(String("EEPROM on RTC Board"));
      break;
      case 0x68:
        return(String("DS3231 RTC"));
      break;
      case 0x6a:
      case 0x6b:
        return(String("GYRO"));
      break;
      case 0x77:
        return(String("BMP180 Pressure"));
      break;
      case 0x19:
        return(String("Accel"));
      break;
      case 0x1D:
      case 0x1E:
        return(String("Accel/Mag"));
      break;
      case 0x70:
        return(String("HT16K33 LED Matrix Driver"));
      break;      
      default:
        return(String("Unknown"));
      break;
    }
  }
}




void i2cScan() {
  uint8_t i, address, error;
  uint8_t first = 0x03 ; 
  uint8_t last = 0x77 ;
  char buff[10];
  bool bRecord = false ;

  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 42:  // load values
          bRecord = true ;
        break;
      }
    }
  }
  
  SendHTTPHeader();

  server.sendContent(F("<center><b>I2C Bus Scan</b><br><table border=1 title='I2C Bus Scan'><tr><th>.</th>\r\n"));
 // table header
  for (i = 0; i < 16; i++) {
    server.sendContent("<th>"+String(i,HEX)+"</th>");
  }
  server.sendContent(F("</tr>"));
  
  // table body
  // addresses 0x00 through 0x77
  if (bRecord ){
    for (address = 0; address < 8; address++) {
        SMTP.iBusState[address] = 0  ;  // clear the device array      
    }
  }
  for (address = 0; address <= 119; address++) {
    if (address % 16 == 0) {
      server.sendContent(F("<tr>"));
      server.sendContent("<td><b>"+String((address & 0xF0),HEX)+"</b></td>");
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {     // device found
        if (bRecord ){
          SMTP.iBusState[address/16] |= ( 0x0001 << (address % 16 )) ;   // device ok        
          server.sendContent("<td title='" + DeviceName(address) + "' bgcolor='lime'>"+String((address),HEX)+"</td>");
        }else{
          if (( SMTP.iBusState[address/16] & ( 0x0001 << (address % 16 ))) != 0 ) {
            server.sendContent("<td title='" + DeviceName(address) + "' bgcolor='green'>"+String((address),HEX)+"</td>");
          }else{
            server.sendContent("<td title='" + DeviceName(address) + " Materialised ??? ' bgcolor='yellow'>"+String((address),HEX)+"</td>");          
          }
        }
      } else {
        if (( SMTP.iBusState[address/16] & ( 0x0001 << (address % 16 ))) != 0 ) {
            server.sendContent("<td title='" + DeviceName(address) + " Missing/Faulted ??? ' bgcolor='red'>+String((address),HEX)+</td>");
        }else{      
          if (error == 4) {   // other error
            server.sendContent("<td title='" + DeviceName(address) + "' bgcolor='magenta'>XX</td>");
          } else {   // error = 2: received NACK on transmit of address....  error = 3: received NACK on transmit of data
            server.sendContent(F("<td align=center>--</td>"));
          }
        }
      }
    } else {
      server.sendContent(F("<td align=center>.</td>"));
    }
    if (address % 16 == 15) {
      server.sendContent(F("</tr>"));
    }
  }
  server.sendContent(F("</tr></table>\r\n"));
  server.sendContent("<a href='" + server.uri() + "?command=42'>Save Active Configration</a><br>\r\n");
//  server.sendContent("<br><br>lMinBusScan "+String(lMinBusScan)+"\r\n");
//  server.sendContent("<br>SMTP.iBusScanInterval "+String(SMTP.iBusScanInterval)+"\r\n");
//  server.sendContent("<br>bBusGood "+String(bBusGood)+"\r\n");
  i = i2cBusCheck();
  if (i != 0 ){
    server.sendContent(strBusResults);
    
  }
  SendHTTPPageFooter();  
  server.sendContent(F("</body></html>\r\n"));
}


