//for sockets
#include "esp_netif.h"
#include <lwip/inet.h>
#include "inttypes.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/inet.h"

//freeRtos
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include "wifiConnectTest.h"
#include "led.h"

//Other
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"



//GPIO



//BSD socket style activated through defining LWIP Socket 1
#ifndef LWIP_SOCKET
    #define LWIP_SOCKET 1
#endif
#define MAXCONNECTIONS 5
#define PORT 1024

int fd,connectionFd = -1;


static const char *TAG = "serverConnected";

wifi_infos * infoWifi;
struct sockaddr_in test;




void init(){
    struct in_addr ipStruct;

    fd = socket(AF_INET,SOCK_STREAM,0);
    printf("#####################################\n"); 
    printf("# Socket created with %d number\n",fd);
    printf("#####################################\n\n");

    ipStruct.s_addr = (in_addr_t)infoWifi->ipAddr;

    test.sin_family = AF_INET;
    test.sin_port = htons(PORT);
    test.sin_addr.s_addr = infoWifi->ipAddr;

    memset(test.sin_zero,0,8);
        
    int err = bind(fd,(struct sockaddr *)&test,sizeof(test));
    if(err >= 0){
        printf("#####################################\n");
        printf("# Successfully binded socket! :)\n");
        printf("# Socket Ip Adress: %s\n",inet_ntoa(ipStruct));
        printf("# Port: %d\n",PORT);
        printf("#####################################\n");
    }
    
}

void networking(void*pvParameters){
    
    init();
    
    
    socklen_t length;
    struct sockaddr_in clientInfos;
    
    int err = lwip_listen(fd,MAXCONNECTIONS);
    
    for(;;){
        
        length = sizeof(clientInfos);
        ESP_LOGI(TAG,"ist minus 1");

        //    hier ist das problem
        // connectionFd = accept(fd,(struct sockaddr *) &clientInfos,&length);
        //    hier ist das problem 



        if(connectionFd != -1){
            ESP_LOGI(TAG,"# Connected to IP: %s\n",inet_ntoa(clientInfos.sin_addr));
            ESP_LOGI(TAG,"# Connection Established\n");
            ESP_LOGI(TAG,"# Port of Client : %d\n",clientInfos.sin_port);

            
        }
        ESP_LOGI(TAG,"ist minus 1");
    }
    //vTaskDelete(NULL);
}



void app_main(void)
{   
    ESP_ERROR_CHECK(nvs_flash_init());
    //infoWifi = wifi_init_sta();
    
    //xTaskCreate(networking,"tcp_ip",2048,NULL,6,NULL);
    //xTaskCreate(xblinkLed,"lightled",2048,NULL,1,NULL);
    xTaskCreate(vdriveLeds_public,"ss",2048,NULL,5,NULL);
    vdriveLeds_public();
}
