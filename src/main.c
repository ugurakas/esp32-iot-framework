#include "config_store.h"
#include "wifi_manager.h"
#include "mqtt_service.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app);

int main(void)
{
    static struct app_config config;
    int err = config_store_init();
    if (err) {
        LOG_ERR("Settings initialization failed: %d", err);
        return 0;
    }
    config_store_get(&config);
    if (!config.ssid[0]) {
        LOG_INF("Configure Wi-Fi with 'iot set', then reboot");
        return 0;
    }
    err = wifi_manager_init(config.ssid, config.password);
    if (err) {
        LOG_ERR("Wi-Fi initialization failed: %d", err);
        return 0;
    }
    if (config.broker[0]) {
        err = mqtt_service_start(&config);
        if (err) {
            LOG_ERR("MQTT initialization failed: %d", err);
        }
    }
    /* Test upgrades remain unconfirmed until an operator verifies health.
     * This preserves MCUboot rollback if networking or the application fails. */
    return 0;
}
