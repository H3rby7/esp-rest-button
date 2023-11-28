#include "soft_ap.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "stdint.h"
#include "stdio.h"
#include "nvs_flash.h"
#include <string.h>

static const char *SOFT_AP_TAG = "wifi station";

#define EXAMPLE_ESP_MAXIMUM_RETRY 5

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

//Defines WLAN
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

//other stuff

static int s_retry_num = 0;
esp_ip4_addr_t self_ip = {};

#define EXAMPLE_ESP_WIFI_AP_SSID "ESP32-REST-BTN"
#define EXAMPLE_ESP_WIFI_AP_PASS ""
#define EXAMPLE_ESP_WIFI_AP_CHANNEL 1
#define EXAMPLE_MAX_STA_CONN 5

/**
  * @brief  Get default AP IP-Adress
  * @retval uint32_t the address
  */
uint32_t get_default_ap_adress() 
{
    const char* ipAddress = "192.168.4.1";

    uint32_t ipValue = 0;
    sscanf(ipAddress, "%hhu.%hhu.%hhu.%hhu", 
           (unsigned char*)&((uint8_t*)&ipValue)[0], 
           (unsigned char*)&((uint8_t*)&ipValue)[1], 
           (unsigned char*)&((uint8_t*)&ipValue)[2], 
           (unsigned char*)&((uint8_t*)&ipValue)[3]);

    return ipValue;
}

/**
  * @brief  Event Handler for WIFI events 
  * @param  void* event_handler_arg,
  * @param  esp_event_base_t event_base,
  * @param  int32_t event_id,
  * @param  void* event_data
  * @retval None
  * @see esp_event_handler_t.c
  */
static void wifi_connect_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    esp_err_t err;

    ESP_LOGI(SOFT_AP_TAG, "Received an event of type '%ld' with base %s", event_id, event_base);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGD(SOFT_AP_TAG, "Connecting to the AP");
        err = esp_wifi_connect();
        if (err != ESP_OK) {
            ESP_LOGW(SOFT_AP_TAG, "Connecting to Wifi failed due to static reasons, informing wifi_event_group of FAIL");
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGD(SOFT_AP_TAG, "Disconnected from the AP");
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            s_retry_num++;
            ESP_LOGI(SOFT_AP_TAG, "Reconnecting to the AP, attempt #%d", s_retry_num);
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGW(SOFT_AP_TAG, "Connecting to Wifi failed due to static reasons, informing wifi_event_group of FAIL");
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
        }
        else
        {
            ESP_LOGW(SOFT_AP_TAG, "Maximum Retries (%d) reached, informing wifi_event_group of FAIL", s_retry_num);
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(SOFT_AP_TAG, "(Connected) Received an IP:" IPSTR, IP2STR(&event->ip_info.ip));
        self_ip = event->ip_info.ip;
        s_retry_num = 0;
        ESP_LOGD(SOFT_AP_TAG, "Informing wifi_event_group of CONNECTED");
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(SOFT_AP_TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(SOFT_AP_TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

/**
  * @brief  Open an access point with fallback args
  * @retval None
  */
void open_ap(void)
{
    ESP_LOGD(SOFT_AP_TAG, "Creating Config with WPA2 and default 'ssid' and 'pwd'");
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_AP_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_AP_SSID),
            .channel = EXAMPLE_ESP_WIFI_AP_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_AP_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_AP_PASS) == 0) {
        ESP_LOGW(SOFT_AP_TAG, "EXAMPLE_ESP_WIFI_AP_PASS not set, using authmode=WIFI_AUTH_OPEN");
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_LOGI(SOFT_AP_TAG, "Setting WIFI Mode to 'AP'");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    ESP_LOGI(SOFT_AP_TAG, "Falling back to default WIFI config");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

    ESP_LOGI(SOFT_AP_TAG, "Starting WIFI in 'AP' Mode");
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(SOFT_AP_TAG, "ESP_WIFI_MODE_SOFTAP finished. Connect to AP via SSID:%s password:%s channel:%d",
            EXAMPLE_ESP_WIFI_AP_SSID, EXAMPLE_ESP_WIFI_AP_PASS, EXAMPLE_ESP_WIFI_AP_CHANNEL);

    self_ip.addr = get_default_ap_adress();
}

/**
  * @brief  Connect to an existing WIFI using ssid and password
  * @param wifi_sta_config_t* wifi_config of the network
  * @retval None
  */
void attempt_connect_to_existing_wifi(wifi_sta_config_t* wifi_config)
{
    ESP_LOGI(SOFT_AP_TAG, "Attempting to connect to existing WIFI");

    ESP_LOGI(SOFT_AP_TAG, "Setting WIFI Mode to 'STA'");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config->threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config_t conf = {
        .sta = *wifi_config,
    };

    ESP_LOGI(SOFT_AP_TAG, "Applying Config to WIFI STA interface");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &conf));

    ESP_LOGI(SOFT_AP_TAG, "Starting WIFI in 'STA' Mode");
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(SOFT_AP_TAG, "Started WIFI in 'STA' Mode");
}

/**
  * @brief  Attempt onnect to an existing WIFI or create own on failure
  * @param char* ssid of the existing network
  * @param pwd* password of the existing network
  * @retval esp_ip4_addr_t IPV4 adress for convenience
  */
esp_ip4_addr_t connect_or_ap(wifi_sta_config_t* wifi_config)
{
    ESP_LOGI(SOFT_AP_TAG, "Starting ESP_WIFI_MODE_STA");

    s_wifi_event_group = xEventGroupCreate();
    ESP_LOGD(SOFT_AP_TAG, "Created Wifi Event Group");

    ESP_LOGI(SOFT_AP_TAG, "Initializing TCP/IP Stack");
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_LOGI(SOFT_AP_TAG, "Creating Default ESP Event Loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(SOFT_AP_TAG, "Creating Default ESP WIFI STA");
    esp_netif_create_default_wifi_sta();

    ESP_LOGI(SOFT_AP_TAG, "Creating Default ESP WIFI AP");
    esp_netif_create_default_wifi_ap();

    ESP_LOGI(SOFT_AP_TAG, "Initializing WIFI with default config");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGD(SOFT_AP_TAG, "Initialized WIFI");

    ESP_LOGV(SOFT_AP_TAG, "Registering Handler for WIFI_EVENT");
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_connect_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_LOGD(SOFT_AP_TAG, "Successfully registered Handler for WIFI_EVENT");

    ESP_LOGV(SOFT_AP_TAG, "Registering Handler for IP_EVENT");
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_connect_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    ESP_LOGD(SOFT_AP_TAG, "Successfully registered Handler for IP_EVENT");

    attempt_connect_to_existing_wifi(wifi_config);

    ESP_LOGI(SOFT_AP_TAG, "Waiting for Connected or Fail Event");
    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    ESP_LOGI(SOFT_AP_TAG, "Done waiting, checking result");
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(SOFT_AP_TAG, "connected to SSID:%s", wifi_config->ssid);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(SOFT_AP_TAG, "Failed to connect to SSID:%s, password:%s", wifi_config->ssid, wifi_config->password);
        open_ap();
    }
    else
    {
        ESP_LOGE(SOFT_AP_TAG, "UNEXPECTED EVENT");
    }
    return self_ip;
}
