#include "Linkers/funcs.h"
Uint8 LIGHT_TILE_COLOR[] = {130, 120, 150, 255};
Uint8 DARK_TILE_COLOR[] = {40, 40, 40, 255};
Uint8 SELECTED_TILE_COLOR[] = {200, 100, 80, 255};
Uint8 BACKGROUND_COLOR[] = {200, 40, 40, 255};

const int PAWN_X[] = {1, 2, 3, 4, 5, 6, 7, 8};
const int BISHOP_X[] = {3, 6};
const int QUEEN_X[] = {4};
const int KING_X[] = {5};
const int ROOK_X[] = {1, 8};
const int KNIGHT_X[] = {2, 7};

const Tile SHADOW_REALM = (Tile){-X_TILES, -Y_TILES};
const int KING_NO = sizeof(KING_X) / sizeof(int);
const int BISHOP_NO = sizeof(BISHOP_X) / sizeof(int);
const int QUEEN_NO = sizeof(QUEEN_X) / sizeof(int);
const int ROOK_NO = sizeof(ROOK_X) / sizeof(int);
const int KNIGHT_NO = sizeof(KNIGHT_X) / sizeof(int);
const int PAWN_NO = sizeof(PAWN_X) / sizeof(int);