// calendar_api.h

#ifndef CALENDAR_API_H
#define CALENDAR_API_H

#include <ESP8266WiFi.h>             // <--- ADD THIS for WiFi.status()
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Declaration of global variables as extern
// This tells the compiler that these variables will be defined in the .cpp file
extern String _currentCalendarSummary; // For summary on the main screen
extern String _detailedCalendarInfo;   // For details on a separate page

// Function declarations
void getCalendarSchedule();
String getCalendarInfo();            // Returns summary
String getDetailedCalendarInfo();    // Returns detailed info

#endif