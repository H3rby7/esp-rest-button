#ifndef _WEB_AP_CONF_H
#define _WEB_AP_CONF_H

#include <esp_http_server.h>
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "esp_wifi.h"
#include "nvs_wifi.h"
#include "../web/web_util.h"

static const char *WEB_AP_CONF_TAG = "WEB-CONFIG";

esp_err_t index_get_handler(httpd_req_t *req)
{
    httpd_resp_sendstr_chunk(req, "<!DOCTYPE html><html>");

    httpd_resp_sendstr_chunk(req, "<head>");
    httpd_resp_sendstr_chunk(req, "<title>Esp32</title>");
    httpd_resp_sendstr_chunk(req, "<meta charset='UTF-8'>");
    httpd_resp_sendstr_chunk(req, "</head>");

    httpd_resp_sendstr_chunk(req, "<body>");
    httpd_resp_sendstr_chunk(req, "<h1>Wählen Sie Ihr Netwerk</h1>");
    httpd_resp_sendstr_chunk(req, "<form method=\"POST\" action=\"/\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"SSID\">WiFi Name</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"ssid\" type=\"text\" name=\"ssid\" maxlength=\"32\" minlength=\"4\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"Password\">Password</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"pwd\" type=\"text\" name=\"pwd\" maxlength=\"64\" minlength=\"0\">");

    httpd_resp_sendstr_chunk(req, "<input id=\"dummy\" type=\"text\" name=\"dummy\" maxlength=\"64\" minlength=\"0\" style=\"display:none;\">");

    httpd_resp_sendstr_chunk(req, "<input type=\"submit\" value=\"Speichern\"/>");
    httpd_resp_sendstr_chunk(req, "</form>");

    httpd_resp_sendstr_chunk(req, "<br><br><br>");

    httpd_resp_sendstr_chunk(req, "<form method=\"POST\" action=\"/restart\">");
    httpd_resp_sendstr_chunk(req, "<input type=\"submit\" value=\"Restart ESP32\"/>");
    httpd_resp_sendstr_chunk(req, "</form>");

    httpd_resp_sendstr_chunk(req, "</body></html>");

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t index_get = {
        .uri        = "/",
        .method     = HTTP_GET,
        .handler    = index_get_handler,
        .user_ctx   = NULL
};

esp_err_t index_post_handler(httpd_req_t *req)
{
    esp_err_t err;
    uint8_t buffer[100];

    char ssid[32];
    char pwd[64];

    httpd_req_recv(req, (char *) buffer, 100);
 
    if (httpd_query_key_value((char *) buffer, "ssid", ssid, 32) == ESP_ERR_NOT_FOUND) {
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "SSID required", -1);
        return ESP_OK;
    }
    if (httpd_query_key_value((char *) buffer, "pwd", pwd, 64) == ESP_ERR_NOT_FOUND) {
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "Password is required", -1);
        return ESP_OK;
    }

    char *ssidDecoded = url_decode(ssid);
    char *pwdDecoded = url_decode(pwd);
 
    if (strlen((char *) ssid) < 1) {
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "<p>Invalid ssid</p>", -1);
        return ESP_OK;
    }
 
    ESP_LOGI(WEB_AP_CONF_TAG, "Received new WIFI login data -> SSID: %s, Pass: %s", ssidDecoded, pwdDecoded);
    httpd_resp_send(req, "<h1>ESP startet neu, um die Konfiguration anzuwenden...</h1>", -1);

    wifi_sta_config_t wifi_config = {};

    memcpy(wifi_config.ssid, ssidDecoded, strlen(ssidDecoded));
    memcpy(wifi_config.password, pwdDecoded, strlen(pwdDecoded));

    free(ssidDecoded);
    free(pwdDecoded);
    err = save_wifi_config_to_nvs(&wifi_config);
    if (err != ESP_OK) return err;

    int secondsToReboot = 5;
    while (secondsToReboot > 0)
    {
        ESP_LOGW(WEB_AP_CONF_TAG, "Rebooting in %d seconds to apply new WIFI config", secondsToReboot);
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
        secondsToReboot--;
    }

    esp_restart();
 
    return ESP_OK;
}

static const httpd_uri_t index_post = {
        .uri        = "/",
        .method     = HTTP_POST,
        .handler    = index_post_handler,
        .user_ctx   = NULL
};

#endif
