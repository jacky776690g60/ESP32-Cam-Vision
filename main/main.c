/** ================================================================
| main.c  --  main/main.c
|
| Created by Jack on 11/18, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#include <stdio.h>
#include <string.h>
/** Contains many variables like the ones in .env (fullstack project) */
#include "sdkconfig.h"

#include "esp_log.h"

#include "utility/utility.h"
#include "camera/camera.h"
#include "wifi/wifi.h"
#include "httpserver/httpserver.h"

static const char *TAG = "ESP32_CAM_SIMPLE";


void app_main(void)
{
    /**
     * Initiate Non-Volatile Storage (NVS) flash, which is used to store key-value 
     * pairs persistently across device resets or power cycles.
     */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Starting ESP32-CAM Web Server Project...");
    
    ESP_ERROR_CHECK(wifi_init());
    ESP_ERROR_CHECK(camera_init());

    log_heap_info();
    httpserver_init();
}