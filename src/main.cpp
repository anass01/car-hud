/*
   MIT License

  Copyright (c) 2025 Felix Biego

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

#include <Arduino.h>
#include "main.h"
#include <Preferences.h>
#include "hud_ui.h"
#include <NimBLEDevice.h>
#include <Timber.h>

#define LVGL_LOCK() xSemaphoreTakeRecursive(lvgl_mutex, portMAX_DELAY)
#define LVGL_UNLOCK() xSemaphoreGiveRecursive(lvgl_mutex)

// Locking LVGL calls to prevent concurrent access
// This is important because LVGL is not thread-safe
// and we need to ensure that only one task is accessing it at a time
// This is done using a recursive mutex
#define LVGL_EXEC(code) \
  do                    \
  {                     \
    if (LVGL_LOCK())    \
    {                   \
      code;             \
      LVGL_UNLOCK();    \
    }                   \
  } while (0)

/* ---------- OBD COMMANDS (ASCII + CR) ---------- */
const uint8_t CMD_ATZ[] = {0x41, 0x54, 0x5A, 0x0D};               // Reset (ATZ)
const uint8_t CMD_ATE0[] = {0x41, 0x54, 0x45, 0x30, 0x0D};        // Echo off (ATE0)
const uint8_t CMD_ATSP6[] = {0x41, 0x54, 0x53, 0x50, 0x36, 0x0D}; // Set protocol to CAN 11/500 (ATSP6)

const uint8_t CMD_SPEED[] = {0x30, 0x31, 0x30, 0x44, 0x0D}; // Vehicle speed (010D)
const uint8_t CMD_RPM[] = {0x30, 0x31, 0x30, 0x43, 0x0D};   // Engine RPM (010C)
const uint8_t CMD_MAP[] = {0x30, 0x31, 0x30, 0x42, 0x0D};   // Intake Manifold Absolute Pressure / Turbo MAP (010B)
const uint8_t CMD_FUEL[] = {0x30, 0x31, 0x32, 0x46, 0x0D};  // Fuel Capacity (012F)
const uint8_t CMD_TEMP[] = {0x30, 0x31, 0x30, 0x35, 0x0D};  // Coolant Temperature (0105)

Preferences prefs;
HWCDC USBSerial;

/* ---------- LVGL DISPLAY ---------- */
uint8_t lv_buffer[2][LV_BUFFER_SIZE];
lv_obj_t *boot_screen;
lv_obj_t *dashboard_screen;
lv_obj_t *settings_screen;
SemaphoreHandle_t lvgl_mutex;

bool should_restart = false;

/* OBD UUIDs (16-bit, vendor specific) */
static NimBLEUUID OBD_SERVICE_UUID("FFF0");
static NimBLEUUID OBD_CHAR_UUID("FFF1");

static const NimBLEAdvertisedDevice *obdDevice = nullptr;
static NimBLEClient *client = nullptr;
static NimBLERemoteCharacteristic *obdChar = nullptr;
static NimBLEScan *scan = nullptr;

/* ---------- HEX UTILS ---------- */
static uint8_t hexToByte(uint8_t hi, uint8_t lo)
{
  if (hi >= 'a' && hi <= 'f') hi -= 32;
  if (lo >= 'a' && lo <= 'f') lo -= 32;
  uint8_t h = (hi >= '0' && hi <= '9') ? (hi - '0') : ((hi >= 'A' && hi <= 'F') ? (hi - 'A' + 10) : 0);
  uint8_t l = (lo >= '0' && lo <= '9') ? (lo - '0') : ((lo >= 'A' && lo <= 'F') ? (lo - 'A' + 10) : 0);
  return (h << 4) | l;
}

/**
 * Format byte buffer to hex String
 *
 * @param data    Input byte buffer
 * @param len     Number of bytes
 * @param prefix  Add "0x" before each byte
 * @param space   Add space between bytes
 * @param wrap    Bytes per line (0 = no wrap)
 */
String formatHexString(
    const uint8_t *data,
    size_t len,
    bool prefix = true,
    bool space = true,
    uint8_t wrap = 0)
{
  const char *hex = "0123456789ABCDEF";
  String out;
  out.reserve(len * (prefix ? 4 : 2) + len); // avoid reallocations

  for (size_t i = 0; i < len; i++)
  {
    if (wrap && i > 0 && (i % wrap) == 0)
      out += '\n';

    if (prefix)
      out += "0x";

    out += hex[(data[i] >> 4) & 0x0F];
    out += hex[data[i] & 0x0F];

    if (space && i < len - 1)
      out += ' ';
  }

  return out;
}

