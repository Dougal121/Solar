
String result;
String weatherDescription ="";
String weatherLocation = "";



void GetWeatherDataESP8266(){
    int i ;
    HTTPClient http;
    String url = "http://" + String(ghks.servername) + "/data/2.5/onecall?lat="+String(ghks.latitude)+"&lon="+String(ghks.longitude)+"&units=metric&cnt=1&exclude=alerts,minutely,hourly&APPID="+ghks.apikey;
    String payload = "\0" ;
    
  if (WiFi.isConnected())  {
    Serial.println(url);
    
    http.begin(url);                      //HTTP
    int httpCode = http.GET();
    
    shams.iLasthttpResponseCode = httpCode ;
    
    if (httpCode > 0) {     // httpCode will be negative on error
      
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);   // HTTP header has been send and Server response header has been handled
      
      if (httpCode == HTTP_CODE_OK) {   // file found at server
        payload = http.getString();
         payload += "\0" ;
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    Serial.println("Chars Recieved:" + String(payload.length()));
    
    DynamicJsonBuffer jb(8000);
    JsonObject &root = jb.parseObject(payload);
    if (!root.success()){
      Serial.println("parseObject() failed");
    }else{
   
      String str_TZO = root["timezone_offset"] ;
      Serial.println("Time Offset =>" + str_TZO);

      JsonObject& currentroot = root["current"];
      long MyDate = currentroot["dt"];    
      Serial.println("Time  =>" + String(MyDate));
      shams.WForecastDate = MyDate + long(SECS_PER_HOUR * ghks.fTimeZone ) ;
      JsonArray& dailyroot = root["daily"];
      
      for ( i = 0 ; i < 8 ; i++ ) {
        JsonObject& MyDaily = dailyroot[i];
        JsonObject& MyDailyTemp = MyDaily["temp"];
        long  lFC = MyDaily["dt"] ;
        float maxtemp = MyDailyTemp["max"] ;
        float mintemp = MyDailyTemp["min"] ;
        int clouds = MyDaily["clouds"];
        Serial.println(String(i) + "   " + String(lFC) + "   " + String(clouds) + "   " +String(maxtemp)+ "   " +String(mintemp));
        if (i < MAX_FORECAST_DAYS ){
          shams.WClouds[i] = clouds ;
          shams.WMaxTemp[i] = maxtemp ;
          shams.WMinTemp[i] = mintemp ;
        }
//        shams.WForecastDate = now();
      }
    }
    if (( year() < 2019 )&&(year(shams.WForecastDate) > 2019 )) {
      setTime(shams.WForecastDate);
    }
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
    if (ghks.fTimeZone > 0 ) {
      Serial.println("As at "+ String(buff) + " UTC +" + String(ghks.fTimeZone,1) ) ;   
    }else{
      Serial.println("As at "+ String(buff) + " UTC " + String(ghks.fTimeZone,1) ) ;       
    }
    Serial.println("=== END OPEN WEATHER CALL ===");
  }
}

void OpenWeatherPage(){
  int i , ii , iTmp , iX ;
  uint8_t j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];
  time_t MyTime ; 
//  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 13:  // manual get forecast
          shams.bDoGetWeather = true ;
        break;
      }  
    }   
  }
  
  SendHTTPHeader();
  MyNum = String(ghks.latitude,8) ;
  if (ghks.latitude>0){
    MyNum += " N "; 
  }else{
    MyNum += " S ";     
  }
  MyNum += String(ghks.longitude,8);
  if (ghks.longitude>0){
    MyNum += " E "; 
  }else{
    MyNum += " W ";     
  }
  message = "<br><center><b>Weather Forcast<br>" + MyNum + "</b><br>";
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second() );
  if (ghks.fTimeZone > 0 ) {
    message += "<b>"+ String(buff) + " UTC +" + String(ghks.fTimeZone,1) ;   
  }else{
    message += "<b>"+ String(buff) + " UTC " + String(ghks.fTimeZone,1) ;       
  }
  if ( year() < 2000 ) {
    message += F("<br><font color='red'>--- CLOCK NOT SET ---</font>") ;
  }
   if (shams.bDoGetWeather) {
    message += F("<br><b><i><font color='red'>Please Wait While Forcast Updates (30 Seconds)</font></b></i><b>") ;
  }
  server.sendContent(message);
  
  message = F("<table border=1 title='Weather Forcast'>");
  message +=  F("<tr><th><b>Day</th><th align=center><b>Max (C)</b></th><th align=center><b>Min (C)</b></th><th>Cloud (%)</th></tr>") ;
  for ( i = 0 ; ((i < 8) && (i < MAX_FORECAST_DAYS )) ; i++ ) {
    if ( year(shams.WForecastDate) > 2019 ) {
      MyTime = previousMidnight(shams.WForecastDate)+(SECS_PER_DAY*i) ;
    }else{
      MyTime = previousMidnight(now())+(SECS_PER_DAY*i) ;
    }
    snprintf(buff, BUFF_MAX, "%02d/%02d/%d", day(MyTime), month(MyTime), year(MyTime) );
    message += "<tr><td align=center>" + String(buff) + "</td><td align=center>" + String(shams.WMaxTemp[i]) + "</td><td align=center>" + String(shams.WMinTemp[i]) + "</td><td align=center>" + String( shams.WClouds[i] ) + "</td></tr>"  ;        
  }
  message += "<tr><form method=post action=" + server.uri() + "><td colspan=4 align='center'><input type='hidden' name='command' value='13'><input type='submit' value='### MANUALLY GET FORCAST ###'></td></form></tr>" ;
  
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(shams.WForecastDate), month(shams.WForecastDate), day(shams.WForecastDate) , hour(shams.WForecastDate), minute(shams.WForecastDate), second(shams.WForecastDate));
  message += "<tr><td colspan=4 align=center>Last Forcast "+String(buff)+" </td></tr></table><br>" ;  
  server.sendContent(message);

  SendHTTPPageFooter();
}


