#include "id.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_random.h"
#include "math.h"
#include "string.h"

static const char *GET_ID_TAG = "UID";

uint64_t uniqueID;

/**
 * @brief Convert an array of uint8 and size 8 to a unit64
 */
uint64_t uint8arr_to_uint64(uint8_t *arr) {
  uint64_t res = 0;
  for (size_t i = 0; i < 8; i++)
  {
    uint8_t val = arr[i];
    ESP_LOGD(GET_ID_TAG, "MAC at %d is %u", i, val);
    res += powl(256, i) * arr[i]; 
  }
  return res;
}

/**
 * @brief Get a unique ID for this device.
 * @retval a unique ID
 * @details utilizes esp_efuse_mac_get_default(MacAddress) OR generates a random UID
 * @details Note that if MAC address fails, the UID changes with every boot!
 */
uint64_t read_mac_or_random_uid()
{
  uint8_t macAdress[8];
  memset(&macAdress[0], 0, sizeof(macAdress));
  if (esp_efuse_mac_get_default(&macAdress) == ESP_OK)
  {
    ESP_LOGD(GET_ID_TAG, "Successfully acquired MAC to use as UID");
  }
  else
  {
    ESP_LOGW(GET_ID_TAG, "Could not retrieve MAC as UID, generating random UID");
    esp_fill_random(&macAdress, 8);
  }
  return uint8arr_to_uint64(&macAdress);
}

/**
 * @brief Read a unique ID for this device
 * @details MAC based OR if that fails a random UID (that changes every boot)
 */
uint64_t get_unique_id()
{
  if (uniqueID == NULL) {
    ESP_LOGD(GET_ID_TAG, "Unique ID not initialized, setting up...");
    uniqueID = read_mac_or_random_uid();
    ESP_LOGI(GET_ID_TAG, "Unique ID found -> '%llu'", uniqueID);
  }
  return uniqueID;
}