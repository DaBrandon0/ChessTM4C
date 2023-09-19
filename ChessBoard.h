// ChessBoard.h

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdint.h>
#include "InputManager.h"
#include "Language.h"

// note: (0, 0) is top left of board (black rook on light square)


// colors

#define WhitePieceColor 0xFFFF
#define BlackPieceColor 0x0000

#define LightSquareColor 					0b1110010100010000
#define DarkSquareColor 					0b1010001000001000

#define DarkSquareHighlightColor 	0b1011111000001011
#define LightSquareHighlightColor	0b1100011000010000

#define SquareFocusColor		 			0b1110011000011111
#define SquareSelectedColor				0b1010011111101000

#define CheckSquareColor					0b0000010000011110
#define CheckmateSquareColor			0b0000000000011000

#define TextColorDefault					0b1111111111111111
#define TextColorAccent						0b0100011111010000


// constants

#define SquareBlinkPeriod	500

#define StartTime 300000
#define AddTime 5000



// classes

enum ChessPieceType {
	NONE = 0,
	PAWN = 1,
	BISHOP = 2,
	KNIGHT = 3,
	ROOK = 4,
	QUEEN = 5,
	KING = 6
};

typedef struct ChessPiece {
	bool isWhite;
	ChessPieceType type;
} ChessPiece;

enum TurnState {
	NORMAL = 0,
	CHECK = 1,
	CHECKMATE = 2,
	STALEMATE = 3
};

class ChessBoard {
	private:
		ChessPiece board[8][8];
	
		bool languageSelected;
		Language selectedLanguage;
	
		bool gameOver;
	
		bool areWeWhite;
		bool isOurTurn; 
		uint32_t blinkTimer;
		uint32_t ourTimer;
		uint32_t theirTimer;
	
		bool isPieceChosen;
		bool isMovementChosen;
		bool isPromotionChosen;
		uint8_t chosenPieceX, chosenPieceY;
		uint8_t chosenMoveX, chosenMoveY;
		ChessPieceType chosenPromotion;
	
		uint8_t options[64];
		uint8_t numOptions;
		uint8_t focusedOption;
	
		TurnState turnState;
	
		void start();
	
		uint16_t getSquareColor(uint8_t x, uint8_t y);
		void drawSquare(uint8_t x, uint8_t y);
	
		void addOption(uint8_t x, uint8_t y);
		void calculatePossiblePieces();
		void calculatePossibleMoves(uint8_t x, uint8_t y, bool checkForCheck);
		void sortOptions();
		
		bool isKingInCheck(bool isKingWhite);
		bool wouldMoveCauseCheck(uint8_t oldX, uint8_t oldY, uint8_t newX, uint8_t newY, bool isKingWhite);
		
		void completeTurn();
	
	public:
		ChessBoard();
	
		bool mailboxReady;
		uint8_t* mailbox;
	
		void drawAll();
	
		void update(InputData& inputData);
		void acceptMessage(uint8_t* message);
};




// data

const ChessPieceType initialState[8][8] = {
	{ ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
	{ PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN },
	{ NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
	{ NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
	{ NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
	{ NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
	{ PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN },
	{ ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK }
};

const int8_t knightX[8] = {1, 1, -1, -1, 2, 2, -2, -2};
const int8_t knightY[8] = {2, -2, 2, -2, 1, -1, 1, -1};
const int8_t kingX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int8_t kingY[8] = {1, 1, 0, -1, -1, -1, 0, 1};

const ChessPieceType promotionOptions[4] = { BISHOP, KNIGHT, ROOK, QUEEN };

const bool ChessGraphics[7][196] = {
	// NONE
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	},
	
	// PAWN
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	}, 
	
	// BISHOP
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,0,1,1,1,0,0,0,
		0,0,0,1,1,1,0,0,1,1,1,0,0,0,
		0,0,0,0,1,0,0,1,1,1,0,0,0,0,
		0,0,0,0,0,0,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	},
	
	// KNIGHT
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,1,1,1,1,0,0,0,0,
		0,0,1,1,1,1,0,1,1,1,1,0,0,0,
		0,0,1,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,0,1,1,1,1,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,0,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	},
	
	// ROOK
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,0,1,1,1,1,0,1,1,0,0,
		0,0,1,1,0,1,1,1,1,0,1,1,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	},
	// QUEEN
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,0,0,1,1,1,1,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,1,1,0,1,1,1,1,0,1,1,0,0,
		0,1,1,0,0,1,0,0,1,0,0,1,1,0,
		0,1,1,0,1,1,0,0,1,1,0,1,1,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	},
	
	// KING
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,0,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,0,0,
		0,1,1,1,0,0,1,1,0,0,1,1,1,0,
		0,1,1,1,0,0,1,1,0,0,1,1,1,0,
		0,1,1,1,0,0,1,1,0,0,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,1,0,
		0,0,1,1,1,0,1,1,0,1,1,1,0,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	}
};


#endif