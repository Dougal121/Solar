void datalog1_page(){
  int i , ii , iTmp , iX ;
  int j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6] ;
  time_t prev_time ;
  bool  bDownLoad = false  ;  

  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("download");
    if (i != -1){  // 
      bDownLoad = true ;
    } 
  }

  if ( bDownLoad ){ 
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/csv; charset=utf-8", "");  // application/octet-stream
    message = "Date,Temp,Pres,RSSI,EW Angle,NS Angle,EW Target, NS Target" ;
    for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
      message += " ," + String(adcs.chan[i].ADC_Description) ;      
    }
    message += "\r\n" ;
    message += " ,(c),(mBar),(dB),(Deg),(Deg),(Deg),(Deg)" ;
    for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
      message += " ," + String(adcs.chan[i].ADC_Unit) ;      
    }
    message += "\r\n" ;
    server.sendContent(message) ;
    for ( i = 0 ; i < MAX_LOG ; i++ ) {
      j = (i + ii ) % MAX_LOG ;
      prev_time = DataLog[j].RecTime ;
      snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
      message = String(buff) + "," + String(DataLog[j].Temp) + "," + String(DataLog[j].Pres) + "," + String(DataLog[j].RSSI) + "," + String(DataLog[j].EWAngle) + "," + String(DataLog[j].NSAngle) + "," + String(DataLog[j].EWTarget) + "," + String(DataLog[j].NSTarget); 
      for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
        message += " ," + String(DataLog[j].ADCValue[i]) ;      
      }
      message += "\r\n" ;
      server.sendContent(message) ;
    }
    message += "\r\n\r\n" ;
    server.sendContent(message) ;    
  }else{
    SendHTTPHeader();
    message = F("<table border=1 title='Data Log'>\r\n");
    message += "<tr><th>Date</th><th>Temperature<br>(C)</th><th>Pressure<br>(mBar)</th><th>RSSI<br>(dB)</th><th>E/W Angle<br>(Deg)</th><th>N/S Angle<br>(Deg)</th><th>E/W Target<br>(Deg)</th><th>N/S Target<br>(Deg)</th>";
    for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
      message += "<th>" + String(adcs.chan[i].ADC_Description) + "<br>(" + String(adcs.chan[i].ADC_Unit) +")</th>" ;      
    }
    message += F( "</tr>\r\n" ) ; 
    server.sendContent(message) ;    
    ii = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) )+1;
    prev_time = previousMidnight(now()) - (( MAX_LOG - ii ) * ( 60 / LOG_PER_HOUR ) * 60 );
  
    for ( i = 0 ; i < MAX_LOG ; i++ ) {
      j = (i + ii ) % MAX_LOG ;
      prev_time = DataLog[j].RecTime ;
      snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
        message = "<tr><td>" + String(buff) + "</td><td>" + String(DataLog[j].Temp) + "</td><td>" + String(DataLog[j].Pres) + "</td><td>" + String(DataLog[j].RSSI) + "</td><td>" + String(DataLog[j].EWAngle) + "</td><td>" + String(DataLog[j].NSAngle) + "</td><td>" + String(DataLog[j].EWTarget) + "</td><td>" + String(DataLog[j].NSTarget) + "</td>";
        for ( kk = 0 ; kk < ADC_MAX_CHAN ; kk++ ) {
          message += "<td>" + String(DataLog[j].ADCValue[kk]) + "</td>" ;      
        }        
        message += F( "</tr>\r\n" ) ; 
        server.sendContent(message) ;    
      prev_time += ((60/LOG_PER_HOUR) * 60 ) ;
    }
    server.sendContent("<tr><td colspan=8><form method='GET' action=" + server.uri() + ".csv action='' enctype='multipart/form-data'><input type='hidden' name='download' value='doit'><input type='submit' value='Download'></form></td></tr>");
    server.sendContent(F("</table><br>\r\n"));    
    SendHTTPPageFooter();
  }
}




