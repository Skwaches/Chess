#ifndef CONSTANTS_H
#define CONSTANTS_H

// SCREEN
#define SCREENWIDTH 400
#define SCREENHEIGHT 400
#define X_TILES 8
#define Y_TILES 8

#define PIECE_TYPES 6

// FPS
#define FPS 24
#define WAIT_TIME (1 / FPS) * 1000
// PATHS

#define WHITE_PIECES_PATH "./assets/pieces/white"
#define BLACK_PIECES_PATH "./assets/pieces/black"

#define MAX_NAME_LENGTH 10
#define MAX_ASSET_PATH 35

// PIECES RANK
#define BLACK_Y 8
#define BPAWNY 7
#define WHITE_Y 1
#define WPAWNY 2

// IMAGE FORMAT
#define SVG_WIDTH 45
#define SVG_HEIGHT 45

// IMAGE FILE NAME
#define KING_FILE_NAME "king.svg"
#define BISHOP_FILE_NAME "bishop.svg"
#define QUEEN_FILE_NAME "queen.svg"
#define ROOK_FILE_NAME "rook.svg"
#define KNIGHT_FILE_NAME "knight.svg"
#define PAWN_FILE_NAME "pawn.svg"

// PIECE NAME
#define KING_NAME "King"
#define BISHOP_NAME "Bishop"
#define QUEEN_NAME "Queen"
#define ROOK_NAME "Rook"
#define KNIGHT_NAME "Knight"
#define PAWN_NAME "Pawn"

// XPOSITIONS
extern const int PAWN_X[];
extern const int BISHOP_X[];
extern const int QUEEN_X[];
extern const int KING_X[];
extern const int ROOK_X[];
extern const int KNIGHT_X[];

// CALCULATIONS
#define TILE_WIDTH (SCREENWIDTH / X_TILES)
#define TILE_HEIGHT (SCREENHEIGHT / Y_TILES)

// NO OF PIECES
extern const int KING_NO;
extern const int BISHOP_NO;
extern const int QUEEN_NO;
extern const int ROOK_NO;
extern const int KNIGHT_NO;
extern const int PAWN_NO;

#endif