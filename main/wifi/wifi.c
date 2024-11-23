/** ================================================================
| wifi.c  --  main/wifi/wifi.c
|
| Created by Jack on 11/19, 2024
| Copyright © 2024 jacktogon. All rights reserved.
================================================================= */
#include "wifi.h"


static const char *TAG = "Wifi_Module";


// Define the event handler function
static void event_handler(
    void*             arg, 
    esp_event_base_t  event_base,
    int32_t           event_id, 
    void*             event_data
) {
    // Handle Wi-Fi and IP events here
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Got IP");
    }
}

esp_err_t wifi_init(void)
{
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create the default Wi-Fi station network interface
    esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi with default configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers for Wi-Fi and IP events
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &event_handler,
            NULL,
            NULL
        )
    );
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &event_handler,
            NULL,
            NULL
        )
    );

    // Configure Wi-Fi parameters
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,

            /* Setting a password implies station will connect to all security modes 
             * including WEP/WPA. However, these modes are deprecated and not advisable 
             * to be used. In case Access Point doesn't support WPA2, these mode 
             * can be enabled by commenting the below line */
            .threshold = {  // Properly designate 'threshold'
                .authmode = WIFI_AUTH_WPA2_PSK
            },


            .pmf_cfg = {  // Properly designate 'pmf_cfg'
                .capable = true,
                .required = false
            },
        },
    };

    // Set Wi-Fi mode to Station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // Set the Wi-Fi configuration
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    // Start the Wi-Fi driver
    ESP_ERROR_CHECK(esp_wifi_start());


    ESP_LOGI(TAG, "Wi-Fi initialization complete.");
    return ESP_OK;
}
