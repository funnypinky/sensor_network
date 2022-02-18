#pragma once

#include "SPI.h"
#include "TouchScreen.h"
#include "TFT_eSPI.h"

#include "FS.h"
#include "FFat.h"

#include "lvgl.h"

#include "lv_fs_if.h"

#define HOR 240
#define VER 320

LV_FONT_DECLARE(heebo_thin_24);
LV_FONT_DECLARE(heebo_thin_30);
LV_FONT_DECLARE(heebo_thin_48);

void setupHMI();
void updateTime();
void updateIndoorValues(float pressure, float temperature, float humidity);