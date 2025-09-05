// weather_api.h 

#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <ESP8266WiFi.h>             // <--- ADD this for WiFi.status()
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Declare global variables as extern
// This tells the compiler that the variables are defined elsewhere (in the .cpp file)
extern String temperature;
extern String condition;
extern String _detailedWeatherInfo; // Stores detailed weather info for detail page

// Deklarasi fungsi
String interpretWeatherCode(int code);
void getWeatherData();
String getWeatherInfo();
String getDetailedWeatherInfo(); // Get full detailed weather info

#endif