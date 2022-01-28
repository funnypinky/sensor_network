#pragma once

#include "SPI.h"
#include "TouchScreen.h"
#include "TFT_eSPI.h"

#include "FS.h"
#include "SPIFFS.h"

#include "esp_spiffs.h"

#include "lv_png.h"

#include "lvgl.h"

#define HOR 240
#define VER 320

void setupHMI();
void updateTime();
void updateIndoorValues(float pressure, float temperature, float humidity);