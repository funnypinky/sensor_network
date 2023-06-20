#include "display.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

CircularBuffer<logInfo, 3> infoFifo;
void initDisplay() {
  Serial.println("\n init Display");
  if(!u8x8.begin()) {
    Serial.println(F("Display init failed!"));
    for(;;);
  }
  u8x8.clearDisplay();
  u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  }

void writeInfo(int row, const char *info, bool little) {
    
    if(little) {
      u8x8.setFont(u8x8_font_5x7_f);
    } else {
      u8x8.setFont(u8x8_font_victoriamedium8_r);
    }
    u8x8.clearLine(row);
    u8x8.drawString(0,row,info);
}
void writeDateTime(const char *date, const char *time){
  int lenDate = u8x8.getUTF8Len(date);
  int lenTime = u8x8.getUTF8Len(time);
  u8x8.clearLine(1);
  u8x8.setCursor(0,0);
  u8x8.print(date);
  u8x8.setCursor(u8x8.getCols()-lenTime,0);
  u8x8.print(time);
}

void writeLog(logInfo info) {
  infoFifo.unshift(info);
  for (decltype(infoFifo)::index_t i = 0; i < infoFifo.size(); i++) {
    writeInfo(i+3, infoFifo[i].timeStamp.c_str(),true);
    writeInfo(i+4, infoFifo[i].rssi.c_str(),true);
}
}