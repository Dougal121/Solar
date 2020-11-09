#define DBG_OUTPUT_PORT Serial

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
  server.sendContent("<br><form method=post action=" + server.uri() + ">");
  server.sendContent("Radix: <select name=RADIX>");
  switch(r){
    case 2:
      server.sendContent(F("<option value='2' SELECTED>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
      server.sendContent(F("<option value='1'>ASCII")); 
    break;
    case 8:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8' SELECTED>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
      server.sendContent(F("<option value='1'>ASCII")); 
    break;
    case 10:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10' SELECTED>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
      server.sendContent(F("<option value='1'>ASCII")); 
    break;
    case 1:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16'>Hexadecimal")); 
      server.sendContent(F("<option value='1' SELECTED>ASCII")); 
    break;
    default:
      server.sendContent(F("<option value='2'>Binary")); 
      server.sendContent(F("<option value='8'>Octal")); 
      server.sendContent(F("<option value='10'>Decimal")); 
      server.sendContent(F("<option value='16' SELECTED>Hexadecimal")); 
      server.sendContent(F("<option value='1'>ASCII")); 
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
          case 1:
            if ( isPrintable(char(i)) ){
              server.sendContent("<td>"+String(char(i))+"</td>");
            }else{
              server.sendContent("<td>-</td>");              
            }
          break;
          default:
            if ( i < 16 ){
              server.sendContent("<td>0"+String(i,HEX)+"</td>");              
            }else{
              server.sendContent("<td>"+String(i,HEX)+"</td>");
            }
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


void handleBackup(){
  int i , j ;
  bool bDownLoad = false ;
  String message ;
  String value ;
  uint32_t MyDWord ;
  uint8_t MyByte ;
  long fileSize ;
  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("download");
    if (i != -1){  // 
      bDownLoad = true ;
    }  
  }
  if ( bDownLoad ){ 
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
//    fileSize = MAX_EEPROM * 2 ;
//    server.setContentLength(fileSize);
   server.send(200, "application/octet-stream; charset=utf-8", "");
   MyCheckSum = Caculate_EEPROM_Checksum();
   message = String(MyCheckSum,HEX) ;
   j = 4 - message.length() ;
   if ( j > 0  ){
    for ( i = 0 ; i < j ; i++ ) { 
      message = "0" + message ;
    }
   }
    DBG_OUTPUT_PORT.print("CheckSum:"); 
    DBG_OUTPUT_PORT.println(String(message));
   message += "\r\n" ;
   server.sendContent(message);
   message = "" ;
   for (int address = 0; address < ( MAX_EEPROM  ) ; address++ ) {
       MyByte = EEPROM.read(address);
//       EEPROM.get(address, MyByte );
       value = String(MyByte,HEX) ;
       if ( MyByte < 16 ){
          message += "0" + value ;
       }else{
          message += value ;        
       }
       if ((( address+1) % 32 ) == 0 ){
         server.sendContent(message + "\r\n");
         message = "" ;
       }
    }     
     if ( message.length() > 0  ){
       server.sendContent(message);
       message = "" ;
     }
  }else{
    SendHTTPHeader();
    message = "<br><form method='GET' action=" + server.uri() + ".txt action='' enctype='multipart/form-data'><input type='hidden' name='download' value='doit'><input type='submit' value='Download'></form><br><br>" ;
    server.sendContent(message);
    message = "<form method='POST' action=" + server.uri() + ".txt action='' enctype='multipart/form-data'><input type='file' name='upload'><input type='submit' value='Upload'></form><br><br>" ;
    server.sendContent(message);
    SendHTTPPageFooter();
  }
}

void handleFileUpload() {
int i , j , ii ;
uint8_t  MyByte ; 
String message ;
String value ;
char   vc[6];
bool bULC = false ;
/*  if (server.uri() != "/edit") {
    return;
  }*/
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {  // don't care not saving the file
/*    if (SD.exists((char *)upload.filename.c_str())) {
      SD.remove((char *)upload.filename.c_str());
    }
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE); */
    iUploadPos = -1 ;
    MyCheckSum = 0 ;
    DBG_OUTPUT_PORT.print("Upload: START, filename: "); DBG_OUTPUT_PORT.println(upload.filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
//    if (uploadFile) {
      // ok this is it we need to seek trough the data here
      ///uploadFile.write(upload.buf, upload.currentSize);
//    }
    DBG_OUTPUT_PORT.print("Upload: WRITE, Bytes: ");
    DBG_OUTPUT_PORT.println(upload.currentSize);

    if ( iUploadPos == -1 ){
      iUploadPos++ ;
      for ( i = 0 ; ( i < 4 ) ; i++ ){
        vc[i] = upload.buf[i] ;
      }
      vc[4] = 0 ;   
      vc[5] = 0 ;   
      MyTestSum = long( strtoul(vc,NULL,16));
      DBG_OUTPUT_PORT.print("TestSum:"); 
      DBG_OUTPUT_PORT.println(String(MyTestSum,HEX));
      j = 4 ;
      ii = 0  ;
      message = "" ; 
    }else{
      j = 0 ;
    }
    
    for ( i = j ; ( i < upload.currentSize ) && ( i < HTTP_UPLOAD_BUFLEN ) ; i += 2 ){
      vc[0] = upload.buf[i] ;
      vc[1] = upload.buf[i+1] ;
      vc[2] = 0 ;
      value = String(vc) ;  
      MyByte = byte( strtoul(vc,NULL,16));
      if ((upload.buf[i] != '\r') && (upload.buf[i+1] != '\n')) {
        EEPROM.put(iUploadPos, MyByte );
        iUploadPos++ ;
        MyCheckSum += MyByte ;
        MyCheckSum &= 0xffff ;
        if ( MyByte < 16 ){
            message += "0" + String(MyByte,HEX) ;
         }else{
            message += String(MyByte,HEX) ;        
         }
         if ((( ii) % 32 ) == 31 ) {
//            DBG_OUTPUT_PORT.println(message);
            message = "" ; 
         }  
        ii++;
      }
    }
//    DBG_OUTPUT_PORT.print("Upload: WRITE, Bytes: "); DBG_OUTPUT_PORT.println(upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {  // again dont care as not writting the file

    DBG_OUTPUT_PORT.print("Upload: END, Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);

    if (( MyCheckSum == MyTestSum ) || ( MyTestSum == 0 )) { 
//      EEPROM.commit();                
      bULC = true ;
    }              
    SendHTTPHeader();
    message = "<br>Uploaded: " + String(upload.filename) + ", Size: " + String(upload.totalSize) ;
    if ( bULC ) {
       message = "<br>Backup Restored and commited<br>" ;
    }else{
       message = "<br>INVALID Checksum " + String(MyCheckSum , HEX) + "vs" + String(MyTestSum , HEX) + " file not restored " ;
    }
    server.sendContent(message);
    DBG_OUTPUT_PORT.println(message); 
    message = String(iUploadPos) + " Bytes written to EEPROM <br>" ;
    server.sendContent(message);
    DBG_OUTPUT_PORT.println(message); 
    server.sendContent(F("<br><br><font color=red>If you want to use this data do a <a href='/?command=1'>Load Parameters from EEPROM</a>"));
    server.sendContent(F("<br>if you don't do a <a href='/?command=2'>Save Parameters to EEPROM</a></font><br><br><br>")) ;
    SendHTTPPageFooter();
  }
}

long Caculate_EEPROM_Checksum(){
long MyCS = 0 ;
uint8_t MyByte ;

   for (int  address = 0; address < ( MAX_EEPROM  ) ; address++ ) {
//     EEPROM.get(address, MyByte );
     MyByte = EEPROM.read(address);
     MyCS += MyByte ;
     MyCS &= 0xffff ;
   }
   return(MyCS);
}
