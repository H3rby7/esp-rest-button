#ifndef _BUTTON_H
#define _BUTTON_H

#include "iot_button.h"

button_handle_t create_button(int32_t gpio_num, button_cb_t cb);
#endif
