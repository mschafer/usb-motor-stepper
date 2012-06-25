#ifndef ums_messages_h
#define ums_messages_h

#include <stdint.h>

/** \file
 * This file describes all the message that might be sent by the ums.
 */

#define UMS_ERROR_UNKNOWN_CMD_ID 0      ///\< Error Id for an unrecognized cmd Id.
#define UMS_ERROR_CONFIGURE_PIN  1      ///\< Error Id for failure to configure a port/pin
#define UMS_ERROR_BAD_AXIS       2      ///\< Error Id for axis name that is not x,y,z, or u

/**
 * This message is sent whenever a host connection is accepted.
 */
static const uint8_t AcceptMsg_ID = 0;
#define AcceptMsg_LENGTH (sizeof(struct AcceptMsg))
struct AcceptMsg {
    uint8_t msgId;
    uint8_t majorVersion;
    uint8_t minorVersion;
};

/**
 * Message sent from the ums to indicate a fatal error.
 * The device resets itself and enters a disabled state after sending one of these.
 */
static const uint8_t ErrorMsg_ID = 1;
#define ErrorMsg_LENGTH (sizeof(struct ErrorMsg))
struct ErrorMsg {
    uint8_t msgId;
    uint8_t errorId;
    uint8_t data[2];
};


static const uint8_t WarnMsg_ID = 2;
#define WarnMsg_LENGTH (sizeof(struct WarnMsg))
struct WarnMsg {
    uint8_t msgId;
    uint8_t warnId;
    uint8_t data[2];
};

static const uint8_t DebugMsg_ID = 3;
#define DebugMsg_LENGTH (sizeof(struct DebugMsg))
struct DebugMsg {
    uint8_t msgId;
    uint8_t warnId;
    uint8_t data[2];
};

static const uint8_t StatusMsg_ID = 4;
#define StatusMsg_LENGTH (sizeof(struct StatusMsg))
struct StatusMsg {
	uint8_t msgId;
	uint8_t stepCounter_hi;
	uint8_t stepCounter_mh;
	uint8_t stepCounter_ml;
	uint8_t stepCounter_lo;
	uint8_t commandCounter_hi;
	uint8_t commandCounter_mh;
	uint8_t commandCounter_ml;
	uint8_t commandCounter_lo;
};

static const uint8_t PongMsg_ID = 5;
#define PongMsg_LENGTH (sizeof(struct PongMsg))
struct PongMsg {
	uint8_t msgId;
};

#endif
