//Config includes
#include <sys/param.h>

//Vendor includes
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"

//APP includes
#include "wifi/soft_ap.h"
#include "wifi/save_load_cfg.h"
#include "wifi/web_ap_conf.h"
#include "button/button.h"

static const char *TAG = "MAIN";

void configure_wifi(void) {
    ESP_LOGI(TAG, "Configuring Wifi");

    wifi_sta_config_t wifi_config = {};
    esp_ip4_addr_t self_ip;

    ESP_LOGI(TAG, "Loading previous WIFI config");
    load_known_wifi_config(&wifi_config);

    ESP_LOGI(TAG, "Connecting to known WIFI or launching Config AP if known WIFI is not in range.");
    self_ip = connect_or_ap(&wifi_config);

    ESP_LOGI(TAG, "Starting Webserver with config page");
    start_webserver();

    ESP_LOGW(TAG, "My Webserver is running here: http://" IPSTR, IP2STR(&self_ip));
}

void init_nvs(void)
{
    ESP_LOGI(TAG, "Initializing NVS");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

static void button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting APP");
    init_nvs();
    configure_wifi();
    create_button(21, button_single_click_cb);
    while (1) {
        ESP_LOGI(TAG, "I'm still standing!");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
