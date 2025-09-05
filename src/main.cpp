// main.cpp 

#include <Arduino.h>
#include <ESP8266WiFi.h> // For ESP32, use <WiFi.h> instead of <ESP8266WiFi.h>
#include "wifi_manager.h"
#include "ntp_time.h"
#include "weather_api.h"
#include "calendar_api.h"
#include "display_manager.h" // Refers to display_manager.h that has been adapted

// Timer tracking
unsigned long lastTimeUpdate = 0;
unsigned long lastWeatherUpdate = 0;
unsigned long lastCalendarUpdate = 0;
unsigned long lastTouchCheck = 0;
const unsigned long touchCheckInterval = 50; // Check touch every 50ms

extern ScreenState currentScreen;

// Define TFT and Touch Screen Pins for ESP32
// Note: Some pins may be shared (e.g., MOSI, SCK, MISO) if your setup uses SPI for both.
// TFT Pins (Using VSPI - typically GPIO 18, 19, 23 for SCK, MISO, MOSI)
#define TFT_CS   5
#define TFT_RST  18
#define TFT_DC   19
#define TFT_MOSI 23 // SDA/SDI pin on some displays, usually GPIO23 on ESP32
#define TFT_SCK  18 // SCL/SCK pin, usually GPIO18 on ESP32
// TFT_LED is usually connected directly to 3.3V (or via a resistor), no GPIO needed
// TFT_MISO is typically GPIO19 on ESP32, but often unused for display output.
// If your display explicitly requires MISO, define and configure it in display_manager.h

