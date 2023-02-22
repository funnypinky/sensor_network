#include "display.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
// Create a U8g2log object
U8G2LOG u8g2log;

// assume 4x6 font, define width and height
#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 10

// allocate memory
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

void initDisplay() {
  Serial.println("\n init Display");
  u8g2.begin();
  u8g2.setFont(u8g2_font_squeezed_b7_tr);
}

void writeInfo(int row, const char *info) {
  u8g2.drawStr(0,row*7,info);
  u8g2.sendBuffer();
}