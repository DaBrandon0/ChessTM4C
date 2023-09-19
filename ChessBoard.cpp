// ChessBoard.cpp

#include "ChessBoard.h"

#include "ST7735.h"
#include "Sound.h"

ChessBoard::ChessBoard() {
	languageSelected = false;
}



// private methods

void ChessBoard::start() {
	for(uint8_t x = 0; x < 8; x++) {
		for(uint8_t y = 0; y < 8; y++) {
			this->board[y][x] = (ChessPiece){(y > 3), initialState[y][x]};
		}
	}
		
	areWeWhite = true;
	isOurTurn = areWeWhite;
	isPieceChosen = false;
	isMovementChosen = false;
	isPromotionChosen = false;
	gameOver = false;
	turnState = NORMAL;
	ourTimer = StartTime;
	theirTimer = StartTime;
	
	calculatePossiblePieces();
	InputManager_SetNumPotPositions(numOptions);
}

uint16_t ChessBoard::getSquareColor(uint8_t x, uint8_t y) {
	bool isDarkSquare = ((x + y) % 2);
	
	if(isOurTurn) {
		if(isPieceChosen && !isMovementChosen && x == chosenPieceX && y == chosenPieceY) {
			return SquareSelectedColor;
		}
		for(uint8_t i = 0; i < numOptions; i++) {
			uint8_t option = options[i];
			if(((option >> 3) == y) && ((option & 7) == x)) {
				if(i == focusedOption && (blinkTimer % SquareBlinkPeriod < (SquareBlinkPeriod / 2))) {
					return SquareFocusColor;
				} else {
					return isDarkSquare ? DarkSquareHighlightColor : LightSquareHighlightColor;
				}
			}
		}
		if((board[y][x].type == KING) && (board[y][x].isWhite == areWeWhite)) {
			if(turnState == CHECK) return CheckSquareColor;
			if(turnState == CHECKMATE) return CheckmateSquareColor;
		}
	}
	
	return isDarkSquare ? DarkSquareColor : LightSquareColor;
}

void ChessBoard::drawSquare(uint8_t x, uint8_t y) {
	ChessPiece piece = this->board[y][x];
	uint16_t squareColor = getSquareColor(x, y);
	uint16_t pieceColor = piece.isWhite ? WhitePieceColor : BlackPieceColor;
	
	// render 180 degrees if white
	if(areWeWhite) {
		x = 7 - x;
		y = 7 - y;
	}
	
	ST7735_DrawMaskedShape(x*16 + 1, y*16 + 14, ChessGraphics[piece.type], pieceColor, squareColor, 14, 14);
	ST7735_DrawFastHLine(x*16, y*16, 16, squareColor);
	ST7735_DrawFastHLine(x*16, y*16 + 15, 16, squareColor);
	ST7735_DrawFastVLine(x*16, y*16 + 1, 14, squareColor);
	ST7735_DrawFastVLine(x*16 + 15, y*16 + 1, 14, squareColor);
}

void ChessBoard::addOption(uint8_t x, uint8_t y) {
	options[numOptions++] = (y << 3) | x;
}

void ChessBoard::sortOptions() {
	for(uint8_t i = numOptions; i > 1; i--) {
		for(uint8_t j = 1; j < i; j++) {
			uint8_t o1 = options[j-1];
			uint8_t o2 = options[j];
			uint8_t c1 = (o1 >> 3) + ((o1 & 7) * 8);
			uint8_t c2 = (o2 >> 3) + ((o2 & 7) * 8);
			if((c1 < c2) ^ areWeWhite) {
				options[j-1] = o2;
				options[j] = o1;
			}
		}
	}
}

void ChessBoard::calculatePossiblePieces() {
	uint8_t pieceOptions[16];
	uint8_t numPieceOptions = 0;
	for(uint8_t x = 0; x < 8; x++) {
		for(uint8_t y = 0; y < 8; y++) {
			if(board[y][x].isWhite == areWeWhite) {
				calculatePossibleMoves(x, y, true);
				if(numOptions > 0) {
					pieceOptions[numPieceOptions] = (y << 3) | x;
					numPieceOptions++;
				}
			}
		}
	}
	for(uint8_t i = 0; i < numPieceOptions; i++) {
		options[i] = pieceOptions[i];
	}
	numOptions = numPieceOptions;
	sortOptions();
}

