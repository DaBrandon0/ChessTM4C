// InputManager.cpp

#include "InputManager.h"
#include "Timer1.h"
#include "../inc/tm4c123gh6pm.h"

#define potThreshold 50

InputData inputData;

static uint32_t millisPerTick;

static bool btnBackAlreadyPressed;
static bool btnSelectAlreadyPressed;
static uint32_t lastPotPos = 9999;
static uint32_t potInterval = 1;
static uint32_t numPositions = 1;

void InputManager_Periodic() {
	inputData.timeMillis += millisPerTick;
	inputData.deltaTimeMillis += millisPerTick;
	
	bool btnBackPressed = GPIO_PORTE_DATA_R & 0x4;
	inputData.btnBackPressed |= btnBackPressed > btnBackAlreadyPressed;
	btnBackAlreadyPressed = btnBackPressed;
	
	bool btnSelectPressed = GPIO_PORTE_DATA_R & 0x2;
	inputData.btnSelectPressed |= btnSelectPressed > btnSelectAlreadyPressed;
	btnSelectAlreadyPressed = btnSelectPressed;
	
	ADC0_PSSI_R = 0x0008;
  while((ADC0_RIS_R&0x08) == 0);
	uint32_t rawPotValue = ADC0_SSFIFO3_R & 0xFFF;
  ADC0_ISC_R = 0x0008;
	
  inputData.potPos =  rawPotValue / potInterval;
	if(inputData.potPos >= numPositions)
		inputData.potPos = numPositions - 1;
	if(inputData.potPos != lastPotPos) {
		if(((rawPotValue % potInterval) < potThreshold) || ((rawPotValue % potInterval) > (potInterval - potThreshold))) {
			// this prevents flickering
			inputData.potPos = lastPotPos;
			return;
		};
		lastPotPos = inputData.potPos;
		inputData.potPosChanged = true;
	}
}

void InputManager_Init(uint32_t frequency) {	
	// init PE0 and PE1 as inputs
	SYSCTL_RCGCGPIO_R |= 0x10;
	volatile int delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_DIR_R &= ~0x6;
	GPIO_PORTE_DEN_R |= 0x6;
	
	// init PD2 as analog input
	SYSCTL_RCGCADC_R |= 0x0001;
	SYSCTL_RCGCGPIO_R |= 0x08;
	while((SYSCTL_PRGPIO_R&0x08) != 0x08) {}
	GPIO_PORTD_DIR_R &= ~0x04;  // 4) make PD2 input
  GPIO_PORTD_AFSEL_R |= 0x04; // 5) enable alternate function on PD2
  GPIO_PORTD_DEN_R &= ~0x04;  // 6) disable digital I/O on PD2
  GPIO_PORTD_AMSEL_R |= 0x04; // 7) enable analog functionality on PD2
  ADC0_PC_R &= ~0xF;
  ADC0_PC_R |= 0x1;           // 8) configure for 125K samples/sec
	ADC0_SAC_R = 4;							// 16 samples
  ADC0_SSPRI_R = 0x0123;      // 9) sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;    // 10) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;     // 11) sequence 3 is software trigger
	ADC0_SSMUX3_R &= ~0xF;
  ADC0_SSMUX3_R += 5;         // 12) set channel
  ADC0_SSCTL3_R = 0x0006;     // 13) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;       // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;			// 15) enable sample sequencer 3
	
	// init Timer1 interrupt
	millisPerTick = 1000 / frequency;
	uint32_t period = 80000000 / frequency;
	Timer1_Init(&InputManager_Periodic, period);
}

InputData& InputManager_GetInput() {
	return inputData;
}

void InputManager_SetNumPotPositions(uint32_t num) {
	numPositions = num;
	potInterval = 4096 / num;
}