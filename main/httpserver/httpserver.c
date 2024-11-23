/** ================================================================
| httpserver.c  --  main/httpserver/httpserver.c
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#include "httpserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

#include "cJSON.h"
#include "mbedtls/base64.h"

#include "esp_log.h"
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"

#include "camera/camera.h"

static const char *TAG = "HTTP_SERVER";

/** HTTP GET handler for the root path */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    // ~~~~~~~~ Open the HTML file from SPIFFS ~~~~~~~~
    FILE* f = fopen("/spiffs/index.html", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open index.html");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    // ~~~~~~~~ Get the file ~~~~~~~~
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    char* file_buffer = (char*) malloc(file_size + 1); // Allocate memory to contain the whole file
    if (file_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for index.html");
        fclose(f);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Read the file into the buffer
    size_t read_size = fread(file_buffer, 1, file_size, f);
    fclose(f);

    if (read_size != file_size) {
        ESP_LOGE(TAG, "Failed to read index.html");
        free(file_buffer);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    file_buffer[file_size] = '\0'; // Null-terminate the string

    httpd_resp_set_type(req, "text/html");
    esp_err_t ret = httpd_resp_send(req, file_buffer, file_size);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to sent index.html to client.");
    }

    free(file_buffer);
    return ret;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};




/**
 * HTTP GET handler specifically for /src/style.css
 */
static esp_err_t style_css_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "style_css_handler invoked for /src/style.css");

    const char* filepath = "/spiffs/src/style.css";

    // Open the file in binary mode
    FILE* f = fopen(filepath, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File Not Found");
        return ESP_FAIL;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    // Allocate buffer
    char* file_buffer = (char*) malloc(file_size);
    if (file_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for %s", filepath);
        fclose(f);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Read file into buffer
    size_t read_size = fread(file_buffer, 1, file_size, f);
    fclose(f);

    if (read_size != file_size) {
        ESP_LOGE(TAG, "Failed to read file: %s", filepath);
        free(file_buffer);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Set headers
    httpd_resp_set_type(req, "text/css");
    httpd_resp_set_hdr(req, "Cache-Control", "max-age=86400"); // Optional caching

    // Send the file content
    esp_err_t ret = httpd_resp_send(req, file_buffer, file_size);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send file: %s", filepath);
    } else {
        ESP_LOGI(TAG, "File sent successfully: %s", filepath);
    }

    free(file_buffer);
    return ret;
}

static const httpd_uri_t style_css_uri = {
    .uri       = "/src/style.css",
    .method    = HTTP_GET,
    .handler   = style_css_handler,
    .user_ctx  = NULL
};




/**
 * HTTP GET handler specifically for /src/entryDeferred.js
 */
static esp_err_t entry_deferred_js_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "entry_deferred_js_handler invoked for /src/entryDeferred.js");

    const char* filepath = "/spiffs/src/entryDeferred.js";

    // Open the file in binary mode
    FILE* f = fopen(filepath, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File Not Found");
        return ESP_FAIL;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    // Allocate buffer
    char* file_buffer = (char*) malloc(file_size);
    if (file_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for %s", filepath);
        fclose(f);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Read file into buffer
    size_t read_size = fread(file_buffer, 1, file_size, f);
    fclose(f);

    if (read_size != file_size) {
        ESP_LOGE(TAG, "Failed to read file: %s", filepath);
        free(file_buffer);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Set headers
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_set_hdr(req, "Cache-Control", "max-age=86400"); // Optional caching

    // Send the file content
    esp_err_t ret = httpd_resp_send(req, file_buffer, file_size);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send file: %s", filepath);
    } else {
        ESP_LOGI(TAG, "File sent successfully: %s", filepath);
    }

    free(file_buffer);
    return ret;
}

static const httpd_uri_t entry_deferred_js_uri = {
    .uri       = "/src/entryDeferred.js",
    .method    = HTTP_GET,
    .handler   = entry_deferred_js_handler,
    .user_ctx  = NULL
};












/**
 * HTTP GET handler to list all files in /spiffs/src
 * @note Purely for testing
 */
static esp_err_t list_files_handler(httpd_req_t *req)
{
    DIR* dir = opendir("/spiffs/src");
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open /spiffs/src/");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open /spiffs/src/");
        return ESP_FAIL;
    }

    struct dirent* entry;
    char response[2048] = {0};
    strcat(response, "[\n");

    while ((entry = readdir(dir)) != NULL) {
        strcat(response, "  \"");
        strcat(response, entry->d_name);
        strcat(response, "\"\n");
    }

    strcat(response, "]");

    closedir(dir);

    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, response, strlen(response));
}

