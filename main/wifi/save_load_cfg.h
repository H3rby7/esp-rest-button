#ifndef _SAVE_LOAD_H
#define _SAVE_LOAD_H

#include <stdint.h>
#include "esp_err.h"
#include "esp_wifi.h"

esp_err_t load_known_wifi_config(wifi_sta_config_t* wifi_config);
esp_err_t save_wifi_config(wifi_sta_config_t* wifi_config);
#endif
