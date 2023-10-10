/*
   MIT License

  Copyright (c) 2022 Felix Biego

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ______________  _____
  ___  __/___  /_ ___(_)_____ _______ _______
  __  /_  __  __ \__  / _  _ \__  __ `/_  __ \
  _  __/  _  /_/ /_  /  /  __/_  /_/ / / /_/ /
  /_/     /_.___/ /_/   \___/ _\__, /  \____/
                              /____/

*/

#define LGFX_USE_V1

#include <Arduino.h>
#include "main.h"
#include <lvgl.h>
#include <LovyanGFX.hpp>
#include <ChronosESP32.h>
#include <Timber.h>

#ifdef USE_UI
#include "ui/ui.h"
#endif

#ifdef PLUS
#define SCR 30
class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_ST7796 _panel_instance;

  lgfx::Bus_Parallel8 _bus_instance;

  lgfx::Light_PWM _light_instance;

  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.port = 0;
      cfg.freq_write = 40000000;
      cfg.pin_wr = 47; // pin number connecting WR
      cfg.pin_rd = -1; // pin number connecting RD
      cfg.pin_rs = 0;  // Pin number connecting RS(D/C)
      cfg.pin_d0 = 9;  // pin number connecting D0
      cfg.pin_d1 = 46; // pin number connecting D1
      cfg.pin_d2 = 3;  // pin number connecting D2
      cfg.pin_d3 = 8;  // pin number connecting D3
      cfg.pin_d4 = 18; // pin number connecting D4
      cfg.pin_d5 = 17; // pin number connecting D5
      cfg.pin_d6 = 16; // pin number connecting D6
      cfg.pin_d7 = 15; // pin number connecting D7

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }

    {                                      // Set display panel control.
      auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

      cfg.pin_cs = -1;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = 4;   // pin number where RST is connected (-1 = disable)
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values ​​for each panel, and the pin number (-1 = disable) to which BUSY is connected, so please try commenting out any unknown items.

      cfg.memory_width = 320;  // Maximum width supported by driver IC
      cfg.memory_height = 480; // Maximum height supported by driver IC
      cfg.panel_width = 320;   // actual displayable width
      cfg.panel_height = 480;  // actual displayable height
      cfg.offset_x = 0;        // Panel offset in X direction
      cfg.offset_y = 0;        // Panel offset in Y direction
#ifdef PORTRAIT
      cfg.offset_rotation = 2;
#else
      cfg.offset_rotation = 3;
#endif
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      _panel_instance.config(cfg);
    }

    {                                      // Set backlight control. (delete if not necessary)
      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = 45;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 0; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }

    { // Configure settings for touch screen control. (delete if not necessary)
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;   // Minimum X value (raw value) obtained from the touchscreen
      cfg.x_max = 319; // Maximum X value (raw value) obtained from the touchscreen
      cfg.y_min = 0;   // Minimum Y value obtained from touchscreen (raw value)
      cfg.y_max = 479; // Maximum Y value (raw value) obtained from the touchscreen
      cfg.pin_int = 7; // pin number to which INT is connected
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;

      // For I2C connection
      cfg.i2c_port = 0;    // Select I2C to use (0 or 1)
      cfg.i2c_addr = 0x38; // I2C device address number
      cfg.pin_sda = 6;     // pin number where SDA is connected
      cfg.pin_scl = 5;     // pin number to which SCL is connected
      cfg.freq = 400000;   // set I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance); // Set the touchscreen to the panel.
    }

    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

