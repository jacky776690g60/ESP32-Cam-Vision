/** ================================================================
| wifi.h  --  main/wifi/wifi.h
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#pragma once

#include "esp_err.h"

#include "esp_wifi.h"          // Ensure ESP-IDF Wi-Fi functions are included
#include "esp_log.h"           // For ESP_LOGI
#include "esp_event.h"         // For event handling
#include "nvs_flash.h"         // For NVS initialization

esp_err_t wifi_init(void);