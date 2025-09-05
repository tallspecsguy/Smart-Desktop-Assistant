// calendar_api.cpp (NEW FILE OR MODIFICATION OF OLD calendar_api.h CONTENT)

#include "calendar_api.h" // Must include this header file

// Definition of global variables declared as extern in the header
// This is the only place where these variables are actually defined
String _currentCalendarSummary = "No schedule";
String _detailedCalendarInfo = "No details";

void getCalendarSchedule() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[Calendar] Connecting to API...");
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    String url = "https://script.google.com/macros/s/AKfycbyQ_3PCH3VG8lCRgaYtUp9UjxbSJKZ_qP_eFoGEjjTG94UnWQXb-OoMrjuWvUkzMook/exec"; // URL GAS Anda

    Serial.print("[Calendar] Trying to connect to URL: ");
    Serial.println(url);

    if (http.begin(*client, url)) {
      Serial.println("[Calendar] HTTP client.begin() successful.");
      int httpCode = http.GET();

      Serial.printf("[Calendar] HTTP GET finished, response code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("[Calendar] Response Payload:");
        Serial.println(payload);
        Serial.printf("[Calendar] Payload Lenght: %d\n", payload.length());

        DynamicJsonDocument doc(2048); // Increase JSON buffer size if payload is larger
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          Serial.println("[Calendar] JSON deserialization successful.");
          if (doc.containsKey("status") && doc["status"].as<String>() == "success") {
            Serial.println("[Calendar] API status is 'success'.");
            JsonArray events = doc["events"];

            if (!events.isNull()) {
              if (events.size() > 0) {
                String tempCalendarSummary = "";
                String tempCalendarDetail = ""; // To store full details
                int eventsToShowSummary = 0; // Limit number of events for summary (e.g. 1-2)
                int eventsToShowDetail = 0; // Limit number of events for details (e.g. 5)

                for (JsonObject event_item : events) {
                  // Limit number of events for summary on the main screen
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

                  // Add all events or limit for details if too many
                  if (eventsToShowDetail < 5 && event_item.containsKey("title") &&
                      event_item.containsKey("start") && event_item.containsKey("end") &&
                      event_item.containsKey("description") && event_item.containsKey("location")) {
                    String title = event_item["title"].as<String>();
                    String startFull = event_item["start"].as<String>();
                    String endFull = event_item["end"].as<String>();
                    String description = event_item["description"].as<String>();
                    String location = event_item["location"].as<String>();

                    if (eventsToShowDetail > 0) {
                      tempCalendarDetail += "\n---\n"; // Separator between events in details
                    }
                    tempCalendarDetail += "Title: " + title + "\n";
                    tempCalendarDetail += "Start: " + startFull + "\n";
                    tempCalendarDetail += "End: " + endFull + "\n";
                    if (description.length() > 0) tempCalendarDetail += "Description: " + description + "\n";
                    if (location.length() > 0) tempCalendarDetail += "Location: " + location + "\n";

                    eventsToShowDetail++;
                  } else if (eventsToShowDetail >= 5) {
                    // Stop processing details if limit reached
                    tempCalendarDetail += "\n...and other events.";
                  }
                }

                if (eventsToShowSummary > 0) {
                  _currentCalendarSummary = tempCalendarSummary;
                } else {
                  _currentCalendarSummary = "Tidak ada jadwal";
                }

                if (eventsToShowDetail > 0) {
                  _detailedCalendarInfo = tempCalendarDetail;
                } else {
                  _detailedCalendarInfo = "No schedule details available.";
                }

                Serial.println("[Calendar] Summary of Events: " + _currentCalendarSummary);
                Serial.println("[Calendar] Detailed Events: " + _detailedCalendarInfo);

              } else {
                Serial.println("[Calendar] No events found in JSON (events array empty).");
                _currentCalendarSummary = "No schedule";
                _detailedCalendarInfo = "No schedule details available.";
              }
            } else {
              Serial.println("[Calendar] Field 'events' is missing or null in JSON.");
              _currentCalendarSummary = "Invalid events structure";
              _detailedCalendarInfo = "Invalid events structure.";
            }
          } else {
            Serial.println("[Calendar] API status not 'success' or 'status' field missing.");
            if(doc.containsKey("status")){
                Serial.print("[Calendar] doc[\"status\"]: ");
                Serial.println(doc["status"].as<String>());
            } else {
                Serial.println("[Calendar] Field 'status' not found in JSON.");
            }
            _currentCalendarSummary = "API status error";
            _detailedCalendarInfo = "API status error.";
          }
        } else {
          Serial.print(F("[Calendar] deserializeJson() failed: "));
          Serial.println(error.f_str());
          Serial.println(F("[Calendar] Received Payload (failed parsing):"));
          Serial.println(payload);
          _currentCalendarSummary = "JSON Parse error";
          _detailedCalendarInfo = "JSON Parse error.";
        }
      } else {
        Serial.printf("[Calendar] HTTP GET request failed, error: %s (HTTP code: %d)\n", http.errorToString(httpCode).c_str(), httpCode);
        _currentCalendarSummary = "HTTP error " + String(httpCode);
        _detailedCalendarInfo = "HTTP error " + String(httpCode) + " while fetching calendar data.";
      }
      http.end();
    } else {
      Serial.println("[Calendar] HTTP connection failed (http.begin() failed).");
      _currentCalendarSummary = "HTTP begin error";
      _detailedCalendarInfo = "HTTP connection to calendar API failed.";
    }
  } else {
    Serial.println("[Calendar] WiFi not connected.");
    _currentCalendarSummary = "WiFi disconnected";
    _detailedCalendarInfo = "WiFi not connected, unable to fetch calendar.";
  }
  Serial.println("[Calendar] calendarEvent (summary) summary updated to: " + _currentCalendarSummary);
}

String getCalendarInfo() {
  return _currentCalendarSummary;
}

String getDetailedCalendarInfo() {
  return _detailedCalendarInfo;
}