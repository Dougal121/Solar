void i2cScan() {
  uint8_t i, address, error;
  uint8_t first = 0x03 ; 
  uint8_t last = 0x77 ;
  char buff[10];

  SendHTTPHeader();

  server.sendContent(F("<center><b>I2C Bus Scan</b><br><table border=1 title='I2C Bus Scan'><tr><th>.</th>"));
 // table header
  for (i = 0; i < 16; i++) {
    server.sendContent("<th>"+String(i,HEX)+"</th>");
  }
  server.sendContent(F("</tr>"));
  
  // table body
  // addresses 0x00 through 0x77
  for (address = 0; address <= 119; address++) {
    if (address % 16 == 0) {
      server.sendContent(F("<tr>"));
      server.sendContent("<td><b>"+String((address & 0xF0),HEX)+"</b></td>");
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {
        // device found
        server.sendContent("<td bgcolor='lime'>"+String((address),HEX)+"</td>");
      } else if (error == 4) {
        // other error
        server.sendContent(F("<td bgcolor='red'>XX</td>"));
      } else {
        // error = 2: received NACK on transmit of address
        // error = 3: received NACK on transmit of data
        server.sendContent(F("<td align=center>--</td>"));
      }
    } else {
      server.sendContent(F("<td align=center>.</td>"));
    }
    if (address % 16 == 15) {
      server.sendContent(F("</tr>"));
    }
  }
  server.sendContent(F("</tr></table></body></html>\r\n"));
  SendHTTPPageFooter();  
}



