/** ================================================================
| camera.c  --  main/camera/camera.c
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#include "camera.h"

static const char *TAG = "Camera_Module";

// =================== Camera =========================
static bool camera_initialized = false;  // Flag to track initialization state
SemaphoreHandle_t camera_mutex = NULL;
camera_config_t   camera_config = {
    // ◼︎ Power and Control Pins
    .pin_pwdn       = 32,   // GPIO32 connected to PWDN (Power Down) pin of the camera
    .pin_reset      = -1,   // GPIO not connected (-1 indicates unused RESET pin)
    
    // ◼︎ Clock and Communication Pins
    .pin_xclk       = 0,    // GPIO0 connected to XCLK (External Clock) pin
    .pin_sscb_sda   = 26,   // GPIO26 connected to SCCB SDA (Serial Data) for camera configuration
    .pin_sscb_scl   = 27,   // GPIO27 connected to SCCB SCL (Serial Clock) for camera configuration
    
    // ◼︎ Data Interface Pins (D7 to D0)
    .pin_d7         = 35,   // GPIO35 connected to D7 (Data Bit 7)
    .pin_d6         = 34,   // GPIO34 connected to D6 (Data Bit 6)
    .pin_d5         = 39,   // GPIO39 connected to D5 (Data Bit 5)
    .pin_d4         = 36,   // GPIO36 connected to D4 (Data Bit 4)
    .pin_d3         = 21,   // GPIO21 connected to D3 (Data Bit 3)
    .pin_d2         = 19,   // GPIO19 connected to D2 (Data Bit 2)
    .pin_d1         = 18,   // GPIO18 connected to D1 (Data Bit 1)
    .pin_d0         = 5,    // GPIO5  connected to D0 (Data Bit 0)
    
    // ◼︎ Synchronization and Pixel Clock Pins
    .pin_vsync      = 25,   // GPIO25 connected to VSYNC (Vertical Sync) pin
    .pin_href       = 23,   // GPIO23 connected to HREF (Horizontal Reference) pin
    .pin_pclk       = 22,   // GPIO22 connected to PCLK (Pixel Clock) pin

    // ◼︎ Camera Operating Parameters
    .xclk_freq_hz   = 20000000,          // External clock frequency set to 20 MHz
    .ledc_timer     = LEDC_TIMER_0,      // LEDC timer 0 used for generating XCLK
    .ledc_channel   = LEDC_CHANNEL_0,    // LEDC channel 0 used for generating XCLK
    
    // ◼︎ Image Format and Quality Settings
    .pixel_format   = PIXFORMAT_JPEG,        // Use JPEG to save memory
     //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    .frame_size     = FRAMESIZE_QVGA,        // Reduced frame size
    .jpeg_quality   = 12,                    // Lower quality
    
    // ◼︎ Frame Buffer Settings
    .fb_count       = 4,                     
    .grab_mode      = CAMERA_GRAB_WHEN_EMPTY
};


esp_err_t camera_init(void)
{
    if (camera_mutex == NULL) {
        camera_mutex = xSemaphoreCreateMutex();
        if (camera_mutex == NULL) {
            ESP_LOGE(TAG, "Failed to create camera mutex");
            return ESP_FAIL;
        }
    }

    if (xSemaphoreTake(camera_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to obtain camera mutex for initialization");
        return ESP_FAIL;
    }

    // ~~~~~~~~ Deinitialize if already initialized ~~~~~~~~
    if (camera_initialized) {
        ESP_LOGI(TAG, "Deinitializing camera before reinitialization.");
        esp_err_t ret = esp_camera_deinit();
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "Camera deinit failed: 0x%x", ret);
            xSemaphoreGive(camera_mutex);
            return ret;
        }
        camera_initialized = false;
    }
    // ~~~~~~~~ Initialize the camera ~~~~~~~~
    ESP_LOGI(TAG, "Initializing camera with frame_size=%d, jpeg_quality=%d",
             camera_config.frame_size, camera_config.jpeg_quality);
    esp_err_t ret = esp_camera_init(&camera_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed with error 0x%x", ret);
        xSemaphoreGive(camera_mutex);
        return ret;
    }
    
    /** 
     * Settings to make it more natural at the beginning.
     * @note for ESP32 cam module specifically
     */
    sensor_t *s   = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_brightness(s, 1); 
    // s->set_saturation(s, -1);
    

    camera_initialized = true;
    ESP_LOGI(TAG, "Camera initialization complete.");

    xSemaphoreGive(camera_mutex);
    return ESP_OK;
}




// ================ Frame Buffer ======================
frame_buffer_t frame_buffer;

void init_frame_buffer() {
    for(int i = 0; i < FRAME_BUFFER_SIZE; i++) {
        frame_buffer.frames[i].buf = NULL;
        frame_buffer.frames[i].len = 0;
    }
    frame_buffer.head = 0;
    frame_buffer.tail = 0;
    frame_buffer.mutex = xSemaphoreCreateMutex();
}

void capture_frames_task(void *pvParameters) {
    while (1) {
        camera_fb_t *fb = NULL;
        if (xSemaphoreTake(camera_mutex, portMAX_DELAY) == pdTRUE) {
            fb = esp_camera_fb_get();
            xSemaphoreGive(camera_mutex);
        }
        if (fb) {
            xSemaphoreTake(frame_buffer.mutex, portMAX_DELAY);
            // Free old frame if exists
            if (frame_buffer.frames[frame_buffer.head].buf) {
                free(frame_buffer.frames[frame_buffer.head].buf);
            }
            // Allocate and copy frame
            frame_buffer.frames[frame_buffer.head].buf = malloc(fb->len);
            if (frame_buffer.frames[frame_buffer.head].buf) {
                memcpy(frame_buffer.frames[frame_buffer.head].buf, fb->buf, fb->len);
                frame_buffer.frames[frame_buffer.head].len = fb->len;
                frame_buffer.head = (frame_buffer.head + 1) % FRAME_BUFFER_SIZE;
                // Overwrite oldest
                if (frame_buffer.head == frame_buffer.tail) {
                    frame_buffer.tail = (frame_buffer.tail + 1) % FRAME_BUFFER_SIZE;
                }
            }
            xSemaphoreGive(frame_buffer.mutex);
            esp_camera_fb_return(fb);
        }
        /**
         * @note Adjust to desired FPS (e.g., 100ms for 10 FPS)
         */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}