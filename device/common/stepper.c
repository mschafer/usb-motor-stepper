#include "stepper.h"
#include "platform.h"
#include <stdlib.h>

typedef struct
{
	uint8_t stepPort;
	uint8_t stepPin;
	uint8_t dirPort;
	uint8_t dirPin;
	uint8_t fwdPort;
	uint8_t fwdPin;
	uint8_t revPort;
	uint8_t revPin;
} Axis_t;

struct
{
	Axis_t x;
	Axis_t y;
	Axis_t z;
	Axis_t u;
} Axes;

Axis_t *getAxis(char axisName)
{
	Axis_t *ret = NULL;
	switch (axisName) {
	case 'x':
	case 'X':
		ret = &Axes.x;
		break;
	case'y':
	case 'Y':
		ret = &Axes.y;
		break;
	case 'z':
	case 'Z':
		ret = &Axes.z;
		break;
	case 'u':
	case 'U':
		ret = &Axes.u;
		break;
	};

	return ret;
}

#define STEP_FIFO_SIZE 8
uint8_t stepFIFO[STEP_FIFO_SIZE];
uint16_t delayFIFO[STEP_FIFO_SIZE];
uint8_t stepHead, stepTail;

struct
{
    int adx, ady, adz, adu;
    int ex, ey, ez, eu;
    int maxd;
    int count;
    uint8_t dir;
    uint16_t delay;
} Line;


void st_line_setup(int dx, int dy, int dz, int du, uint16_t delay)
{
    Line.delay = delay;

    Line.adx = abs(dx);
    Line.ady = abs(dy);
    Line.adz = abs(dz);
    Line.adu = abs(du);

    Line.dir = 0;
    Line.dir |= (dx < 0) ? 0 : UMC_X_DIR;
    Line.dir |= (dy < 0) ? 0 : UMC_Y_DIR;
    Line.dir |= (dz < 0) ? 0 : UMC_Z_DIR;
    Line.dir |= (du < 0) ? 0 : UMC_U_DIR;

    Line.maxd = Line.adx;
    Line.maxd = Line.ady > Line.maxd ? Line.ady : Line.maxd;
    Line.maxd = Line.adz > Line.maxd ? Line.adz : Line.maxd;
    Line.maxd = Line.adu > Line.maxd ? Line.adu : Line.maxd;

    Line.ex = Line.ey = Line.ez = Line.eu = 0;
    Line.count = 0;
}

uint8_t st_line_next_step()
{
    // return 0 if there we are at the end of the line
    if (Line.count == Line.maxd) {
        return 0;
    }

    uint8_t step = Line.dir;

    Line.ex += 2 * Line.adx;
    if (Line.ex > Line.maxd) {
        step |= UMC_X_STEP;
        Line.ex -= 2 * Line.maxd;
    }

    Line.ey += 2 * Line.ady;
    if (Line.ey > Line.maxd) {
        step |= UMC_Y_STEP;
        Line.ey -= 2 * Line.maxd;
    }

    Line.ez += 2 * Line.adz;
    if (Line.ez > Line.maxd) {
        step |= UMC_Z_STEP;
        Line.ez -= 2 * Line.maxd;
    }

    Line.eu += 2 * Line.adu;
    if (Line.eu > Line.maxd) {
        step |= UMC_U_STEP;
        Line.eu -= 2 * Line.maxd;
    }

    Line.count++;
    return step;
}

void st_init( )
{
	// default pin configuration
	struct AxisCmd ac;
	ac.cmdId = AxisCmd_ID;
	ac.name = 'x';
	ac.stepPort = 3;
	ac.stepPin = 0;
	ac.dirPort = 3;
	ac.dirPin = 1;
	ac.fwdPort = 2;
	ac.fwdPin = 0 | UMC_INVERT_PIN;
	ac.revPort = 2;
	ac.revPin = 1 | UMC_INVERT_PIN;
	st_setup_axis(&ac);

    ac.name = 'y';
    ac.stepPort = 3;
    ac.stepPin = 2;
    ac.dirPort = 3;
    ac.dirPin = 3;
    ac.fwdPort = 2;
    ac.fwdPin = 2 | UMC_INVERT_PIN;
    ac.revPort = 2;
    ac.revPin = 3 | UMC_INVERT_PIN;
    st_setup_axis(&ac);

    ac.name = 'z';
    ac.stepPort = 2;
    ac.stepPin = 4;
    ac.dirPort = 2;
    ac.dirPin = 5;
    ac.fwdPort = 2;
    ac.fwdPin = 8 | UMC_INVERT_PIN;
    ac.revPort = 2;
    ac.revPin = 9 | UMC_INVERT_PIN;
    st_setup_axis(&ac);

    ac.name = 'u';
    ac.stepPort = 2;
    ac.stepPin = 6;
    ac.dirPort = 2;
    ac.dirPin = 7;
    ac.fwdPort = 2;
    ac.fwdPin = 10 | UMC_INVERT_PIN;
    ac.revPort = 2;
    ac.revPin = 11 | UMC_INVERT_PIN;
    st_setup_axis(&ac);

    // initialize step fifo
    stepHead = stepTail = 0;

    // no active line
    st_line_setup(0, 0, 0, 0, 1000);
}

uint8_t st_full()
{
    // return 1 if full
	if  ((stepTail == stepHead+1) ||
			(stepTail == 0 && stepHead == STEP_FIFO_SIZE-1)) {
		return 1;
	}

	// if not full, try to fill up with steps from active line
	else {
	    uint8_t lineStep;
	    while ((lineStep = st_line_next_step()) != 0) {
	        st_add_step(lineStep, Line.delay);
	        if  ((stepTail == stepHead+1) ||
	                (stepTail == 0 && stepHead == STEP_FIFO_SIZE-1)) {
	            return 1;
	        }
	    }
		return 0;
	}
}

