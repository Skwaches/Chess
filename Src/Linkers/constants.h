#ifndef CONSTANTS_H
#define CONSTANTS_H

// SCREEN
#define SCREENWIDTH 600
#define SCREENHEIGHT 600
#define X_TILES 8
#define Y_TILES 8

// BOARD
#define TILE_WIDTH (SCREENWIDTH / X_TILES)
#define TILE_HEIGHT (SCREENHEIGHT / Y_TILES)

// PIECES RANK
#define BLACK_Y Y_TILES
#define BPAWNY (Y_TILES - 1)
#define WHITE_Y 1
#define WPAWNY (WHITE_Y + 1)

// FPS
#define FPS 100.0
#define WAIT_TIME ((1 / FPS) * 1000.0)
#define LIMIT_FPS false
// PATHS

#define WHITE_PIECES_PATH "../Assets/Visual/Pieces/White/Svgs/Set2"
#define BLACK_PIECES_PATH "../Assets/Visual/Pieces/Black/Svgs/Set2"

#define SOUNDS_PATH "../Assets/Audio/Sound"
#define MUSIC_PATH "../Assets/Audio/Music"

#define MAX_NAME_LENGTH 10
#define MAX_ASSET_PATH 100

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

// SOUNF FILE NAMES
#define CAPTURE_SOUND_FILE_NAME "capture.mp3"
#define MOVE_SOUND_FILE_NAME "move.mp3"
#define CASTLE_SOUND_FILE_NAME "castle.mp3"
#define CHECK_SOUND_FILE_NAME "check.mp3"

// PIECE NAME
#define KING_NAME "King"
#define BISHOP_NAME "Bishop"
#define QUEEN_NAME "Queen"
#define ROOK_NAME "Rook"
#define KNIGHT_NAME "Knight"
#define PAWN_NAME "Pawn"

// Move Codes
#define INVALID 0
#define VALID 1
#define VALID_CAPTURE 2
#define KINGSIDE_CASTLING 3
#define QUEENSIDE_CASTLING 4
#define ENPASSANT 5

// Board Colors
extern Uint8 LIGHT_TILE_COLOR[];
extern Uint8 DARK_TILE_COLOR[];

//  XPOSITIONS
extern const int PAWN_X[];
extern const int BISHOP_X[];
extern const int QUEEN_X[];
extern const int KING_X[];
extern const int ROOK_X[];
extern const int KNIGHT_X[];

// CALCULATIONS

// NO OF PIECES
extern const int KING_NO;
extern const int BISHOP_NO;
extern const int QUEEN_NO;
extern const int ROOK_NO;
extern const int KNIGHT_NO;
extern const int PAWN_NO;

#endif