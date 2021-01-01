
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "led.h"

#include "esp_libc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static gpio_config_t cfg;
static led_info * leds[DEVICE_LED_ARRAY];


static inline void init_leds_struct(){
     int i;
    
     for(i = 0;i < DEVICE_LED_ARRAY;i++){
         leds[i] = os_malloc(sizeof(led_info));
     }
}



static inline void set_gpio_config(gpio_mode_t mode,gpio_int_type_t type,gpio_pullup_t pU,gpio_pulldown_t pD){
    cfg.mode = mode;
    cfg.intr_type = type;
    cfg.pull_up_en = pU;
    cfg.pull_down_en = pD;
}


static inline void setHardwareMask(){
    int i = 0; 
    
    while(i < DEVICE_LED_ARRAY){ 

        if(leds[i]->status != 0){
            cfg.pin_bit_mask = cfg.pin_bit_mask | (leds[i]->status << leds[i]->ledPin);
        }
        i++;
    }
}
static inline void set_defaultLInfo(int sequence){
    int i,j = 1;

    for(i = 0;i < DEVICE_LED_ARRAY;i++){
        
         switch(i){

             //improve with enumeration?
             case 0:
                    leds[i]->ledPin = DEVICE_LGREEN;
                    break;
             case 1:
                    leds[i]->ledPin = DEVICE_LBLUE;
                    break;
             case 2:
                    leds[i]->ledPin = DEVICE_LRED;
                    break;
             case 3:
                    leds[i]->ledPin = DEVICE_LWHITE;
                    break;
         }

        
        leds[i]->status = (((j) & sequence) == (j) ? 1:0);

        j *=2;
    }
}

void vdriveLeds_public(void *pvParameter){
    init_leds_struct();
    int sequence = MASK(0,1,0,1);

    int i;
    set_defaultLInfo(sequence);
    set_gpio_config(GPIO_MODE_OUTPUT,GPIO_INTR_DISABLE,GPIO_PULLUP_DISABLE,GPIO_PULLDOWN_DISABLE);
    setHardwareMask();
    
    ESP_ERROR_CHECK(gpio_config(&cfg));
    
    

    for(i=0;i < DEVICE_LED_ARRAY;i++){
        if(leds[i]->status != 0){
            xTaskCreate(vdriveLeds_individual_private,"driveLedX",200,(led_info*)leds[i],5,NULL);
        }
    }
    vTaskDelete(NULL);
    
}

// try calculating how much bytes this task needs! do functions get copied when invoked?
void vdriveLeds_individual_private(void *pvParameters){
    led_info *currLed = (led_info*)pvParameters;

    for(;;){
        vTaskDelay(pdMS_TO_TICKS(150));

        gpio_set_level(currLed->ledPin,1);
            
        vTaskDelay(pdMS_TO_TICKS(150));
        gpio_set_level(currLed->ledPin,0);
    }
    vTaskDelete(NULL);
}