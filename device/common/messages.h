#ifndef ums_messages_h
#define ums_messages_h

#include <stdint.h>

/** \file
 * This file describes all the message that might be sent by the ums.
 */

#define UMS_ERROR_UNKNOWN_CMD_ID 0      ///\< Error Id for an unrecognized cmd Id.

static const uint8_t PongMsg_ID = 0;
#define PongMsg_LENGTH (sizeof(struct PongMsg))
struct PongMsg {
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

#endif