void st_add_step(uint8_t stepDir, uint16_t delay)
{
	stepFIFO[stepHead]  = stepDir;
	delayFIFO[stepHead] = delay;
	uint8_t p = stepHead+1;
	if (p == STEP_FIFO_SIZE)
		stepHead = 0;
	else
		stepHead = p;

	// need to start the timer if it isn't already running so this step will happen
	if (pf_is_timer_running() == 0) {
		pf_set_step_timer(1);
	}
}

/**
 * Set the level on an output pin taking inversion into account.
 */
void st_set_ipin(uint8_t port, uint8_t ipin, uint8_t val)
{
	uint8_t pin = ipin & ~UMC_INVERT_PIN;
	if (pin != ipin) {
		val = (val==0) ? 1 : 0;
	}
	pf_set_port_pin(port, pin, val);
}

/**
 * Read an input pin and invert result if necessary.
 */
uint8_t st_read_ipin(uint8_t port, uint8_t ipin)
{
	uint8_t pin = ipin & ~UMC_INVERT_PIN;
	uint8_t val = pf_read_port_pin(port, pin);
	if (pin != ipin) {
		val = (val==0) ? 1 : 0;
	}
	return val;
}

#define UMC_STEP_BIT 0x01
#define UMC_DIR_BIT  0x02
/**
 * Generate a step in the specified direction on the specified axis.
 * \param stepDir bit0 is step, bit 1 is dir.
 */
void st_do_step(Axis_t *a, uint8_t stepDir)
{
	// do nothing if this axis has no step dir pins assigned
	if (a->dirPin == UMC_UNASSIGNED_PORT || a->stepPin == UMC_UNASSIGNED_PORT)
		return;

	// set direction bit first
	st_set_ipin(a->dirPort, a->dirPin, stepDir & UMC_DIR_BIT);

	if ( (stepDir & UMC_STEP_BIT) != 0) {
		// moving forward
		if (a->fwdPort != UMC_UNASSIGNED_PORT && (stepDir & UMC_DIR_BIT) != 0) {
			uint8_t limit = st_read_ipin(a->fwdPort, a->fwdPin);
			if (limit != 0) {
				// limit activated, don't step
				return;
			}
		} else if (a->revPort != UMC_UNASSIGNED_PORT) { // moving reverse
			uint8_t limit = st_read_ipin(a->revPort, a->revPin);
			if (limit != 0) {
				// limit activated, don't step
				return;
			}
		}

		st_set_ipin(a->stepPort, a->stepPin, 1);
	}
}

/**
 * Steps are edge driven so this clears the step outputs.
 */
void st_clear_steps()
{

	if (Axes.x.stepPort != UMC_UNASSIGNED_PORT)
		st_set_ipin(Axes.x.stepPort, Axes.x.stepPin, 0);
	if (Axes.y.stepPort != UMC_UNASSIGNED_PORT)
		st_set_ipin(Axes.y.stepPort, Axes.y.stepPin, 0);
	if (Axes.z.stepPort != UMC_UNASSIGNED_PORT)
		st_set_ipin(Axes.z.stepPort, Axes.z.stepPin, 0);
	if (Axes.u.stepPort != UMC_UNASSIGNED_PORT)
		st_set_ipin(Axes.u.stepPort, Axes.u.stepPin, 0);
}


void st_run_once()
{
	// nothing to do if the FIFO is empty
	if (stepTail != stepHead) {
		// read the next step from FIFO and increment tail
		uint8_t stepDir = stepFIFO[stepTail];
		pf_set_step_timer(delayFIFO[stepTail]);
		uint8_t p = stepTail + 1;
		if (p == STEP_FIFO_SIZE)
			stepTail = 0;
		else
			stepTail = p;

		st_do_step(&Axes.x, stepDir);
		st_do_step(&Axes.y, stepDir >> 2);
		st_do_step(&Axes.z, stepDir >> 4);
		st_do_step(&Axes.u, stepDir >> 6);

		// wait a little, then clear the steps
		st_clear_steps();
	}
}

void st_setup_axis(struct AxisCmd *c)
{
	Axis_t *axis = getAxis(c->name);
	axis->stepPort = c->stepPort;
	axis->stepPin  = c->stepPin;
	pf_configure_port_pin(axis->stepPort, axis->stepPin & ~UMC_INVERT_PIN, UMC_OUTPUT_PIN);

	axis->dirPort  = c->dirPort;
	axis->dirPin   = c->dirPin;
    pf_configure_port_pin(axis->stepPort, axis->stepPin & ~UMC_INVERT_PIN, UMC_OUTPUT_PIN);

    // configure the limit pins with a pullup or pulldown to be inactive when disconnected
    axis->fwdPort  = c->fwdPort;
	axis->fwdPin   = c->fwdPin;
	if (axis->fwdPin & UMC_INVERT_PIN) {
	    pf_configure_port_pin(axis->fwdPort, axis->fwdPin & ~UMC_INVERT_PIN, UMC_INPUT_PULLUP_PIN);
	} else {
        pf_configure_port_pin(axis->fwdPort, axis->fwdPin & ~UMC_INVERT_PIN, UMC_INPUT_PULLDOWN_PIN);
	}

	axis->revPort  = c->revPort;
	axis->revPin   = c->revPin;
    if (axis->revPin & UMC_INVERT_PIN) {
        pf_configure_port_pin(axis->revPort, axis->revPin & ~UMC_INVERT_PIN, UMC_INPUT_PULLUP_PIN);
    } else {
        pf_configure_port_pin(axis->revPort, axis->revPin & ~UMC_INVERT_PIN, UMC_INPUT_PULLDOWN_PIN);
    }
}
