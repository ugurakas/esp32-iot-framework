#ifndef APP_CONFIG_STORE_H
#define APP_CONFIG_STORE_H
#include <stdint.h>

struct app_config {
    char ssid[33];
    char password[65];
    char broker[128];
    char client_id[48];
};
int config_store_init(void);
void config_store_get(struct app_config *out);
/* Persist one value; takes effect after reboot. No secret values are logged. */
int config_store_set(const char *key, const char *value);
#endif
