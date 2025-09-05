// display_manager.h 

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

extern TFT_eSPI tft;
extern XPT2046_Touchscreen touch;

#ifndef TOUCH_CS
#define TOUCH_CS 15 // Change to your Touchscreen CS (Chip Select) pin
#endif
#ifndef TOUCH_IRQ
#define TOUCH_IRQ -1 // Change to your Touchscreen IRQ pin (-1 if not used)
#endif

extern String _prevWeatherStr;
extern String _prevCalendarStr;
extern String _prevTimeStr;
extern bool _initialDisplay;

enum ScreenState {
  SCREEN_MAIN,
  SCREEN_WEATHER_DETAIL,
  SCREEN_CALENDAR_DETAIL
};

extern ScreenState currentScreen;

// --- UI Element Positions and Sizes ---
// Time (occupies most of the screen area)
#define TIME_FONT_SIZE 4
#define TIME_RECT_W tft.width()
#define TIME_RECT_H (tft.height() - BUTTON_HEIGHT - 10)

// --- UI Buttons on Main Screen (displayed at bottom, side-by-side) ---
#define BUTTON_WIDTH ((tft.width() / 2) - 10)
#define BUTTON_HEIGHT 35 // Tinggi tombol

// Weather Detail Button
#define WEATHER_BUTTON_X 5
#define WEATHER_BUTTON_Y_VISUAL (tft.height() - BUTTON_HEIGHT - 5) // Y visual untuk menggambar
#define WEATHER_BUTTON_W BUTTON_WIDTH
#define WEATHER_BUTTON_H BUTTON_HEIGHT
#define WEATHER_BUTTON_TEXT "Cuaca"
#define WEATHER_BUTTON_FONT_SIZE 1

// Calendar Detail Button
#define CALENDAR_BUTTON_X (tft.width() - BUTTON_WIDTH - 5)
#define CALENDAR_BUTTON_Y_VISUAL (tft.height() - BUTTON_HEIGHT - 5) // Y visual untuk menggambar
#define CALENDAR_BUTTON_W BUTTON_WIDTH
#define CALENDAR_BUTTON_H BUTTON_HEIGHT
#define CALENDAR_BUTTON_TEXT "Kalender"
#define CALENDAR_BUTTON_FONT_SIZE 1

// --- Back Button (on detail screens) ---
#define BACK_BUTTON_W 80
#define BACK_BUTTON_H 35
#define BACK_BUTTON_X (tft.width() - BACK_BUTTON_W - 5)
#define BACK_BUTTON_Y 5
#define BACK_BUTTON_TEXT "Kembali"
#define BACK_BUTTON_FONT_SIZE 1

// Display management function declarations
void initDisplay();
void updateDisplay(const String& timeStr, const String& weatherStr = "", const String& calendarStr = "");
bool getTouchPoint(int &x, int &y);
void displayWeatherDetail(const String& weatherDetail);
void displayCalendarDetail(const String& calendarDetail);

#endif