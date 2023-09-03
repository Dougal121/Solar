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
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  
      switch (String(server.arg(j)).toInt()){
        case 73:  
          clearDataLog();
        break;
        case 74:  
          bDataLogDirty = true ;
          WriteDataLogsToEEPROM();
        break;
        case 75:  
          ReadDataLogsFromEEPROM();
        break;
      }
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
    ii = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) )+1;    
    for ( i = 0 ; i < MAX_LOG ; i++ ) {
      j = (i + ii ) % MAX_LOG ;
      prev_time = DataLog[j].RecTime ;
      if (prev_time != 0 ){
        snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
        message = String(j) + "," + String(buff) + "," + String(DataLog[j].Temp) + "," + String(DataLog[j].Pres) + "," + String(DataLog[j].RSSI) + "," + String(DataLog[j].EWAngle) + "," + String(DataLog[j].NSAngle) + "," + String(DataLog[j].EWTarget) + "," + String(DataLog[j].NSTarget); 
        for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
          message += " ," + String(DataLog[j].ADCValue[i]) ;      
        }
        message += "\r\n" ;
        server.sendContent(message) ;
      }
    }
    message += "\r\n\r\n" ;
    server.sendContent(message) ;    
  }else{
    SendHTTPHeader();
    message = F("<table border=1 title='Data Log'>\r\n");
    message += "<tr><th>Log No</th><th>Date</th><th>Temperature<br>(C)</th><th>Pressure<br>(mBar)</th><th>RSSI<br>(dB)</th><th>E/W Angle<br>(Deg)</th><th>N/S Angle<br>(Deg)</th><th>E/W Target<br>(Deg)</th><th>N/S Target<br>(Deg)</th>";
    for ( i = 0 ; i < ADC_MAX_CHAN ; i++ ) {
      message += "<th>" + String(adcs.chan[i].ADC_Description) + "<br>(" + String(adcs.chan[i].ADC_Unit) +")</th>" ;      
    }
    message += F( "</tr>\r\n" ) ; 
    server.sendContent(message) ;    
    ii = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) )+1;
//    prev_time = previousMidnight(now()) - (( MAX_LOG - ii ) * ( 60 / LOG_PER_HOUR ) * 60 );
  
    for ( i = 0 ; i < MAX_LOG ; i++ ) {
      j = (i + ii ) % MAX_LOG ;
      prev_time = DataLog[j].RecTime ;
      if (prev_time != 0 ){
        snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
        message = "<tr><td>"+String(j)+"</td><td>" + String(buff) + "</td><td>" + String(DataLog[j].Temp) + "</td><td>" + String(DataLog[j].Pres) + "</td><td>" + String(DataLog[j].RSSI) + "</td><td>" + String(DataLog[j].EWAngle) + "</td><td>" + String(DataLog[j].NSAngle) + "</td><td>" + String(DataLog[j].EWTarget) + "</td><td>" + String(DataLog[j].NSTarget) + "</td>";
        for ( kk = 0 ; kk < ADC_MAX_CHAN ; kk++ ) {
          message += "<td>" + String(DataLog[j].ADCValue[kk]) + "</td>" ;      
        }        
        message += F( "</tr>\r\n" ) ; 
        server.sendContent(message) ;    
      }
//      prev_time += ((60/LOG_PER_HOUR) * 60 ) ;
    }
    server.sendContent("<tr><td colspan=15><form method='GET' action=" + server.uri() + ".csv action='' enctype='multipart/form-data'><input type='hidden' name='download' value='doit'><input type='submit' value='Download'></form></td></tr>");
    server.sendContent(F("</table><br>\r\n"));    
    message = "<a href="+ server.uri() + "?command=73>Clear Logged Data</a><br>";
    message += "<a href="+ server.uri() + "?command=74>Save Data to RTC EEPROM</a><br>\r\n";
    message += "<a href="+ server.uri() + "?command=75>Load Data from RTC EEPROM</a><br>\r\n";
    server.sendContent(message) ;    
    SendHTTPPageFooter();
  }
}