bool ChessBoard::isKingInCheck(bool isKingWhite) {
	uint8_t tempOptions[64];
	uint8_t tempNumOptions = numOptions;
	for(uint8_t i = 0; i < numOptions; i++)
		tempOptions[i] = options[i];
	
	bool foundKing = false;
	uint8_t kingPos = 0;
	for(uint8_t x = 0; x < 8; x++) {
		for(uint8_t y = 0; y < 8; y++) {
			if((board[y][x].type == KING) && (board[y][x].isWhite == isKingWhite)) {
				kingPos = (y << 3) | x;
				foundKing = true;
				break;
			}
		}
		if(foundKing) break;
	}
	
	bool tempAreWeWhite = areWeWhite;
	areWeWhite = !areWeWhite;
	
	bool result = false;
	
	for(uint8_t x = 0; x < 8; x++) {
		for(uint8_t y = 0; y < 8; y++) {
			if(board[y][x].isWhite == areWeWhite) {
				calculatePossibleMoves(x, y, false);
				for(uint8_t i = 0; i < numOptions; i++) {
					result = (options[i] == kingPos);
					if(result) break;
				}
			}
			if(result) break;
		}
		if(result) break;
	}
	
	areWeWhite = tempAreWeWhite;
	for(uint8_t i = 0; i < tempNumOptions; i++)
		options[i] = tempOptions[i];
	numOptions = tempNumOptions;
	
	return result;
}

bool ChessBoard::wouldMoveCauseCheck(uint8_t oldX, uint8_t oldY, uint8_t newX, uint8_t newY, bool isKingWhite) {
	ChessPiece tempPiece = board[newY][newX];
	board[newY][newX] = board[oldY][oldX];
	board[oldY][oldX].type = NONE;
	
	bool result = isKingInCheck(isKingWhite);
	
	board[oldY][oldX] = board[newY][newX];
	board[newY][newX] = tempPiece;
	
	return result;
}

