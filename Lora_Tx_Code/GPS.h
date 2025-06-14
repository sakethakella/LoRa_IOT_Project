#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

//declare pins and baud rate
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;

void displaygpsdata(float lattitude,float longitude);
#endif