/* ---------- SIMPLE PARSER ---------- */
void parseObd(const uint8_t *data, size_t len)
{
  if (len < 4)
    return;

  // Check for error response ("ERROR", "CAN ERROR", etc.)
  for (size_t i = 0; i + 4 <= len; i++)
  {
    if ((data[i] == 'E' || data[i] == 'e') &&
        (data[i + 1] == 'R' || data[i + 1] == 'r') &&
        (data[i + 2] == 'R' || data[i + 2] == 'r') &&
        (data[i + 3] == 'O' || data[i + 3] == 'o'))
    {
      LVGL_EXEC(lv_subject_set_int(&can_error, 1));
      return;
    }
  }

  // Look for Mode 01 response: ASCII "41" anywhere in buffer
  int p41 = -1;
  for (size_t i = 0; i + 1 < len; i++)
  {
    if (data[i] == '4' && data[i + 1] == '1')
    {
      p41 = i;
      break;
    }
  }

  if (p41 < 0)
    return;

  size_t cur = p41 + 2;
  while (cur < len && data[cur] == ' ') cur++;
  if (cur + 2 > len) return;

  uint8_t pid = hexToByte(data[cur], data[cur + 1]);
  cur += 2;

  while (cur < len && data[cur] == ' ') cur++;
  if (cur + 2 > len) return;

  uint8_t A = hexToByte(data[cur], data[cur + 1]);
  cur += 2;

  while (cur < len && data[cur] == ' ') cur++;
  uint8_t B = 0;
  if (cur + 2 <= len)
  {
    B = hexToByte(data[cur], data[cur + 1]);
  }

  switch (pid)
  {
  case 0x0D: // Speed
  {
    Timber.i("Speed: %u km/h\n", A);
    LVGL_EXEC(lv_subject_set_int(&speed, (int)A));
    break;
  }

  case 0x0C: // RPM
  {
    float rpm = ((A << 8) | B) / 4.0f;
    Timber.i("RPM: %.0f\n", rpm);
    LVGL_EXEC(lv_subject_set_int(&engine_rpm, (int)rpm));
    break;
  }

  case 0x0B: // MAP (Intake Manifold Absolute Pressure) -> Turbo Boost
  {
    float map_kpa = (float)A;

    // Auto-calibrate ambient atmospheric pressure at idle/stop
    static float ambient_kpa = 100.0f;
    static bool ambient_calibrated = false;
    int current_rpm = lv_subject_get_int(&engine_rpm);
    int current_speed = lv_subject_get_int(&speed);
    if (!ambient_calibrated && current_speed == 0 && current_rpm > 500 && current_rpm < 1100 && map_kpa >= 90.0f && map_kpa <= 110.0f)
    {
      ambient_kpa = map_kpa;
      ambient_calibrated = true;
      Timber.i("Calibrated ambient atmospheric pressure: %.0f kPa\n", ambient_kpa);
    }

    float boost_bar = (map_kpa - ambient_kpa) / 100.0f;
    if (boost_bar < 0.0f)
      boost_bar = 0.0f;
    Timber.i("MAP: %.0f kPa, Ambient: %.0f kPa, Boost: %.2f bar\n", map_kpa, ambient_kpa, boost_bar);

    int boost_int = (int)(boost_bar * 100.0f); // 0 to 250 (hundredths of bar)
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f bar", boost_bar);

    LVGL_EXEC({
      lv_subject_set_int(&turbo_boost, boost_int);
      lv_subject_copy_string(&turbo_boost_str, buf);
    });
    break;
  }

  case 0x2F: // Fuel Tank Level
  {
    float fuel_pct = (A * 100.0f) / 255.0f;
    int litres = (int)((fuel_pct * 45.0f) / 100.0f + 0.5f); // Hyundai Accent MC 45L tank
    Timber.i("Fuel: %.1f %% (%d L)\n", fuel_pct, litres);
    prefs.putInt("fuel_level", litres);
    LVGL_EXEC(lv_subject_set_int(&fuel_capacity, litres));
    break;
  }

  case 0x05: // Coolant temp
  {
    float temp = A - 40.0f;
    Timber.i("Coolant: %.1f C\n", temp);
    LVGL_EXEC(lv_subject_set_int(&coolant_temp, (int)temp));
    break;
  }
  }
  LVGL_EXEC(lv_subject_set_int(&can_error, 0));
}