void chart1_page(){
  int i , ii , iTmp , iX ;
  int j , k , kk = 0;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];
  boolean bFirst = true ;
  time_t prev_time;
  time_t min_start;

  for (uint8_t j=0; j<server.args(); j++){
    k = String(server.arg(j)).toInt() ;
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  
      switch (String(server.arg(j)).toInt()){
        case 73:  
          clearDataLog();
        break;
        case 74:  
          bDataLogDirty = true ;
          WriteDataLogsToEEPROM();
        break;
        case 75:  
          ReadDataLogsFromEEPROM();
        break;
        case 78:
          clearChartData();
        break;
      }
    }
    i = String(server.argName(j)).indexOf("ct");
    if (i != -1){  
      kk = k ;
    }
    
    i = String(server.argName(j)).indexOf("cl");
    if (i != -1){  
      chart.trace[kk].Color = strtol(server.arg(j).c_str(), NULL, 16);
    }
    i = String(server.argName(j)).indexOf("dis");
    if (i != -1){  
      chart.trace[kk].Display = k ;
    }
    i = String(server.argName(j)).indexOf("ax");
    if (i != -1){  
      chart.trace[kk].Axis = k ;
    }
    i = String(server.argName(j)).indexOf("maxy0");
    if (i != -1){  
      chart.YMax[0] = k ;
    }
    i = String(server.argName(j)).indexOf("miny0");
    if (i != -1){  
      chart.YMin[0] = k ;
    }
    i = String(server.argName(j)).indexOf("maxy1");
    if (i != -1){  
      chart.YMax[1] = k ;
    }
    i = String(server.argName(j)).indexOf("miny1");
    if (i != -1){  
      chart.YMin[1] = k ;
    }
    i = String(server.argName(j)).indexOf("bgcol");
    if (i != -1){  
      chart.BGColor = strtol(server.arg(j).c_str(), NULL, 16);
    }
    i = String(server.argName(j)).indexOf("maxx");
    if (i != -1){  
      chart.XMax = k ;
    }
    i = String(server.argName(j)).indexOf("minx");
    if (i != -1){  
      chart.XMin = k ;
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
//  prev_night = previousMidnight(now()) ; // - (( MAX_LOG - ii ) * (60 / LOG_PER_HOUR ) * 60 );
  min_start = now() - ( 24 * 3600 ) ;
  
  for ( k = i = 0 ; i < MAX_LOG ; i++ ) {
    j = (i + ii ) % MAX_LOG ;
    if (( min_start < DataLog[j].RecTime ) && (DataLog[j].RecTime < now())){
      prev_time = DataLog[j].RecTime ;
      snprintf(buff, BUFF_MAX, "new Date(\'%4d-%02d-%02dT%02d:%02d:%02d\')", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
      if ( !isnan(DataLog[j].Temp)){
        message += "[ " + String(buff) + "," + String(DataLog[j].Temp) + "," + String(DataLog[j].Pres/10) + "," + String(DataLog[j].RSSI) + "," + String(DataLog[j].EWAngle) + "," + String(DataLog[j].NSAngle) + "," + String(DataLog[j].EWTarget) + "," + String(DataLog[j].NSTarget)  ; 
        for ( kk = 0 ; kk < ADC_MAX_CHAN ; kk++ ) {
          message += "," + String(DataLog[j].ADCValue[kk]) ;
        }
        message += F( " ] ,\r\n" ) ;    
      }
      if (( k++ % 10) == 9 ){
        server.sendContent(message);
        message = "" ;      
      }
    }
  }
  message += F("]);\r\n");
  server.sendContent(message);
  message = "var view = new google.visualization.DataView(data);\r\n" ;
  message += "view.setColumns([";
  for (  i = 0 ; i < ADC_MAX_CHAN+7 ; i++ ) {
    if ((chart.trace[i].Display % 2) == 1){
      if ( bFirst ){
        bFirst = false ;
      }else{
        message += ",";  
      }
      message += String(i);      
    }
  }
  message += "]);\r\n" ;     
  message += F("var options = {title: 'System Logs 5 min intervals for last 24 Hours' , series: {");
  for (  i = 0 ; i < ADC_MAX_CHAN+7 ; i++ ) {
    MyNum = ((chart.trace[i].Display % 2) == 1) ? "true" : "false";    
    message += String(i) + ": {targetAxisIndex: " + String(chart.trace[i].Axis % 2) + ", visibleInLegend: "+ MyNum + " }";
    if ( i < ADC_MAX_CHAN+6 ) 
      message += "," ;
  }
  message += F("}, vAxes: { 0: {title: 'Temp,Pres,RSSI', viewWindow:{");
  message += "max: "+String(chart.YMax[0])+", min: "+String(chart.YMin[0]);
  message += F("}}, 1: {title: 'Angle', viewWindow:{ ");
  message += "max: "+String(chart.YMax[1])+", min: "+String(chart.YMin[1]);
  message += F("}}} , height: 700 , opacity:100 , interpolateNulls:true , colors: [");
  bFirst = true ;
  for (  i = 0 ; i < ADC_MAX_CHAN+7 ; i++ ) {
    if ((chart.trace[i].Display % 2) == 1){
      if ( bFirst ){
        bFirst = false ;
      }else{
        message += ",";  
      }
      message += FormatColor(chart.trace[i].Color,"#") ;
    }
  }
  message += F("],");
  message += "backgroundColor: " + FormatColor (chart.BGColor , "#" )+  ", " ;  // Set chart options
  message += F("  };\r\n");

  message += F("var chart = new google.visualization.LineChart(document.getElementById('linechart'));\r\n");
  message += F("chart.draw(view, options); } </script>\r\n");
  message += F("<div id='linechart'></div>\r\n");                                          //  style='width:1000; height:800'
  server.sendContent(message);

  
  message = F("<table border=1 title='Chart Setup'>\r\n");
  message += "<tr><th>No</th><th>Trace</th><th>Show</th><th>Y Axis</th><th>Color</th><th>.</th></tr>\r\n";
  for (  i = 0 ; i < ADC_MAX_CHAN+7 ; i++ ) {
    if ( i < 7 ){
      switch(i){
        case 0:  MyColor = "Temp (C)" ; break;
        case 1:  MyColor = "Pres (mBar)" ; break;
        case 2:  MyColor = "RSSI (dB)" ; break;
        case 3:  MyColor = "EW Ang (Deg)" ; break;
        case 4:  MyColor = "NS Ang (Deg)" ; break;
        case 5:  MyColor = "EW Tar (Deg)" ; break;
        case 6:  MyColor = "NS Tar (Deg)" ; break;
      }
    }
    else{
      MyColor = String(adcs.chan[i-7].ADC_Description) + " " + String(adcs.chan[i-7].ADC_Unit);
    }
    message += "<tr><form method=get action=" + server.uri() + "><input type='hidden' name='ct' value='"+String(i)+"'><td bgcolor=" + FormatColor(chart.trace[i].Color,"#") + "><font color="+FormatColor(chart.trace[i].Color ^ 0xffffff ,"#")+">"+String(i)+"</font></td>";
    message += "<td>"+MyColor+"</td>";
    message += F("<td><select name='dis'>") ;
    if ( chart.trace[i].Display == 0 ){
      message += F("<option value='0' SELECTED>0 - Hide"); 
      message += F("<option value='1'>1 - Display"); 
    }else{
      message += F("<option value='0'>0 - Hide"); 
      message += F("<option value='1' SELECTED>1 - Display"); 
    }
    message += "</select></td>";
    message += F("<td><select name='ax'>") ;
    if ( chart.trace[i].Axis == 0 ){
      message += F("<option value='0' SELECTED>0 - Left"); 
      message += F("<option value='1'>1 - Right"); 
    }else{
      message += F("<option value='0'>0 - Left"); 
      message += F("<option value='1' SELECTED>1 - Right"); 
    }
    message += "</select></td>";
    message += "<td><input type='text' name='cl' value='"+String(chart.trace[i].Color,HEX)+"'></td>";
    message += "<td><input type='submit' value='SET'></form></td></tr>\r\n";   // 
  }
  message += "</table>\r\n<br><table border=1 title='Chart Setup'>\r\n" ;
  server.sendContent(message);
  message = "<tr><th>Parameter</th><th>Value</th><th>.</th></tr>\r\n";
  message += "<form method=get action=" + server.uri();
  message += F( "><tr><td>Max Left Scale</td><td align=center><input type='text' name='maxy0' value='") ; 
  message += String(chart.YMax[0],2);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 
  message += "<form method=get action=" + server.uri();
  message += F("><tr><td>Y Min Left Scale</td><td align=center><input type='text' name='miny0' value='") ; 
  message += String(chart.YMin[0],2);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += "<form method=get action=" + server.uri();
  message += F("><tr><td>Y Max Left Scale</td><td align=center><input type='text' name='maxy1' value='") ; 
  message += String(chart.YMax[1],2);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 
  message += "<form method=get action=" + server.uri();
  message += F("><tr><td>Y Min Left Scale</td><td align=center><input type='text' name='miny1' value='") ; 
  message += String(chart.YMin[1],2);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += "<form method=get action=" + server.uri();
  message += F("><tr><td>X Max Time Scale</td><td align=center><input type='text' name='maxx' value='") ; 
  message += String(chart.XMax);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 
  message += "<form method=get action=" + server.uri();
  message += F("><tr><td>X Min Time Scale</td><td align=center><input type='text' name='minx' value='") ; 
  message += String(chart.XMin);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += "<form method=get action=" + server.uri();
  message += "><tr><td bgcolor=" + FormatColor(chart.BGColor,"#") + "><font color="+FormatColor(chart.trace[i].Color ^ 0xffffff ,"#")+">Background Color</td><td align=center><input type='text' name='bgcol' value='" ; 
  message += String(chart.BGColor,HEX);
  message += F("' size=5></td><td><input type='submit' value='SET'></td></tr></form>\r\n") ; 

  message += "</table>\r\n" ;
  server.sendContent(message);
  
  message = "<br><a href="+ server.uri() + "?command=78>Clear Chart Setup</a><br>";
  message += "<a href="+ server.uri() + "?command=73>Clear Logged Data</a><br>";
  message += "<a href="+ server.uri() + "?command=74>Save Data to RTC EEPROM</a><br>\r\n";
  message += "<a href="+ server.uri() + "?command=75>Load Data from RTC EEPROM</a><br>\r\n";
  server.sendContent(message);
  
  SendHTTPPageFooter();
}

void DoDataLog(){
  int i , k ;
  if (((minute() % 5) == 0 )) { // data logging
    i = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) ) ;
    DataLog[i].RecTime = now() ;
    DataLog[i].Temp = tv.gT ;               
    DataLog[i].Pres = tv.Pr ;                
    DataLog[i].RSSI = WiFi.RSSI() ;               
    DataLog[i].EWAngle = tv.yzAng ;            
    DataLog[i].NSAngle = tv.xzAng ;             
    DataLog[i].EWTarget = tv.yzTarget ;            
    DataLog[i].NSTarget = tv.xzTarget ;   
    for ( k = 0 ; k < ADC_MAX_CHAN ; k++ ) {
      DataLog[i].ADCValue[k] = adcs.chan[k].ADC_Value ;
    }    
    bDataLogDirty = true ;             
  }      
}

