// InputManager.h

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <stdint.h>

// note that the bool flags must be set to false upon reading
// this is to make sure input isn't missed

typedef struct InputData {
	
	uint32_t timeMillis;	
	uint32_t deltaTimeMillis;
	
	bool btnSelectPressed;
	bool btnBackPressed;
	
	uint32_t potPos;
	bool potPosChanged;
	
} InputData;

void InputManager_Init(uint32_t frequency);
InputData& InputManager_GetInput();
void InputManager_SetNumPotPositions(uint32_t num);

#endif