#else
#define SCR 8
class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_ST7796 _panel_instance;

  lgfx::Bus_SPI _bus_instance;

  lgfx::Light_PWM _light_instance;

  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config(); // Get the structure for bus configuration.

      // SPI bus settings
      cfg.spi_host = VSPI_HOST; // Select the SPI to use ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // * With the ESP-IDF version upgrade, VSPI_HOST and HSPI_HOST descriptions are deprecated, so if an error occurs, use SPI2_HOST and SPI3_HOST instead.
      cfg.spi_mode = 3;                  // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000;         // SPI clock when sending (up to 80MHz, rounded to 80MHz divided by an integer)
      cfg.freq_read = 6000000;           // SPI clock when receiving
      cfg.spi_3wire = false;             // set to true if receiving on MOSI pin
      cfg.use_lock = true;               // set to true to use transaction lock
      cfg.dma_channel = SPI_DMA_CH_AUTO; // Set the DMA channel to use (0=not use DMA / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=auto setting)
      // * With the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting) is recommended for the DMA channel. Specifying 1ch and 2ch is deprecated.
      cfg.pin_sclk = 14; // set SPI SCLK pin number
      cfg.pin_mosi = 13; // Set MOSI pin number for SPI
      cfg.pin_miso = -1; // set SPI MISO pin number (-1 = disable)
      cfg.pin_dc = 21;   // Set SPI D/C pin number (-1 = disable)

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }

    {                                      // Set display panel control.
      auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

      cfg.pin_cs = 15;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = 22;  // pin number where RST is connected (-1 = disable)
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values ​​for each panel, and the pin number (-1 = disable) to which BUSY is connected, so please try commenting out any unknown items.

      cfg.memory_width = 320;  // Maximum width supported by driver IC
      cfg.memory_height = 480; // Maximum height supported by driver IC
      cfg.panel_width = 320;   // actual displayable width
      cfg.panel_height = 480;  // actual displayable height
      cfg.offset_x = 0;        // Panel offset in X direction
      cfg.offset_y = 0;        // Panel offset in Y direction
#ifdef PORTRAIT
      cfg.offset_rotation = 0;
#else
      cfg.offset_rotation = 1;
#endif
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;

      _panel_instance.config(cfg);
    }

    {                                      // Set backlight control. (delete if not necessary)
      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = 23;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 1; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }

    { // Configure settings for touch screen control. (delete if not necessary)
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;    // Minimum X value (raw value) obtained from the touchscreen
      cfg.x_max = 319;  // Maximum X value (raw value) obtained from the touchscreen
      cfg.y_min = 0;    // Minimum Y value obtained from touchscreen (raw value)
      cfg.y_max = 479;  // Maximum Y value (raw value) obtained from the touchscreen
      cfg.pin_int = 39; // pin number to which INT is connected
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;

      // For I2C connection
      cfg.i2c_port = 1;    // Select I2C to use (0 or 1)
      cfg.i2c_addr = 0x38; // I2C device address number
      cfg.pin_sda = 18;    // pin number where SDA is connected
      cfg.pin_scl = 19;    // pin number to which SCL is connected
      cfg.freq = 400000;   // set I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance); // Set the touchscreen to the panel.
    }

    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

#endif

// Create an instance of the prepared class.
LGFX tft;
ChronosESP32 watch("Standby");

ChronosTimer alertTimer;

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

static lv_color_t disp_draw_buf[screenWidth * SCR];
static lv_color_t disp_draw_buf2[screenWidth * SCR];

lv_img_dsc_t digits[10] = {ui_img_zero_png, ui_img_one_png, ui_img_two_png, ui_img_three_png, ui_img_four_png,
                           ui_img_five_png, ui_img_six_png, ui_img_seven_png, ui_img_eight_png, ui_img_nine_png};

lv_img_dsc_t notificationIcons[] = {
    ui_img_sms_png,       // SMS
    ui_img_mail_png,      // Mail
    ui_img_penguin_png,   // Penguin
    ui_img_skype_png,     // Skype
    ui_img_whatsapp_png,  // WhatsApp
    ui_img_mail_png,      // Mail2
    ui_img_line_png,      // Line
    ui_img_twitter_png,   // Twitter
    ui_img_facebook_png,  // Facebook
    ui_img_messenger_png, // Messenger
    ui_img_instagram_png, // Instagram
    ui_img_weibo_png,     // Weibo
    ui_img_kakao_png,     // Kakao
    ui_img_viber_png,     // Viber
    ui_img_vkontakte_png, // Vkontakte
    ui_img_telegram_png,  // Telegram
    ui_img_wechat_png     // Wechat
};

lv_img_dsc_t weatherIcons[] = {
    ui_img_602206286,
    ui_img_602205261,
    ui_img_602199888,
    ui_img_602207311,
    ui_img_dy4_png,
    ui_img_602200913,
    ui_img_602195540,
    ui_img_602202963};

String weatherConditions[] = {"Partial Clouds", "Sunny", "Snow", "Rain", "Cloudy", "Tornado", "Windy", "Haze"};

int getNotificationIconIndex(int id);
int getWeatherIconIndex(int id);


