#include "hmi.hpp"
#include <stdio.h>
#include <string.h>

#include "FS.h"
#include "FFat.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

lv_obj_t *clockLabel;
lv_obj_t *dateLabel;

lv_obj_t *tempLabelIn;
lv_obj_t *humiLabelIn;
lv_obj_t *pressLabel;
lv_obj_t *tempLabelOut;
lv_obj_t *humiLabelOut;
lv_obj_t *minMaxTemp;

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

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x ");
    Serial.println(touchX);

    Serial.print("Data y ");
    Serial.println(touchY);
  }
}

void createHMI()
{

  static lv_style_t timeStyle;
  static lv_style_t dateStyle;
  static lv_style_t rightStyle;
  static lv_style_t borderless;
  static lv_style_t statusbar;
  static lv_style_t base;

  lv_style_init(&timeStyle);
  lv_style_init(&dateStyle);
  lv_style_init(&rightStyle);
  lv_style_init(&borderless);
  lv_style_init(&statusbar);
  lv_style_init(&base);

  lv_style_set_text_font(&timeStyle, &heebo_thin_48);
  lv_style_set_text_align(&timeStyle, LV_TEXT_ALIGN_RIGHT);
  lv_style_set_align(&timeStyle, LV_ALIGN_RIGHT_MID);
  lv_style_set_pad_all(&timeStyle, 1);

  lv_style_set_text_font(&dateStyle, &heebo_thin_24);
  lv_style_set_text_align(&dateStyle, LV_TEXT_ALIGN_RIGHT);
  lv_style_set_pad_all(&dateStyle, 1);

  lv_style_set_text_align(&rightStyle, LV_TEXT_ALIGN_LEFT);

  lv_style_set_border_width(&borderless, 0);
  lv_style_set_radius(&borderless, 0);

  lv_style_set_pad_all(&statusbar, 0);
  lv_style_set_bg_color(&statusbar, lv_color_hex(0x696969));
  // lv_style_set_bg_opa(&statusbar, LV_OPA_TRANSP);

  // lv_style_set_pad_all(&base, 0);
  //  lv_style_set_pad_row(&base, 0);
  //  lv_style_set_pad_column(&base, 0);
  lv_style_set_bg_color(&base, lv_color_hex(0x696969));

  lv_style_set_pad_column(&timeStyle, 0);
  lv_style_set_pad_row(&timeStyle, 0);
  lv_style_set_align(&timeStyle, LV_ALIGN_BOTTOM_RIGHT);
  lv_style_set_bg_opa(&timeStyle, LV_OPA_TRANSP);
  lv_style_set_border_side(&timeStyle, LV_BORDER_SIDE_RIGHT);
  lv_style_set_border_width(&timeStyle, 2);
  lv_style_set_border_color(&timeStyle, lv_color_hex(0xebe8e1));
  lv_style_set_border_opa(&timeStyle, LV_OPA_100);

  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_size(scr, VER, HOR);
  lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
  lv_obj_add_style(scr, &base, 0);
  lv_obj_add_style(scr, &borderless, 0);

  lv_obj_t *topStatus = lv_obj_create(scr);
  lv_obj_t *btn = lv_btn_create(topStatus);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_add_style(topStatus, &statusbar, 0);
  lv_obj_add_style(topStatus, &borderless, 0);
  lv_obj_set_size(topStatus, VER, HOR / 10);
  lv_obj_set_flex_flow(topStatus, LV_FLEX_FLOW_COLUMN);
  lv_obj_t *wifiStatus = lv_label_create(btn);
  lv_label_set_text(wifiStatus, LV_SYMBOL_WIFI);

  lv_obj_t *middle = lv_obj_create(scr);
  lv_obj_set_layout(middle, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(middle, LV_FLEX_FLOW_ROW);
  lv_obj_add_style(middle, &borderless, 0);
  lv_obj_set_style_bg_opa(middle, LV_OPA_TRANSP, 0);
  lv_obj_set_scrollbar_mode(middle, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_width(middle, LV_PCT(100));
  lv_obj_set_height(middle, LV_PCT(75));

  lv_obj_t *clockView = lv_obj_create(middle);
  lv_obj_set_layout(clockView, LV_LAYOUT_FLEX);
  lv_obj_set_scrollbar_mode(clockView, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_flex_flow(clockView, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_width(clockView, LV_PCT(52));
  lv_obj_set_height(clockView, LV_PCT(100));

  lv_obj_add_style(clockView, &borderless, 0);
  lv_obj_add_style(clockView, &timeStyle, 0);

  clockLabel = lv_label_create(clockView);
  // lv_obj_set_width(clockLabel, VER * .40);
  lv_obj_set_align(clockLabel, LV_ALIGN_RIGHT_MID);
  lv_obj_set_flex_grow(clockLabel, 0);
  lv_obj_set_width(clockLabel, LV_PCT(100));

  dateLabel = lv_label_create(clockView);
  lv_obj_set_align(dateLabel, LV_ALIGN_RIGHT_MID);
  lv_obj_add_style(dateLabel, &dateStyle, 0);
  lv_obj_set_width(dateLabel, LV_PCT(100));

  tempLabelIn = lv_label_create(clockView);
  lv_label_set_text_fmt(tempLabelIn, "%0.1f°C/%0.1f%%", 24.123, 24.23);
  lv_obj_add_style(tempLabelIn, &dateStyle, 0);
  lv_obj_set_width(tempLabelIn, LV_PCT(100));

  static lv_coord_t col_dsc[] = {LV_PCT(10), LV_PCT(95), LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_PCT(91), LV_PCT(10), LV_PCT(10), LV_GRID_TEMPLATE_LAST};

  lv_obj_t *apiView = lv_obj_create(middle);
  lv_obj_set_style_bg_opa(apiView, LV_OPA_TRANSP, 0);
  lv_obj_add_style(apiView, &borderless, 0);
  lv_obj_set_width(apiView, LV_PCT(50));
  lv_obj_set_style_pad_all(apiView, 0, 0);
  lv_obj_set_layout(apiView, LV_LAYOUT_GRID);
  lv_obj_set_style_grid_column_dsc_array(apiView, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(apiView, row_dsc, 0);
  lv_obj_set_style_pad_column(apiView, 0, 0);
  lv_obj_set_style_pad_row(apiView, 0, 0);
  lv_obj_set_style_pad_left(apiView, -2, 0);

  lv_obj_t *icon = lv_img_create(apiView);
  lv_obj_set_style_pad_top(icon, 5, 0);
  lv_obj_set_style_pad_bottom(icon, 8, 0);
  lv_obj_set_align(icon, LV_ALIGN_TOP_LEFT);
  lv_img_set_src(icon, "A:/100.png");
  lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_STRETCH, 0, 1,
                       LV_GRID_ALIGN_STRETCH, 0, 1);

  pressLabel = lv_label_create(apiView);
  lv_obj_set_grid_cell(pressLabel, LV_GRID_ALIGN_START, 1, 1,
                       LV_GRID_ALIGN_CENTER, 0, 1);
  lv_label_set_text_fmt(pressLabel, "%0.1fhPa", 1024.1);
  // lv_obj_set_width(pressLabel, LV_PCT(100));
  lv_obj_add_style(pressLabel, &dateStyle, 0);
  lv_obj_set_style_text_font(pressLabel, &heebo_thin_20, 0);

  minMaxTemp = lv_label_create(apiView);
  lv_label_set_text_fmt(minMaxTemp, "%0.0f°C/%0.0f°C", 24.123, 24.23);
  lv_obj_set_grid_cell(minMaxTemp, LV_GRID_ALIGN_STRETCH, 0, 2,
                       LV_GRID_ALIGN_STRETCH, 1, 1);
  lv_obj_add_style(minMaxTemp, &dateStyle, 0);
  lv_obj_add_style(minMaxTemp, &rightStyle, 0);
  lv_obj_set_width(minMaxTemp, LV_PCT(100));

  tempLabelOut = lv_label_create(apiView);
  lv_label_set_text_fmt(tempLabelOut, "%0.1f°C/%0.1f%%", 24.123, 24.23);
  lv_obj_set_grid_cell(tempLabelOut, LV_GRID_ALIGN_STRETCH, 0, 2,
                       LV_GRID_ALIGN_STRETCH, 2, 1);
  lv_obj_add_style(tempLabelOut, &dateStyle, 0);
  lv_obj_add_style(tempLabelOut, &rightStyle, 0);
  lv_obj_set_width(tempLabelOut, LV_PCT(100));
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
  lv_label_set_text_fmt(tempLabelIn, "%0.1f°C/%0.1f%%", temperature, humidity);
  lv_label_set_text_fmt(pressLabel, "%0.1fhPa", pressure);
  lv_refr_now(NULL);
}

void updateOutdoorValues(float temperature, float humidity)
{
  lv_label_set_text_fmt(tempLabelOut, "%0.1f°C/%0.1f%%", temperature, humidity);
  lv_refr_now(NULL);
}

void updateForecastTemp(float min, float max)
{
  lv_label_set_text_fmt(minMaxTemp, "%0.0f°C/%0.0f°C", min, max);
  lv_refr_now(NULL);
}

void listDir(const char *path)
{
  lv_fs_dir_t dir;
  lv_fs_res_t res;

  res = lv_fs_dir_open(&dir, path);

  if (res != LV_FS_RES_OK)
  {
    Serial.println("Error");
    return;
  }

  char fn[256];
  while (1)
  {
    res = lv_fs_dir_read(&dir, fn);
    if (res != LV_FS_RES_OK)
    {
      Serial.printf("Error opening... %s \n", fn);
      Serial.println(res);
      break;
    }
    if (strlen(fn) == 0)
    {
      break;
    }
    printf("%s\n", fn);
  }
  lv_fs_dir_close(&dir);
}

void setupHMI()
{

  tft.begin();

  tft.setRotation(1);

  lv_init();
  lv_fs_if_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;

  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv); /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;

  lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);

  lv_png_init();
  lv_split_jpeg_init();

  createHMI();
  // updateTime();
}