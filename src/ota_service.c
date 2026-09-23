#include "ota_service.h"
#include <zephyr/dfu/mcuboot.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/util.h>
#ifdef CONFIG_MCUMGR_TRANSPORT_UDP
#include <zephyr/mgmt/mcumgr/transport/smp_udp.h>
#endif

/* Fail at build time if the target cannot stage an update and retain settings. */
BUILD_ASSERT(FIXED_PARTITION_EXISTS(slot0_partition), "Missing primary image slot");
BUILD_ASSERT(FIXED_PARTITION_EXISTS(slot1_partition), "Missing secondary image slot");
BUILD_ASSERT(FIXED_PARTITION_EXISTS(storage_partition), "Missing settings partition");

int ota_service_init(void)
{
#ifdef CONFIG_MCUMGR_TRANSPORT_UDP
    return smp_udp_open();
#else
    return 0;
#endif
}

int ota_service_confirm(void)
{
    return boot_is_img_confirmed() ? 0 : boot_write_img_confirmed();
}
