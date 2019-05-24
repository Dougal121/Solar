void DisplayEEPROM() {
  uint8_t i ;
  uint16_t ii ;
  uint32_t iiii ;
  int j , k ;
  int r  = 0  ;
  int b = 0 ;
  int d = 1 ;
  int  address;
  char buff[10];

  for (uint8_t j=0; j<server.args(); j++){
    k = String(server.argName(j)).indexOf("RADIX");
    if (k != -1){  // have a request to set the time zone
      r = String(server.arg(j)).toInt() ;
    }
    k = String(server.argName(j)).indexOf("BITS");
    if (k != -1){  // have a request to set the time zone
      b = String(server.arg(j)).toInt() ;
      switch(b){
        case 32:
          d = 4 ;
        break;
        case 16:
          d = 2 ;
        break;
        default:
          d = 1 ;
        break;
      }
    }
  }  
//  SerialOutParams();
  SendHTTPHeader();
  server.sendContent(F("<a href='/'>Home</a><br>")) ;         
  server.sendContent(F("<a href='/eeprom'>Refresh</a><br><br>")) ;         
  
  server.sendContent("<br><form method=post action=" + server.uri() + ">");
  server.sendContent("Radix: <select name=RADIX>");
  switch(r){
    case 2:
      server.sendContent(F("<option value='2' SELECTED>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
    break;
    case 8:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8' SELECTED>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
    break;
    case 10:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10' SELECTED>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
    break;
    default:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16' SELECTED>Hexadecimal")); 
    break;
  }
  server.sendContent("</select>");
  server.sendContent("Bits: <select name=BITS>");
  switch(b){
    case 32:
      server.sendContent(F("<option value='8'>8 Bit - Byte")); 
      server.sendContent(F("<option value='16'>16 Bit - Word")); 
      server.sendContent(F("<option value='32' SELECTED>32 Bit - DWord")); 
    break;
    case 16:
      server.sendContent(F("<option value='8'>8 Bit - Byte")); 
      server.sendContent(F("<option value='16' SELECTED>16 Bit - Word")); 
      server.sendContent(F("<option value='32'>32 Bit - DWord")); 
    break;
    default:
      server.sendContent(F("<option value='8' SELECTED>8 Bit - Byte")); 
      server.sendContent(F("<option value='16'>16 Bit - Word")); 
      server.sendContent(F("<option value='32'>32 Bit - DWord")); 
    break;
  }
  server.sendContent("</select>");
  server.sendContent(F("<input type='submit' value='SET'></form><br><table border=1 title='EEPROM Contents'><tr><th>.</th>"));
 // table header
  for (i = 0; i < 32; i+=d) {
    server.sendContent("<th>"+String(i,HEX)+"</th>");
  }
  server.sendContent(F("</tr>"));
  for (address = 0; address < 1984  ; address+=d ) {
    if (address % 32 == 0) {
      server.sendContent(F("<tr>"));
      server.sendContent("<td align=center><b>"+String((address & 0xFFE0),HEX)+"</b></td>");
    }
    switch(b){
      case 16:
        EEPROM.get(address,ii);
        switch(r){
          case 8:
            server.sendContent("<td>"+String(ii,OCT)+"</td>");
          break;
          case 10:
            server.sendContent("<td>"+String(ii,DEC)+"</td>");
          break;
          case 2:
            server.sendContent("<td>"+String(ii,BIN)+"</td>");
          break;
          default:
            server.sendContent("<td>"+String(ii,HEX)+"</td>");
          break;
        }  
      break;
      case 32:
        EEPROM.get(address,iiii);
        switch(r){
          case 8:
            server.sendContent("<td>"+String(iiii,OCT)+"</td>");
          break;
          case 10:
            server.sendContent("<td>"+String(iiii,DEC)+"</td>");
          break;
          case 2:
            server.sendContent("<td>"+String(iiii,BIN)+"</td>");
          break;
          default:
            server.sendContent("<td>"+String(iiii,HEX)+"</td>");
          break;
        }  
      break;
      default: // byte 8
        EEPROM.get(address,i);
        switch(r){
          case 8:
            server.sendContent("<td>"+String(i,OCT)+"</td>");
          break;
          case 10:
            server.sendContent("<td>"+String(i,DEC)+"</td>");
          break;
          case 2:
            server.sendContent("<td>"+String(i,BIN)+"</td>");
          break;
          default:
            server.sendContent("<td>"+String(i,HEX)+"</td>");
          break;
        }  
      break;
    }
    if (address % 32 == 31) {
      server.sendContent(F("</tr>"));
    }
  }
  server.sendContent(F("</tr></table>"));
  SendHTTPPageFooter() ;
}

