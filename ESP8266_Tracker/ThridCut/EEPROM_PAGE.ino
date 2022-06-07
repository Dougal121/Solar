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
//  SerialOutParams();
  
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




void DisplayRTCEEPROM() {
  uint8_t i[32] ;
  uint16_t ii[16] ;
  uint32_t iiii[8] ;
  int j , k ;
  int r  = 0  ;
  int b = 0 ;
  int d = 1 ;
  int iAddr = 0 ; 
  int  address = 0 ;
  int  iLen = 256 ;
  char buff[10];
  String message ;

  for (uint8_t j=0; j<server.args(); j++){
    k = String(server.argName(j)).indexOf("RADIX");
    if (k != -1){  // have a request to set the time zone
      r = String(server.arg(j)).toInt() ;
    }
    k = String(server.argName(j)).indexOf("ADDR");
    if (k != -1){  // have a request to set the time zone
      iAddr = String(server.arg(j)).toInt() ;
    }
    k = String(server.argName(j)).indexOf("LEN");
    if (k != -1){  // have a request to set the time zone
      iLen = String(server.arg(j)).toInt() ;
      if ( iLen < 32 ) iLen = 32 ;
      if ( iLen > 1024 ) iLen = 1024 ;
      iLen -=  (iLen % 32)  ;
      iLen += 32 ;
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
  if ( hasRTC ){
    message = "<br><form method=post action=" + server.uri() + ">";
    message += "Base Address: <input type='text' name='ADDR' value='" + String(iAddr) + "' maxlength=4 size=4>" ;  
    message += "Len: <input type='text' name='LEN' value='" + String(iLen) + "' maxlength=4 size=4>" ;  
    message += "Radix: <select name=RADIX>" ;
    switch(r){
      case 2:
        message += F("<option value='2' SELECTED>Binary"); 
        message += F("<option value='8'>Octal"); 
        message += F("<option value='10'>Decimal"); 
        message += F("<option value='16'>Hexadecimal"); 
        message += F("<option value='1'>ASCII"); 
      break;
      case 8:
        message += F("<option value='2'>Binary"); 
        message += F("<option value='8' SELECTED>Octal"); 
        message += F("<option value='10'>Decimal"); 
        message += F("<option value='16'>Hexadecimal"); 
        message += F("<option value='1'>ASCII"); 
      break;
      case 10:
        message += F("<option value='2'>Binary"); 
        message += F("<option value='8'>Octal"); 
        message += F("<option value='10' SELECTED>Decimal"); 
        message += F("<option value='16'>Hexadecimal"); 
        message += F("<option value='1'>ASCII"); 
      break;
      case 1:
        message += F("<option value='2'>Binary"); 
        message += F("<option value='8'>Octal"); 
        message += F("<option value='10'>Decimal"); 
        message += F("<option value='16'>Hexadecimal"); 
        message += F("<option value='1' SELECTED>ASCII"); 
      break;
      default:
        message += F("<option value='2'>Binary"); 
        message += F("<option value='8'>Octal"); 
        message += F("<option value='10'>Decimal"); 
        message += F("<option value='16' SELECTED>Hexadecimal"); 
        message += F("<option value='1'>ASCII"); 
      break;
    }
    message += F("</select>");
    server.sendContent(message);
    message = F("Bits: <select name=BITS>");
    switch(b){
      case 32:
        message += F("<option value='8'>8 Bit - Byte"); 
        message += F("<option value='16'>16 Bit - Word"); 
        message += F("<option value='32' SELECTED>32 Bit - DWord"); 
      break;
      case 16:
        message += F("<option value='8'>8 Bit - Byte"); 
        message += F("<option value='16' SELECTED>16 Bit - Word"); 
        message += F("<option value='32'>32 Bit - DWord"); 
      break;
      default:
        message += F("<option value='8' SELECTED>8 Bit - Byte"); 
        message += F("<option value='16'>16 Bit - Word"); 
        message += F("<option value='32'>32 Bit - DWord"); 
      break;
    }
    message += F("</select>");
    server.sendContent(message);
    message = F("<input type='submit' value='SET'></form><br><table border=1 title='EEPROM Contents'><tr><th>.</th>");
   // table header
    for (k = 0; k < 32; k+=d) {
      message += "<th>"+String(k,HEX)+"</th>";
    }
    message += F("</tr>");
    server.sendContent(message);
    message = "" ;
    for (address = 0; address < iLen  ; address+=d ) {
      if (address % 32 == 0) {
        message += F("<tr>");  // start the line 
        message += "<td align=center><b>"+String(((address+iAddr) & 0xFFE0),HEX)+"</b></td>";
        switch(b){  // read all the data
          case 16:
            rtceeprom.eeprom_read(address+iAddr,(byte *)&ii,32);
          break;
          case 32:
            rtceeprom.eeprom_read(address+iAddr,(byte *)&iiii,32);
          break;
          default: // byte 8
            rtceeprom.eeprom_read(address+iAddr,(byte *)&i,32);
          }  
      }
      switch(b){
        case 16:
  //        rtceeprom.eeprom_read(address,(byte *)&ii,32);
          switch(r){
            case 8:
              message += "<td>"+String(ii[(address % 32)/2],OCT)+"</td>";
            break;
            case 10:
              message += "<td>"+String(ii[(address % 32)/2],DEC)+"</td>";
            break;
            case 2:
              message += "<td>"+String(ii[(address % 32)/2],BIN)+"</td>";
            break;
            default:
              message += "<td>"+String(ii[(address % 32)/2],HEX)+"</td>";
            break;
          }  
        break;
        case 32:
  //        rtceeprom.eeprom_read(address,(byte *)&iiii,4);
          switch(r){
            case 8:
              message += "<td>"+String(iiii[(address % 32)/4],OCT)+"</td>";
            break;
            case 10:
              message += "<td>"+String(iiii[(address % 32)/4],DEC)+"</td>";
            break;
            case 2:
              message += "<td>"+String(iiii[(address % 32)/4],BIN)+"</td>";
            break;
            default:
              message += "<td>"+String(iiii[(address % 32)/4],HEX)+"</td>";
            break;
          }  
        break;
        default: // byte 8
  //        rtceeprom.eeprom_read(address,(byte *)&i,32);
          switch(r){
            case 8:
              message += "<td>"+String(i[address % 32],OCT)+"</td>";
            break;
            case 10:
              message += "<td>"+String(i[address % 32],DEC)+"</td>";
            break;
            case 2:
              message += "<td>"+String(i[address % 32],BIN)+"</td>";
            break;
            case 1:   //ascii
              if ( isPrintable(char(i[address % 32])) ){
                message += "<td>"+String(char(i[address % 32]))+"</td>";
              }else{
                message += "<td>-</td>";              
              }
            break;
            default:
              message += "<td>"+String(i[address % 32],HEX)+"</td>";
            break;
          }  
        break;
      }
      if (address % 32 == 31) {
        message += F("</tr>");
        server.sendContent(message);
        message = "" ;
      }
      
    }
    message += F("</tr></table>");
    server.sendContent(message);
  }else{
    message += F("NO RTC / EEPROM for Valve Logs");
    server.sendContent(message);
  }
  SendHTTPPageFooter() ;
}

void ReadSolarLogsFromEEPROM(long index){
  long lStart = millis();
  if ( hasRTC ){
    rtceeprom.eeprom_read(index*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // read in each block
    Serial.println("RTC EEPROM Read Time " + String((millis()-lStart)/1000,3) + " (s)");
    bSolarLogDirty = false ; 
  }else{
    Serial.println("No RTC - No EEPROM to read");    
  }
  return;
}
void WriteSolarLogsToEEPROM(long index){
  long lStart = millis();
  if ( hasRTC && bSolarLogDirty ){
    rtceeprom.eeprom_write(index*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // write blocks
    snprintf(buff, BUFF_MAX, "%02d/%02d/%04d", day(), month(), year() );     
    Serial.println(String(buff) + " RTC EEPROM Write Time " + String((millis()-lStart)/1000,3) + " (s)");
    bSolarLogDirty = false ; 
  }else{
    if ( bSolarLogDirty ){
      Serial.println("No RTC - No EEPROM to write");        
    }else{
      Serial.println("Log Clean Nothing worth writting to EEPROM");        
    }
  }
  return;  
}


void ZeroSolarLogsMemory(void){
  SolarLog.RecDate = 0;
  SolarLog.Temp = 0 ; 
  SolarLog.Wind = 0 ;           
  SolarLog.Pressure = 0 ;
  SolarLog.XAngle = 0 ;
  SolarLog.YAngle = 0 ;
  SolarLog.XTarget = 0 ;
  SolarLog.YTarget = 0 ;  
  
  for ( long j = 0 ; j < SOLAR_MAX_LOG ; j++){
    rtceeprom.eeprom_write(j*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // write blocks
  }    
  return;  
}

void UpDateSolarLogs(void){
  long j = 0 ;
  if (((minute() % 15) == 0 ) &&  (second() == 0 )) { // data logging  
    j = (hour() * 4) +  ( minute() / 15 ) ;
    SolarLog.RecDate = now();
    SolarLog.Temp = tv.gT ; 
    SolarLog.Wind = 0 ;           
    SolarLog.Pressure = tv.Pr ;
    SolarLog.XAngle = tv.xzAng ;
    SolarLog.YAngle = tv.yzAng ;
    SolarLog.XTarget = tv.xzTarget ;
    SolarLog.YTarget = tv.yzTarget ;  
    rtceeprom.eeprom_write(j*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ;    // write blocks
  }
}


int WeekOfYear(time_t myDate){
  int julian = DayOfYear(myDate);     // Jan 1 = 1, Jan 2 = 2, etc...
  int dow = dayOfWeek(myDate);        // Sun = 0, Mon = 1, etc...
  int dowJan1 = dayOfWeek(FirstDayOfYear(myDate)) ;  // find out first of year's day
  int weekNum = ((julian + 6) / 7);      // probably better.  CHECK THIS LINE. (See comments.)
  if (dow < dowJan1)                     // adjust for being after Saturday of week #1
    ++weekNum;
  if ( weekNum < 1 )  
    weekNum = 1 ;
  if ( weekNum > 53 )  
    weekNum = 53 ;
  return (weekNum);
}


time_t FirstDayOfYear(time_t myDate){
tmElements_t tm;
time_t myTime ;
    tm.Year = CalendarYrToTm(year(myDate)) ;  // dont leave home without the calendar year to tm function
//    Serial.println("Year ->" +String(tm.Year));
    tm.Month = 1 ;
    tm.Day = 1 ;
    tm.Hour = 0 ;
    tm.Minute = 0 ;
    tm.Second = 0 ;
    myTime = makeTime(tm) ;
//    snprintf(buff, BUFF_MAX, "First Day of year  %04d/%02d/%02d", year(myTime), month(myTime), day(myTime) );     
//    Serial.println(buff);
    return(myTime);
}
int DayOfYear(time_t myDate){
int iED1Jan ;  
int iEDNow = elapsedDays(myDate);
    iED1Jan = elapsedDays(FirstDayOfYear(myDate)) ;     
    return((iEDNow-iED1Jan)+1) ;
}
String MAH(int iMin){
  if (iMin == 0 ){
    return(String("-:-"));
  }else{
    if ( iMin > 1440 ){
      snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(iMin/1440),((iMin/60)%24),(iMin%60));
    }else{
      snprintf(buff, BUFF_MAX, "%02d:%02d",((iMin/60)%24),(iMin%60));
    }
  }
  return(String(buff));
}


void DisplaySolarLog() {
  uint8_t i ;
  uint16_t ii ;
  uint32_t iiii ;
  long j , k ;
  int r  = 0  ;
  int b = 0 ;
  int d = 1 ;
  int  address;
  bool bDownLoad = false ;
  char buff[10];
  String message ;
  String bgcolor ;
  String bgcolor2 ;
  time_t myTime ;
  time_t myTime2 ;
  time_t myBaseTime ;
  solar_log_t  MySolarLog ;  
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 999:
          ZeroSolarLogsMemory();
        break;        
      }  
    }        
    k = String(server.argName(j)).indexOf("RADIX");
    if (k != -1){  // 
      r = String(server.arg(j)).toInt() ;
    }
    i = String(server.argName(j)).indexOf("download");
    if (i != -1){  // 
      bDownLoad = true ;
    } 
  }  
//  SerialOutParams();
  if ( bDownLoad ){ 
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/octet-stream; charset=utf-8", "");
    message = "Date,Temperature,Wind,Pressure,X_Angle,Y_Angle,X_Target,Y_Target\r\n" ;
    server.sendContent(message) ;
    for ( long j = 0 ; j < SOLAR_MAX_LOG ; j++){
      rtceeprom.eeprom_read(j*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // write blocks   
      myTime = MySolarLog.RecDate ;
      snprintf(buff, BUFF_MAX, "%02d/%02d/%04d %02d:%02d:%02d", day(myTime), month(myTime), year(myTime), hour(myTime), minute(myTime), second(myTime) );     
      message += String(buff) ;
      message += "," + String(MySolarLog.Temp)  ; 
      message += "," + String(MySolarLog.Wind)  ; 
      message += "," + String(MySolarLog.Pressure)  ; 
      message += "," + String(MySolarLog.XAngle)  ; 
      message += "," + String(MySolarLog.YAngle)  ; 
      message += "," + String(MySolarLog.XTarget)  ; 
      message += "," + String(MySolarLog.YTarget)  ; 
      message += "\r\n" ;
      server.sendContent(message) ;
      message = "" ;
    }
    message += "\r\n\r\n" ;
    server.sendContent(message) ;
    message = "" ;    
  }else{   // not downloading send it user as HTML
    SendHTTPHeader();
    message = F("<br><b>Solar Logging</b><br><a href='/?command=999'>Reset All Solar Log to Zero</a><br><table border=1 title='Solar Logging'>") ;
    
    for ( j = 0 ; j < SOLAR_MAX_LOG ; j++){
      rtceeprom.eeprom_read(j*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // write blocks   
      myTime = MySolarLog.RecDate ;
      snprintf(buff, BUFF_MAX, "%02d/%02d/%04d %02d:%02d:%02d", day(myTime), month(myTime), year(myTime), hour(myTime), minute(myTime), second(myTime) );     
      message += "<tr><td align=center>"+  String(buff) +"</td>" ;
      message += "<td>" + String(MySolarLog.Temp) + "</td>"  ; 
      message += "<td>" + String(MySolarLog.Wind) + "</td>" ; 
      message += "<td>" + String(MySolarLog.Pressure) + "</td>" ; 
      message += "<td>" + String(MySolarLog.XAngle)  + "</td>" ; 
      message += "<td>" + String(MySolarLog.YAngle)  + "</td>" ; 
      message += "<td>" + String(MySolarLog.XTarget) +"</td>" ; 
      message += "<td>" + String(MySolarLog.YTarget) + "</td>";       
      message += "</tr>" ;
      server.sendContent(message) ; 
      message = "" ;
    }
    
    message = "<tr><th colspan=3><a href='/?command=998'>Read Valve Log from EEPROM</a></th><th colspan=3><form method='GET' action=" + server.uri() + ".csv action='' enctype='multipart/form-data'><input type='hidden' name='download' value='doit'><input type='submit' value='Download'></form></th><th colspan=2><a href='/?command=997'>Write Valve Log to EEPROM</a><br></th></tr>" ;
    message += "</table><br>" ;
    server.sendContent(message) ; 
    SendHTTPPageFooter() ;
  }
}


void DisplaySolarChart(){
  int i , ii , iTmp , iX ;
  int j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];
  time_t prev_time;
  solar_log_t  MySolarLog ;  

  SendHTTPHeader();

  server.sendContent(F("<center>\r\n<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\r\n"));  
  server.sendContent(F("\r\n<script type=\"text/javascript\">\r\n"));      
  server.sendContent(F("google.charts.load('current', {'packages':['corechart']});\r\n"));  // Load the Visualization API and the piechart package.    
  server.sendContent(F("google.charts.setOnLoadCallback(drawChart);\r\n"));                 // Set a callback to run when the Google Visualization API is loaded.
  server.sendContent(F("function drawChart() {\r\n"));
  server.sendContent(F("var data = google.visualization.arrayToDataTable([[{label: 'Time', type: 'datetime'},{label: 'Temperature', type: 'number'},{label: 'Wind Speed', type: 'number'},{label: 'Pressure', type: 'number'},{label: 'Actual X', type: 'number'},{label: 'Actaul Y', type: 'number'},{label: 'Target X', type: 'number'},{label: 'Target Y', type: 'number'}],\r\n"));
  ii = (hour() * 4) +  ( minute() / 15 )+1;
  rtceeprom.eeprom_read(ii*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // write blocks   
  prev_time = SolarLog.RecDate ;
  for ( i = 0 ; i < SOLAR_MAX_LOG ; i++ ) {
    rtceeprom.eeprom_read(j*sizeof(solar_log_t),(byte *)&SolarLog,sizeof(solar_log_t)) ; // write blocks   
    j = (i + ii ) % SOLAR_MAX_LOG ;
    snprintf(buff, BUFF_MAX, "new Date(\'%4d-%02d-%02dT%02d:%02d:%02d\')", year(SolarLog.RecDate), month(SolarLog.RecDate), day(SolarLog.RecDate) , hour(SolarLog.RecDate), minute(SolarLog.RecDate), second(SolarLog.RecDate));    
    if (( !isnan(MySolarLog.Temp)) && (SolarLog.RecDate>=prev_time)){
      server.sendContent("[ " + String(buff) + "," + String(MySolarLog.Temp) + "," + String(MySolarLog.Wind) + "," + String(MySolarLog.Pressure) + "," + String(MySolarLog.XAngle) + "," + String(MySolarLog.YAngle) + "," + String(MySolarLog.XTarget) + "," + String(MySolarLog.YTarget) + " ] ,\r\n" ) ; 
      prev_time = SolarLog.RecDate ;
    }
  }
  server.sendContent(F("]);\r\n"));
     
  server.sendContent(F("var options = {title: 'System Logs 15 min intervals for last 24 Hours' , vAxis:{viewWindow:{ max: 60, min: 0}} , height: 700 , opacity:100 , interpolateNulls:true , colors: ['Red','Blue','Green','Yellow','Cyan','Orange','Magenta'], backgroundColor: '#FFFFFF', "));  // Set chart options
  server.sendContent(F("  };\r\n"));

  server.sendContent(F("var chart = new google.visualization.LineChart(document.getElementById('linechart'));\r\n"));
  server.sendContent(F("chart.draw(data, options); } </script>\r\n"));
  server.sendContent(F("<div id='linechart'></div><br>\r\n"));                                                       //  style='width:1000; height:800'
  
  SendHTTPPageFooter();
}



String DayString(int i, bool bLong){
  switch(i){
    case 1:
      if ( bLong ){
        return("Sunday");
      }else{
        return("Sun");
      }
    break;
    case 2:
      if ( bLong ){
        return("Monday");
      }else{
        return("Mon");
      }
    break;
    case 3:
      if ( bLong ){
        return("Tuesday");
      }else{
        return("Tue");
      }
    break;
    case 4:
      if ( bLong ){
        return("Wednesday");
      }else{
        return("Wed");
      }
    break;
    case 5:
      if ( bLong ){
        return("Thursday");
      }else{
        return("Thr");
      }
    break;
    case 6:
      if ( bLong ){
        return("Friday");
      }else{
        return("Fri");
      }
    break;
    case 7:
      if ( bLong ){
        return("Saturday");
      }else{
        return("Sat");
      }
    break;
    default:
      if ( bLong ){
        return(" -- * -- ");
      }else{
        return("-*-");
      }
    break;
  }
}

String MonthString(int i, bool bLong){
  switch(i){
    case 1:
      if ( bLong ){
        return("January");
      }else{
        return("Jan");
      }
    break;
    case 2:
      if ( bLong ){
        return("February");
      }else{
        return("Feb");
      }
    break;
    case 3:
      if ( bLong ){
        return("March");
      }else{
        return("Mar");
      }
    break;
    case 4:
      if ( bLong ){
        return("April");
      }else{
        return("Apr");
      }
    break;
    case 5:
      if ( bLong ){
        return("May");
      }else{
        return("May");
      }
    break;
    case 6:
      if ( bLong ){
        return("June");
      }else{
        return("Jun");
      }
    break;
    case 7:
      if ( bLong ){
        return("July");
      }else{
        return("Jul");
      }
    break;
    case 8:
      if ( bLong ){
        return("August");
      }else{
        return("Aug");
      }
    break;
    case 9:
      if ( bLong ){
        return("Spetember");
      }else{
        return("Sep");
      }
    break;
    case 10:
      if ( bLong ){
        return("October");
      }else{
        return("Oct");
      }
    break;
    case 11:
      if ( bLong ){
        return("November");
      }else{
        return("Nov");
      }
    break;
    case 12:
      if ( bLong ){
        return("December");
      }else{
        return("Dec");
      }
    break;
    default:
      if ( bLong ){
        return(" -- * -- ");
      }else{
        return("-*-");
      }
    break;
  }
}
