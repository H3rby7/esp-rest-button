#include "id.h"
#include "request.h"
#include "esp_log.h"
#include <stdint.h>
#include "esp_http_client.h"

static const char *REST_REQ_TAG = "REST";

/**
 * @brief Send REST request
 * @retval 1 = SUCCESS
 * @retval 0 = FAIL
 * @details Send this device's UID to given URL
 */
int send_rest_request()
{
  ESP_LOGV(REST_REQ_TAG, "Getting UID...");
  uint64_t ownID = get_unique_id();
  ESP_LOGD(REST_REQ_TAG, "UID acquired -> '%llu'", ownID);

  ESP_LOGV(REST_REQ_TAG, "Initializing HTTP Client...");
  esp_http_client_config_t config = {
      .host = CONFIG_DMX_WEB_SERVER,
      .path = "/api/v1/trigger",
      .method = HTTP_METHOD_POST,
      .transport_type = HTTP_TRANSPORT_OVER_TCP,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  ESP_LOGD(REST_REQ_TAG, "HTTP Client initialized.");

  // UID as POST content
  char uid[20];
  sprintf(uid, "%llu", ownID);
  esp_http_client_set_post_field(client, uid, strlen(uid));

  ESP_LOGI(REST_REQ_TAG, "Performing POST to '%s', using path '%s' with content '%s'", config.host, config.path, uid);
  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK)
  {
    ESP_LOGD(REST_REQ_TAG, "HTTP POST Status = %d, content_length = %" PRId64,
             esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));
  }
  else
  {
    ESP_LOGE(REST_REQ_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
  }

  ESP_LOGV(REST_REQ_TAG, "Cleaning up HTTP Client...");
  esp_http_client_cleanup(client);
  ESP_LOGD(REST_REQ_TAG, "HTTP Client cleaned up.");
  return 1;
}
