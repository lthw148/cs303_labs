#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <stdio.h>
#include "SCC_Charts.h"

volatile int buttonChanged = 0;

int main(){
    TickData data;
    reset(&data);

    while(1) {
        // Fetch button inputs
        // A is Key 2, B is Key 1, R is Key 0
        // Remember that keys are ACTIVE LOW

        int buttonValue = IORD_ALTERA_AVALON_PIO_DATA(KEYS_BASE);

        // Clear previous inputs
        data.A = 0;
        data.B = 0;
        data.R = 0;

        // Update inputs
        if ((buttonValue & 0x04) == 0) { // KEY2 pressed
            data.A = 1;
        }
        if ((buttonValue & 0x02) == 0) { // KEY1 pressed
            data.B = 1;
        }
        if ((buttonValue & 0x01) == 0) { // KEY0 pressed
            data.R = 1;
        }

        // Check for value change
        if (data.A || data.B || data.R) {
            buttonChanged = 1;
        }

        // Do a tick!
        tick(&data);

        // Red LED thing (check if we need this)
        if (data.O == 1) {
            IOWR_ALTERA_AVALON_PIO_DATA(LEDS_RED_BASE, 0x01);
        } else {
            IOWR_ALTERA_AVALON_PIO_DATA(LEDS_RED_BASE, 0x00);
        }

        if (buttonChanged) {
            printf("A: %d	B: %d	R: %d	O: %d\n", data.A, data.B, data.R, data.O);
            buttonChanged = 0;
        }

        usleep(100000); // Delay so it doesn't overload
    }

    return 0;
}
