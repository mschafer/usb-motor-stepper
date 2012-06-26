#define UMS_IMPL
#include "ums.h"
#undef UMS_IMPL

#include <stdlib.h>
#include "commands.h"
#include "messages.h"
#include "stepper.h"
#include "platform.h"

const char UMS_ENABLE[] = "ums enable";
#define UMS_ENABLE_LEN (sizeof(UMS_ENABLE)-1)   // exclude trailing '\0'

uint32_t umsEnabled;

uint32_t umsCommandCounter;

void ums_init()
{
    st_init();
    umsEnabled = 0;
    umsCommandCounter = 0;
}

void ums_send_status()
{
	struct StatusMsg sm;
	sm.msgId = StatusMsg_ID;
	sm.commandCounter_hi = (umsCommandCounter >> 24) & 0xFF;
	sm.commandCounter_hm = (umsCommandCounter >> 16) & 0xFF;
	sm.commandCounter_lm = (umsCommandCounter >> 8)  & 0xFF;
	sm.commandCounter_lo =  umsCommandCounter        & 0xFF;
	sm.stepCounter_hi = (umsStepCounter >> 24) & 0xFF;
	sm.stepCounter_hm = (umsStepCounter >> 16) & 0xFF;
	sm.stepCounter_lm = (umsStepCounter >> 8)  & 0xFF;
	sm.stepCounter_lo =  umsStepCounter        & 0xFF;
	pf_send_bytes((uint8_t*)&sm, StatusMsg_LENGTH);
}

void ums_send_accept()
{
	struct AcceptMsg am;
	am.msgId = AcceptMsg_ID;
	am.majorVersion = UMS_MAJOR_VERSION;
	am.minorVersion = UMS_MINOR_VERSION;
	pf_send_bytes((uint8_t*)&am, AcceptMsg_LENGTH);
}

void ums_idle()
{
    // disabled until complete enable string is received
    uint8_t nextByte;
    while (umsEnabled < UMS_ENABLE_LEN && pf_receive_byte(&nextByte) != 0) {
        if (nextByte == UMS_ENABLE[umsEnabled]) {
            ++umsEnabled;
            if (umsEnabled == UMS_ENABLE_LEN) {
            	ums_send_accept();
            }
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
            	umsCommandCounter++;
                cmd_handler(rx);
            } else {
                break;
            }
        }
    }
}
