
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "funcs.h"
// SCREEN
#define SCREENWIDTH 600
#define SCREENHEIGHT 600
#define X_TILES 8
#define Y_TILES 8

// BOARD
#define TILE_WIDTH (SCREENWIDTH / X_TILES)
#define TILE_HEIGHT (SCREENHEIGHT / Y_TILES)
#define PIECE_ZOOM 20 /*This is the size of a selected piece.*/
 
// FPS_CAP
#define FPS_CAP 1500.0
#define WAIT_TIME ((1 / FPS_CAP) * 1000.0)
#define LIMIT_FPS true

/* Relative PATHS
 * Note : Assumption is that game is running from build.
 * Adjust accordingly.*/
#define ASSETS_PATH                   "../assets/"
#define DATABASE_PATH ASSETS_PATH     "moves.db"
#define VISUAL_PATH ASSETS_PATH       "visual/"
#define AUDIO_PATH ASSETS_PATH        "audio/"
#define PIECES_PATH VISUAL_PATH       "pieces/"
#define WHITE_PIECES_PATH PIECES_PATH "white"
#define BLACK_PIECES_PATH PIECES_PATH "black"
#define SOUNDS_PATH AUDIO_PATH        "sound"
#define MUSIC_PATH AUDIO_PATH         "music"


#define MAX_NAME_LENGTH 10
#define MAX_ASSET_PATH 100
#define MAX_MOVE_SYNTAX 40
#define MAX_COMMAND_LENGTH 128
#define MAX_POSSIBLE_MOVES (7 * (X_TILES > Y_TILES ? X_TILES : Y_TILES))
#define MAX_TOTAL_POSSIBLE_MOVES 220

// IMAGE FORMAT
#define SVG_WIDTH TILE_WIDTH
#define SVG_HEIGHT TILE_HEIGHT

// IMAGE FILE NAME
#define KING_FILE_NAME "king.svg"
#define BISHOP_FILE_NAME "bishop.svg"
#define QUEEN_FILE_NAME "amazon.svg"
#define ROOK_FILE_NAME "rook.svg"
#define KNIGHT_FILE_NAME "knight.svg"
#define PAWN_FILE_NAME "pawn.svg"
#define PIECETYPES 6

// SOUND FILE NAMES
#define CAPTURE_SOUND_FILE_NAME "capture.mp3"
#define MOVE_SOUND_FILE_NAME "move.mp3"
#define CASTLE_SOUND_FILE_NAME "castle.mp3"
#define CHECK_SOUND_FILE_NAME "check.mp3"
#define ILLEGAL_SOUND_FILE_NAME "illegal.mp3"
#define LOWTIME_SOUND_FILE_NAME "lowtime.mp3"
#define PREMOVE_SOUND_FILE_NAME "premove.mp3"
#define PROMOTE_SOUND_FILE_NAME "promote.mp3"
#define GAMESTART_SOUND_FILE_NAME "gamestart.mp3"
#define GAMEEND_SOUND_FILE_NAME "gameend.mp3"

// PIECE NAME
// These are unique identifiers for pieces.
#define BISHOP_NAME 'B'
#define QUEEN_NAME 'Q'
#define KING_NAME 'K'
#define ROOK_NAME 'R'
#define KNIGHT_NAME 'N'
#define PAWN_NAME '\0'

// Move Codes
#define INVALID 0
#define VALID 1
#define VALID_CAPTURE 2
#define KINGSIDE_CASTLING 3
#define QUEENSIDE_CASTLING 4
#define ENPASSANT 5
#define PROMOTION 6
#define PROMOTION_CAPTURE 7

/*White is true. Black is false;*/
#define PROMODEFAULT QUEEN_NAME
extern bool STARTSIDE; 
extern bool HUMAN;
extern bool BOT ;
extern int  BOT_DELAY ;
extern bool LIMIT_MOVES_PER_GAME; 
extern int MAX_MOVES_PER_GAME ;
extern bool AUTOSTART_NEWGAME;

// DELETE PIECE
#define SHADOW_REALM (Tile){-X_TILES,-Y_TILES}
#define NULL_PIECE (Piece){NULL, -1}

// Board Colors
extern SDL_Color BACKGROUND_COLOR;
extern SDL_Color LIGHT_TILE_COLOR;
extern SDL_Color DARK_TILE_COLOR;
extern SDL_Color SELECTED_TILE_COLOR;
extern SDL_Color ORIG_COLOR;
extern SDL_Color DEST_COLOR;
extern SDL_Color POSS_DEST_COLOR;

//TileNode Color Codes
#define NORMAL_TILE 0
#define SELECTED_TILE 1
#define PIECE_HOVER 2
#define PREV_ORIG 3
#define PREV_DEST 4

/*Showing possible moves*/

//  XPOSITIONS
extern int PAWN_X[];
extern int BISHOP_X[];
extern int QUEEN_X[];
extern int KING_X[];
extern int ROOK_X[];
extern int KNIGHT_X[];

// YPOSITIONS
#define BLACK_Y Y_TILES
#define BPAWNY (Y_TILES - 1)
#define WHITE_Y 1
#define WPAWNY (WHITE_Y + 1)


// NO OF PIECES
extern const int KING_NO;
extern const int BISHOP_NO;
extern const int QUEEN_NO;
extern const int ROOK_NO;
extern const int KNIGHT_NO;
extern const int PAWN_NO;
#endif
