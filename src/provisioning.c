#include "config_store.h"
#include "wifi_manager.h"
#include "ota_service.h"
#include <zephyr/shell/shell.h>
#include <zephyr/sys/reboot.h>

static int set_config(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    int err = config_store_set(argv[1], argv[2]);
    if (err) {
        shell_error(sh, "Setting rejected: %d", err);
    } else {
        shell_print(sh, "Saved. Reboot to apply.");
    }
    return err;
}

static int status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc); ARG_UNUSED(argv);
    shell_print(sh, "Wi-Fi state: %d (0=down, 1=connecting, 2=IPv4 ready)",
                wifi_manager_get_state());
    return 0;
}

static int confirm(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc); ARG_UNUSED(argv);
    int err = ota_service_confirm();
    if (err) {
        shell_error(sh, "Image confirmation failed: %d", err);
    } else {
        shell_print(sh, "Running image confirmed");
    }
    return err;
}

static int reboot_device(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(sh); ARG_UNUSED(argc); ARG_UNUSED(argv);
    sys_reboot(SYS_REBOOT_COLD);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(iot_commands,
    SHELL_CMD_ARG(set, NULL, "set <ssid|password|broker|client_id> <value>",
                  set_config, 3, 0),
    SHELL_CMD_ARG(status, NULL, "Network status (no secrets)", status, 1, 0),
    SHELL_CMD_ARG(confirm, NULL, "Confirm tested MCUboot image", confirm, 1, 0),
    SHELL_CMD_ARG(reboot, NULL, "Reboot to apply settings", reboot_device, 1, 0),
    SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(iot, &iot_commands, "IoT provisioning and image lifecycle", NULL);
