#ifndef _SAVE_LOAD_H
#define _SAVE_LOAD_H

#include "stdint.h"
#include "esp_err.h"

typedef struct
{
  char host[32]; /**< Host for the REST endpoint. */
  uint16_t port;    /**< Port for the REST endpoint. */
} endpoint_config_t;

esp_err_t load_rest_config_from_nvs(endpoint_config_t *config);
esp_err_t save_rest_config_to_nvs(endpoint_config_t *config);
#endif
