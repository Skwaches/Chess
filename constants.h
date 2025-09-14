#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SCREENWIDTH 400
#define SCREENHEIGHT 400
#define X_TILES 8
#define Y_TILES 8

#define PIECE_PATH "./assets/pieces"
#define MAX_NAME_LENGTH 10
#define MAX_ASSET_PATH 30
#define KING_NO 1
#define BISHOP_NO 2
#define QUEEN_NO 1
#define ROOK_NO 2
#define KNIGHT_NO 3
#define PAWN_NO 8

// CALCULATIONS
#define TILE_WIDTH (SCREENWIDTH / X_TILES)
#define TILE_HEIGHT (SCREENHEIGHT / Y_TILES)
#define SVG_WIDTH TILE_WIDTH - 10
#define SVG_HEIGHT TILE_HEIGHT - 10
#endif