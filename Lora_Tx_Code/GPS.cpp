#include "GPS.h"

TinyGPSPlus gps;

void displaygpsdata(float *lattitude, float *longitude){
        if (gps.location.isValid()){
          *lattitude=gps.location.lat();
          *longitude = gps.location.lng();
        }
        else{
          Serial.print(F("INVALID"));
        }

  if (millis() > 5000 && gps.charsProcessed() < 10){
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

