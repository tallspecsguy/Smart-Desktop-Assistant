
// ntp_time.cpp 

#include "ntp_time.h" // Included the header file
#include <Arduino.h>  // Include for basic Arduino functions like Serial.println

// Definition of the objects declared as extern in the header
// This is the only place where these objects are actually defined
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // GMT+7

// Initialize and sync the NTP time
void syncNTPTime() {
  Serial.println("[Time] Starting NTP client...");
  timeClient.begin();
  // NTPClient.update() may block for a few seconds if time is not yet synchronized.
  // Make sure WiFi is connected before calling this.
  timeClient.update();
  Serial.println("[Time] NTP time synced: " + timeClient.getFormattedTime());
}

String getFormattedTime() {
  // Get the current formatted time as a string
  timeClient.update();
  String currentTime = timeClient.getFormattedTime();
  // Serial.println("[Time] Current Time: " + currentTime); // Optional: disable to reduce serial spam  
  return currentTime;
}
