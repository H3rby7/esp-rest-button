#include "id.h"
#include "request.h"
#include "esp_log.h"
#include <stdint.h>

static const char *REST_REQ_TAG = "REST";

/**
 * @brief Send REST request
 * @retval 1 = SUCCESS
 * @retval 0 = FAIL
 * @details Send this device's UID to given URL
 */
int send_rest_request()
{
  uint64_t ownID = get_unique_id();
  ESP_LOGW(REST_REQ_TAG, "Would send REST with ID %llu", ownID);
  return 1;
}
