float getPressure(float* Temp)
{
  char status;
  double T,P;

  status = pressure.startTemperature();
  if (status != 0){
    delay(status);    // Wait for the measurement to complete:
    status = pressure.getTemperature(T);
    if (status != 0){
      status = pressure.startPressure(3);
      if (status != 0){
        // Wait for the measurement to complete:
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0){ 
          tv.gT = T ;
          return(float(P));
        }
      }
    }
  }
}


