void StopYZ(){
  iPWM_YZ=0 ;
  tv.motor_recycle_y = MOTOR_DWELL ;
}
void StopXZ(){
  iPWM_XZ=0 ;
  tv.motor_recycle_x = MOTOR_DWELL ;
}

void ActivateOutput(int iAllStop) {
    switch (tv.iOutputType){
      case 0: //  PWM/DIR     Normal PWM board
      case 1: //  L298        IN1 IN2 ( EN tied high )
        ActivatePWMDIR(iAllStop) ;        // now refactored with crispy bits and red wine
      break;
      case 2: // active high and low combinations ( code rescued from the original part one tracker )    
      case 3:     
        ActivateRelays(iAllStop) ;     
      break;
    }  
}

void ActivatePWMDIR(int iAllStop) {
  if (tv.motor_recycle_y > 0 ){
    tv.motor_recycle_y-- ;
  }
  if (tv.motor_recycle_x > 0 ){
    tv.motor_recycle_x-- ;
  }
  if ( iAllStop == 0 ) {
    StopYZ() ;
    StopXZ() ;
  } else {
    if (( iPWM_YZ==0 ) && (tv.motor_recycle_y == 0 )){
      if ((( tv.yzAng  ) < ( tv.yzTarget - tv.yzH )) ) {   // do Y ie E/W before N/S
        digitalWrite(tv.RELAY_YZ_DIR, LOW) ;
        iPWM_YZ=2 ;
      }
      if ((( tv.yzAng ) > ( tv.yzTarget + tv.yzH )) ) {
        digitalWrite(tv.RELAY_YZ_DIR, HIGH) ;
        iPWM_YZ=2 ;
      }
    }
    if ( iPWM_YZ>0 ){
      if ((tv.yzAng > tv.yzTarget) && ( digitalRead(tv.RELAY_YZ_DIR)==LOW )) {
        StopYZ() ;
      }
      if ((tv.yzAng < tv.yzTarget) && ( digitalRead(tv.RELAY_YZ_DIR)==HIGH )) {
        StopYZ() ;
      }
    }

    if (( iPWM_YZ==0) || ( tv.iMultiDrive == 1 )) {  // if finished on E/W you can do N/S  or if we are doing multidrive
        if (( iPWM_XZ==0 ) && (tv.motor_recycle_x == 0 )){
          if ((tv.xzAng < ( tv.xzTarget - tv.xzH ))  )  { // turn on if not in tolerance
            digitalWrite(tv.RELAY_XZ_DIR, LOW) ;
            iPWM_XZ=2 ;
          }
          if ((tv.xzAng > ( tv.xzTarget + tv.xzH )) ) { // turn on if not in tolerance
            digitalWrite(tv.RELAY_XZ_DIR, HIGH) ;
            iPWM_XZ=2 ;
          }
        }
    }else{
      if ((iPWM_XZ>0 )){
        StopXZ() ;
      }
    }
    if ( iPWM_XZ>0 ){
      if ((tv.xzAng > tv.xzTarget ) && ( digitalRead(tv.RELAY_XZ_DIR)==LOW ))  { // if on turn off
        StopXZ() ;
      }
      if ((tv.xzAng < tv.xzTarget ) && ( digitalRead(tv.RELAY_XZ_DIR)==HIGH ))  { // if on turn off
        StopXZ() ;
      }
    }
  }
  if (iPWM_XZ>0){
    iPWM_XZ += 1 ;
  }
  if (iPWM_YZ>0){
    iPWM_YZ += 1 ;
  }
  iPWM_XZ = constrain(iPWM_XZ,0,MAX_MOTOR_PWM);  // 254 in atmel - arduino land 
  iPWM_YZ = constrain(iPWM_YZ,0,MAX_MOTOR_PWM);  // 1023 for a tensilica  Hmmm stangness on the H-SWITCH board especially if at highish (27V) voltages
  iPWM_XZ = constrain(iPWM_XZ,0,tv.xMaxMotorSpeed);  // 254 in atmel - arduino land 
  iPWM_YZ = constrain(iPWM_YZ,0,tv.yMaxMotorSpeed);  // 1023 for a tensilica  Hmmm stangness on the H-SWITCH board especially if at highish (27V) voltages
  if (tv.iTrackMode==5){                // need a dont go anywhere mode  
    StopXZ();
    StopYZ();
  }
  ledcWrite(RELAY_XZ_PWM,iPWM_XZ);
  ledcWrite(RELAY_YZ_PWM,iPWM_YZ);
  if (( digitalRead(tv.RELAY_XZ_DIR) == LOW ) || ((tv.iOutputType & 0x01 ) == 0 )) {
    ledcWrite(RELAY_XZ_PWM,iPWM_XZ);
  }else{
    ledcWrite(RELAY_XZ_PWM,1023-iPWM_XZ);    
  }
  if (( iPWM_XZ == 0 ) && ( (tv.iOutputType & 0x01 ) != 0 )){ // if it's off and using L298 then flip so both are at ground
     digitalWrite(tv.RELAY_XZ_DIR, LOW) ;
  }
  if (( digitalRead(tv.RELAY_XZ_DIR) == LOW )  || ( (tv.iOutputType & 0x01 ) == 0 )) {
    ledcWrite(RELAY_YZ_PWM,iPWM_YZ);
  }else{
    ledcWrite(RELAY_YZ_PWM,1023-iPWM_YZ);      
  }
  if (( iPWM_YZ == 0 ) && ( (tv.iOutputType & 0x01 ) != 0 )){ // if it's off and using L298 then flip so both are at ground
     digitalWrite(tv.RELAY_YZ_DIR, LOW) ;
  }
}
/*   look carefully below, RTFM !!! yes one is oppisite of the other (god knows why ?)
RELAY_XZ_PWM D5;  PWM 1    Speed North / South     Was the X- S relay  Orange 
RELAY_YZ_PWM D6;  PWM 2    Speed East / West       Was the Y+ W relay  Blue
RELAY_YZ_DIR D7;  DIR 2    Y+ Y- East / West       Was the Y- E relay  Yellow
RELAY_XZ_DIR D8;  DIR 1    X+ X-  North / South    Was the X+ N relay  Brown
*/
void ActivateRelays(int iAllStop) {
int iEIN ; // ON - Active
int iAUS ; // OFF - Inactive 
  
  if ((tv.iOutputType & 0x01 ) == 0 ){  // active low relays (usually
    iEIN = LOW ;
    iAUS = HIGH ;
  }else{               // active high relays
    iEIN = HIGH ;
    iAUS = LOW   ; 
  }
  if (tv.motor_recycle_y > 0 ){
    tv.motor_recycle_y-- ;
  }
  if (tv.motor_recycle_x > 0 ){
    tv.motor_recycle_x-- ;
  }
  if ( iAllStop == 0 ) {
    digitalWrite(tv.RELAY_YZ_DIR, iAUS) ;
    digitalWrite(tv.RELAY_YZ_PWM, iAUS) ;
    digitalWrite(tv.RELAY_XZ_DIR, iAUS) ;
    digitalWrite(tv.RELAY_XZ_PWM, iAUS) ;
  } else {
    if (( digitalRead(tv.RELAY_YZ_DIR) == iAUS ) && ( digitalRead(tv.RELAY_YZ_PWM) == iAUS ) && (tv.motor_recycle_y == 0 )){
      if ((( tv.yzAng  ) < ( tv.yzTarget - tv.yzH )) && ( digitalRead(tv.RELAY_YZ_PWM) == iAUS )) {   // do Y ie E/W before N/S
        digitalWrite(tv.RELAY_YZ_PWM, iEIN) ;
        digitalWrite(tv.RELAY_YZ_DIR, iAUS) ;
      }
      if ((( tv.yzAng ) > ( tv.yzTarget + tv.yzH )) && ( digitalRead(tv.RELAY_YZ_DIR) == iAUS )) {
        digitalWrite(tv.RELAY_YZ_PWM, iAUS) ;
        digitalWrite(tv.RELAY_YZ_DIR, iEIN) ;
      }
    }
    if ((tv.yzAng > tv.yzTarget) && ( digitalRead(tv.RELAY_YZ_PWM)==iEIN )) {
      digitalWrite(tv.RELAY_YZ_DIR, iAUS) ;
      digitalWrite(tv.RELAY_YZ_PWM, iAUS) ;
      tv.motor_recycle_y = MOTOR_DWELL ;
    }
    if ((tv.yzAng < tv.yzTarget) && ( digitalRead(tv.RELAY_YZ_DIR)==iEIN )) {
      digitalWrite(tv.RELAY_YZ_DIR, iAUS) ;
      digitalWrite(tv.RELAY_YZ_PWM, iAUS) ;
      tv.motor_recycle_y = MOTOR_DWELL ;
    }

    if (( tv.iMultiDrive == 1 ) || (( digitalRead(tv.RELAY_YZ_DIR) == iAUS ) && ( digitalRead(tv.RELAY_YZ_PWM) == iAUS ))  ) {  // if finished on E/W you can do N/S  or if we are doing multidrive
        if ( tv.motor_recycle_x == 0 ){
          if ((tv.xzAng < ( tv.xzTarget - tv.xzH )) && ( digitalRead(tv.RELAY_XZ_DIR) == iAUS ))  { // turn on if not in tolerance
            digitalWrite(tv.RELAY_XZ_DIR, iEIN) ;
            digitalWrite(tv.RELAY_XZ_PWM, iAUS) ;
          }
          if ((tv.xzAng > ( tv.xzTarget + tv.xzH )) && ( digitalRead(tv.RELAY_XZ_PWM) == iAUS ) ) { // turn on if not in tolerance
            digitalWrite(tv.RELAY_XZ_DIR, iAUS) ;
            digitalWrite(tv.RELAY_XZ_PWM, iEIN) ;
          }
        }
    }else{
      if ((digitalRead(tv.RELAY_XZ_PWM) == iEIN ) || (digitalRead(tv.RELAY_XZ_PWM) == iEIN )){
        digitalWrite(tv.RELAY_XZ_DIR, iAUS) ;
        digitalWrite(tv.RELAY_XZ_PWM, iAUS) ;
        tv.motor_recycle_x = MOTOR_DWELL ;
      }
    }
    if ((tv.xzAng > tv.xzTarget ) && ( digitalRead(tv.RELAY_XZ_DIR)==iEIN ))  { // if on turn off
      digitalWrite(tv.RELAY_XZ_DIR, iAUS) ;
      digitalWrite(tv.RELAY_XZ_PWM, iAUS) ;
      tv.motor_recycle_x = MOTOR_DWELL ;
    }
    if ((tv.xzAng < tv.xzTarget ) && ( digitalRead(tv.RELAY_XZ_PWM)==iEIN ))  { // if on turn off
      digitalWrite(tv.RELAY_XZ_DIR, iAUS) ;
      digitalWrite(tv.RELAY_XZ_PWM, iAUS) ;
      tv.motor_recycle_x = MOTOR_DWELL ;
    }
  }
  if (tv.iTrackMode==5){                // need a dont go anywhere mode  
    digitalWrite(tv.RELAY_XZ_DIR, iAUS) ;
    digitalWrite(tv.RELAY_XZ_PWM, iAUS) ;
    digitalWrite(tv.RELAY_YZ_DIR, iAUS) ;
    digitalWrite(tv.RELAY_YZ_PWM, iAUS) ;
  }
}


