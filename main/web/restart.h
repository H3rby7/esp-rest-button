#ifndef _WEB_RESTART_H
#define _WEB_RESTART_H

#include <esp_http_server.h>
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "esp_wifi.h"

static const char *WEB_RESTART_TAG = "WEB-RESTART";

esp_err_t restart_handler(httpd_req_t *req)
{
  int secondsToReboot = 5;
  while (secondsToReboot > 0)
  {
    ESP_LOGW(WEB_RESTART_TAG, "Rebooting in %d seconds", secondsToReboot);
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    secondsToReboot--;
  }
  esp_restart();
  return ESP_OK;
}

static const httpd_uri_t restart_post = {
        .uri        = "/restart",
        .method     = HTTP_POST,
        .handler    = restart_handler,
        .user_ctx   = NULL
};

#endif
