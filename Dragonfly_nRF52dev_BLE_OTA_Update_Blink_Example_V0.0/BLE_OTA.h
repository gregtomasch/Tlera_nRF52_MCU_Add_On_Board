#ifndef OTA_h
#define OTA_h

#include "Host_WirelessSerial.h"

#define FLASHSTART                    ((uint32_t)(&__FlashBase))
#define FLASHEND                      ((uint32_t)(&__FlashLimit))
#define DATA_BLOCK_SIZE               32

extern uint8_t                        OTA_bytes[DATA_BLOCK_SIZE + 2];
extern uint32_t                       OTA_Update_Size;
extern uint16_t                       OTA_Update_Blocks;
extern uint16_t                       block;

class OTA
{
  public:
                                      OTA();
     static void                      OTA_Update(uint32_t update_size, uint16_t update_blocks);
  private:
};

#endif // OTA_h

