#ifndef _WEB_REST_CONF_H
#define _WEB_REST_CONF_H

#include <esp_http_server.h>
#include "freertos/task.h"
#include <ctype.h>
#include <stdlib.h>
#include "esp_log.h"
#include <string.h>
#include "esp_wifi.h"

static const char *WEB_REST_CONF_TAG = "WEB-REST-CONFIG";

char *url_decode(const char *str) {
  int d = 0; /* whether or not the string is decoded */

  char *dStr = malloc(strlen(str) + 1);
  char eStr[] = "00"; /* for a hex code */

  strcpy(dStr, str);

  while(!d) {
    d = 1;
    int i; /* the counter for the string */

    for(i=0;i<strlen(dStr);++i) {

      if(dStr[i] == '%') {
        if(dStr[i+1] == 0)
          return dStr;

        if(isxdigit((int)dStr[i+1]) && isxdigit((int)dStr[i+2])) {

          d = 0;

          /* combine the next to numbers into one */
          eStr[0] = dStr[i+1];
          eStr[1] = dStr[i+2];

          /* convert it to decimal */
          long int x = strtol(eStr, NULL, 16);

          /* remove the hex */
          memmove(&dStr[i+1], &dStr[i+3], strlen(&dStr[i+3])+1);

          dStr[i] = x;
        }
      }
      else if(dStr[i] == '+') { dStr[i] = ' '; }
    }
  }

  return dStr;
}

esp_err_t rest_get_handler(httpd_req_t *req)
{
    httpd_resp_sendstr_chunk(req, "<!DOCTYPE html><html>");

    httpd_resp_sendstr_chunk(req, "<head>");
    httpd_resp_sendstr_chunk(req, "<title>Esp32</title>");
    httpd_resp_sendstr_chunk(req, "<meta charset='UTF-8'>");
    httpd_resp_sendstr_chunk(req, "</head>");

    httpd_resp_sendstr_chunk(req, "<body>");
    httpd_resp_sendstr_chunk(req, "<h1>Definieren sie den Host</h1>");
    httpd_resp_sendstr_chunk(req, "<form method=\"POST\" action=\"/\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"HOST\">Host Name/IP</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"host\" type=\"text\" name=\"host\" maxlength=\"32\" minlength=\"4\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"Port\">Host Port</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"port\" type=\"text\" name=\"port\" maxlength=\"5\" minlength=\"1\">");

    httpd_resp_sendstr_chunk(req, "<input id=\"dummy\" type=\"text\" name=\"dummy\" maxlength=\"64\" minlength=\"0\" style=\"display:none;\">");

    httpd_resp_sendstr_chunk(req, "<input type=\"submit\" value=\"Speichern\"/>");
    httpd_resp_sendstr_chunk(req, "</form>");

    httpd_resp_sendstr_chunk(req, "</body></html>");

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t rest_get = {
        .uri        = "/rest",
        .method     = HTTP_GET,
        .handler    = rest_get_handler,
        .user_ctx   = NULL
};

esp_err_t rest_post_handler(httpd_req_t *req)
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
    err = save_wifi_config(&wifi_config);
    if (err != ESP_OK) return err;

    // TODO: needed?
    
    esp_restart();
 
    return ESP_OK;
}

static const httpd_uri_t rest_post = {
        .uri        = "/",
        .method     = HTTP_POST,
        .handler    = rest_post_handler,
        .user_ctx   = NULL
};

#endif
