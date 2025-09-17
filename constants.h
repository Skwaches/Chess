#ifndef CONSTANTS_H
#define CONSTANTS_H

// SCREEN
#define SCREENWIDTH 600
#define SCREENHEIGHT 600
#define X_TILES 8
#define Y_TILES 8

#define PIECE_TYPES 6

// FPS
#define FPS 100.0
#define WAIT_TIME ((1 / FPS) * 1000.0)
#define LIMIT_FPS false
// PATHS

#define WHITE_PIECES_PATH "../Assets/Visual/Pieces/Pngs/Set2/White"
#define BLACK_PIECES_PATH "../Assets/Visual/Pieces/Pngs/Set2/Black"

#define SOUNDS_PATH "../Assets/Audio/Sound"
#define MUSIC_PATH "../Assets/Audio/Music"

#define MAX_NAME_LENGTH 10
#define MAX_ASSET_PATH 100

// PIECES RANK
#define BLACK_Y Y_TILES
#define BPAWNY (Y_TILES - 1)
#define WHITE_Y 1
#define WPAWNY (WHITE_Y + 1)

// IMAGE FORMAT
#define SVG_WIDTH 45
#define SVG_HEIGHT 45

// IMAGE FILE NAME
#define KING_FILE_NAME "king.png"
#define BISHOP_FILE_NAME "bishop.png"
#define QUEEN_FILE_NAME "queen.png"
#define ROOK_FILE_NAME "rook.png"
#define KNIGHT_FILE_NAME "knight.png"
#define PAWN_FILE_NAME "pawn.png"

// PIECE NAME
#define KING_NAME "KING"
#define BISHOP_NAME "BISHOP"
#define QUEEN_NAME "QUEEN"
#define ROOK_NAME "ROOK"
#define KNIGHT_NAME "KNIGHT"
#define PAWN_NAME "PAWN"

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