static const httpd_uri_t list_files_uri = {
    .uri       = "/list_files",
    .method    = HTTP_GET,
    .handler   = list_files_handler,
    .user_ctx  = NULL
};


















/** Helper function to map frame size string to FRAMESIZE enum */
static framesize_t get_framesize(const char *size_str) {
    if (strcmp(size_str, "QQVGA") == 0) return FRAMESIZE_QQVGA;
    if (strcmp(size_str, "QCIF")  == 0) return FRAMESIZE_QCIF;
    if (strcmp(size_str, "QVGA")  == 0) return FRAMESIZE_QVGA;
    if (strcmp(size_str, "CIF")   == 0) return FRAMESIZE_CIF;
    if (strcmp(size_str, "HVGA")  == 0) return FRAMESIZE_HVGA;
    if (strcmp(size_str, "VGA")   == 0) return FRAMESIZE_VGA;
    if (strcmp(size_str, "SVGA")  == 0) return FRAMESIZE_SVGA;
    if (strcmp(size_str, "XGA")   == 0) return FRAMESIZE_XGA;
    if (strcmp(size_str, "SXGA")  == 0) return FRAMESIZE_SXGA;
    if (strcmp(size_str, "UXGA")  == 0) return FRAMESIZE_UXGA;
    return FRAMESIZE_QVGA;
}

/** HTTP POST handler for /settings */
static esp_err_t settings_post_handler(httpd_req_t *req)
{
    char content[200];
    int total_len = req->content_len;
    int received  = 0;
    int ret;

    if (total_len >= sizeof(content)) {
        ESP_LOGE(TAG, "Settings POST request is too long!");
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    while (received < total_len) {
        ret = httpd_req_recv(req, content + received, total_len - received);
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) httpd_resp_send_408(req);
            return ESP_FAIL;
        }
        received += ret;
    }
    /** @note Null-terminate the received data to form proper C-String */
    content[received] = '\0';
    ESP_LOGI(TAG, "Received settings: %s", content);


    cJSON *json = cJSON_Parse(content);
    if (!json) {
        ESP_LOGE(TAG, "Invalid JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_FAIL;
    }
    
    sensor_t *s = esp_camera_sensor_get();
    
    cJSON *frame_size = cJSON_GetObjectItemCaseSensitive(json, "frame_size");
    if (cJSON_IsString(frame_size) && frame_size->valuestring != NULL) {
        framesize_t new_frame_size = get_framesize(frame_size->valuestring);
        s->set_framesize(s, new_frame_size);
    }

    // Extract jpeg_quality
    cJSON *jpeg_quality = cJSON_GetObjectItemCaseSensitive(json, "jpeg_quality");
    if (cJSON_IsNumber(jpeg_quality)) {
        s->set_quality(s, jpeg_quality->valueint);
    }

    cJSON *brightness = cJSON_GetObjectItemCaseSensitive(json, "brightness");
    if (cJSON_IsNumber(brightness)) {
        s->set_brightness(s, brightness->valueint);
    }

    cJSON *ae_level = cJSON_GetObjectItemCaseSensitive(json, "ae_level");
    if (cJSON_IsNumber(ae_level)) {
        s->set_ae_level(s, ae_level->valueint);       // Auto Exposure -2 to 2
    }

    cJSON *aec_value = cJSON_GetObjectItemCaseSensitive(json, "aec_value");
    if (cJSON_IsNumber(aec_value)) {
        s->set_aec_value(s, aec_value->valueint);    // Auto Exposure Control: shutter speed 0 to 1200
    }

    cJSON *special_effect = cJSON_GetObjectItemCaseSensitive(json, "special_effect");
    if (cJSON_IsNumber(special_effect)) {
        s->set_special_effect(s, special_effect->valueint); // special effect 0 ~ 6
    }
    
    cJSON *sharpness = cJSON_GetObjectItemCaseSensitive(json, "sharpness");
    if (cJSON_IsNumber(sharpness)) {
        s->set_sharpness(s, sharpness->valueint);
    }

    cJSON *saturation = cJSON_GetObjectItemCaseSensitive(json, "saturation");
    if (cJSON_IsNumber(saturation)) {
        s->set_saturation(s, saturation->valueint);
    }

    // Respond with success
    const char* resp_str = "{\"status\":\"success\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, strlen(resp_str));

    cJSON_Delete(json);
    return ESP_OK;
}

