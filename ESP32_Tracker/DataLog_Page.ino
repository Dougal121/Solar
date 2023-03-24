void datalog1_page(){
  int i , ii , iTmp , iX ;
  int j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];
  time_t prev_time;
    
  SendHTTPHeader();

  server.sendContent(F("<br><center><b>Data Log</b><br>\r\n"));
  server.sendContent(F("<table border=1 title='Data Log'>\r\n"));
  server.sendContent("<tr><th>Date</th><th>Temperature<br>(C)</th><th>Pressure<br>(mBar)</th><th>RSSI<br>(dB)</th><th>E/W Angle<br>(Deg)</th><th>N/S Angle<br>(Deg)</th><th>E/W Target<br>(Deg)</th><th>N/S Target<br>(Deg)</th></tr>\r\n" ) ; 
  ii = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) )+1;
  prev_time = previousMidnight(now()) - (( MAX_LOG - ii ) * ( 60 / LOG_PER_HOUR ) * 60 );

  for ( i = 0 ; i < MAX_LOG ; i++ ) {
    j = (i + ii ) % MAX_LOG ;
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
//    if ( !isnan(DataLog[j].Temp) ){
      server.sendContent("<tr><td>" + String(buff) + "</td><td>" + String(DataLog[j].Temp) + "</td><td>" + String(DataLog[j].Pres) + "</td><td>" + String(DataLog[j].RSSI) + "</td><td>" + String(DataLog[j].EWAngle) + "</td><td>" + String(DataLog[j].NSAngle) + "</td><td>" + String(DataLog[j].EWTarget) + "</td><td>" + String(DataLog[j].NSTarget) + "</td></tr>\r\n" ) ; 
//    }
    prev_time += ((60/LOG_PER_HOUR) * 60 ) ;
  }
  server.sendContent(F("</table><br>\r\n"));    
  SendHTTPPageFooter();
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
  
  SendHTTPHeader();

  if ( bSaveReq != 0 ){
    message += F("<blink>");      
  }   
  message += F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>") ;     
  if ( bSaveReq != 0 ){
    message += F("</blink><font color='red'><b>Changes Have been made to settings.<br>Make sure you save if you want to keep them</b><br></font><br>") ;     
  }

  message += F("<center>\r\n<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\r\n");  
  message += F("\r\n<script type=\"text/javascript\">\r\n");      
  message += F("google.charts.load('current', {'packages':['corechart']});\r\n");  // Load the Visualization API and the piechart package.    
  message += F("google.charts.setOnLoadCallback(drawChart);\r\n");                 // Set a callback to run when the Google Visualization API is loaded.
  message += F("function drawChart() {\r\n");
  message += F("var data = google.visualization.arrayToDataTable([[{label: 'Time', type: 'datetime'},{label: 'Battery Volatage', type: 'number'},{label: 'RSSI', type: 'number'}],\r\n");
  server.sendContent(message);
  message = "" ;

  ii = (hour() * LOG_PER_HOUR) +  ( minute() / (60 / LOG_PER_HOUR ) )+1;
  prev_time = previousMidnight(now()) - (( MAX_LOG - ii ) * (60 / LOG_PER_HOUR ) * 60 );
  
  for ( i = 0 ; i < MAX_LOG ; i++ ) {
    j = (i + ii ) % MAX_LOG ;
    snprintf(buff, BUFF_MAX, "new Date(\'%4d-%02d-%02dT%02d:%02d:%02d\')", year(prev_time), month(prev_time), day(prev_time) , hour(prev_time), minute(prev_time), second(prev_time));    
    if ( !isnan(DataLog[j].Temp)){
      message += "[ " + String(buff) + "," + String(DataLog[j].Temp) + "," + String(DataLog[j].RSSI) + " ] ,\r\n"  ; 
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
     
  message += F("var options = {title: 'System Logs 5 min intervals for last 24 Hours' , series: {0: {targetAxisIndex: 0},1: {targetAxisIndex: 1}}, vAxes: { 0: {title: 'Battery Volts', viewWindow:{ max: 20, min: 0}}, 1: {title: 'RSSI', viewWindow:{ max: 0, min: -120}}} , height: 700 , opacity:100 , interpolateNulls:true , colors: ['Blue','Red'], backgroundColor: '#FFFFFF', ");  // Set chart options
  message += F("  };\r\n");

  message += F("var chart = new google.visualization.LineChart(document.getElementById('linechart'));\r\n");
  message += F("chart.draw(data, options); } </script>\r\n");
  message += F("<div id='linechart'></div><br>\r\n");                                          //  style='width:1000; height:800'

  server.sendContent(message);
  message = "" ;
  
  SendHTTPPageFooter();
}

void clearDataLog(){
  for ( int i = 0 ; i < MAX_LOG ; i++ ) {
    DataLog[i].RecTime = 0 ;           
    DataLog[i].Temp = 0 ;               
    DataLog[i].Pres = 0 ;               
    DataLog[i].RSSI = 0 ;               
    DataLog[i].EWAngle = 0 ;             
    DataLog[i].NSAngle = 0 ;             
    DataLog[i].EWTarget = 0 ;            
    DataLog[i].NSTarget = 0 ;            
  }  
}

