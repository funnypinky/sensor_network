#pragma once

#include "SPI.h"
#include "TFT_eSPI.h"

#include "lvgl.h"

#include "lv_fs_if.h"

#include "controller.hpp"

#define HOR 240
#define VER 320

LV_FONT_DECLARE(heebo_thin_20);
LV_FONT_DECLARE(heebo_thin_24);
LV_FONT_DECLARE(heebo_thin_30);
LV_FONT_DECLARE(heebo_thin_48);

void setupHMI();
void updateTime();
void updateIndoorValues(float pressure, float temperature, float humidity);
void updateOutdoorValues(float temperature, float humidity);
void updateForecastTemp(float min, float max);