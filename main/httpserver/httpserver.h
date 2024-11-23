/** ================================================================
| httpserver.h  --  main/httpserver/httpserver.h
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#pragma once

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "string.h"
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


/** Init a http server. */
esp_err_t httpserver_init(void);