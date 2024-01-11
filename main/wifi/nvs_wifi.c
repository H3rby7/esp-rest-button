#include "nvs_wifi.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "stdio.h"
#include <string.h>

static const char *NVS_WIFI_TAG = "NVS-WIFI";
static const char *NVS_WIFI_NAMESPACE = "wifi";

/**
  * @brief Load SSID and PWD of a WIFI network from NVS
  * @param wifi_config reference to load data into.
  * @retval esp_err_t error
  */
esp_err_t load_known_wifi_config_from_nvs(wifi_sta_config_t* wifi_config)
{
    ESP_LOGI(NVS_WIFI_TAG, "Loading data");
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    ESP_LOGV(NVS_WIFI_TAG, "Opening NVS storage with namespace '%s'", NVS_WIFI_NAMESPACE);
    err = nvs_open(NVS_WIFI_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_WIFI_TAG, "Successfully opened namespace '%s'", NVS_WIFI_NAMESPACE);

    // Read run time blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS

    // obtain required memory space to store blob being read from NVS
    ESP_LOGD(NVS_WIFI_TAG, "Checking SSID length");
    err = nvs_get_blob(my_handle, "ssid", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    if (required_size == 0) {
        ESP_LOGW(NVS_WIFI_TAG,"SSID length is 0!");
    } else {
        ESP_LOGV(NVS_WIFI_TAG, "SSID is not empty, re-reading to set");
        err = nvs_get_blob(my_handle, "ssid", (wifi_config->ssid), &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_WIFI_TAG,"Failed setting SSID!");
            return err;
        }
        ESP_LOGI(NVS_WIFI_TAG,"Loaded SSID: '%s'", wifi_config->ssid);
    }

    required_size = 0;  // value will default to 0, if not set yet in NVS

    // obtain required memory space to store blob being read from NVS
    ESP_LOGD(NVS_WIFI_TAG, "Checking password length");
    err = nvs_get_blob(my_handle, "pwd", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    if (required_size == 0) {
        ESP_LOGI(NVS_WIFI_TAG,"Password length is 0!");
    } else {
        ESP_LOGV(NVS_WIFI_TAG, "Password is not empty, re-reading to set");
        err = nvs_get_blob(my_handle, "pwd", (wifi_config->password), &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_WIFI_TAG,"Failed setting Password!");
            return err;
        }
        ESP_LOGI(NVS_WIFI_TAG,"Loaded Password.");
    }

    // Close
    ESP_LOGV(NVS_WIFI_TAG, "Closing NVS namespace '%s'", NVS_WIFI_NAMESPACE);
    nvs_close(my_handle);
    ESP_LOGD(NVS_WIFI_TAG, "Successfully closed NVS storage with namespace '%s'", NVS_WIFI_NAMESPACE);

    return ESP_OK;
}

/**
  * @brief Save SSID and PWD of a WIFI network to NVS
  * @param wifi_config config to save
  * @retval esp_err_t error
  */
esp_err_t save_wifi_config_to_nvs(wifi_sta_config_t* wifi_config)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    ESP_LOGV(NVS_WIFI_TAG, "Opening NVS storage with namespace '%s'", NVS_WIFI_NAMESPACE);
    err = nvs_open(NVS_WIFI_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_WIFI_TAG, "Successfully opened namespace '%s'", NVS_WIFI_NAMESPACE);

    ESP_LOGV(NVS_WIFI_TAG, "Writing SSID to storage");
    err = nvs_set_blob(my_handle, "ssid", wifi_config->ssid, sizeof(wifi_config->ssid));

    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_WIFI_TAG, "Successfully wrote SSID to storage");

    ESP_LOGV(NVS_WIFI_TAG, "Writing password to storage");
    err = nvs_set_blob(my_handle, "pwd", wifi_config->password, sizeof(wifi_config->password));

    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_WIFI_TAG, "Successfully wrote password to storage");

    // Commit
    ESP_LOGV(NVS_WIFI_TAG, "Committing changes");
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGI(NVS_WIFI_TAG, "Successfully saved WIFI data");

    // Close
    ESP_LOGV(NVS_WIFI_TAG, "Closing NVS namespace '%s'", NVS_WIFI_NAMESPACE);
    nvs_close(my_handle);
    ESP_LOGD(NVS_WIFI_TAG, "Successfully closed NVS storage with namespace '%s'", NVS_WIFI_NAMESPACE);

    return ESP_OK;
}