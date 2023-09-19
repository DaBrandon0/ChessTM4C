// DAC.cpp

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "DAC.h"

void DAC_Init() {
	SYSCTL_RCGCGPIO_R |= 0x2;
	volatile int delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTB_DIR_R |= 0b111111;
	GPIO_PORTB_DEN_R |= 0b111111;
}

void DAC_Out(uint8_t value) {
	GPIO_PORTB_DATA_R &= ~0b111111;
	GPIO_PORTB_DATA_R |= value & 0b111111;
}
