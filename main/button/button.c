#include "esp_log.h"
#include "iot_button.h"

static const char *BUTTON_TAG = "BTN";

/**
 * @brief  Create a button handle
 * @param gpio_num the number of the GPIO pin to use
 * @param cb callback on (single click) button press
 * @retval button_handle_t the button
 */
button_handle_t create_button(int32_t gpio_num, button_cb_t cb)
{
  // create gpio button
  button_config_t gpio_btn_cfg = {
      .type = BUTTON_TYPE_GPIO,
      .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
      .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
      .gpio_button_config = {
          .gpio_num = gpio_num,
          .active_level = 0,
      },
  };
  button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
  if (NULL == gpio_btn)
  {
    ESP_LOGE(BUTTON_TAG, "Button create failed");
  } else {
    iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, cb, NULL);
  }
  return gpio_btn;
}
