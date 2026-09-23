# ESP32-S3 IoT Framework — Zephyr port

A Zephyr-native C application for **ESP32-S3-DevKitC**, targeting
`esp32s3_devkitc/esp32s3/procpu`. The west manifest pins **Zephyr v4.1.0**.

This branch replaces the incomplete ESP-IDF Wi-Fi skeleton. MQTT, persistent
configuration and the MCUboot update lifecycle are new implementations; they
were roadmap items in the original repository. This is a development firmware,
not a claim of production certification.

## Architecture

| Module | Zephyr implementation |
| --- | --- |
| Wi-Fi | `net_mgmt` station requests, Wi-Fi events, DHCP, dedicated delayed work queue |
| Synchronization | atomic network state, semaphore for the MQTT consumer, mutex around settings |
| MQTT | dedicated kernel thread owns a native MQTT client, socket polling, keepalive, retry and uptime publication |
| Configuration | `settings` handler backed by NVS in `storage_partition` |
| Provisioning | local UART shell; no compiled Wi-Fi credentials |
| Updates | MCUboot via sysbuild, secondary image slot, optional MCUmgr SMP over UDP |
| Logging | Zephyr logging; application logs never print the Wi-Fi password |

The MQTT thread starts only when a broker is provisioned, waits for DHCP and
reconnects after network loss. It publishes QoS 0 JSON uptime to
`devices/<client_id>/status` every 30 seconds. MQTT uses TCP port 1883 on a
trusted development LAN; TLS and broker authentication are not implemented in
this initial port. Do not expose this transport to untrusted networks.

The board's upstream flash partitions are used directly. The application overlay enables the on-chip Wi-Fi radio;
it does not change flash partitions. Compile-time assertions require both MCUboot slots and
the settings partition. For another flash size or board, verify its DTS and
MCUboot partition layout before flashing; do not assume an arbitrary ESP32-S3
module is identical to the DevKitC.

## Build

Install the Zephyr host dependencies, west, and Zephyr SDK 0.17.0 with
`xtensa-espressif_esp32s3_zephyr-elf`. Run from a new workspace:

```sh
git clone --branch zephyr-port https://github.com/ugurakas/esp32-iot-framework.git
west init -l esp32-iot-framework
west update
west zephyr-export
python -m pip install -r zephyr/scripts/requirements.txt
west blobs fetch hal_espressif
west build -b esp32s3_devkitc/esp32s3/procpu --sysbuild esp32-iot-framework -d build
west flash -d build
```

Sysbuild builds and flashes MCUboot alongside the application. Use the board's
UART connector and a 115200-baud terminal for provisioning. If automatic port
selection fails, pass the serial device to the board's flashing runner.

## Provisioning

There are no real credentials in this branch. Enter your own values on the
device's serial console:

```text
iot set ssid "<your-ssid>"
iot set password "<your-wifi-password>"
iot set broker "<broker-hostname-or-ipv4>"
iot set client_id "<unique-device-id>"
iot reboot
iot status
```

For an open access point, leave the password unset. WPA2 passwords must contain
8–63 bytes. Settings persist over reboot and take effect on the next boot.
Assign a different client ID to every device to avoid broker eviction.

The development shell echoes typed input and may retain command history.
Settings are stored in ordinary flash NVS, not encrypted storage. Physical
console access must therefore be trusted. No setting-dump command is provided.
Do not commit local credentials, signing keys or private configuration.

## OTA / MCUboot

Enable network updates explicitly:

```sh
west build -p always -b esp32s3_devkitc/esp32s3/procpu --sysbuild esp32-iot-framework -d build-ota -- -Desp32-iot-framework_EXTRA_CONF_FILE=conf/ota-udp.conf
west flash -d build-ota
```

This enables Zephyr's image and OS management groups over SMP UDP on port 1337.
**SMP UDP is unauthenticated in this development profile.** Use only an isolated,
trusted LAN; signed images do not prevent unauthorized reboot or upload attempts.

With a compatible `mcumgr` client, use the running device's IPv4 address:

```sh
mcumgr --conntype udp --connstring '<device-ip>:1337' image list
mcumgr --conntype udp --connstring '<device-ip>:1337' image upload build-ota/esp32-iot-framework/zephyr/zephyr.signed.bin
mcumgr --conntype udp --connstring '<device-ip>:1337' image list
mcumgr --conntype udp --connstring '<device-ip>:1337' image test <uploaded-image-hash>
mcumgr --conntype udp --connstring '<device-ip>:1337' reset
```

Verify Wi-Fi, MQTT and persistent settings in the new firmware, then run
`iot confirm` on the serial console. No automatic confirmation occurs: a test
image that is not confirmed rolls back on the next reset. An image is validated
by MCUboot before execution. Build successive releases with distinct versions,
for example using an application `VERSION` file; uploading the identical running
image is not an update test.

The default upstream development signing key is for testing only. Before any
deployment, configure your own MCUboot signing key outside Git and use the same
key for bootloader verification and application signing. Keep the flash layout
and signing configuration consistent across updates.

## Validation

GitHub Actions builds both the base and SMP UDP profiles, including MCUboot,
and uploads firmware plus resolved Kconfig/DTS artifacts. A successful build
does not replace tests on a physical ESP32-S3.

Hardware acceptance checklist:
1. Clean flash: boot without credentials and provision via UART.
2. Reboot: verify NVS persistence, DHCP and repeated MQTT messages.
3. Switch the AP off/on: verify reconnect without resetting the device.
4. Restart the MQTT broker: verify automatic reconnection and publication.
5. Upload a newer signed image, test-boot, confirm, and verify persistence.
6. Test-boot without confirmation, reset again, and verify rollback.
7. Reject a corrupt/wrong-key image; interrupt upload and verify the old image boots.

## Layout

```text
CMakeLists.txt / Kconfig / prj.conf / sysbuild.conf / west.yml
include/              Public module interfaces
src/                  Application and native Zephyr services
conf/ota-udp.conf      Opt-in development OTA transport
.github/workflows/    Build verification
```

License: MIT. Author: Uğur Akas.
