// calendar_api.h

#ifndef CALENDAR_API_H
#define CALENDAR_API_H

#include <WiFi.h>           // For WiFi functions on ESP32
#include <HTTPClient.h>     // For HTTP(S) requests
#include <WiFiClientSecure.h> // For secure HTTPS connections
#include <ArduinoJson.h>    // For parsing JSON data

// Global variables (defined in calendar_api.cpp)
extern String _currentCalendarSummary; // Summary for main screen
extern String _detailedCalendarInfo;   // Detailed view

// Function declarations
void getCalendarSchedule();        // Fetch calendar data
String getCalendarInfo();          // Returns summary string
String getDetailedCalendarInfo();  // Returns detailed string

#endif