void clearDataLog(){
  for ( int i = 0 ; i < MAX_LOG ; i++ ) {
    DataLog[i].RecTime = previousMidnight(now()) +  (i*( 60 / LOG_PER_HOUR ) * 60) ;           
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

void clearChartData(){
  chart.BGColor = 0xFFFFFF ;
  chart.XMax = now();
  chart.XMin = now() - SECS_PER_DAY ;
  chart.YMax[0] = 50;
  chart.YMin[0] = -120;
  chart.YMax[1] = 90;
  chart.YMin[1] = -90;
  for ( int i = 0 ; i < (ADC_MAX_CHAN+7) ; i++ ){     
    chart.trace[i].Display = 1 ;
    if (( i > 2 ) && ( i < 7 )){
      chart.trace[i].Axis = 1 ;  
    }else{
      chart.trace[i].Axis = 0 ;  
    }  
    switch(i){
      case 0: chart.trace[i].Color = 0xFF0000 ; break ;   // Temp
      case 1: chart.trace[i].Color = 0x009900 ; break ;   // Pres
      case 2: chart.trace[i].Color = 0x000000 ; break ;   // RSSI
      case 3: chart.trace[i].Color = 0x003399 ; break ;   // E/W Angle
      case 4: chart.trace[i].Color = 0x3333FF ; break ;   // N/S Angle
      case 5: chart.trace[i].Color = 0xFF0066 ; break ;   // E/W Target
      case 6: chart.trace[i].Color = 0x660066 ; break ;   // N/S Target
      case 7: chart.trace[i].Color = 0x00bfff ; break ;   // ADC Chan 0 
      case 8: chart.trace[i].Color = 0x0080ff ; break ;   // ADC Chan 1
      case 9: chart.trace[i].Color = 0x0040ff ; break ;   // ADC Chan 2
      case 10: chart.trace[i].Color = 0x0000ff ; break ;  // ADC Chan 3
      case 11: chart.trace[i].Color = 0x4000ff ; break ;  // ADC Chan 4
      case 12: chart.trace[i].Color = 0x8000ff ; break ;  // ADC Chan 5
    }
  }
}

void ReadDataLogsFromEEPROM(void){
  long lStart = millis();
  if ( hasRTC ){
    rtceeprom.eeprom_read(0,(byte *)&DataLog,4096) ; // read in each block
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
    rtceeprom.eeprom_write(0,(byte *)&DataLog,4096) ; // write blocks
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

void DisplayRTCEEPROM() {
  uint8_t i[56] ;
  uint16_t ii[28] ;
  uint32_t iiii[14] ;
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
    for (k = 0; k < 56; k+=d) {
      message += "<th>"+String(k,HEX)+"</th>";
    }
    message += F("</tr>");
    server.sendContent(message);
    message = "" ;
    for (address = 0; address < iLen  ; address+=d ) {
      if (address % 56 == 0) {
        message += F("<tr>");  // start the line 
        message += "<td align=center><b>"+String(((address+iAddr) & 0xFFE0),HEX)+"</b></td>";
        switch(b){  // read all the data
          case 16:
            rtceeprom.eeprom_read(address+iAddr,(byte *)&ii,56);
          break;
          case 32:
            rtceeprom.eeprom_read(address+iAddr,(byte *)&iiii,56);
          break;
          default: // byte 8
            rtceeprom.eeprom_read(address+iAddr,(byte *)&i,56);
          }  
      }
      switch(b){
        case 16:
  //        rtceeprom.eeprom_read(address,(byte *)&ii,56);
          switch(r){
            case 8:
              message += "<td>"+String(ii[(address % 56)/2],OCT)+"</td>";
            break;
            case 10:
              message += "<td>"+String(ii[(address % 56)/2],DEC)+"</td>";
            break;
            case 2:
              message += "<td>"+String(ii[(address % 56)/2],BIN)+"</td>";
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
              message += "<td>"+String(iiii[(address % 56)/4],OCT)+"</td>";
            break;
            case 10:
              message += "<td>"+String(iiii[(address % 56)/4],DEC)+"</td>";
            break;
            case 2:
              message += "<td>"+String(iiii[(address % 56)/4],BIN)+"</td>";
            break;
            default:
              message += "<td>"+String(iiii[(address % 56)/4],HEX)+"</td>";
            break;
          }  
        break;
        default: // byte 8
  //        rtceeprom.eeprom_read(address,(byte *)&i,56);
          switch(r){
            case 8:
              message += "<td>"+String(i[address % 56],OCT)+"</td>";
            break;
            case 10:
              message += "<td>"+String(i[address % 56],DEC)+"</td>";
            break;
            case 2:
              message += "<td>"+String(i[address % 56],BIN)+"</td>";
            break;
            case 1:   //ascii
              if ( isPrintable(char(i[address % 56])) ){
                message += "<td>"+String(char(i[address % 56]))+"</td>";
              }else{
                message += "<td>-</td>";              
              }
            break;
            default:
              message += "<td>"+String(i[address % 56],HEX)+"</td>";
            break;
          }  
        break;
      }
      if (address % 56 == 55) {
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

String FormatColor ( long lCol , String PreFix ){
    snprintf(buff, BUFF_MAX, "%06X",lCol );      
    return( "'"  + PreFix + String(buff) + "'" ) ;  
}

