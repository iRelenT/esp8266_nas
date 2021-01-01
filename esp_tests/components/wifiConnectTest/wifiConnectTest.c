
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "arpa/inet.h"
#include "wifiConnectTest.h"
#include "string.h"
#include "stdlib.h"

#define WIFI_SSID      CONFIG_WIFI_SSID       
#define WIFI_PASS      CONFIG_WIFI_PASS      
#define MAXIMUM_RETRY  CONFIG_MAXIMUM_RETRY  

static int s_retry_num = 0;



wifi_infos infos_wifi;


#define IPv4_RECEIVED BIT0

EventGroupHandle_t wifi_event_group;


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;

        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {

        ip_event_got_ip_t * event_test = (ip_event_got_ip_t*) event_data;
        infos_wifi.ipAddr = event_test->ip_info.ip.addr;
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group,IPv4_RECEIVED);
    }
}

wifi_infos * wifi_init_sta()
{   
    
    wifi_event_group = xEventGroupCreate();
    


    tcpip_adapter_init();

    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);        
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);




    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };


    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,IPv4_RECEIVED,pdFALSE,
                      pdFALSE,
                      portMAX_DELAY);
    if(bits & IPv4_RECEIVED){
    
        struct in_addr ipStruct;
        ipStruct.s_addr = (in_addr_t)infos_wifi.ipAddr;
        printf("########################################\n");
        printf("# Sucessfully connected to Wifi!...\n");
        printf("# Ip Adress we got: %s\n",inet_ntoa(ipStruct));
        printf("########################################\n\n");
    }

    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    vEventGroupDelete(wifi_event_group);
    return &infos_wifi;

}
