#define UMC_IMPL
#include "umc.h"
#undef UMC_IMPL

#include <stdlib.h>
#include "commands.h"
#include "stepper.h"
#include "platform.h"


const char UMC_ENABLE[] = "umc enable";
#define UMC_ENABLE_LEN (sizeof(UMC_ENABLE)-1)   // exclude trailing '\0'

uint32_t umcEnabled;

void umc_init()
{
    st_init();
    umcEnabled = 0;
}

void umc_idle()
{
    // disabled until complete enable string is received
    uint8_t nextByte;
    while (umcEnabled < UMC_ENABLE_LEN && pf_receive_byte(&nextByte) != 0) {
        if (nextByte == UMC_ENABLE[umcEnabled]) {
            ++umcEnabled;
        } else {
            umcEnabled = 0;
        }
    }

    // not disabled, normal operation
    if (umcEnabled == UMC_ENABLE_LEN) {
        // if the stepper queue is full, then we don't process any more commands
        while (!st_full()) {
            uint8_t *rx = cmd_receive();
            if (rx != NULL) {
                cmd_handler(rx);
            } else {
                break;
            }
        }
    }
}