void chart1_page(){
  int i , ii , iTmp , iX ;
  int j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];
  time_t prev_time;

  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  
      switch (String(server.arg(j)).toInt()){
        case 73:  
          clearDataLog();
        break;
        case 74:  
          WriteDataLogsToEEPROM();
        break;
        case 75:  
          ReadDataLogsFromEEPROM();
        break;
      }
    }
  }
  
  SendHTTPHeader();

  message += F("<center>\r\n<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\r\n");  
  message += F("\r\n<script type=\"text/javascript\">\r\n");      
  message += F("google.charts.load('current', {'packages':['corechart']});\r\n");  // Load the Visualization API and the piechart package.    
  message += F("google.charts.setOnLoadCallback(drawChart);\r\n");                 // Set a callback to run when the Google Visualization API is loaded.
  message += F("function drawChart() {\r\n");
  message += F("var data = google.visualization.arrayToDataTable([[{label: 'Time', type: 'datetime'},{label: 'Temperature (c)', type: 'number'},{label: 'Pressure (mBar)', type: 'number'},{label: 'RSSI (dB)', type: 'number'},{label: 'EW Angle (Deg)', type: 'number'},{label: 'NS Angle (Deg)', type: 'number'},{label: 'EW Target (Deg)', type: 'number'},{label: 'NS Target (Deg)', type: 'number'}");
  for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
    message += ",{label: '" + String(adcs.chan[i].ADC_Description) + " (" + String(adcs.chan[i].ADC_Unit) + ")', type: 'number'}" ;      
  }
  message += F("],\r\n");
  server.sendContent(message);
  message = "" ;

  ii = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) )+1;
  prev_time = previousMidnight(now()) - (( MAX_LOG - ii ) * (60 / LOG_PER_HOUR ) * 60 );
  
  for ( i = 0 ; i < MAX_LOG ; i++ ) {
    j = (i + ii ) % MAX_LOG ;
    prev_time = DataLog[j].RecTime ;
    snprintf(buff, BUFF_MAX, "new Date(\'%4d-%02d-%02dT%02d:%02d:%02d\')", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
    if ( !isnan(DataLog[j].Temp)){
      message += "[ " + String(buff) + "," + String(DataLog[j].Temp) + "," + String(DataLog[j].Pres) + "," + String(DataLog[j].RSSI) + "," + String(DataLog[j].EWAngle) + "," + String(DataLog[j].NSAngle) + "," + String(DataLog[j].EWTarget) + "," + String(DataLog[j].NSTarget)  ; 
      for ( kk = 0 ; kk < ADC_MAX_CHAN ; kk++ ) {
        message += "," + String(DataLog[j].ADCValue[kk]) ;
      }
      message += F( " ] ,\r\n" ) ;    
    }
    prev_time += (( 60 / LOG_PER_HOUR ) * 60 );
    if (( i % 10) == 9 ){
      server.sendContent(message);
      message = "" ;      
    }
  }
  message += F("]);\r\n");
  server.sendContent(message);
  message = "" ;
     
  message += F("var options = {title: 'System Logs 5 min intervals for last 24 Hours' , series: {0: {targetAxisIndex: 0},1: {targetAxisIndex: 0},2: {targetAxisIndex: 0},3: {targetAxisIndex: 1},4: {targetAxisIndex: 1},5: {targetAxisIndex: 1},6: {targetAxisIndex: 1}}, vAxes: { 0: {title: 'Temp,Pres,RSSI', viewWindow:{ max: 50, min: -120}}, 1: {title: 'Angle', viewWindow:{ max: 90, min: -90}}} , height: 700 , opacity:100 , interpolateNulls:true , colors: ['#ff0000','#009900','#000000','#003399','#3333ff','#ff0066','#660066'], backgroundColor: '#FFFFFF', ");  // Set chart options
  message += F("  };\r\n");

  message += F("var chart = new google.visualization.LineChart(document.getElementById('linechart'));\r\n");
  message += F("chart.draw(data, options); } </script>\r\n");
  message += F("<div id='linechart'></div><br>\r\n");                                          //  style='width:1000; height:800'
  message += "<a href="+ server.uri() + "/chart?command=73>Clear Logged Data</a><br>";
  message += "<a href="+ server.uri() + "/chart?command=74>Save Data to RTC EEPROM</a><br>\r\n";
  message += "<a href="+ server.uri() + "/chart?command=75>Load Data from RTC EEPROM</a><br>\r\n";
  server.sendContent(message);
  message = "" ;
  
  SendHTTPPageFooter();
}

void clearDataLog(){
  for ( int i = 0 ; i < MAX_LOG ; i++ ) {
    DataLog[i].RecTime = previousMidnight(now()) - (( MAX_LOG - i ) * (( 60 / LOG_PER_HOUR ) * 60)) ;           
    DataLog[i].Temp = 0 ;               
    DataLog[i].Pres = 0 ;               
    DataLog[i].RSSI = 0 ;               
    DataLog[i].EWAngle = 0 ;             
    DataLog[i].NSAngle = 0 ;             
    DataLog[i].EWTarget = 0 ;            
    DataLog[i].NSTarget = 0 ;    
    for (int k = 0 ; k < ADC_MAX_CHAN ; k++ ) {
      DataLog[i].ADCValue[k] = 0 ;
    }
  }  
  bDataLogDirty = false ; 
}

void ReadDataLogsFromEEPROM(void){
  long lStart = millis();
  if ( hasRTC ){
    rtceeprom.eeprom_read(0,(byte *)&DataLog,sizeof(DataLog)) ; // read in each block
    snprintf(buff, BUFF_MAX, "%02d/%02d/%04d", day(), month(), year() );     
    Serial.println(String(buff) + " RTC EEPROM Read Time " + String((millis()-lStart)/1000,3) + " (s)");
    bDataLogDirty = false ; 
  }else{
    Serial.println("No RTC - No EEPROM to read");    
  }
  return;
}
void WriteDataLogsToEEPROM(void){
  long lStart = millis();
  if ( hasRTC && bDataLogDirty ){
    rtceeprom.eeprom_write(0,(byte *)&DataLog,sizeof(DataLog)) ; // write blocks
    snprintf(buff, BUFF_MAX, "%02d/%02d/%04d", day(), month(), year() );     
    Serial.println(String(buff) + " RTC EEPROM Write Time " + String((millis()-lStart)/1000,3) + " (s)");
    bDataLogDirty = false ; 
  }else{
    if ( bDataLogDirty ){
      Serial.println("No RTC - No EEPROM to write");        
    }else{
      Serial.println("Log Clean Nothing worth writting to EEPROM");        
    }
  }
  return;  
}

