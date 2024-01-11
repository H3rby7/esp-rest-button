#ifndef _WEB_REST_CONF_H
#define _WEB_REST_CONF_H

#include <esp_http_server.h>
#include <stdlib.h>
#include "esp_log.h"
#include <string.h>
#include "nvs_rest.h"
#include "../web/web_util.h"

static const char *WEB_ENDPOINT_CONF_TAG = "WEB-ENDPOINT-CONFIG";

esp_err_t endpoint_get_handler(httpd_req_t *req)
{
    endpoint_config_t config = {};
    load_rest_config_from_nvs(&config);

    ESP_LOGD(WEB_ENDPOINT_CONF_TAG, "Old config is %s:%u", config.host, config.port);

    httpd_resp_sendstr_chunk(req, "<!DOCTYPE html><html>");

    httpd_resp_sendstr_chunk(req, "<head>");
    httpd_resp_sendstr_chunk(req, "<title>Esp32</title>");
    httpd_resp_sendstr_chunk(req, "<meta charset='UTF-8'>");
    httpd_resp_sendstr_chunk(req, "</head>");

    httpd_resp_sendstr_chunk(req, "<body>");
    httpd_resp_sendstr_chunk(req, "<h1>Definieren sie den Host des REST Endpoints</h1>");
    httpd_resp_sendstr_chunk(req, "<form method=\"POST\" action=\"/endpoint\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"HOST\">Endpoint Host Name/IP</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"host\" type=\"text\" name=\"host\" maxlength=\"32\" minlength=\"4\" placeholder=\"192.168.178.23\" value=\"");
    // httpd_resp_sendstr_chunk(req, config.host);
    httpd_resp_sendstr_chunk(req, "\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"Port\">Endpoint Host Port</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"port\" type=\"number\" name=\"port\" max=\"65535\" min=\"1\" placeholder=\"80\">");

    httpd_resp_sendstr_chunk(req, "<input id=\"dummy\" type=\"text\" name=\"dummy\" maxlength=\"64\" minlength=\"0\" style=\"display:none;\">");

    httpd_resp_sendstr_chunk(req, "<input type=\"submit\" value=\"Speichern\"/>");
    httpd_resp_sendstr_chunk(req, "</form>");

    httpd_resp_sendstr_chunk(req, "</body></html>");

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t endpoint_get = {
        .uri        = "/endpoint",
        .method     = HTTP_GET,
        .handler    = endpoint_get_handler,
        .user_ctx   = NULL
};

esp_err_t endpoint_post_handler(httpd_req_t *req)
{
    esp_err_t err;
    uint8_t buffer[100];

    char host[32];
    char port[5];

    httpd_req_recv(req, (char *) buffer, 100);
 
    if (httpd_query_key_value((char *) buffer, "host", host, 32) == ESP_ERR_NOT_FOUND) {
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "HOST required", -1);
        return ESP_OK;
    }
    if (httpd_query_key_value((char *) buffer, "port", port, 64) == ESP_ERR_NOT_FOUND) {
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "Port is required", -1);
        return ESP_OK;
    }

    char *hostDecoded = url_decode(host);
    char *portDecoded = url_decode(port);
 
    if (strlen((char *) host) < 1) {
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "<p>Invalid host</p>", -1);
        return ESP_OK;
    }
 
    ESP_LOGI(WEB_ENDPOINT_CONF_TAG, "Received new ENDPOINT data -> Host: %s, Port: %s", hostDecoded, portDecoded);
    httpd_resp_send(req, "<h1>TODO</h1>", -1);

    endpoint_config_t config = {};

    memcpy(config.host, hostDecoded, strlen(hostDecoded));
    config.port = strtoul(portDecoded, NULL, 10);

    free(hostDecoded);
    free(portDecoded);
    err = save_rest_config_to_nvs(&config);
    if (err != ESP_OK) return err;

    return ESP_OK;
}

static const httpd_uri_t endpoint_post = {
        .uri        = "/endpoint",
        .method     = HTTP_POST,
        .handler    = endpoint_post_handler,
        .user_ctx   = NULL
};

#endif
