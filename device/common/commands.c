#include "ums.h"
#include "commands.h"
#include "messages.h"
#include "platform.h"
#include "stepper.h"
#include <stdlib.h>

uint8_t cmdBuff[MAX_CMD_LENGTH];
uint8_t rxOffset = 0;

void handle_NoOpCmd(uint8_t *cmdData)
{
}

void handle_PingCmd(uint8_t *cmdData)
{
	struct PongMsg pong;
	pong.msgId = PongMsg_ID;
	pf_send_bytes((uint8_t*)&pong, PongMsg_LENGTH);
}

void handle_AxisCmd(uint8_t *cmdData)
{
	struct AxisCmd *c = (struct AxisCmd*)cmdData;
	st_setup_axis(c);
}

void handle_StepCmd(uint8_t *cmdData)
{
	struct StepCmd *c = (struct StepCmd*)cmdData;
	uint16_t delay = UMS_UNPACK_U16(c->delay);
	st_add_step(c->stepDir, delay);
}

/**
 * This handler is used for messages that should never be received or
 * unknow messages.  If we get here, then we are probably out of sync
 * with the command stream and we must abort.  This routine will send
 * an error message and reinitialize the ums putting it back in a
 * disabled state.
 */
void handle_bad_cmd(uint8_t *cmdData)
{
    struct ErrorMsg e;
    e.msgId = ErrorMsg_ID;
    e.errorId = UMS_ERROR_UNKNOWN_CMD_ID;
    e.data[0] = cmdData[0];
    pf_send_bytes((uint8_t*)&e, ErrorMsg_LENGTH);

    // stop timer and reinitialize, back to disabled state
    pf_set_step_timer(0);
    ums_init();
}

void handle_LineCmd(uint8_t *cmdData)
{
    struct LineCmd *lcmd = (struct LineCmd *)cmdData;
    uint16_t delay = UMS_UNPACK_U16(lcmd->delay);
    st_line_setup(lcmd->deltaX, lcmd->deltaY, lcmd->deltaZ, lcmd->deltaU, delay);
    st_full();
}

void handle_LongLineCmd(uint8_t *cmdData)
{
    struct LongLineCmd *lcmd = (struct LongLineCmd *)cmdData;
    st_line_setup( UMS_UNPACK_U16(lcmd->deltaX),
    		UMS_UNPACK_I16(lcmd->deltaY),
    		UMS_UNPACK_I16(lcmd->deltaZ),
    		UMS_UNPACK_I16(lcmd->deltaU),
    		UMS_UNPACK_U32(lcmd->delay));
    st_full();
}

void handle_DelayCmd(uint8_t *cmdData)
{
	struct DelayCmd *dcmd = (struct DelayCmd *)cmdData;
	uint32_t delay = UMS_UNPACK_U32(dcmd->delay);
	st_add_step(0, delay);
}

typedef void(*cmd_handler_t)(uint8_t *cmdData);
static const cmd_handler_t handlers[] = {
        handle_NoOpCmd,
        handle_PingCmd,
        handle_AxisCmd,
        handle_StepCmd,
        handle_LineCmd,
        handle_LongLineCmd,
        handle_DelayCmd
};

static const uint8_t lengths[] = {
        NoOpCmd_LENGTH,
        PingCmd_LENGTH,
        AxisCmd_LENGTH,
        StepCmd_LENGTH,
        LineCmd_LENGTH,
        LongLineCmd_LENGTH,
        DelayCmd_LENGTH
};

void cmd_handler(uint8_t *buff)
{
    handlers[buff[0]](buff);
}

uint8_t cmd_get_length(uint8_t cmdId)
{
	return lengths[cmdId];
}

uint8_t *cmd_receive()
{
	uint8_t len;
	if (rxOffset == 0) {
		if (pf_receive_byte(cmdBuff)) {
			rxOffset = 1;
		} else {
			return NULL;
		}
	}

    if (cmdBuff[0] > MAX_CMD_ID) {
        handle_bad_cmd(cmdBuff);
    }

	len = cmd_get_length(cmdBuff[0]);
	while (rxOffset < len) {
		if (!pf_receive_byte(cmdBuff+rxOffset)) {
			return NULL;
		}
		rxOffset++;
	}
	rxOffset = 0;
	return cmdBuff;
}