void ChessBoard::calculatePossibleMoves(uint8_t x, uint8_t y, bool checkForCheck) {
	ChessPieceType piece = board[y][x].type;
	numOptions = 0;
	
	switch(piece) {
		case NONE:
			break;
		
		
		
		case PAWN:
			if(areWeWhite) { // TODO en passant
				if(board[y-1][x].type == NONE) {
					addOption(x, y-1);
					if((y == 6) && board[y-2][x].type == NONE) {
						addOption(x, y-2);
					}
				}
				if(x > 0 && (board[y-1][x-1].type != NONE) && (board[y-1][x-1].isWhite != areWeWhite)) {
					addOption(x-1, y-1);
				}
				if(x < 7 && (board[y-1][x+1].type != NONE) && (board[y-1][x+1].isWhite != areWeWhite)) {
					addOption(x+1, y-1);
				}
			} else {
				if(board[y+1][x].type == NONE) {
					addOption(x, y+1);
					if((y == 1) && board[y+2][x].type == NONE) {
						addOption(x, y+2);
					}
				}
				if(x > 0 && (board[y+1][x-1].type != NONE) && (board[y+1][x-1].isWhite != areWeWhite)) {
					addOption(x-1, y+1);
				}
				if(x < 7 && (board[y+1][x+1].type != NONE) && (board[y+1][x+1].isWhite != areWeWhite)) {
					addOption(x+1, y+1);
				}
			}
			break;
		
		
			
		case BISHOP:
			for (uint8_t i = 1, j = 1; (x+i < 8) && (y+j < 8); i++,j++) {
				if(board[y+j][x+i].type == NONE) {
					addOption(x+i, y+j);
				} else {
					if(board[y+j][x+i].isWhite != areWeWhite) {
						addOption(x+i, y+j);
					}
					break;
				}
			}
			for (uint8_t i = 1, j = 1; (x+i < 8) && (y-j >= 0); i++,j++) {
				if(board[y-j][x+i].type == NONE) {
					addOption(x+i, y-j);
				} else {
					if(board[y-j][x+i].isWhite != areWeWhite) {
						addOption(x+i, y-j);
					}
					break;
				}
			}
			for (uint8_t i = 1, j = 1; (x-i >= 0) && (y+j < 8); i++,j++) {
				if(board[y+j][x-i].type == NONE) {
					addOption(x-i, y+j);
				} else {
					if(board[y+j][x-i].isWhite != areWeWhite) {
						addOption(x-i, y+j);
					}
					break;
				}
			}
			for (uint8_t i = 1, j = 1; (x-i >= 0) && (y-j >= 0); i++,j++) {
				if(board[y-j][x-i].type == NONE) {
					addOption(x-i, y-j);
				} else {
					if(board[y-j][x-i].isWhite != areWeWhite) {
						addOption(x-i, y-j);
					}
					break;
				}
			}
			break;
		
		
			
		case KNIGHT:
			for(uint8_t i = 0; i < 8; i++) {
				int8_t x1 = ((int8_t)x) + knightX[i];
				int8_t y1 = ((int8_t)y) + knightY[i];
				if(
					x1 <= 7 && x1 >= 0 && y1 <= 7 && y1 >= 0
					&& ((board[y1][x1].type == NONE) || (board[y1][x1].isWhite != areWeWhite))
				) {
					addOption(x1, y1);
				}
			}
			break;
		
		
		
		case ROOK:  
			for( int i = 1 ;y+i<8 ; i++){
				if(board[y+i][x].type == NONE){
					addOption(x, y+i);
				} else {
					if(board[y+i][x].isWhite != areWeWhite){
						addOption(x, y+i);
					}
					break;
				}
			}
			for( int i = 1 ;y-i>=0 ; i++){
				if(board[y-i][x].type == NONE){
					addOption(x, y-i);
				} else {
					if(board[y-i][x].isWhite != areWeWhite){
						addOption(x, y-i);
					}
					break;
				}
			}
			for( int i = 1 ;x+i<8 ; i++){
				if(board[y][x+i].type == NONE){
					addOption(x+i, y);
				} else {
					if(board[y][x+i].isWhite != areWeWhite){
						addOption(x+i, y);
					}
					break;
				}
			}		
			for( int i = 1 ;x-i>=0 ; i++){
				if(board[y][x-i].type == NONE){
					addOption(x-i, y);
				} else {
					if(board[y][x-i].isWhite != areWeWhite){
						addOption(x-i, y);
					}
					break;
				}
			}
      break;
		
		
		
		case QUEEN:
			for (uint8_t i = 1, j = 1; (x+i < 8) && (y+j < 8); i++,j++) {
				if(board[y+j][x+i].type == NONE) {
					addOption(x+i, y+j);
				} else {
					if(board[y+j][x+i].isWhite != areWeWhite) {
						addOption(x+i, y+j);
					}
					break;
				}
			}
			for (uint8_t i = 1, j = 1; (x+i < 8) && (y-j >= 0); i++,j++) {
				if(board[y-j][x+i].type == NONE) {
					addOption(x+i, y-j);
				} else {
					if(board[y-j][x+i].isWhite != areWeWhite) {
						addOption(x+i, y-j);
					}
					break;
				}
			}
			for (uint8_t i = 1, j = 1; (x-i >= 0) && (y+j < 8); i++,j++) {
				if(board[y+j][x-i].type == NONE) {
					addOption(x-i, y+j);
				} else {
					if(board[y+j][x-i].isWhite != areWeWhite) {
						addOption(x-i, y+j);
					}
					break;
				}
			}
			for (uint8_t i = 1, j = 1; (x-i >= 0) && (y-j >= 0); i++,j++) {
				if(board[y-j][x-i].type == NONE) {
					addOption(x-i, y-j);
				} else {
					if(board[y-j][x-i].isWhite != areWeWhite) {
						addOption(x-i, y-j);
					}
					break;
				}
			}
			for( int i = 1 ;y+i<8 ; i++){
				if(board[y+i][x].type == NONE){
					addOption(x, y+i);
				} else {
					if(board[y+i][x].isWhite != areWeWhite){
						addOption(x, y+i);
					}
					break;
				}
			}
			for( int i = 1 ;y-i>=0 ; i++){
				if(board[y-i][x].type == NONE){
					addOption(x, y-i);
				} else {
					if(board[y-i][x].isWhite != areWeWhite){
						addOption(x, y-i);
					}
					break;
				}
			}
			for( int i = 1 ;x+i<8 ; i++){
				if(board[y][x+i].type == NONE){
					addOption(x+i, y);
				} else {
					if(board[y][x+i].isWhite != areWeWhite){
						addOption(x+i, y);
					}
					break;
				}
			}		
			for( int i = 1 ;x-i>=0 ; i++){
				if(board[y][x-i].type == NONE){
					addOption(x-i, y);
				} else {
					if(board[y][x-i].isWhite != areWeWhite){
						addOption(x-i, y);
					}
					break;
				}
			}
      break;
		
		
		
		case KING:
			for(uint8_t i = 0; i < 8; i++) {
				int8_t x1 = ((int8_t)x) + kingX[i];
				int8_t y1 = ((int8_t)y) + kingY[i];
				if(
					x1 <= 7 && x1 >= 0 && y1 <= 7 && y1 >= 0
					&& ((board[y1][x1].type == NONE) || (board[y1][x1].isWhite != areWeWhite))
				) {
					addOption(x1, y1);
				}
			}
			break;
	}
	
	
	// remove all moves that would put us in check
	if(checkForCheck) {
		uint8_t fastPointer = 0;
		uint8_t slowPointer = 0;
		for(; fastPointer < numOptions; fastPointer++) {
			if(!wouldMoveCauseCheck(x, y, options[fastPointer] & 7, options[fastPointer] >> 3, areWeWhite)) {
				options[slowPointer] = options[fastPointer];
				slowPointer++;
			}
		}
		numOptions = slowPointer;
	}
}

