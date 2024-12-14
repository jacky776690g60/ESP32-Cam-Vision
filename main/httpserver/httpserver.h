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


// static const char *TAG = "[HTTP_SERVER]";
extern const char *HTTPSEVER_TAG;


/** Init a http server. */
esp_err_t httpserver_init(void);
/** Helper function to map frame size string to FRAMESIZE enum */
framesize_t get_framesize(const char *size_str);

/** Handler to get root index.html */
esp_err_t root_get_handler(httpd_req_t *req);
const extern httpd_uri_t root_uri;

/** Handler to get /src/style.css */
esp_err_t style_css_handler(httpd_req_t *req);
const extern httpd_uri_t style_css_uri;

/** Handler to get /src/entryDeferred.js */
esp_err_t entry_deferred_js_handler(httpd_req_t *req);
const extern httpd_uri_t entry_deferred_js_uri;

/**
 * HTTP GET handler to list all files in /spiffs/src
 * @note Purely for testing
 */
esp_err_t list_files_handler(httpd_req_t *req);
const extern httpd_uri_t list_files_uri;

