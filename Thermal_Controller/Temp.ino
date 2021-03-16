/*
For reference - probe addresses

28BEA21C070000EA
28B9AC1C070000CD


*/
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

void GetTempLogs(void){
  float fTemp;
  sensors.requestTemperatures();  
  for (int i = 0 ; i < 3 ; i++ ) {
    fTemp = sensors.getTempC(Thermometer[i]) ;
    if ( fTemp > -50.0 ){
      shams.fTemp[i]= fTemp ;
    }
  }
}


