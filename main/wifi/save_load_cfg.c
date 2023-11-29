#include "save_load_cfg.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "stdio.h"
#include <string.h>

static const char *SAVE_LOAD_TAG = "NVS";

/**
  * @brief Load SSID and PWD of a WIFI network from NVS
  * @param wifi_config reference to load data into.
  * @retval esp_err_t error
  */
esp_err_t load_known_wifi_config(wifi_sta_config_t* wifi_config)
{
    ESP_LOGI(SAVE_LOAD_TAG, "Loading WLAN data");
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    ESP_LOGV(SAVE_LOAD_TAG, "Opening storage");
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGD(SAVE_LOAD_TAG, "Successfully opened storage");

    // Read run time blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS

    // obtain required memory space to store blob being read from NVS
    ESP_LOGD(SAVE_LOAD_TAG, "Checking SSID length");
    err = nvs_get_blob(my_handle, "ssid", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    if (required_size == 0) {
        ESP_LOGW(SAVE_LOAD_TAG,"SSID length is 0!");
    } else {
        ESP_LOGV(SAVE_LOAD_TAG, "SSID is not empty, re-reading to set");
        err = nvs_get_blob(my_handle, "ssid", (wifi_config->ssid), &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(SAVE_LOAD_TAG,"Failed setting SSID!");
            return err;
        }
        ESP_LOGI(SAVE_LOAD_TAG,"Loaded SSID: '%s'", wifi_config->ssid);
        //free(run_time);
    }

    required_size = 0;  // value will default to 0, if not set yet in NVS

    // obtain required memory space to store blob being read from NVS
    ESP_LOGD(SAVE_LOAD_TAG, "Checking password length");
    err = nvs_get_blob(my_handle, "pwd", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    if (required_size == 0) {
        ESP_LOGI(SAVE_LOAD_TAG,"Password length is 0!");
    } else {
        ESP_LOGV(SAVE_LOAD_TAG, "Password is not empty, re-reading to set");
        err = nvs_get_blob(my_handle, "pwd", (wifi_config->password), &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(SAVE_LOAD_TAG,"Failed setting Password!");
            return err;
        }
        ESP_LOGI(SAVE_LOAD_TAG,"Loaded Password: '%s'", wifi_config->password);
        //free(run_time);
    }

    // Close
    ESP_LOGV(SAVE_LOAD_TAG, "Closing storage");
    nvs_close(my_handle);
    ESP_LOGD(SAVE_LOAD_TAG, "Successfully closed storage");

    return ESP_OK;
}

/**
  * @brief Save SSID and PWD of a WIFI network to NVS
  * @param wifi_config config to save
  * @retval esp_err_t error
  */
esp_err_t save_wifi_config(wifi_sta_config_t* wifi_config)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    ESP_LOGV(SAVE_LOAD_TAG, "Opening storage");
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGD(SAVE_LOAD_TAG, "Successfully opened storage");

    ESP_LOGV(SAVE_LOAD_TAG, "Writing SSID to storage");
    err = nvs_set_blob(my_handle, "ssid", wifi_config->ssid, sizeof(wifi_config->ssid));

    if (err != ESP_OK) return err;
    ESP_LOGD(SAVE_LOAD_TAG, "Successfully wrote SSID to storage");

    ESP_LOGV(SAVE_LOAD_TAG, "Writing password to storage");
    err = nvs_set_blob(my_handle, "pwd", wifi_config->password, sizeof(wifi_config->password));

    if (err != ESP_OK) return err;
    ESP_LOGD(SAVE_LOAD_TAG, "Successfully wrote password to storage");

    // Commit
    ESP_LOGV(SAVE_LOAD_TAG, "Committing changes");
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGI(SAVE_LOAD_TAG, "Successfully saved WLAN data");

    // Close
    ESP_LOGV(SAVE_LOAD_TAG, "Closing storage");
    nvs_close(my_handle);
    ESP_LOGD(SAVE_LOAD_TAG, "Successfully closed storage");

    // load_known_wifi_config(wifi_config);

    return ESP_OK;
}