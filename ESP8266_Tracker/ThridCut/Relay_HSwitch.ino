void StopYZ(){
  iPWM_YZ=0 ;
  tv.motor_recycle_y = MOTOR_DWELL ;
}
void StopXZ(){
  iPWM_XZ=0 ;
  tv.motor_recycle_x = MOTOR_DWELL ;
}

void ActivateRelays(int iAllStop) {
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
        digitalWrite(RELAY_YZ_DIR, LOW) ;
        iPWM_YZ=2 ;
      }
      if ((( tv.yzAng ) > ( tv.yzTarget + tv.yzH )) ) {
        digitalWrite(RELAY_YZ_DIR, HIGH) ;
        iPWM_YZ=2 ;
      }
    }
    if ( iPWM_YZ>0 ){
      if ((tv.yzAng > tv.yzTarget) && ( digitalRead(RELAY_YZ_DIR)==LOW )) {
        StopYZ() ;
      }
      if ((tv.yzAng < tv.yzTarget) && ( digitalRead(RELAY_YZ_DIR)==HIGH )) {
        StopYZ() ;
      }
    }

    if (( iPWM_YZ==0) || ( tv.iMultiDrive == 1 )) {  // if finished on E/W you can do N/S  or if we are doing multidrive
        if (( iPWM_XZ==0 ) && (tv.motor_recycle_x == 0 )){
          if ((tv.xzAng < ( tv.xzTarget - tv.xzH ))  )  { // turn on if not in tolerance
            digitalWrite(RELAY_XZ_DIR, LOW) ;
            iPWM_XZ=2 ;
          }
          if ((tv.xzAng > ( tv.xzTarget + tv.xzH )) ) { // turn on if not in tolerance
            digitalWrite(RELAY_XZ_DIR, HIGH) ;
            iPWM_XZ=2 ;
          }
        }
    }else{
      if ((iPWM_XZ>0 )){
        StopXZ() ;
      }
    }
    if ( iPWM_XZ>0 ){
      if ((tv.xzAng > tv.xzTarget ) && ( digitalRead(RELAY_XZ_DIR)==LOW ))  { // if on turn off
        StopXZ() ;
      }
      if ((tv.xzAng < tv.xzTarget ) && ( digitalRead(RELAY_XZ_DIR)==HIGH ))  { // if on turn off
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
  analogWrite(RELAY_XZ_PWM,iPWM_XZ);
  analogWrite(RELAY_YZ_PWM,iPWM_YZ);
}