static const httpd_uri_t settings = {
    .uri       = "/settings",
    .method    = HTTP_POST,
    .handler   = settings_post_handler,
    .user_ctx  = NULL
};







static esp_err_t batch_stream_get_handler(httpd_req_t *req)
{
    cJSON *json_array = cJSON_CreateArray();
    if (!json_array) {
        ESP_LOGE(TAG, "Failed to create JSON array");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server failed to create JSON array.");
        return ESP_FAIL;
    }

    xSemaphoreTake(frame_buffer.mutex, portMAX_DELAY);
    int idx = frame_buffer.tail;
    int count = (frame_buffer.head - frame_buffer.tail + FRAME_BUFFER_SIZE) % FRAME_BUFFER_SIZE;
    for(int i = 0; i < count && i < 10; i++) { // Limit to 10 frames
        if(frame_buffer.frames[idx].buf) {
            // Base64 encode
            size_t b64_len = 4 * ((frame_buffer.frames[idx].len + 2) / 3);
            unsigned char *b64_buf = malloc(b64_len + 1);
            if(b64_buf) {
                mbedtls_base64_encode(b64_buf, b64_len + 1, &b64_len, frame_buffer.frames[idx].buf, frame_buffer.frames[idx].len);
                b64_buf[b64_len] = '\0';
                cJSON_AddItemToArray(json_array, cJSON_CreateString((char *)b64_buf));
                free(b64_buf);
            }
        }
        idx = (idx + 1) % FRAME_BUFFER_SIZE;
    }

    // **Update the tail to the new position after serving**
    frame_buffer.tail = idx;
    xSemaphoreGive(frame_buffer.mutex);

    // Convert JSON array to string
    char *response = cJSON_PrintUnformatted(json_array);
    cJSON_Delete(json_array);

    // Send response
    httpd_resp_set_type(req, "application/json");
    esp_err_t res = httpd_resp_send(req, response, strlen(response));
    free(response);
    return res;
}

static const httpd_uri_t batch_stream_uri = {
    .uri       = "/batch_stream",
    .method    = HTTP_GET,
    .handler   = batch_stream_get_handler,
    .user_ctx  = NULL
};




esp_err_t httpserver_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path              = "/spiffs",
        .partition_label        = NULL,
        .max_files              = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }


    init_frame_buffer();
    xTaskCreate(capture_frames_task, "CaptureFrames", 4096, NULL, 5, NULL);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    /** Uncomment and set if you want to change the default port */
    // config.server_port = 8080;
    config.keep_alive_idle     = 5; // seconds
    config.keep_alive_interval = 3; // seconds
    config.keep_alive_count    = 3;
    config.max_uri_handlers    = 10;
    config.max_open_sockets    = 7;      

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_register_uri_handler(server, &list_files_uri); // debug
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &settings);
        httpd_register_uri_handler(server, &batch_stream_uri);

        httpd_register_uri_handler(server, &style_css_uri);           // Handler for /src/style.css
        httpd_register_uri_handler(server, &entry_deferred_js_uri);   // Handler for /src/entryDeferred.js

        ESP_LOGI(TAG, "HTTP server started successfully.");
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Failed to start HTTP server!");
    return ESP_FAIL;
}