void deep_sleep_restart()
{
  /* Here we use deep sleep so we can detect the wakeup source to skip boot logo during startup */
  esp_sleep_enable_timer_wakeup(1000);
  esp_deep_sleep_start();
}

class ClientCallbacks : public NimBLEClientCallbacks
{
  void onConnect(NimBLEClient *pClient) override
  {
    Timber.v("Connected");
  }

  void onDisconnect(NimBLEClient *pClient, int reason) override
  {
    Timber.v("Disconnected");
    LVGL_EXEC(lv_subject_set_int(&con_error, 1));
    obdChar = nullptr;

    if (should_restart)
    {
      /* Restart on disconnect if enabled */
      deep_sleep_restart();
    }

    if (client)
    {
      NimBLEDevice::deleteClient(client);
      client = nullptr;
    }

    if (scan)
    {
      scan->clearResults();
      scan->start(0, false);
    }
  }
} clientCallbacks;

/* ---------- NOTIFY CALLBACK ---------- */
void notifyCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *data, size_t len, bool isNotify)
{

  if (isNotify)
  {
    Timber.v(formatHexString(data, len, false));
    parseObd(data, len);
  }
}

/* ---------- SCAN CALLBACK ---------- */
class ScanCB : public NimBLEScanCallbacks
{
  void onResult(const NimBLEAdvertisedDevice *dev) override
  {
    if (dev->haveServiceUUID() &&
        dev->isAdvertisingService(OBD_SERVICE_UUID))
    {
      Timber.v("OBD Adapter found");
      obdDevice = dev;
      NimBLEDevice::getScan()->stop();
    }
  }
};

/* ---------- CONNECT ---------- */
bool connectObd()
{
  client = NimBLEDevice::createClient();
  client->setClientCallbacks(&clientCallbacks, false);
  if (!client->connect(obdDevice))
  {
    Timber.e("Could not connect to device");
    return false;
  }

  auto service = client->getService(OBD_SERVICE_UUID);
  if (!service)
  {
    Timber.e("OBD_SERVICE_UUID not found");
    return false;
  }

  obdChar = service->getCharacteristic(OBD_CHAR_UUID);
  if (!obdChar)
  {
    Timber.e("OBD_CHAR_UUID not found");
    return false;
  }

  if (obdChar->canNotify())
  {
    Timber.i("Subscribing to notifications");
    obdChar->subscribe(true, notifyCB);
  }

  Timber.v("Connected to OBD");
  LVGL_EXEC(lv_subject_set_int(&con_error, 0));
  return true;
}

/* ---------- WRITE ---------- */
void obdWrite(const uint8_t *cmd, size_t len)
{
  if (obdChar && obdChar->canWrite())
    obdChar->writeValue(cmd, len, false);
}

/* ---------- LVGL DISPLAY & TOUCH DRIVER ---------- */
/*Convert rotation number to lvgl rotation type*/
lv_display_rotation_t get_rotation(uint8_t rotation)
{
  if (rotation > 3)
    return LV_DISPLAY_ROTATION_0;
  return (lv_display_rotation_t)rotation;
}

/* Display flushing */
void my_disp_flush(lv_display_t *display, const lv_area_t *area, unsigned char *data)
{

  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

#ifdef SW_ROTATION
  lv_display_rotation_t rotation = lv_display_get_rotation(display);
  lv_area_t rotated_area;
  if (rotation != LV_DISPLAY_ROTATION_0)
  {
    lv_color_format_t cf = lv_display_get_color_format(display);
    /*Calculate the position of the rotated area*/
    rotated_area = *area;
    lv_display_rotate_area(display, &rotated_area);
    /*Calculate the source stride (bytes in a line) from the width of the area*/
    uint32_t src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
    /*Calculate the stride of the destination (rotated) area too*/
    uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&rotated_area), cf);
    /*Have a buffer to store the rotated area and perform the rotation*/
    static uint8_t rotated_buf[LV_BUFFER_SIZE];
    lv_draw_sw_rotate(data, rotated_buf, w, h, src_stride, dest_stride, rotation, cf);
    /*Use the rotated area and rotated buffer from now on*/
    area = &rotated_area;
    data = rotated_buf;
  }
#endif

  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)data);
  lv_display_flush_ready(display); /* tell lvgl that flushing is done */
}

