
// ntp_time.h 

#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <NTPClient.h>
#include <WiFiUdp.h>

// Declare objects as extern
// This tells the compiler that these objects are defined elsewhere (in ntp_time.cpp)
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

// Function declarations
void syncNTPTime();
String getFormattedTime();

#endif
