#define MINUTESPERDAY 1440

void DoSolarCalcs(){
    SolarApp.tc.year = year();   // get the time into the structure
    SolarApp.tc.mon = month() ;
    SolarApp.tc.mday = day();
    SolarApp.tc.hour = hour();
    SolarApp.tc.min = minute();
    SolarApp.tc.sec = second();

    SolarApp.solar_az_deg = SolarAzimouthRad(ghks.longitude, ghks.latitude, &SolarApp.tc, ghks.fTimeZone) * 180 / PI ;
    SolarApp.solar_el_deg = SolarElevationRad(ghks.longitude, ghks.latitude, &SolarApp.tc, ghks.fTimeZone) * 180 / PI ;

    SolarApp.decl = Decl(gama(&SolarApp.tc)) * 180 / PI ;
    SolarApp.eqtime = eqTime(gama(&SolarApp.tc)) ;
    SolarApp.ha = HourAngle (ghks.longitude , &SolarApp.tc , ghks.fTimeZone )  ;
    SolarApp.sunrise = Sunrise(ghks.longitude, ghks.latitude, &SolarApp.tc, ghks.fTimeZone) ;
    SolarApp.sunset = Sunset(ghks.longitude, ghks.latitude, &SolarApp.tc, ghks.fTimeZone);
    SolarApp.tst = TrueSolarTime(ghks.longitude, &SolarApp.tc, ghks.fTimeZone);
    SolarApp.sunX = abs(ghks.latitude) + SolarApp.decl ;
    if (SolarApp.solar_el_deg >= 0 ) {          // day
      SolarApp.iDayNight = 1 ;
    } else {                                    // night
      SolarApp.iDayNight = 0 ;
    }  
}



float AzFix ( float target ){              // This fixes the dead zone being in the wrong place in southern hemisphere
  if ( ghks.latitude < 0  ) {                // check which hemisphere
    if ( target <= 180.0 ){
      return ( target + 180.0 )  ;  // South        
    }else{
      return ( target - 180.0 )  ;  // South              
    }
  }else{
    return( target ) ;  // North       
  }  
}

// Arduino doesnt have these to we define from a sandard libruary
float arcsin(float x) {
  return (atan(x / sqrt(-x * x + 1)));
}
float arccos(float x) {
  return (atan(x / sqrt(-x * x + 1)) + (2 * atan(1)));
}
// fractional orbital rotation in radians
float gama(struct ts *tm) {
  return ((2 * PI / 365 ) *  DayOfYear(tm->year , tm->mon , tm->mday , tm->hour , tm->min ));
}
// equation of rime
float eqTime(float g) {
  return (229.18 * ( 0.000075 + ( 0.001868 * cos(g)) - (0.032077 * sin(g)) - (0.014615 * cos (2 * g)) - (0.040849 * sin(2 * g))));
}
// declination of sun in radians
float Decl(float g) {
  return ( 0.006918 - (0.399912 * cos(g)) + (0.070257 * sin(g)) - (0.006758 * cos(2 * g)) + ( 0.000907 * sin(2 * g)) - ( 0.002697 * cos(3 * g)) + (0.00148 * sin(3 * g)) );
}
float TimeOffset(float longitude , struct ts *tm ,  float timezone ) {
  float dTmp ;
  dTmp = (-4.0 * longitude ) + (60 * timezone) - eqTime(gama(tm)) ;
  return (dTmp);
}

float TrueSolarTime(float longitude , struct ts *tm ,  float timezone ) {
  float dTmp ;
  dTmp = ( 60.0 * tm->hour ) + (1.0 * tm->min) + (1.0 * tm->sec / 60) - TimeOffset(longitude, tm, timezone) ;
  return (dTmp);
}
float HourAngle(float longitude , struct  ts *tm ,  float timezone) {
  float dTmp;
  dTmp = (TrueSolarTime(longitude, tm, timezone) / 4 ) - 180 ; // 720 minutes is solar noon -- div 4 is 180
  return (dTmp);
}
// Hour angle for sunrise and sunset only
float HA (float lat , struct ts *tm ) {
  float latRad ;
  latRad = lat * 2 * PI / 360 ;
  return ( acos((cos(90.833 * PI / 180 ) / ( cos(latRad) * cos(Decl(gama(tm)))) - (tan(latRad) * tan(Decl(gama(tm)))))) / PI * 180  );
}

