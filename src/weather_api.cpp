// weather_api.cpp 

#include "weather_api.h" // Include header files

// Define global variables declared as extern in the header
String temperature = "??"; // Current Temperature
String condition = "??";   // Current Weather Condition
String _detailedWeatherInfo = "Tidak ada detail cuaca"; // Detailed weather info

// Convert weather codes into human-readable descriptions
String interpretWeatherCode(int code) {
  switch (code) {
    case 0: return "Clear";
    case 1: return "Mostly Clear";
    case 2: return "Partly Cloudy";
    case 3: return "Cloudy";
    case 45: case 48: return "Fog";
    case 51: case 53: case 55: return "Drizzle";
    case 61: case 63: case 65: return "Rain";
    case 66: case 67: return "Freezing Rain";
    case 71: case 73: case 75: return "Snow";
    case 77: return "Snow Grains";
    case 80: case 81: case 82: return "Local Showers";
    case 85: case 86: return "Snow Showers";
    case 95: return "Thunderstorm";
    case 96: case 99: return "Thunderstorm with Hail";
    default: return "Unknown";
  }
}

// Fetch weather data from Open-Meteo API
void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[Weather] Connecting to API...");

    WiFiClientSecure client;
    client.setInsecure(); // Disable certificate verification (needed for Wokwi/ESP32)

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    String url = "https://api.open-meteo.com/v1/forecast?latitude=-6.9147&longitude=107.6098&current_weather=true&temperature_unit=celsius&windspeed_unit=ms&precipitation_unit=mm"; 

    if (http.begin(client, url)) {
      int httpCode = http.GET();
      Serial.print("[Weather] HTTP Response Code: ");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(2048); 

        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("[Weather] JSON Deserialization Error: ");
          Serial.println(error.c_str());
          temperature = "Error";
          condition = "Parse Err";
          _detailedWeatherInfo = "Error parsing weather data.";
        } else {
          if (doc.containsKey("current_weather") &&
              doc["current_weather"].is<JsonObject>()) {

            JsonObject currentWeather = doc["current_weather"];
            if (currentWeather.containsKey("temperature") &&
                currentWeather.containsKey("weathercode") &&
                currentWeather.containsKey("windspeed") && 
                currentWeather.containsKey("winddirection")) { 

              float temp_val = currentWeather["temperature"];
              int weathercode_val = currentWeather["weathercode"];
              float windspeed_val = currentWeather["windspeed"];
              int winddirection_val = currentWeather["winddirection"];

              temperature = String(temp_val, 1) + "°C";
              condition = interpretWeatherCode(weathercode_val);

              _detailedWeatherInfo = "Temperature: " + String(temp_val, 1) + "°C\n"
                                   + "Condition: " + interpretWeatherCode(weathercode_val) + "\n"
                                   + "Wind Speed: " + String(windspeed_val, 1) + " m/s\n"
                                   + "Wind Direction: " + String(winddirection_val) + "°";

              Serial.println("[Weather] Temperature: " + temperature);
              Serial.println("[Weather] Condition: " + condition);
              Serial.println("[Weather] Detailed Info: " + _detailedWeatherInfo);
            } else {
              Serial.println("[Weather] Missing required data in current_weather JSON.");
              temperature = "Data Err";
              condition = "Missing";
              _detailedWeatherInfo = "Missing key weather data.";
            }
          } else {
            Serial.println("[Weather] Missing current_weather object in JSON.");
            temperature = "Struct Err";
            condition = "API";
            _detailedWeatherInfo = "API response structure error.";
          }
        }
      } else {
        Serial.println("[Weather] Failed to connect. HTTP error code: " + String(httpCode));
        temperature = "HTTP Err";
        condition = String(httpCode);
        _detailedWeatherInfo = "HTTP Error: " + String(httpCode);
      }
      http.end();
    } else {
      Serial.println("[Weather] HTTP client.begin() failed.");
      temperature = "Conn Err";
      condition = "Begin";
      _detailedWeatherInfo = "HTTP connection failed.";
    }
  } else {
    Serial.println("[Weather] WiFi not connected!");
    temperature = "No WiFi";
    condition = "";
    _detailedWeatherInfo = "No WiFi Connection.";
  }
}

// Get weather summary (short form)
String getWeatherInfo() {
  if (temperature == "No WiFi" || temperature == "HTTP Err" || temperature == "Error" || 
      temperature == "Data Err" || temperature == "Struct Err" || temperature == "Conn Err") {
    return temperature;
  }
  return temperature + " | " + condition;
}

// Get detailed weather information (long form)
String getDetailedWeatherInfo() {
  return _detailedWeatherInfo;
}
