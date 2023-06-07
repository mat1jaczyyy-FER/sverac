#define NAME "DominikMatijaca"
#define JMBAG "0036524568"
#define UUID 0x4568

#define clamp(x, a, b) (x < a? a : (x > b? b : x))

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"

static const char *TAG = "Lab4-BLE-Server";

#define MAX_LISTENERS 4
static volatile uint16_t listeners[MAX_LISTENERS];
static volatile int listeners_count = 0;

static void listeners_push(uint16_t conn_handle) {
    if (listeners_count >= MAX_LISTENERS)
        return;

    for (int i = 0; i < listeners_count; i++) {
        if (listeners[i] == conn_handle) {
            return;
        }
    }

    listeners[listeners_count++] = conn_handle;
}

static void listeners_remove(uint16_t conn_handle) {
    for (int i = 0; i < listeners_count; i++) {
        if (listeners[i] == conn_handle) {
            listeners[i] = listeners[--listeners_count];
            break;
        }
    }
}

static uint16_t handle_notify = 0;

static uint32_t counterBLE = 0;
static const char* readBLE = JMBAG;
static volatile uint8_t writeBLE = 1;

static int device_nothing(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    ESP_LOGI("GATT", "NOTHING");
    return 0;
}

static int device_read(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    os_mbuf_append(ctxt->om, readBLE, strlen(readBLE));
    return 0;
}

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    if (ctxt->om->om_len > 0)
        writeBLE = clamp(ctxt->om->om_data[0], 1, 10);
        
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(UUID),
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = BLE_UUID16_DECLARE(UUID + 1),
                .flags = BLE_GATT_CHR_F_NOTIFY,
                .access_cb = device_nothing,
                .val_handle = &handle_notify
            },
            {
                .uuid = BLE_UUID16_DECLARE(UUID + 2),
                .flags = BLE_GATT_CHR_F_READ,
                .access_cb = device_read
            },
            {
                .uuid = BLE_UUID16_DECLARE(UUID + 3),
                .flags = BLE_GATT_CHR_F_WRITE,
                .access_cb = device_write
            },
            {0}
        }
    },
    {0}
};

uint8_t ble_addr_type;
void ble_app_advertise(void);

// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        // Advertise if connected
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0? "OK!" : "FAILED");
            ble_app_advertise();
            break;
        
        // Advertise again after completion of the event
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI("GAP", "BLE GAP EVENT");
            ble_app_advertise();
            break;
        
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECT");
            listeners_remove(event->disconnect.conn.conn_handle);
            ble_app_advertise();
            break;
        
        case BLE_GAP_EVENT_SUBSCRIBE:
            if (event->subscribe.attr_handle == handle_notify) {
                if (event->subscribe.cur_notify) {
                    ESP_LOGI("GAP", "BLE GAP EVENT SUBSCRIBE - %d", event->subscribe.conn_handle);
                    listeners_push(event->subscribe.conn_handle);
                } else {
                    ESP_LOGI("GAP", "BLE GAP EVENT UNSUBSCRIBE - %d", event->subscribe.conn_handle);
                    listeners_remove(event->subscribe.conn_handle);
                }
            }
            break;

        default:
            break;
    }
    return 0;
}

// Define the BLE connection
void ble_app_advertise(void) {
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void) {
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

void host_task(void *param) {
    nimble_port_run();
}

void app_main() {
    nvs_flash_init();                           // 1 - Initialize NVS flash using
    esp_nimble_hci_and_controller_init();       // 2 - Initialize ESP controller
    nimble_port_init();                         // 3 - Initialize the host stack
    ble_svc_gap_device_name_set(NAME);          // 4 - Initialize NimBLE configuration - server name
    ble_svc_gap_init();                         // 4 - Initialize NimBLE configuration - gap service
    ble_svc_gatt_init();                        // 4 - Initialize NimBLE configuration - gatt service
    ble_gatts_count_cfg(gatt_svcs);             // 4 - Initialize NimBLE configuration - config gatt services
    ble_gatts_add_svcs(gatt_svcs);              // 4 - Initialize NimBLE configuration - queues gatt services
    ble_hs_cfg.sync_cb = ble_app_on_sync;       // 5 - Initialize application
    nimble_port_freertos_init(host_task);       // 6 - Run the thread

    while (1) {
        ESP_LOGI(TAG, "Counter: %d", counterBLE);

        for (int i = 0; i < listeners_count; i++) {
            ble_gattc_notify_custom(listeners[i], handle_notify,
                                    ble_hs_mbuf_from_flat(&counterBLE, sizeof(counterBLE)));
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        counterBLE += writeBLE;
    }
}
