#define UMS_IMPL
#include "ums.h"
#undef UMS_IMPL

#include <stdlib.h>
#include "commands.h"
#include "stepper.h"
#include "platform.h"


const char UMS_ENABLE[] = "ums enable";
#define UMS_ENABLE_LEN (sizeof(UMS_ENABLE)-1)   // exclude trailing '\0'

uint32_t umsEnabled;

void ums_init()
{
    st_init();
    umsEnabled = 0;
}

void ums_idle()
{
    // disabled until complete enable string is received
    uint8_t nextByte;
    while (umsEnabled < UMS_ENABLE_LEN && pf_receive_byte(&nextByte) != 0) {
        if (nextByte == UMS_ENABLE[umsEnabled]) {
            ++umsEnabled;
        } else {
            umsEnabled = 0;
        }
    }

    // not disabled, normal operation
    if (umsEnabled == UMS_ENABLE_LEN) {
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
