cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp32_iot_framework)
target_include_directories(app PRIVATE include)
target_sources(app PRIVATE src/main.c src/config_store.c src/wifi_manager.c
  src/mqtt_service.c src/ota_service.c src/provisioning.c)
