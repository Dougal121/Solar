
void DisplayMeatBall() {
  int  pos , led , x , y;
  float dx , dy , dt ;
  float dxa , dya ;

  if (tv.iDayNight == 1){  // try an be a little less bright at night
    HT.setBrightness(15);
  } 
  else{
    HT.setBrightness(7); 
  }
  
//  if ( tv.xMul > 0 ) {
    dx = tv.xzAng - tv.xzTarget ;    // NS
//  }else{
//    dx = tv.xzTarget - tv.xzAng  ;   // NS     reverse
//  }
//  if ( tv.yMul > 0 ) {
    dy = tv.yzAng - tv.yzTarget ;    // EW
//  }else{
//    dy = tv.yzTarget - tv.yzAng  ;   // EW     reverse    
//  }
/*  if ( tv.iXYS != 0 ){               //  swap mount orientation so spin around display
    dt = dx ;
    dx = dy ;
    dy = dt ;
  }*/
  dxa = abs(dx) ;
  dya = abs(dy) ;
  if (dxa < 6) {
    x = 0 ;
  } else {
    if (dxa < 12) {
      x = sign(dx);
    } else {
      if (dxa < 50) {
        x = 2 * sign(dx);
      } else {
        x = 3 * sign(dx);
      }
    }
  }
  if (dya < 6) {
    y = 0 ;
  } else {
    if (dya < 12) {
      y = sign(dy);
    } else {
      if (dya < 25) {
        y = 2 * sign(dy);
      } else {
        y = 3 * sign(dy);
      }
    }
  }
  pos = 27 ; // netral position
  pos += (y * 8) ; // add or sumtract the x in range of -3 to +3
  pos += (x ) ; // add or sumtract 8 * y or y in range of -3 to +3
  for (led = 0; led < 63; led++) {
    switch (led){
       case 0:
       case 7:
       case 56:
       case 63:
       break;
       default:
         HT.clearLed(MapLedNo(led));
       break;  
    }
  }
  
  if ( bPower ){            // dont turn on anything if in low power mode   
    HT.setLed(MapLedNo(0)); // turn on four courners
    HT.setLed(MapLedNo(7));
    HT.setLed(MapLedNo(56));
    HT.setLed(MapLedNo(63));
  
  
    //  HT.setLedNow(MapLedNo(tc.sec));
    if ((iPWM_YZ == 0) && (iPWM_XZ == 0)) {
      HT.setBlinkRate(HT16K33_DSP_NOBLINK); // not attempting to move
      if (second() % 2 == 0 ) {
        HT.setLed(MapLedNo(pos + 0)); // display the meatball
        HT.setLed(MapLedNo(pos + 9));
      }else{
        HT.setLed(MapLedNo(pos + 1));
        HT.setLed(MapLedNo(pos + 8));
      }
    } else {
      HT.setBlinkRate(HT16K33_DSP_BLINK2HZ); //moving so blink meat ball
      HT.setLed(MapLedNo(pos + 0)); // display the meatball
      HT.setLed(MapLedNo(pos + 1));
      HT.setLed(MapLedNo(pos + 8));
      HT.setLed(MapLedNo(pos + 9));
    }
  }
  HT.sendLed() ; // update all in one go
}


int MapLedNo(int target)  // this compensates for the screwy setup of the matrix driver to the chip
{
  int row ;
  int col ;
  row = target / 8 ;
  col = target % 8 ;
  if (col == 0 ) {
    return ((row * 16 ) + 7) ;
  } else {
    return ((row * 16 ) + col - 1) ;
  }
}