int getWeatherIconIndex(int id)
{
  switch (id)
  {
  case 0:
    return 0;
  case 1:
    return 1;
  case 2:
    return 2;
  case 3:
    return 3;
  case 4:
    return 4;
  case 5:
    return 5;
  case 6:
    return 6;
  case 7:
    return 7;
  default:
    return 0;
  }
}

int getNotificationIconIndex(int id)
{
  switch (id)
  {
  case 0x03:
    return 0;
  case 0x04:
    return 1;
  case 0x07:
    return 2;
  case 0x08:
    return 3;
  case 0x0A:
    return 4;
  case 0x0B:
    return 5;
  case 0x0E:
    return 6;
  case 0x0F:
    return 7;
  case 0x10:
    return 8;
  case 0x11:
    return 9;
  case 0x12:
    return 10;
  case 0x13:
    return 11;
  case 0x14:
    return 12;
  case 0x16:
    return 13;
  case 0x17:
    return 14;
  case 0x18:
    return 15;
  case 0x09:
    return 16;
  default:
    return 0;
  }
}
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);

  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY);

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
  }
}

void connectionCallback(bool state)
{
  Serial.print("Connection state: ");
  Serial.println(state ? "Connected" : "Disconnected");
  // bool connected = watch.isConnected();
}

void notificationCallback(Notification notification)
{
  Serial.print("Notification received at ");
  Serial.println(notification.time);
  Serial.print("From: ");
  Serial.print(notification.app);
  Serial.print("\tIcon: ");
  Serial.println(notification.icon);
  Serial.println(notification.message);

  lv_label_set_text(ui_alertTitle, notification.app.c_str());
  lv_label_set_text(ui_alertText, notification.message.c_str());
  lv_img_set_src(ui_alertIcon, &notificationIcons[getNotificationIconIndex(notification.icon)]);

  alertTimer.time = millis();
  alertTimer.active = true;
  lv_obj_clear_flag(ui_alertPanel, LV_OBJ_FLAG_HIDDEN);
}