/* Rounder event callback to align area to 2x2 or 4x2 pixels */
void rounder_event_cb(lv_event_t *e)
{
  lv_area_t *area = lv_event_get_invalidated_area(e);
  uint16_t x1 = area->x1;
  uint16_t x2 = area->x2;

  uint16_t y1 = area->y1;
  uint16_t y2 = area->y2;

#ifdef WAVESHARE_146
  // SPD2010 requires horizontal coordinates aligned to multiples of 4
  area->x1 = (x1 >> 2) << 2;
  area->x2 = ((x2 >> 2) << 2) + 3;
#else
  // round the start of coordinate down to the nearest 2M number
  area->x1 = (x1 >> 1) << 1;
  area->x2 = ((x2 >> 1) << 1) + 1;
#endif
  area->y1 = (y1 >> 1) << 1;
  area->y2 = ((y2 >> 1) << 1) + 1;
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
  else
  {
    data->state = LV_INDEV_STATE_PRESSED;
    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;
  }
}

/*Tick function*/
static uint32_t my_tick(void)
{
  return millis();
}

// void on_rotation_change(lv_observer_t *observer, lv_subject_t *subject)
// {
//   int32_t rotation = lv_subject_get_int(subject);

// #ifdef SW_ROTATION
//   lv_display_set_rotation(lv_display_get_default(), get_rotation(rotation));
// #else
//   tft.setRotation(rotation);
//   // screen rotation has changed, invalidate to redraw
//   lv_obj_invalidate(lv_screen_active());
// #endif

// prefs.putInt("rotation", rotation);

// }

void on_brightness_change(lv_observer_t *observer, lv_subject_t *subject)
{
  int32_t brightness = lv_subject_get_int(subject);
  tft.setBrightness((uint8_t)brightness);
  prefs.putInt("brightness", brightness);
}

void on_hud_change(lv_observer_t *observer, lv_subject_t *subject)
{
  int32_t hud = lv_subject_get_int(subject);
  prefs.putInt("hud", hud);
  tft.setFlipMode(hud);
  lv_obj_invalidate(lv_screen_active());
}

void on_restart_change(lv_observer_t *observer, lv_subject_t *subject)
{
  int32_t restart = lv_subject_get_int(subject);
  prefs.putInt("restart", restart);
  should_restart = restart;
}

void on_restart_cb(lv_event_t *e)
{
  deep_sleep_restart();
}

#if LV_USE_LOG != 0
/**
 * Function to print lvgl logs when enabled
 * @param level the log level
 * @param buf the log buffer to print
 */
void lv_log_print(lv_log_level_t level, const char *buf)
{
  USBSerial.write(buf);
}
#endif

void logCallback(Level level, unsigned long time, String message)
{
  USBSerial.print(message);
}

void setup()
{

  USBSerial.begin(115200);

  Timber.setColors(true);
  Timber.setLogCallback(logCallback);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause(); // get wake up reason

  bool show_boot = true;

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_TIMER:
    Timber.i("Wakeup caused by timer");
    show_boot = false;
    break;
  case ESP_SLEEP_WAKEUP_EXT0:
    Timber.i("Wakeup caused by RTC alarm");
    show_boot = true;
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Timber.i("Wakeup caused by Button press");
    show_boot = false;
    break;
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    Timber.i("Wakeup was not caused by deep sleep");
    break;
  default:
    Timber.i("Wakeup was not caused by deep sleep: %d", wakeup_reason);
    break;
  }

#ifdef ELECROW_C3
  elecrow_c3_init();
#endif

  prefs.begin("my-app");

  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.fillScreen(0x0000);

  lv_init();

  lv_tick_set_cb(my_tick);

#if LV_USE_LOG != 0
  lv_log_register_print_cb(lv_log_print);
