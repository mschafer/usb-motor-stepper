#ifndef ums_commands_h
#define ums_commands_h

#include <stdint.h>

/** \file
 * This file lists all the commands that are recognized by the ums.
 */

/**
 * A NoOp command is received without error by the device but ignored.
 */
static const uint8_t NoOpCmd_ID = 0;
#define NoOpCmd_LENGTH (sizeof(struct NoOpCmd))
struct NoOpCmd {
	uint8_t cmdId;
};

/**
 * The device responds to a PingCmd with a PongMsg.
 */
static const uint8_t PingCmd_ID = 1;
#define PingCmd_LENGTH (sizeof(struct PingCmd))
struct PingCmd {
	uint8_t cmdId;
};


/** Invalid port number used to indicate an unassigned pin. */
#define UMS_UNASSIGNED_PORT 0xFF

/** Valid pin numbers are 0-15.  Setting this bit indicates the pin is inverted. */
#define UMS_INVERT_PIN 0x10

/**
 * IO pin descriptors consist of an 8 bit unsigned port number and an 8 bit unsigned pin number.
 * Valid pin numbers are 0-15.  Adding 16 to the pin numbers denotes inverted logic.
 * Inverted step pin generates falling edges to trigger a step.
 * Inverted dir pin generates a 0 for fwd and a 1 for rev.
 * Inverted limit pins are active low.
 * Note that port 0xFF (255) is reserved to indicate unassigned pins.
 */
static const uint8_t AxisCmd_ID = 2;
#define AxisCmd_LENGTH (sizeof(struct AxisCmd))
struct AxisCmd {
	uint8_t cmdId;
	char name;            ///< Axis designator: 'X', 'Y', 'Z', or 'U'
	uint8_t stepPort;
	uint8_t stepPin;
	uint8_t dirPort;
	uint8_t dirPin;
	uint8_t fwdPort;
	uint8_t fwdPin;
	uint8_t revPort;
	uint8_t revPin;
};

#define UMS_X_STEP 0x01     /**< setting this bit in stepDir causes the x axis to move */
#define UMS_X_DIR  0x02     /**< setting this bit in stepDir sets the x direction to fwd */
#define UMS_Y_STEP 0x04     /**< setting this bit in stepDir causes the y axis to move */
#define UMS_Y_DIR  0x08     /**< setting this bit in stepDir sets the y direction to fwd */
#define UMS_Z_STEP 0x10     /**< setting this bit in stepDir causes the z axis to move */
#define UMS_Z_DIR  0x20     /**< setting this bit in stepDir sets the z direction to fwd */
#define UMS_U_STEP 0x40     /**< setting this bit in stepDir causes the u axis to move */
#define UMS_U_DIR  0x80     /**< setting this bit in stepDir sets the u direction to fwd */

/**
 * Steps all the axes and then waits.
 * \param stepDir Each pair of bits sets the direction and whether or not a step is taken
 * for a single axis.
 * \param delay_hi Most significant byte of the 16 bit delay value.
 * \param delay_hi Least significant byte of the 16 bit delay value.
 */
static const uint8_t StepCmd_ID = 3;
#define StepCmd_LENGTH (sizeof(struct StepCmd))
struct StepCmd {
	uint8_t cmdId;
	uint8_t stepDir;
    uint8_t delay[2];
};

static const uint8_t LineCmd_ID = 4;
#define LineCmd_LENGTH (sizeof(struct LineCmd))
struct LineCmd {
    uint8_t cmdId;
    int8_t deltaX;
    int8_t deltaY;
    int8_t deltaZ;
    int8_t deltaU;
    uint8_t delay[2];
};

static const uint8_t LongLineCmd_ID = 5;
#define LongLineCmd_LENGTH (sizeof(struct LongLineCmd))
struct LongLineCmd {
    uint8_t cmdId;
    uint8_t deltaX[2];
    uint8_t deltaY[2];
    uint8_t deltaZ[2];
    uint8_t deltaU[2];
    uint8_t delay[2];
};

#define MAX_CMD_LENGTH LongLineCmd_LENGTH
struct MaxCmdBuff {
	uint8_t cmdId;
	uint8_t data[MAX_CMD_LENGTH];
};

static const uint8_t DelayCmd_ID = 6;
#define DelayCmd_LENGTH (sizeof(struct DelayCmd))
struct DelayCmd {
	uint8_t cmdId;
	uint8_t delay[4];
};

#define MAX_CMD_ID DelayCmd_ID

/** This function dispatches received commands to the appropriate handler based on the id in buff. */
void cmd_handler(uint8_t *buff);

/** Return the expected length of a command based on its Id. */
uint8_t cmd_get_length(uint8_t cmdId);

/** Parses commands out of the incoming byte stream.
 * This routine uses a static buffer to parse commands.  Any call to it overwrites
 * previously returned commands.
 * \return NULL if a complete command is not available, otherwise a pointer to valid command.
 */
uint8_t *cmd_receive();

#endif
