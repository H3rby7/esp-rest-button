#include "nvs_rest.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "stdio.h"
#include <string.h>

static const char *NVS_REST_TAG = "NVS-REST";
static const char *NVS_REST_NAMESPACE = "rest";

/**
  * @brief Load host and port of REST endpoint from NVS
  * @param config reference to load data into.
  * @retval esp_err_t error
  */
esp_err_t load_rest_config_from_nvs(rest_config_t* config)
{
    ESP_LOGI(NVS_REST_TAG, "Loading data");
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    ESP_LOGV(NVS_REST_TAG, "Opening NVS storage with namespace '%s'", NVS_REST_NAMESPACE);
    err = nvs_open(NVS_REST_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_REST_TAG, "Successfully opened namespace '%s'", NVS_REST_NAMESPACE);

    // Read run time blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS

    // obtain required memory space to store blob being read from NVS
    ESP_LOGD(NVS_REST_TAG, "Checking host length");
    err = nvs_get_blob(my_handle, "host", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    if (required_size == 0) {
        ESP_LOGW(NVS_REST_TAG,"host length is 0!");
    } else {
        ESP_LOGV(NVS_REST_TAG, "host is not empty, re-reading to set");
        err = nvs_get_blob(my_handle, "host", (config->host), &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_REST_TAG,"Failed setting host!");
            return err;
        }
        ESP_LOGI(NVS_REST_TAG,"Loaded host: '%s'", config->host);
    }

    required_size = 0;  // value will default to 0, if not set yet in NVS

    // obtain required memory space to store blob being read from NVS
    ESP_LOGD(NVS_REST_TAG, "Checking port length");
    err = nvs_get_blob(my_handle, "port", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    if (required_size == 0) {
        ESP_LOGI(NVS_REST_TAG,"port length is 0!");
    } else {
        ESP_LOGV(NVS_REST_TAG, "port is not empty, re-reading to set");
        err = nvs_get_blob(my_handle, "port", (config->port), &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_REST_TAG,"Failed setting port!");
            return err;
        }
        ESP_LOGI(NVS_REST_TAG,"Loaded port: '%d'", config->port);
    }

    // Close
    ESP_LOGV(NVS_REST_TAG, "Closing storage");
    nvs_close(my_handle);
    ESP_LOGD(NVS_REST_TAG, "Successfully closed storage");

    return ESP_OK;
}

/**
  * @brief Save host and port of REST endpoint to NVS
  * @param config config to save
  * @retval esp_err_t error
  */
esp_err_t save_rest_config_to_nvs(rest_config_t* config)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    ESP_LOGV(NVS_REST_TAG, "Opening NVS storage with namespace '%s'", NVS_REST_NAMESPACE);
    err = nvs_open(NVS_REST_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_REST_TAG, "Successfully opened namespace '%s'", NVS_REST_NAMESPACE);

    ESP_LOGV(NVS_REST_TAG, "Writing host to storage");
    err = nvs_set_blob(my_handle, "host", config->host, sizeof(config->host));

    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_REST_TAG, "Successfully wrote host to storage");

    ESP_LOGV(NVS_REST_TAG, "Writing port to storage");
    err = nvs_set_blob(my_handle, "port", config->port, sizeof(config->port));

    if (err != ESP_OK) return err;
    ESP_LOGD(NVS_REST_TAG, "Successfully wrote port to storage");

    // Commit
    ESP_LOGV(NVS_REST_TAG, "Committing changes");
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;
    ESP_LOGI(NVS_REST_TAG, "Successfully saved REST data");

    // Close
    ESP_LOGV(NVS_REST_TAG, "Closing storage");
    nvs_close(my_handle);
    ESP_LOGD(NVS_REST_TAG, "Successfully closed storage");

    return ESP_OK;
}