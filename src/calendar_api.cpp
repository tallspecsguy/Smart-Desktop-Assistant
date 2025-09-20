// calendar_api.cpp

#include "calendar_api.h"

// Definition of global variables declared as extern in the header
String _currentCalendarSummary = "No schedule";
String _detailedCalendarInfo = "No details";

void getCalendarSchedule() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[Calendar] Connecting to API...");

    WiFiClientSecure client;  
    client.setInsecure(); // Disable certificate verification (works in Wokwi)

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    String url = "https://script.google.com/macros/s/AKfycbyQ_3PCH3VG8lCRgaYtUp9UjxbSJKZ_qP_eFoGEjjTG94UnWQXb-OoMrjuWvUkzMook/exec";

    Serial.print("[Calendar] Trying to connect to URL: ");
    Serial.println(url);

    if (http.begin(client, url)) {
      Serial.println("[Calendar] HTTP client.begin() successful.");
      int httpCode = http.GET();

      Serial.printf("[Calendar] HTTP GET finished, response code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("[Calendar] Response Payload:");
        Serial.println(payload);

        DynamicJsonDocument doc(2048); 
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          if (doc.containsKey("status") && doc["status"] == "success") {
            JsonArray events = doc["events"];
            if (!events.isNull() && events.size() > 0) {
              String tempCalendarSummary = "";
              String tempCalendarDetail = "";

              int eventsToShowSummary = 0;
              int eventsToShowDetail = 0;

              for (JsonObject event_item : events) {
                if (eventsToShowSummary < 2 && event_item.containsKey("title") && event_item.containsKey("start")) {
                  String title = event_item["title"].as<String>();
                  String startFull = event_item["start"].as<String>();
                  String startTime = startFull.substring(11, 16);

                  if (eventsToShowSummary > 0) {
                    tempCalendarSummary += "\n";
                  }
                  tempCalendarSummary += title + " (" + startTime + ")";
                  eventsToShowSummary++;
                }

                if (eventsToShowDetail < 5 && event_item.containsKey("title")) {
                  String title = event_item["title"].as<String>();
                  String startFull = event_item["start"].as<String>();
                  String endFull = event_item["end"].as<String>();
                  String description = event_item["description"].as<String>();
                  String location = event_item["location"].as<String>();

                  if (eventsToShowDetail > 0) {
                    tempCalendarDetail += "\n---\n";
                  }
                  tempCalendarDetail += "Title: " + title + "\n";
                  tempCalendarDetail += "Start: " + startFull + "\n";
                  tempCalendarDetail += "End: " + endFull + "\n";
                  if (description.length() > 0) tempCalendarDetail += "Description: " + description + "\n";
                  if (location.length() > 0) tempCalendarDetail += "Location: " + location + "\n";

                  eventsToShowDetail++;
                }
              }

              _currentCalendarSummary = (eventsToShowSummary > 0) ? tempCalendarSummary : "No schedule";
              _detailedCalendarInfo   = (eventsToShowDetail > 0) ? tempCalendarDetail : "No schedule details available.";
            } else {
              _currentCalendarSummary = "No schedule";
              _detailedCalendarInfo   = "No details";
            }
          } else {
            _currentCalendarSummary = "API error";
            _detailedCalendarInfo   = "API error in response.";
          }
        } else {
          _currentCalendarSummary = "JSON Parse error";
          _detailedCalendarInfo   = "JSON parsing failed.";
        }
      } else {
        _currentCalendarSummary = "HTTP error " + String(httpCode);
        _detailedCalendarInfo   = "HTTP request failed.";
      }
      http.end();
    } else {
      _currentCalendarSummary = "HTTP begin error";
      _detailedCalendarInfo   = "Connection failed.";
    }
  } else {
    _currentCalendarSummary = "WiFi disconnected";
    _detailedCalendarInfo   = "WiFi not connected.";
  }

  Serial.println("[Calendar] Summary updated: " + _currentCalendarSummary);
}

String getCalendarInfo() {
  return _currentCalendarSummary;
}

String getDetailedCalendarInfo() {
  return _detailedCalendarInfo;
}
