/** ================================================================
| camera.h  --  main/camera/camera.h
|
| Created by Jack on 11/18, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "string.h"


// =================== Camera =========================
/** Shared Instance to ensure synchronisation */
extern SemaphoreHandle_t camMutex;
/** The initial global camera_config variable */
extern camera_config_t   camConfig;
/** Init a camera, or reinit if one exists already */
esp_err_t camera_init(void);


// ================ Frame Buffer ======================

/** Buffer size for the captured frames */
#define FRAME_BUFFER_SIZE 50

typedef struct {
    uint8_t *buf; // to store raw bytes. Can be anything
    size_t   len;
} frame_t;

typedef struct {
    frame_t           frames[FRAME_BUFFER_SIZE];
    int               head;
    int               tail;
    SemaphoreHandle_t mutex;
} frame_buffer_t;

/** Custom frame buffer for the program */
extern frame_buffer_t    frameBuffer;
/** Init a frame buffer given size. */
void init_frame_buffer();
void capture_frames_task(void *pvParameters);