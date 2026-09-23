#include "mqtt_service.h"
#include "wifi_manager.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/net/socket.h>

LOG_MODULE_REGISTER(mqtt_service);
static struct app_config config;
static struct mqtt_client client;
static struct sockaddr_storage broker;
static uint8_t rx_buffer[1024], tx_buffer[1024];
static bool connected;
static struct k_thread mqtt_thread;
K_THREAD_STACK_DEFINE(mqtt_stack, 6144);

static void mqtt_event(struct mqtt_client *c, const struct mqtt_evt *event)
{
    ARG_UNUSED(c);
    if (event->type == MQTT_EVT_CONNACK) {
        connected = event->result == 0;
    } else if (event->type == MQTT_EVT_DISCONNECT) {
        connected = false;
    }
}

static int connect_broker(void)
{
    struct zsock_addrinfo *result = NULL;
    struct zsock_addrinfo hints = {
        .ai_family = AF_INET, .ai_socktype = SOCK_STREAM
    };
    int err = zsock_getaddrinfo(config.broker, "1883", &hints, &result);
    if (err) {
        return -EHOSTUNREACH;
    }
    if (!result || result->ai_addrlen > sizeof(broker)) {
        if (result) {
            zsock_freeaddrinfo(result);
        }
        return -EINVAL;
    }
    memset(&broker, 0, sizeof(broker));
    memcpy(&broker, result->ai_addr, result->ai_addrlen);
    zsock_freeaddrinfo(result);

    mqtt_client_init(&client);
    client.broker = &broker;
    client.evt_cb = mqtt_event;
    client.client_id.utf8 = (uint8_t *)config.client_id;
    client.client_id.size = strlen(config.client_id);
    client.protocol_version = MQTT_VERSION_3_1_1;
    client.rx_buf = rx_buffer;
    client.rx_buf_size = sizeof(rx_buffer);
    client.tx_buf = tx_buffer;
    client.tx_buf_size = sizeof(tx_buffer);
    client.transport.type = MQTT_TRANSPORT_NON_SECURE;
    return mqtt_connect(&client);
}

static int publish_status(void)
{
    char payload[80];
    char topic[96];
    int len = snprintf(payload, sizeof(payload), "{\"uptime_ms\":%lld}",
                       (long long)k_uptime_get());
    snprintf(topic, sizeof(topic), "devices/%s/status", config.client_id);
    struct mqtt_publish_param msg = {
        .message.topic.qos = MQTT_QOS_0_AT_MOST_ONCE,
        .message.topic.topic.utf8 = (uint8_t *)topic,
        .message.topic.topic.size = strlen(topic),
        .message.payload.data = payload,
        .message.payload.len = len,
    };
    return mqtt_publish(&client, &msg);
}

static void run(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);
    for (;;) {
        if (wifi_manager_wait_ready(K_SECONDS(5))) {
            continue;
        }
        connected = false;
        int err = connect_broker();
        if (err) {
            LOG_WRN("MQTT connect failed: %d", err);
            k_sleep(K_SECONDS(5));
            continue;
        }
        struct zsock_pollfd fd = {
            .fd = client.transport.tcp.sock, .events = ZSOCK_POLLIN
        };
        int64_t deadline = k_uptime_get() + 10000;
        int64_t next_publish = 0;
        while (wifi_manager_is_connected()) {
            int polled = zsock_poll(&fd, 1, 1000);
            if (polled < 0 || (fd.revents &
                (ZSOCK_POLLERR | ZSOCK_POLLHUP | ZSOCK_POLLNVAL))) {
                break;
            }
            if (fd.revents & ZSOCK_POLLIN) {
                if (mqtt_input(&client)) {
                    break;
                }
            }
            if (!connected) {
                if (k_uptime_get() >= deadline) {
                    break;
                }
                continue;
            }
            err = mqtt_live(&client);
            if (err && err != -EAGAIN) {
                break;
            }
            if (k_uptime_get() >= next_publish) {
                if (publish_status()) {
                    break;
                }
                next_publish = k_uptime_get() +
                    CONFIG_APP_MQTT_PUBLISH_SECONDS * 1000;
            }
        }
        mqtt_abort(&client);
        connected = false;
        k_sleep(K_SECONDS(5));
    }
}

int mqtt_service_start(const struct app_config *cfg)
{
    if (!cfg || !cfg->broker[0] || !cfg->client_id[0]) {
        return -EINVAL;
    }
    config = *cfg;
    /* This thread exclusively owns the MQTT client and socket. */
    k_thread_create(&mqtt_thread, mqtt_stack, K_THREAD_STACK_SIZEOF(mqtt_stack),
                    run, NULL, NULL, NULL, 7, 0, K_NO_WAIT);
    return 0;
}
