#include "web.h"
#include <esp_http_server.h>
#include "esp_log.h"
#include <string.h>
#include "esp_wifi.h"
#include "restart.h"
#include "../wifi/web_ap_conf.h"

static const char *WEB_TAG = "WEB";

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(WEB_TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(WEB_TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &index_get);
        httpd_register_uri_handler(server, &index_post);
        httpd_register_uri_handler(server, &restart_post);
        #if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
        #endif
        return server;
    }

    ESP_LOGI(WEB_TAG, "Error starting server!");
    return NULL;
}