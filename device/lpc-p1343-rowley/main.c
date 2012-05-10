/**************************************************************************/
/*! 
 @file     main.c
 @author   K. Townsend (microBuilder.eu)

 @section LICENSE

 Software License Agreement (BSD License)

 Copyright (c) 2011, microBuilder SARL
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 3. Neither the name of the copyright holders nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "projectconfig.h"
#include "sysinit.h"
#include "core/gpio/gpio.h"
#include "core/systick/systick.h"
#include "core/usbcdc/cdcuser.h"
#include "step_timer.h"

uint8_t buffer[64];
/**************************************************************************/
/*! 
 Main program entry point.  After reset, normal code execution will
 begin here.
 */
/**************************************************************************/
int main(void) {
    // Configure cpu and mandatory peripherals
    systemInit();

    step_timer_init();

    uint32_t currentSecond, lastSecond;
    currentSecond = lastSecond = 0;

    // led 0 on LPC-P1343
    gpioSetPinFunction(3, 0, GPIO_OUTPUT_PIN, 1);

    uint32_t chars = 0;
    while (1) {
        uint32_t cnt = CDC_GetOutEpBuff(buffer);
        chars += cnt;

        for (uint32_t i=0; i<cnt; i++) {
            if (buffer[i] == 'x') {
                step_timer_start(10000);
            }
        }

        // output some stuff once per second
        currentSecond = systickGetSecondsActive();
        if (currentSecond != lastSecond) {
            lastSecond = currentSecond;
            gpioSetPin(3, 0, currentSecond & 0x01);
            int len = sprintf(buffer, "hello %d\n", chars);
            NVIC_DisableIRQ(USB_IRQn);
            CDC_WriteInEp(buffer, len);
            NVIC_EnableIRQ(USB_IRQn);
        }
    }
    return 0;
}
