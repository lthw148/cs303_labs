/*
Different version, hopefully at least one of them work :'D
*/

#include <stdio.h>
#include "system.h"
#include <sys/alt_alarm.h>
#include <altera_avalon_pio_regs.h>
#include "alt_types.h"
#include "altera_avalon_timer_regs.h"
#include "sys/alt_irq.h"

volatile int counter = 0;
volatile int key1_pressed = 0;
volatile int counter_changed = 0;

void timer_isr(void* context) {
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);
    if (key1_pressed) {
        counter++;
        counter_changed = 1;
    }
}

void key0_isr(void* context) {
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSHBUTTON_BASE, 0x1);
    counter++;
    counter_changed = 1;
}

void key1_isr(void* context) {
    int buttons = IORD_ALTERA_AVALON_PIO_DATA(PUSHBUTTON_BASE);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSHBUTTON_BASE, 0x2);
    
    if (buttons & 0x2) {
        key1_pressed = 1;
    } else {
        key1_pressed = 0;
    }
}

int main() {
    alt_alarm alarm;
    int timeCountMain = 0;
    void* timerContext = (void*) &timeCountMain;
    alt_alarm_start(&alarm, 500, timer_isr, timerContext);

    // Set interrupt specifically for KEY0
    alt_ic_isr_register(PUSHBUTTON_IRQ_INTERRUPT_CONTROLLER_ID, PUSHBUTTON_IRQ, key0_isr, NULL, NULL);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSHBUTTON_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PUSHBUTTON_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_IRQ_EDGE_CAP(PUSHBUTTON_BASE, 0x1);

    // Set interrupt specifically for KEY1
    alt_ic_isr_register(PUSHBUTTON_IRQ_INTERRUPT_CONTROLLER_ID, PUSHBUTTON_IRQ, key1_isr, NULL, NULL);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSHBUTTON_BASE, 0x2);
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PUSHBUTTON_BASE, 0x2);
    IOWR_ALTERA_AVALON_PIO_IRQ_EDGE_CAP(PUSHBUTTON_BASE, 0x2);

    alt_ic_isr_register(TIMER_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_IRQ, timer_isr, NULL, NULL);

    while (1) {
        if (counter_changed) {
            printf("Counter: %d\n", counter);
            counter_changed = 0;
        }
        // usleep(1000000); // Optional delay
    }
}
