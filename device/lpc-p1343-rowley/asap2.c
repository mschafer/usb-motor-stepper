#include "ums.h"
#include "projectconfig.h"
#include "sysinit.h"
#include "core/gpio/gpio.h"
#include "core/systick/systick.h"
#include "core/usbcdc/cdcuser.h"

struct {
    void *p_umsStepCounter;
    void *p_xPos, *p_yPos, *p_zPos, *p_uPos;
} addresses = { &umsStepCounter, &umsXPos, &umsYPos, &umsZPos, &umsUPos };

void sendAddresses()
{
    NVIC_DisableIRQ(USB_IRQn);
    CDC_WriteInEp(&addresses, sizeof(addresses));
    NVIC_EnableIRQ(USB_IRQn);
}
