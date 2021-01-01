
void vdriveLeds_public();
void vdriveLeds_individual_private(void *);


/*
                ----> sdkconfig.h
            #define ...... CONFIG_EXAMPLE
               ------> put fastmodi,slow etc. in led_info struct, "modi" member should be miliseconds 
*/

//default amount of LEDS is 4 (MAX amount). Decrease it if you need less
#ifndef DEVICE_LED_ARRAY
    #define DEVICE_LED_ARRAY 4
#endif

#ifndef DEVICE_LGREEN
    #define DEVICE_LGREEN 5;
#endif

#ifndef DEVICE_LBLUE
    #define DEVICE_LBLUE 4;
#endif

#ifndef DEVICE_LRED
    #define DEVICE_LRED 14;
#endif

#ifndef DEVICE_LWHITE
    #define DEVICE_LWHITE 12;
#endif

typedef struct led_info{
    char status;
    int ledPin;
}led_info;

#define MASK(hl4,hl3,hl2,hl1) \
            ((hl4 << (DEVICE_LED_ARRAY - 1)) | (hl3 << (DEVICE_LED_ARRAY - 2)) | (hl2 << (DEVICE_LED_ARRAY - 3)) | (hl1 << (DEVICE_LED_ARRAY - 4)));

