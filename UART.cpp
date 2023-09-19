// UART.cpp

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "UART.h"

#define PF1  (*((volatile uint32_t *)0x40025008))

extern "C" void DisableInterrupts(void); // Disable interrupts
extern "C" void EnableInterrupts(void);


//extern "C" void UART3_Handler(void);

// Initialize UART3
// Baud rate is 115200 bits/sec
void UART_Init(void){
  // init GPIO
	SYSCTL_RCGCGPIO_R |= 0x4;
	volatile int delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTC_DIR_R |= 0x1 << 7;			// PC7 is LED (output)
	GPIO_PORTC_DEN_R |= 0x1 << 7;
	//GPIO_PORTC_DIR_R &= ~(0x1 << 6);
	
	// init UART
	SYSCTL_RCGCUART_R |= 0x00000008; // activate UART3 clock
  UART3_CTL_R &= ~0x00000001;	// disable UART
  UART3_IBRD_R = 2105;     
  UART3_FBRD_R = 17;	
  UART3_LCRH_R = 0x00000070;	// 8 bit, no parity bits, one stop, FIFOs
	
  GPIO_PORTC_AFSEL_R |= 0x40;    // enable alt funct on PC6
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0x00FFFFFF) | 0x11000000;
  GPIO_PORTC_AMSEL_R &= ~0x40;   // disable analog on PC6
	GPIO_PORTC_DEN_R |= 0x1 << 6;
	
  UART3_CTL_R |= 0x00000201;	// enable UART
}

static bool mailboxReady;
static uint8_t* mailbox;
static uint8_t mailboxPointer;


#define halfPeriod 86

static void transmitBit(bool bit) {
	if(bit) {
		for(uint8_t i = 0; i < 16; i++) {
			uint32_t c = halfPeriod;
			while(c) c--;
			GPIO_PORTC_DATA_R &= ~(1 << 7);
		}
	} else {
		for(uint8_t i = 0; i < 16; i++) {
			uint32_t c = halfPeriod;
			while(c) c--;
			GPIO_PORTC_DATA_R ^= 1 << 7;
		}
	}
}

static void transmit(uint8_t* message) {
	while(*message != 0xFF) {
		transmitBit(0);// start bit
		for (int i = 0; i<8; i++)
		{
			transmitBit(((*message)>>i) & 1);
		}
		transmitBit(1);// end bit
		message++;
	}
}

static bool receive(uint8_t* message) {
	// TODO	
	return false;
}

static void checkForNewData() {
	//if(UART1_FR_R & 0x10)
		return; // rx fifo is empty
	
	mailbox[mailboxPointer++] = UART3_DR_R;
	if(mailbox[mailboxPointer - 1] == 0xFF) {
		// sentinel
	}
}


void UART_TransmitAndWaitForAck(uint8_t* messageOut) {
	while(1);
	
	// while loop:
		// transmit messageOut
		// wait for ack
		// if no ack, try again
	
	transmit(messageOut);
	uint8_t* origmessage = messageOut;
		while(receive(messageOut)&& *messageOut != 0x00)
		{
			transmit(origmessage);
		}
	
	uint8_t* messageIn = new uint8_t[MAX_MESSAGE_SIZE];
	
	// while loop:
		// wait for messageIn
		// verify ack
	
}

bool UART_CheckInbox() {
	checkForNewData();
	return mailboxReady;
}

uint8_t* UART_GetInbox() {
	mailboxReady = false;
	return mailbox;
}

