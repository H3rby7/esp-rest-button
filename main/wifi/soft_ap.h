#ifndef _SOFT_AP_H
#define _SOFT_AP_H

#include "esp_wifi.h"
#include "esp_netif.h"

esp_ip4_addr_t connect_or_ap(wifi_sta_config_t* wifi_config);
#endif
