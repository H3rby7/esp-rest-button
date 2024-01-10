#ifndef _WEB_H
#define _WEB_H

#include <stdint.h>
#include "esp_http_server.h"

httpd_handle_t start_webserver(void);
#endif
