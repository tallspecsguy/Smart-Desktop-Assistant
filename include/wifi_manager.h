
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include<WiFi.h> 

void initWiFi(const char* ssid, const char* password) {
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retry++;
    if (retry >= 20) {
      Serial.println("\n[WiFi] Connection Timeout.");
      return;
    }
  }

  Serial.println("\n[WiFi] Connected successfully.");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());
}

#endif
