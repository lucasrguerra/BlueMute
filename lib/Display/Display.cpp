#include <Display.h>

Display::Display():
    display(TFT_CS, TFT_DC, TFT_RESET)
{}

void Display::begin() {
    display.init(SCREEN_HEIGHT, SCREEN_WIDTH, SPI_MODE3);
    display.setRotation(1);
    display.fillScreen(ST77XX_BLUE);
}

void Display::clearScreen() {
    display.fillScreen(ST77XX_BLUE);
}

uint16_t Display::interpolateColor(uint16_t color1, uint16_t color2, uint32_t progress, uint32_t total) {
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5) & 0x3F;
    uint8_t b1 = color1 & 0x1F;

    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5) & 0x3F;
    uint8_t b2 = color2 & 0x1F;

    uint8_t r = r1 + (r2 - r1) * progress / total;
    uint8_t g = g1 + (g2 - g1) * progress / total;
    uint8_t b = b1 + (b2 - b1) * progress / total;

    return (r << 11) | (g << 5) | b;
}

void Display::drawCenteredText(const char* text, int y_offset) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((display.width() - w) / 2, (display.height() - h) / 2 + y_offset);
    display.print(text);
}

void Display::showLoadingScreen() {
    unsigned long startTime = millis();
    const int centerX = display.width() / 2;
    const int centerY = display.height() / 2;
    const int spinnerRadius = 20;

    while (millis() - startTime < 2000) {
        unsigned long elapsed = millis() - startTime;
        
        uint16_t bgColor = interpolateColor(ST77XX_BLACK, ST77XX_BLUE, elapsed, 2000);
        display.fillScreen(bgColor);

        float angle = (elapsed * 360.0f) / 500;
        for(int i = 0; i < 4; i++) {
            float lineAngle = angle + i * 90.0f;
            float rad = lineAngle * DEG_TO_RAD;
            int x = centerX + spinnerRadius * cos(rad);
            int y = centerY + spinnerRadius * sin(rad);
            display.drawLine(centerX, centerY, x, y, ST77XX_WHITE);
        }

        delay(35);
    }

    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2); 
    int index = 0;
    while (index < 40) {
        display.fillScreen(ST77XX_BLUE);
        drawCenteredText("BlueMute v1.0", -index);
        delay(5);
        index++;
    }
}

void Display::showMessage(const char* message) {
    display.fillScreen(ST77XX_BLUE);
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2);
    drawCenteredText("BlueMute v1.0", -40);

    // separate the message into lines by "\n"
    char* line = strtok((char*)message, "\n");
    int lineCount = 0;
    while (line != NULL) {
        drawCenteredText(line, lineCount * 20);
        line = strtok(NULL, "\n");
        lineCount++;
    }
}
