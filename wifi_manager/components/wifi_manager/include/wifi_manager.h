#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    WIFI_MANAGER_DISCONNECTED = 0,
    WIFI_MANAGER_CONNECTING,
    WIFI_MANAGER_CONNECTED

} wifi_manager_state_t;

esp_err_t wifi_manager_init(void);

esp_err_t wifi_manager_connect(const char *ssid,
                               const char *password);

esp_err_t wifi_manager_disconnect(void);

wifi_manager_state_t wifi_manager_get_state(void);

bool wifi_manager_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif
