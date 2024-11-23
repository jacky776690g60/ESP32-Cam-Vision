/** ================================================================
| utility.cpp  --  main/utility/utility.cpp
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#include "utility.h"

static const char *TAG = "UTILITY";

extern "C" void log_heap_info() {
    ESP_LOGI(TAG, "Heap Info:");
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
}


// static const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// char* base64_encode(const unsigned char* data, size_t input_length, size_t* output_length)
// {
//     *output_length = 4 * ((input_length + 2) / 3);

//     char *encoded_data = static_cast<char *>(malloc(*output_length + 1));
//     if (encoded_data == NULL) return NULL;

//     for (size_t i = 0, j = 0; i < input_length;) {

//         uint32_t octet_a = i < input_length ? data[i++] : 0;
//         uint32_t octet_b = i < input_length ? data[i++] : 0;
//         uint32_t octet_c = i < input_length ? data[i++] : 0;

//         uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

//         encoded_data[j++] = encoding_table[(triple >> 18) & 0x3F];
//         encoded_data[j++] = encoding_table[(triple >> 12) & 0x3F];
//         encoded_data[j++] = (i > (input_length + 1)) ? '=' : encoding_table[(triple >> 6) & 0x3F];
//         encoded_data[j++] = (i > input_length) ? '=' : encoding_table[triple & 0x3F];
//     }

//     encoded_data[*output_length] = '\0';
//     return encoded_data;
// }