float Sunrise(float longitude , float lat , struct ts *tm , float timezone) {   // outputs in minutes
float fTmp ;
  fTmp = 720 - ( 4.0 * (longitude + HA(lat, tm))) + (60 * timezone) - eqTime(gama(tm))  ;
  if ( fTmp > MINUTESPERDAY )
    fTmp -= MINUTESPERDAY ;
  if ( fTmp < 0 )  
    fTmp += MINUTESPERDAY ;
  return (fTmp) ;
}

float Sunset(float longitude , float lat , struct ts *tm , float timezone) {   // outputs in minutes
float fTmp ;
  fTmp = 720 - ( 4.0 * (longitude - HA(lat, tm))) + (60 * timezone) - eqTime(gama(tm)) ;
  if ( fTmp > MINUTESPERDAY )
    fTmp -= MINUTESPERDAY ;
  if ( fTmp < 0 )  
    fTmp += MINUTESPERDAY ;
  return (fTmp) ;
}
float SNoon(float longitude , float lat , struct ts *tm , float timezone) {    // outputs in minutes
float fTmp ;
  fTmp = 720 - ( 4.0 * (longitude  + (60 * timezone) - eqTime(gama(tm)))) ;
  if ( fTmp > MINUTESPERDAY )
    fTmp -= MINUTESPERDAY ;
  if ( fTmp < 0 )  
    fTmp += MINUTESPERDAY ;
  return (fTmp) ;
}

float SolarZenithRad(float longitude , float lat , struct ts *tm , float timezone) {
  float latRad ;
  float decRad ;
  float HourAngleRad ;
  float dTmp ;

  latRad = lat * 2 * PI / 360 ;
  decRad = Decl(gama(tm));
  HourAngleRad = HourAngle (longitude , tm , timezone ) * PI / 180 ;
  dTmp = acos((sin(latRad) * sin(decRad)) + (cos(latRad) * cos(decRad) * cos(HourAngleRad)));
  return (dTmp) ;

}
float SolarElevationRad(float longitude , float lat , struct ts *tm ,  float timezone ) {
  return ((PI / 2) - SolarZenithRad(longitude , lat , tm , timezone )) ;
}

float SolarAzimouthRad(float longitude , float lat , struct ts *tm ,  float timezone) {
  float latRad ;
  float decRad ;
  float solarzenRad ;
  float HourAngleRad ;
  float dTmp ;
  latRad = lat * 2 * PI / 360 ;
  decRad = Decl(gama(tm));
  solarzenRad = SolarZenithRad ( longitude , lat , tm , timezone ) ;
  HourAngleRad = HourAngle (longitude , tm , timezone ) * PI / 180 ;
  dTmp = acos(((sin(decRad) * cos(latRad)) - (cos(HourAngleRad) * cos(decRad) * sin(latRad))) / sin(solarzenRad)) ;
  if ( HourAngleRad < 0 ) {
    return (dTmp) ;
  } else {
    return ((2 * PI) - dTmp) ;
  }
}

int NumberOK (float target) {
  int tmp = 0 ;
  tmp = isnan(target);
  if ( tmp != 1 ) {
    tmp = isinf(target);
  }
  return (tmp);
}

float DayOfYear(uint16_t iYear , uint8_t iMon , uint8_t iDay , uint8_t iHour , uint8_t iMin ) {
  int i ;
  float iTDay ;

  iTDay = iDay - 1 ;  // this is zero referenced
  for ( i = 1 ; i < iMon ; i++ ) {
    switch (i) {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
        iTDay += 31 ;
        break;
      case 4:
      case 6:
      case 9:
      case 11:
        iTDay += 30 ;
        break;
      case 2 :
        if ((iYear % 4) == 0 ) {
          iTDay += 29 ;
        } else {
          iTDay += 28 ;
        }
        break;
    }
  }
  iTDay += (( 1.0 * iHour - 12 ) / 24 ) ;
  //  iDay +=  1.0 * iMin  / 1440 ;
  return (iTDay);
}


int HrsSolarTime(float target) {
  int i ;
  i = target ;
  return (  i / 60 );
}
int MinSolarTime(float target) {
  int i ;
  i = target  ;
  return (   i % 60 );
}

float sign(float target) {
  if (target > 0 ) {
    return (1);
  } else {
    if (target < 0 ) {
      return (-1);
    } else {
      return (0);
    }
  }
}