#endif

  static lv_display_t *lv_display = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_color_format(lv_display, LV_COLOR_FORMAT_RGB565_SWAPPED);
  lv_display_set_flush_cb(lv_display, my_disp_flush);
  lv_display_set_buffers(lv_display, lv_buffer[0], lv_buffer[1], LV_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_add_event_cb(lv_display, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);

  static lv_indev_t *lv_input = lv_indev_create();
  lv_indev_set_type(lv_input, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(lv_input, my_touchpad_read);

  /* Create a mutex for LVGL */
  /* This mutex is used to protect the LVGL library from concurrent access */
  lvgl_mutex = xSemaphoreCreateRecursiveMutex();

  int rotation = prefs.getInt("rotation", 0);
  int brightness = prefs.getInt("brightness", 128);
  int hud = prefs.getInt("hud", 0);
  int restart = prefs.getInt("restart", 0);

  should_restart = restart;

  tft.setBrightness((uint8_t)brightness);
  tft.setFlipMode(hud);
  // #ifdef SW_ROTATION
  //   lv_display_set_rotation(lv_display, get_rotation(rotation));
  // #else
  //   tft.setRotation(rotation);
  // #endif

  hud_ui_init("");

  int fuel_level = prefs.getInt("fuel_level", 45); // Default to 45L full tank for Hyundai Accent MC
  LVGL_EXEC(lv_subject_set_int(&fuel_capacity, fuel_level));

  // lv_subject_set_int(&settings_rotation, rotation);
  lv_subject_set_int(&settings_brightness, brightness);
  lv_subject_set_int(&settings_hud, hud);
  lv_subject_set_int(&settings_restart, restart);

  // lv_subject_add_observer(&settings_rotation, on_rotation_change, NULL);
  lv_subject_add_observer(&settings_brightness, on_brightness_change, NULL);
  lv_subject_add_observer(&settings_hud, on_hud_change, NULL);
  lv_subject_add_observer(&settings_restart, on_restart_change, NULL);

  boot_screen = boot_create();
  dashboard_screen = dashboard_create();
  settings_screen = settings_create();

  lv_obj_t *settings_back = lv_obj_find_by_name(settings_screen, "settings_back");
  if (settings_back)
  {
    lv_obj_add_screen_load_event(settings_back, LV_EVENT_CLICKED, dashboard_screen,
                                 LV_SCREEN_LOAD_ANIM_FADE_IN, 500, 0);
  }

  lv_obj_t *settings_restart = lv_obj_find_by_name(settings_screen, "settings_restart");
  if (settings_restart)
  {
    lv_obj_add_event_cb(settings_restart, on_restart_cb, LV_EVENT_CLICKED, NULL);
  }

  lv_obj_add_screen_load_event(boot_screen, LV_EVENT_SCREEN_LOADED, dashboard_screen,
                               LV_SCREEN_LOAD_ANIM_FADE_IN, 500, 3000);
  lv_obj_add_screen_load_event(dashboard_screen, LV_EVENT_LONG_PRESSED, settings_screen,
                               LV_SCREEN_LOAD_ANIM_FADE_IN, 500, 0);
  // Settings back button navigates back to dashboard; avoid long-press loop on settings_screen

  if (show_boot)
  {
    lv_screen_load(boot_screen);
  }
  else
  {
    lv_screen_load(dashboard_screen);
  }

  /* BLE Setup */
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  scan = NimBLEDevice::getScan();
  scan->setScanCallbacks(new ScanCB());
  scan->setActiveScan(true);
  scan->start(0);
}

void loop()
{
  LVGL_EXEC(lv_timer_handler()); // Update the UI
  delay(5);

  if (obdDevice && !client)
  {
    if (connectObd())
    {
      delay(500);
      obdWrite(CMD_ATZ, sizeof(CMD_ATZ));
      delay(800);
      obdWrite(CMD_ATE0, sizeof(CMD_ATE0));
      delay(300);
      obdWrite(CMD_ATSP6, sizeof(CMD_ATSP6));
      delay(300);
    }
  }

  uint32_t now = millis();

  if (!obdChar)
    return;

  // Poll OBD sequentially: one command every 75ms
  // Cycle: RPM -> MAP (Boost) -> SPEED -> RPM -> MAP (Boost) -> SLOW (FUEL / TEMP)
  static uint32_t lastQueryTime = 0;
  static uint8_t queryStep = 0;
  static bool slowToggle = false;
  const uint32_t OBD_POLL_INTERVAL = 75; // 75 ms between commands

  if (now - lastQueryTime >= OBD_POLL_INTERVAL)
  {
    lastQueryTime = now;

    switch (queryStep)
    {
    case 0:
      obdWrite(CMD_RPM, sizeof(CMD_RPM));
      break;
    case 1:
      obdWrite(CMD_MAP, sizeof(CMD_MAP));
      break;
    case 2:
      obdWrite(CMD_SPEED, sizeof(CMD_SPEED));
      break;
    case 3:
      obdWrite(CMD_RPM, sizeof(CMD_RPM));
      break;
    case 4:
      obdWrite(CMD_MAP, sizeof(CMD_MAP));
      break;
    case 5:
      if (slowToggle)
      {
        obdWrite(CMD_TEMP, sizeof(CMD_TEMP));
      }
      else
      {
        obdWrite(CMD_FUEL, sizeof(CMD_FUEL));
      }
      slowToggle = !slowToggle;
      break;
    }

    queryStep = (queryStep + 1) % 6;
  }
}
