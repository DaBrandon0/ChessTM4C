// main.cpp
// Runs on TM4C123
// Matthew Mekha, Brandon Lui, Mauro Rodriguez

// ******* Hardware I/O connections *******************
// potentiometer 	= PD2
// back button 		= PE0
// select button 	= PE1

// DAC 						= PB5-0

// IR LED 				= PC7
// IR Receiver		= PC6

// ST7735
// LED-   (pin 16) TFT, connected to ground
// LED+   (pin 15) TFT, connected to +3.3 V
// CS     (pin 10) TFT, PA3 (SSI0Fss)
// SCL    (pin 9)  TFT, SCK  PA2 (SSI0Clk)
// SDA    (pin 8)  TFT, MOSI PA5 (SSI0Tx)
// A0     (pin 7)  TFT, Data/Command PA6 (GPIO), high for data, low for command
// RESET  (pin 6)  TFT, to PA7 (GPIO)
// VCC    (pin 2)  connected to +3.3 V
// GND    (pin 1)  connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "random.h"
#include "InputManager.h"
#include "Images.h"
#include "UART.h"
#include "Sound.h"
#include "ChessBoard.h"

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

ChessBoard chessBoard;

void init() {
	PLL_Init(Bus80MHz);
  Random_Init(1);
	Sound_Init();
  Output_Init(); // ST7735
	InputManager_Init(100);
  EnableInterrupts();
}



int main(void){
  init();
  
	while(true) {
		InputData& inputData = InputManager_GetInput();
		chessBoard.update(inputData);
		
		if(UART_CheckInbox())
			chessBoard.acceptMessage(UART_GetInbox());
		
		// TODO uncomment
		//if(chessBoard.mailboxReady)
		//	UART_TransmitAndWaitForAck(chessBoard.mailbox);
	}
}