// Touch Screen Pins (XPT2046-based, typically uses SPI)
#define T_CLK    18 // SCK - Shared with TFT_SCK if on same SPI bus
#define T_CS     21
#define T_DIN    23 // MOSI - Shared with TFT_MOSI if on same SPI bus
#define T_OUT    19 // MISO - Shared with TFT_MISO if on same SPI bus

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Make sure functions like initWiFi, initDisplay, getTouchPoint, updateDisplay
  // and other display-related functions in their respective files
  // (wifi_manager.h, display_manager.h, etc.) are adapted for ESP32 libraries and pins.
  // For display, Adafruit_GFX + a specific driver (or TFT_eSPI) is typical.
  // For touch, the XPT2046_Touchscreen library is commonly used.

  // Example for TFT_eSPI (configured via User_Setup.h or direct pin config):
  // tft.begin();
  // tft.setRotation(desired_rotation);

  initWiFi("Shankar's S23", "jinjuShankar"); // Ensure this uses ESP32 WiFi library
  initDisplay(); // Needs to be updated for ESP32 and your pins

  syncNTPTime(); // Ensure compatible with ESP32 NTP libraries

  getWeatherData();    // Must use ESP32 HTTP client
  getCalendarSchedule(); // Must use ESP32 HTTP client

  currentScreen = SCREEN_MAIN;
  String initialTime = getFormattedTime();
  updateDisplay(initialTime); // Must work with ESP32 display library

  lastWeatherUpdate = millis();
  lastCalendarUpdate = millis();
  lastTimeUpdate = millis();

  Serial.println("\n--- Smart Desktop Assistant Started ---");
  Serial.print("Initial screen state: "); 
  Serial.println(currentScreen == SCREEN_MAIN ? "MAIN" : "UNKNOWN");
  Serial.print("Display width: "); 
  Serial.println(tft.width());
  Serial.print("Display height: "); 
  Serial.println(tft.height());

  // Debugging button coordinates (printed once at startup)
  Serial.println("Visual Button Coords (at screen bottom):");
  Serial.print("Weather Button: X=["); 
  Serial.print(WEATHER_BUTTON_X); Serial.print(" to "); 
  Serial.print(WEATHER_BUTTON_X + WEATHER_BUTTON_W);
  Serial.print("], Y=["); 
  Serial.print(WEATHER_BUTTON_Y_VISUAL); Serial.print(" to "); 
  Serial.print(WEATHER_BUTTON_Y_VISUAL + WEATHER_BUTTON_H); 
  Serial.println("]");

  Serial.print("Calendar Button: X=["); 
  Serial.print(CALENDAR_BUTTON_X); Serial.print(" to "); 
  Serial.print(CALENDAR_BUTTON_X + CALENDAR_BUTTON_W);
  Serial.print("], Y=["); 
  Serial.print(CALENDAR_BUTTON_Y_VISUAL); Serial.print(" to "); 
  Serial.print(CALENDAR_BUTTON_Y_VISUAL + CALENDAR_BUTTON_H); 
  Serial.println("]");

  Serial.print("Back Button: X=["); 
  Serial.print(BACK_BUTTON_X); Serial.print(" to "); 
  Serial.print(BACK_BUTTON_X + BACK_BUTTON_W);
  Serial.print("], Y=["); 
  Serial.print(BACK_BUTTON_Y); Serial.print(" to "); 
  Serial.print(BACK_BUTTON_Y + BACK_BUTTON_H); 
  Serial.println("]");

  // Touch detection info (hardcoded ranges)
  Serial.println("\n--- HARDCODED TOUCH DETECTION RANGES ---");
  Serial.println("Weather Touch Area: X=[5 to 155], Y=[64 to 98] (around reported MAPPED Y=81)");
  Serial.println("Calendar Touch Area: X=[165 to 315], Y=[165 to 199] (around reported MAPPED Y=182)");
  Serial.println("----------------------------------------");

  getDetailedCalendarInfo();
  getDetailedWeatherInfo();
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Touch Input Reading ---
  if (currentMillis - lastTouchCheck >= touchCheckInterval) {
    lastTouchCheck = currentMillis;
    int touchX, touchY;

    // The getTouchPoint() function in display_manager.h must be updated for ESP32 and new pins
    if (getTouchPoint(touchX, touchY)) {
      // Debug printing is inside getTouchPoint()

      if (currentScreen == SCREEN_MAIN) {
        // Check touch on "Weather Detail" button (hardcoded coordinates)
        if (touchX >= 5 && touchX <= (5 + BUTTON_WIDTH) && 
            touchY >= (81 - (BUTTON_HEIGHT / 2)) && 
            touchY <= (81 + (BUTTON_HEIGHT / 2))) {
          Serial.println(">>> Weather Button Pressed! Navigating to Weather Detail...");
          currentScreen = SCREEN_WEATHER_DETAIL;
          displayWeatherDetail(getDetailedWeatherInfo()); 
          delay(300);
        }
        // Check touch on "Calendar Detail" button (hardcoded coordinates)
        else if (touchX >= (tft.width() - BUTTON_WIDTH - 5) && 
                 touchX <= (tft.width() - 5) && 
                 touchY >= (182 - (BUTTON_HEIGHT / 2)) && 
                 touchY <= (182 + (BUTTON_HEIGHT / 2))) {
          Serial.println(">>> Calendar Button Pressed! Navigating to Calendar Detail...");
          currentScreen = SCREEN_CALENDAR_DETAIL;
          displayCalendarDetail(getDetailedCalendarInfo()); 
          delay(300);
        }
      } else { 
        // If already in detail screen (Weather or Calendar), check for "Back" button
        if (touchX >= BACK_BUTTON_X && touchX <= (BACK_BUTTON_X + BACK_BUTTON_W) &&
            touchY >= BACK_BUTTON_Y && touchY <= (BACK_BUTTON_Y + BACK_BUTTON_H)) {
          Serial.println(">>> Back Button Pressed! Returning to Main Screen...");
          currentScreen = SCREEN_MAIN;
          String currentTimeStr = getFormattedTime();
          updateDisplay(currentTimeStr); 
          delay(300);
        }
      }
    }
  }

  // --- Data & Display Update Logic ---
  if (currentMillis - lastWeatherUpdate >= 60000) { // update weather every 1 min
    lastWeatherUpdate = currentMillis;
    Serial.println("[MainLoop] Updating weather data...");
    getWeatherData();
    if (currentScreen == SCREEN_WEATHER_DETAIL) {
      displayWeatherDetail(getDetailedWeatherInfo());
    }
  }

  if (currentMillis - lastCalendarUpdate >= 300000) { // update calendar every 5 min
    lastCalendarUpdate = currentMillis;
    Serial.println("[MainLoop] Updating calendar data...");
    getCalendarSchedule();
    if (currentScreen == SCREEN_CALENDAR_DETAIL) {
      displayCalendarDetail(getDetailedCalendarInfo());
    }
  }

  if (currentScreen == SCREEN_MAIN && currentMillis - lastTimeUpdate >= 1000) { // update clock every 1 sec
    lastTimeUpdate = currentMillis;
    String currentTimeStr = getFormattedTime();
    updateDisplay(currentTimeStr);
  }
}
