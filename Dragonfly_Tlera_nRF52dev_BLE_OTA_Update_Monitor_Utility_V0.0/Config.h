/*****************************************************/
/*************** Serial Configuration ****************/
/*****************************************************/
#define UART_NUMBER                    4                                                              // Total number of possible serial ports on the STM32L4 (including USB); differs by variant
#define OTA_DATA_BLOCK_SIZE            32                                                             // ".iap" file data block size; 32 bytes works well for the NUS UART pass-through without HW flow control

/************* BLE UART Port Selection ***************/
// Uncomment one only
#define BLE_UART_1
//#define BLE_UART_2
//#define BLE_UART_3

/************** UART Port Definitions ****************/
#ifdef BLE_UART_1
  #define STM32L4_BLE_UART               1
  #define NRF52_UART                     Serial1
#endif
#ifdef BLE_UART_2
  #define STM32L4_BLE_UART               2
  #define NRF52_UART                     Serial2
#endif
#ifdef BLE_UART_3
  #define STM32L4_BLE_UART               3
  #define NRF52_UART                     Serial3
#endif

/********** STM32L4 Board Pin Definitions ************/
#define LED_PIN                          13                                                           // Dragonfly Blue LED
#define BLE_CONNECT_PIN                  2                                                            // BLE pairing status monitor pin; Paired=Low
#define DRAGONFLY_3V3                    39
#define DRAGONFLY_GND                    40

