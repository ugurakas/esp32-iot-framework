#include "wifi_manager.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"

static const char *TAG = "wifi_manager";

static wifi_manager_state_t wifi_state = WIFI_MANAGER_DISCONNECTED;

static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0

static int retry_count = 0;

static wifi_manager_state_t wifi_state =
    WIFI_MANAGER_DISCONNECTED;

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data);

esp_err_t wifi_manager_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_event_group = xEventGroupCreate();

ESP_ERROR_CHECK(
    esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL));

ESP_ERROR_CHECK(
    esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL));

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
static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch(event_id)
        {
            case WIFI_EVENT_STA_START:

                ESP_LOGI(TAG,"WiFi Started");

                break;

            case WIFI_EVENT_STA_CONNECTED:

                ESP_LOGI(TAG,"Connected to AP");

                wifi_state = WIFI_MANAGER_CONNECTED;

                break;

            case WIFI_EVENT_STA_DISCONNECTED:

                ESP_LOGW(TAG,"Disconnected");

                wifi_state = WIFI_MANAGER_DISCONNECTED;

                break;

            default:
                break;
        }
    }

    if(event_base == IP_EVENT)
    {
        if(event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *event = event_data;

            ESP_LOGI(TAG,
                     "IP:" IPSTR,
                     IP2STR(&event->ip_info.ip));

            xEventGroupSetBits(
                wifi_event_group,
                WIFI_CONNECTED_BIT);
        }
    }
}