void ringerCallback(String caller, bool state)
{
  if (state)
  {
    Serial.print("Ringer: Incoming call from ");
    Serial.println(caller);
    lv_label_set_text(ui_callerName, caller.c_str());
    pulseCall_Animation(ui_callIcon, 0);
    textUpDown_Animation(ui_callText, 0);
    textSide_Animation(ui_callerName, 0);
    lv_obj_clear_flag(ui_callPanel, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    Timber.i("Ringer dismissed");
    lv_obj_add_flag(ui_callPanel, LV_OBJ_FLAG_HIDDEN);
    lv_anim_del(ui_callerName, NULL);
    lv_anim_del(ui_callIcon, NULL);
    lv_anim_del(ui_callText, NULL);
  }
}

void configCallback(Config config, uint32_t a, uint32_t b)
{
  switch (config)
  {
  case CF_TIME:
    // time is saved internally
    // command with no parameters
    Timber.i("The time has been set");
    Timber.i(watch.getTimeDate());

    lv_anim_del_all();
    lv_img_set_angle(ui_secondHand, watch.getSecond() * 60);
    lv_img_set_angle(ui_secondHand1, watch.getSecond() * 60);
    clockWise_Animation(ui_secondHand, 0);
    clockWise_Animation(ui_secondHand1, 0);

    lv_calendar_set_today_date(ui_calendar, watch.getYear(), watch.getMonth() + 1, watch.getDay());
    lv_calendar_set_showed_date(ui_calendar, watch.getYear(), watch.getMonth() + 1);

    break;
  case CF_WEATHER:
    // weather is saved
    Serial.println("Weather received");
    if (a)
    {
      // if a == 1, high & low temperature values might not yet be updated
      if (a == 2)
      {
        String temp = String(watch.getWeatherAt(0).temp) + "°";
        String range = "H:" + String(watch.getWeatherAt(0).high) + "°  L:" + String(watch.getWeatherAt(0).low) + "°";

        lv_label_set_text(ui_weatherTemperature, temp.c_str());
        lv_label_set_text(ui_weatherRange, range.c_str());
        lv_label_set_text(ui_weatherCondition, weatherConditions[getWeatherIconIndex(watch.getWeatherAt(0).icon)].c_str());
        lv_img_set_src(ui_weatherIcon, &weatherIcons[getWeatherIconIndex(watch.getWeatherAt(0).icon)]);
      }
    }
    if (b)
    {
      Serial.print("City name: ");
      String city = watch.getWeatherCity(); //
      Serial.print(city);
      lv_label_set_text(ui_weatherCity, city.c_str());
    }
    Serial.println();
    break;
  }
}

void logCallback(Level level, unsigned long time, String message)
{
  Serial.print(message);

  switch (level)
  {
  case ERROR:
    // maybe save only errors to local storage
    break;
  }
}

void homeScreenLoaded(lv_event_t *e)
{
  lv_obj_set_parent(ui_alertPanel, ui_homeScreen);
  lv_obj_set_parent(ui_callPanel, ui_homeScreen);

  lv_anim_del_all();
  int angle = watch.getSecond() * 60;
  lv_img_set_angle(ui_secondHand, angle);
  clockWise_Animation(ui_secondHand, 0);
}

void clockScreenLoaded(lv_event_t *e)
{
  lv_obj_set_parent(ui_alertPanel, ui_clockScreen);
  lv_obj_set_parent(ui_callPanel, ui_clockScreen);

  lv_anim_del_all();
  int angle = watch.getSecond() * 60;
  lv_img_set_angle(ui_secondHand1, angle);
  clockWise_Animation(ui_secondHand1, 0);
}

void musicPrevious(lv_event_t *e)
{
  watch.musicControl(MUSIC_PREVIOUS);
}

void musicToggle(lv_event_t *e)
{
  watch.musicControl(MUSIC_TOGGLE);
}

void musicNext(lv_event_t *e)
{
  watch.musicControl(MUSIC_NEXT);
}

void volumeChanged(lv_event_t *e)
{
  uint8_t volume = (uint8_t)lv_slider_get_value(ui_volumeSlider);
  watch.setVolume(volume);
}

void setup()
{
  Serial.begin(115200);

  Timber.setLogCallback(logCallback);

  tft.init();
  tft.initDMA();
  tft.startWrite();

  lv_init();

  Timber.i("Width %d\tHeight %d", screenWidth, screenHeight);

  if (!disp_draw_buf)
  {
    Timber.e("LVGL disp_draw_buf allocate failed!");
  }
  else
  {

    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, disp_draw_buf2, screenWidth * SCR);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

#ifdef USE_UI
    ui_init();

    lv_obj_set_scroll_snap_y(ui_infoPanel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_snap_y(ui_clockPanel, LV_SCROLL_SNAP_CENTER);

#else
    lv_obj_t *label1 = lv_label_create(lv_scr_act());
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 100);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label1, screenWidth - 30);
    lv_label_set_text(label1, "Hello there! You have not included UI files, add you UI files and "
                              "uncomment this line\n'//#define USE_UI' in include/main.h\n"
                              "You should be able to move the slider below");

    /*Create a slider below the label*/
    lv_obj_t *slider1 = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider1, screenWidth - 40);
    lv_obj_align_to(slider1, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
#endif

    watch.setConnectionCallback(connectionCallback);
    watch.setNotificationCallback(notificationCallback);
    watch.setRingerCallback(ringerCallback);
    watch.setConfigurationCallback(configCallback);

    watch.begin();
    watch.setBattery(50);
    watch.clearNotifications();
    watch.set24Hour(true);

    Timber.i("Setup done");
  }
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  watch.loop();

  int hour = watch.getHourC();
  int minute = watch.getMinute();

  lv_img_set_src(ui_hour1, &digits[hour / 10]);
  lv_img_set_src(ui_hour2, &digits[hour % 10]);
  lv_img_set_src(ui_minute1, &digits[minute / 10]);
  lv_img_set_src(ui_minute2, &digits[minute % 10]);

  lv_img_set_angle(ui_minuteHand, minute * 60);
  lv_img_set_angle(ui_hourHand, hour * 300 + minute * 5);
  lv_img_set_angle(ui_minuteHand1, minute * 60);
  lv_img_set_angle(ui_hourHand1, hour * 300 + minute * 5);

  if (alertTimer.active)
  {
    if (alertTimer.time + alertTimer.duration < millis())
    {
      alertTimer.active = false;
      lv_obj_add_flag(ui_alertPanel, LV_OBJ_FLAG_HIDDEN);
    }
  }
}