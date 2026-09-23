#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H
int ota_service_init(void);
/* Call only after the application's health checks have passed. */
int ota_service_confirm(void);
#endif
