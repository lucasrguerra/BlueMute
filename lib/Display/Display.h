#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Arduino_GFX.h>
#include <Adafruit_ST7789.h>
#include <math.h>

#define TFT_SCL 18
#define TFT_SDA 23
#define TFT_RESET 33
#define TFT_DC 4
#define TFT_CS 21
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

class Display {
    private:
        Adafruit_ST7789 display;
        uint16_t interpolateColor(uint16_t color1, uint16_t color2, uint32_t progress, uint32_t total);
        void drawCenteredText(const char* message, int y_offset);

    public:
        Display();
        void begin();
        void clearScreen();
        void showLoadingScreen();
        void showMessage(const char* message);
};

#endif
