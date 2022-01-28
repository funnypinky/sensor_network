#include "hmi.hpp"
#include <stdio.h>
#include <string.h>

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

lv_obj_t *clockLabel;
lv_obj_t *dateLabel;

lv_obj_t *tempLabelIn;
lv_obj_t *humiLabelIn;
lv_obj_t *pressLabel;
lv_obj_t *tempLabelOut;
lv_obj_t *humiLabelOut;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void createHMI()
{

  static lv_style_t timeStyle;
  static lv_style_t dateStyle;
  static lv_style_t valueStyle;
  static lv_style_t borderless;
  static lv_style_t pressureStyle;
  lv_style_init(&timeStyle);
  lv_style_init(&dateStyle);
  lv_style_init(&borderless);
  lv_style_init(&pressureStyle);
  lv_style_init(&valueStyle);
  lv_style_set_text_font(&timeStyle, LV_STATE_DEFAULT, &lv_font_montserrat_48);
  lv_style_set_text_font(&dateStyle, LV_STATE_DEFAULT, &lv_font_montserrat_20);
  lv_style_set_text_font(&valueStyle, LV_STATE_DEFAULT, &lv_font_montserrat_30);
  lv_style_set_border_width(&borderless, LV_STATE_DEFAULT, 0);
  lv_style_set_radius(&borderless, LV_STATE_DEFAULT, 0);
  lv_style_set_text_font(&pressureStyle, LV_STATE_DEFAULT, &lv_font_montserrat_24);

  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean_style_list(scr, LV_OBJ_PART_MAIN);
  lv_obj_set_style_local_bg_opa(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_size(scr, VER, HOR);

  lv_obj_t *apiView = lv_obj_create(scr, NULL);
  lv_obj_add_style(apiView, LV_OBJ_PART_MAIN, &borderless);
  lv_obj_set_size(apiView, VER / 2, HOR * 2 / 3);
  lv_obj_t *icon = lv_img_create(apiView, NULL);
  lv_obj_set_pos(icon, (VER / 2) * .25, 10);
  lv_img_set_src(icon, "/spiffs/100.png");

  lv_obj_t *weatherView = lv_obj_create(scr, NULL);
  lv_obj_add_style(weatherView, LV_OBJ_PART_MAIN, &borderless);
  lv_obj_set_size(weatherView, VER / 2, HOR * 3 / 3);
  lv_obj_set_pos(weatherView, VER / 2, 0);

  lv_obj_t *clockView = lv_obj_create(scr, NULL);
  lv_obj_set_size(clockView, VER / 2, HOR * 1 / 3);
  lv_obj_add_style(clockView, LV_OBJ_PART_MAIN, &borderless);
  lv_obj_set_y(clockView, HOR * 2 / 3);

  clockLabel = lv_label_create(clockView, NULL);
  lv_obj_align(clockLabel, NULL, LV_ALIGN_CENTER, 0, -10);
  lv_label_set_align(clockLabel, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(clockLabel, LV_LABEL_PART_MAIN, &timeStyle);
  lv_obj_set_auto_realign(clockLabel, true);

  dateLabel = lv_label_create(clockView, NULL);
  lv_obj_align(dateLabel, NULL, LV_ALIGN_CENTER, 0, 21);
  lv_obj_set_auto_realign(dateLabel, true);
  lv_label_set_align(dateLabel, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(dateLabel, LV_LABEL_PART_MAIN, &dateStyle);

  tempLabelIn = lv_label_create(weatherView, NULL);
  lv_obj_set_auto_realign(tempLabelIn, true);
  lv_obj_align(tempLabelIn, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 4);
  lv_label_set_text_fmt(tempLabelIn, "%0.2f °C", 24.123);
  lv_obj_add_style(tempLabelIn, LV_LABEL_PART_MAIN, &valueStyle);

  humiLabelIn = lv_label_create(weatherView, NULL);
  lv_obj_set_auto_realign(humiLabelIn, true);
  lv_obj_align(humiLabelIn, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 34);
  lv_label_set_text_fmt(humiLabelIn, "%0.2f  %%", 24.123);
  lv_obj_add_style(humiLabelIn, LV_LABEL_PART_MAIN, &valueStyle);

  pressLabel = lv_label_create(weatherView, NULL);
  lv_obj_set_auto_realign(pressLabel, true);
  lv_obj_align(pressLabel, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 100);
  lv_label_set_text_fmt(pressLabel, "%0.1f hPa", 1024.123);
  lv_obj_add_style(pressLabel, LV_LABEL_PART_MAIN, &pressureStyle);

  tempLabelOut = lv_label_create(weatherView, NULL);
  lv_obj_set_auto_realign(tempLabelOut, true);
  lv_obj_align(tempLabelOut, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 170);
  lv_label_set_text_fmt(tempLabelOut, "%0.2f °C", 24.123);
  lv_obj_add_style(tempLabelOut, LV_LABEL_PART_MAIN, &valueStyle);

  humiLabelOut = lv_label_create(weatherView, NULL);
  lv_obj_set_auto_realign(humiLabelOut, true);
  lv_obj_align(humiLabelOut, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 200);
  lv_label_set_text_fmt(humiLabelOut, "%0.2f  %%", 24.123);
  lv_obj_add_style(humiLabelOut, LV_LABEL_PART_MAIN, &valueStyle);
}
void listDir()
{
  lv_fs_dir_t dir;
  lv_fs_res_t res;

  res = lv_fs_dir_open(&dir, "L:/");
  char fn[256];
  while (1)
  {
    res = lv_fs_dir_read(&dir, fn);
    if (res != LV_FS_RES_OK)
    {
      break;
    }
    if (strlen(fn) == 0)
    {
      break;
    }
    Serial.printf("%s\n", fn);
  }
  lv_fs_dir_close(&dir);
}
void updateTime()
{
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  char bufferClock[10];
  strftime(bufferClock, sizeof(bufferClock), "%R", &timeinfo);
  lv_label_set_text(clockLabel, bufferClock);
  char bufferDate[14];
  strftime(bufferDate, sizeof(bufferDate), "%b %d, %Y", &timeinfo);
  lv_label_set_text_fmt(dateLabel, "%s", bufferDate);
  lv_refr_now(NULL);
}

void updateIndoorValues(float pressure, float temperature, float humidity)
{
  lv_label_set_text_fmt(humiLabelIn, "%0.2f  %%", humidity);
  lv_label_set_text_fmt(tempLabelIn, "%0.2f °C", temperature);
  lv_label_set_text_fmt(pressLabel, "%0.1f hPa", pressure);
  lv_refr_now(NULL);
}
void filesystem_list_path(const char *path)
{
  lv_fs_dir_t dir;
  lv_fs_res_t res;
  res = lv_fs_dir_open(&dir, path);
  if (res != LV_FS_RES_OK)
  {
    Serial.printf("Error opening directory %s\n", path);
  }
  else
  {
    char fn[256];
    while (1)
    {
      res = lv_fs_dir_read(&dir, fn);
      if (res != LV_FS_RES_OK)
      {
        Serial.printf("Directory %s can not be read\n", path);
        break;
      }

      /*fn is empty, if not more files to read*/
      if (strlen(fn) == 0)
      {
        Serial.printf("Directory %s listing complete\n", path);
        break;
      }

      // LOG_VERBOSE(TAG_LVFS, D_BULLET "%s", fn);
    }
  }

  lv_fs_dir_close(&dir);
}

void setupHMI()
{

  tft.begin();

  tft.setRotation(1);

  lv_init();

  lv_disp_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &draw_buf;

  lv_disp_drv_register(&disp_drv);

  lv_png_init();

  /* Mount SPIFFS */
  esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 99,
      .format_if_mount_failed = true};

  esp_vfs_spiffs_register(&conf);
  size_t total = 0, used = 0;
  esp_spiffs_info(conf.partition_label, &total, &used);

  createHMI();
  // updateTime();
}