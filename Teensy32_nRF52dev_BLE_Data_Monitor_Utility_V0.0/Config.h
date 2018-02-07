/*****************************************************/
/*************** Serial Configuration ****************/
/*****************************************************/
#define UART_NUMBER                      4                                                            // Total number of possible serial ports
#define OTA_DATA_BLOCK_SIZE              32                                                           // FW file OTA data block size; 32 bytes works well for the NUS UART pass-through without HW flow control
#define BLE_UART_1

/************** UART Port Definitions ****************/
#ifdef BLE_UART_1
  #define TEENSY_BLE_UART                1
  #define NRF52_UART                     Serial1
#endif
#ifdef BLE_UART_2
  #define TEENSY_BLE_UART                2
  #define NRF52_UART                     Serial2
#endif
#ifdef BLE_UART_3
  #define TEENSY_BLE_UART                3
  #define NRF52_UART                     Serial3
#endif

/********* Teensy3.X Board Pin Definitions ***********/
#define LED_PIN                          13                                                           // Teensy3.X LED
#define BLE_CONNECT_PIN                  2                                                            // BLE pairing status monitor pin; Paired=Low