void ChessBoard::completeTurn() {
	uint8_t mailboxI = 0;
	mailbox[mailboxI++] = (chosenPieceY << 3) | chosenPieceX;
	mailbox[mailboxI++] = (chosenMoveY << 3) | chosenMoveX;
	if(isPromotionChosen) mailbox[mailboxI++] = chosenPromotion;
	mailbox[mailboxI++] = 0xFF;
	mailboxReady = true;
	
	isPieceChosen = false;
	isMovementChosen = false;
	isPromotionChosen = false;

	// TODO temp
	acceptMessage(nullptr);
}




// public methods

void ChessBoard::drawAll() {
	for(uint8_t x = 0; x < 8; x++) {
		for(uint8_t y = 0; y < 8; y++) {
			drawSquare(x, y);
		}
	}
}

void ChessBoard::update(InputData& inputData) {
	uint32_t deltaTime = inputData.deltaTimeMillis;
	inputData.deltaTimeMillis = 0;
	
	blinkTimer += deltaTime;
	
	
	
	// language select
	if(!languageSelected) {
		InputManager_SetNumPotPositions(2);
		
		Language lastDisplayLanguage = (((blinkTimer - deltaTime) / 3000) % 2) ? Spanish : English;
		Language currentDisplayLanguage = ((blinkTimer / 3000) % 2) ? Spanish : English;
		
		if((blinkTimer == deltaTime) || (currentDisplayLanguage != lastDisplayLanguage) || (inputData.potPosChanged)) {
			inputData.potPosChanged = false;
			
			Sound_Click();
			
			// redraw
			ST7735_FillScreen(0);
			
			ST7735_SetCursor(0, 0);
			ST7735_SetTextColor(TextColorDefault);
			ST7735_OutString(GetTranslatedString(String_SelectLanguage, currentDisplayLanguage));
			
			ST7735_SetCursor(1, 2);
			ST7735_SetTextColor(inputData.potPos ? TextColorDefault : TextColorAccent);
			ST7735_OutString(GetTranslatedString(String_LanguageName, English));
			
			ST7735_SetCursor(1, 3);
			ST7735_SetTextColor(inputData.potPos ? TextColorAccent : TextColorDefault);
			ST7735_OutString(GetTranslatedString(String_LanguageName, Spanish));
		}
		
		
		if(inputData.btnSelectPressed) {
			inputData.btnSelectPressed = false;
			
			selectedLanguage = (inputData.potPos) ? Spanish : English;
			ST7735_SetTextColor(TextColorDefault);
			languageSelected = true;
			
			start();
			
			ST7735_FillScreen(0);
			drawAll();
		}
		
		return;
	}
	
	
	
	// game over
	if(gameOver) {
		if(inputData.btnSelectPressed || inputData.btnBackPressed) {
			inputData.btnSelectPressed = false;
			inputData.btnBackPressed = false;
			
			start();
			drawAll();
		}
		return;
	}
	
	
	
	// redraw timers if necessary
	
	if(isOurTurn){
		ourTimer -= deltaTime;
		if((ourTimer/1000) < ((ourTimer + deltaTime)/1000)){
			//redraw (TODO)
		}
	}else{
		theirTimer -= deltaTime; 
		if((theirTimer/1000) < ((theirTimer + deltaTime)/1000)){
			//redraw (TODO)
		}
	}
	
	
	
	
	// process input
	
	if(!isOurTurn) return;
	
	if(inputData.btnSelectPressed) {
		inputData.btnSelectPressed = false;
		
		if(isPieceChosen) {
			if(isMovementChosen) {
				// promotion complete
				
				chosenPromotion = promotionOptions[inputData.potPos];
				isPromotionChosen = true;
				
				Sound_Promotion();
				
				completeTurn();
			} else {
				isMovementChosen = true;
				chosenMoveX = options[focusedOption] & 7;
				chosenMoveY = options[focusedOption] >> 3;
				
				// move piece
				if(board[chosenMoveY][chosenMoveX].type == NONE) {
					Sound_Move();
				} else {
					Sound_Capture();
				}
				board[chosenMoveY][chosenMoveX] = board[chosenPieceY][chosenPieceX];
				board[chosenPieceY][chosenPieceX].type = NONE;
				
				// pawn ready for promotion
				if(chosenMoveY == (!areWeWhite * 7) && board[chosenMoveY][chosenMoveX].type == PAWN) {
					uint8_t temp = numOptions;
					numOptions = 0;
					for(uint8_t i = 0; i < temp; i++) {
						drawSquare(options[i] & 7, options[i] >> 3);
					}
					
					options[0] = options[focusedOption];
					numOptions = 1;
					focusedOption = 0;
					InputManager_SetNumPotPositions(4);
					
					// init promotion option
					board[chosenMoveY][chosenMoveX].type = promotionOptions[inputData.potPos];
					
					// redraw
					drawSquare(chosenPieceX, chosenPieceY);
					drawSquare(chosenMoveX, chosenMoveY);
				} else {
					numOptions = 0;
					
					// redraw
					drawSquare(chosenPieceX, chosenPieceY);
					drawSquare(chosenMoveX, chosenMoveY);
					
					completeTurn();
				}
			}
		} else {
			isPieceChosen = true;
			chosenPieceX = options[focusedOption] & 7;
			chosenPieceY = options[focusedOption] >> 3;
			
			// redraw previously highlighted options
			uint8_t temp = numOptions;
			numOptions = 0;
			for(uint8_t i = 0; i < temp; i++) {
				drawSquare(options[i] & 7, options[i] >> 3);
			}
			
			// calculate movement options for that piece
			calculatePossibleMoves(chosenPieceX, chosenPieceY, true);
			sortOptions();
			InputManager_SetNumPotPositions(numOptions);
			
			// redraw new options
			for(uint8_t i = 0; i < numOptions; i++) {
				drawSquare(options[i] & 7, options[i] >> 3);
			}
		}
	}
	
	if (inputData.btnBackPressed) {
		inputData.btnBackPressed = false;
		
		if(isPieceChosen && !isMovementChosen) {
			isPieceChosen = false;
			
			// redraw previously highlighted options
			uint8_t temp = numOptions;
			numOptions = 0;
			for(uint8_t i = 0; i < temp; i++) {
				drawSquare(options[i] & 7, options[i] >> 3);
			}
			
			// recalculate
			calculatePossiblePieces();
			InputManager_SetNumPotPositions(numOptions);
			
			// redraw new options
			for(uint8_t i = 0; i < numOptions; i++) {
				drawSquare(options[i] & 7, options[i] >> 3);
			}
		}
	}
	
	if (inputData.potPosChanged) {
		inputData.potPosChanged = false;
		blinkTimer = 0;
		
		Sound_Click();
		
		if(isMovementChosen) {
			// choosing pawn promotion
			board[chosenMoveY][chosenMoveX].type = promotionOptions[inputData.potPos];
			drawSquare(chosenMoveX, chosenMoveY);
		} else {
			// choosing piece or position
			uint8_t lastOption = options[focusedOption];
			focusedOption = inputData.potPos;

			drawSquare(lastOption & 7, lastOption >> 3);
			
			uint8_t newOption = options[focusedOption];
			drawSquare(newOption & 7, newOption >> 3);
		}
	}
	
	// blink focus square
	if(numOptions > 0) {
		uint8_t focusSquare = options[focusedOption];
		drawSquare(focusSquare & 7, focusSquare >> 3);
	}
}


void ChessBoard::acceptMessage(uint8_t* message) {
	// temporary
	areWeWhite = !areWeWhite;
	uint32_t temp = ourTimer;
	ourTimer = theirTimer;
	theirTimer = temp;
	calculatePossiblePieces();
	
	bool areWeInCheck = isKingInCheck(areWeWhite);
	if(areWeInCheck) {
		if(numOptions == 0) {
			turnState = CHECKMATE;
			gameOver = true;
		} else {
			turnState = CHECK;
			Sound_Check();
		}
	} else {
		if(numOptions == 0) {
			turnState = STALEMATE;
			gameOver = true;
		} else {
			turnState = NORMAL;
		}
	}
	
	if(gameOver) {
		// TODO transmit game over
		
		Sound_Checkmate();
	} else {
		InputManager_SetNumPotPositions(numOptions);
	}
	
	drawAll();
}