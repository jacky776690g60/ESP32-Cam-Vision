/** ================================================================
| utility.h  --  main/utility/utility.h
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#pragma once

/** Contains many variables like the ones in .env (fullstack project) */
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Log info about the heap. */
void log_heap_info();

// char* base64_encode(const unsigned char* data, size_t input_length, size_t* output_length);



#ifdef __cplusplus
}
#endif