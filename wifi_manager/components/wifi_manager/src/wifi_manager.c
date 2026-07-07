#include "wifi_manager.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

static const char *TAG = "wifi_manager";

static wifi_manager_state_t wifi_state =
    WIFI_MANAGER_DISCONNECTED;

esp_err_t wifi_manager_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_LOGI(TAG, "WiFi Manager Initialized");

    return ESP_OK;
}

esp_err_t wifi_manager_connect(const char *ssid,
                               const char *password)
{
    ESP_LOGI(TAG, "Connecting to %s", ssid);

    wifi_state = WIFI_MANAGER_CONNECTING;

    return ESP_OK;
}

esp_err_t wifi_manager_disconnect(void)
{
    wifi_state = WIFI_MANAGER_DISCONNECTED;

    ESP_LOGI(TAG, "Disconnected");

    return ESP_OK;
}

wifi_manager_state_t wifi_manager_get_state(void)
{
    return wifi_state;
}

bool wifi_manager_is_connected(void)
{
    return wifi_state == WIFI_MANAGER_CONNECTED;
}
