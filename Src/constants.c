#include "Linkers/funcs.h"
int PAWN_X[] = {1, 2, 3, 4, 5, 6, 7, 8};
int BISHOP_X[] = {3, 6};
int QUEEN_X[] = {4};
int KING_X[] = {5};
int ROOK_X[] = {1, 8};
int KNIGHT_X[] = {2, 7};

const Tile SHADOW_REALM = (Tile){-X_TILES, -Y_TILES};
const int KING_NO = sizeof(KING_X) / sizeof(int);
const int BISHOP_NO = sizeof(BISHOP_X) / sizeof(int);
const int QUEEN_NO = sizeof(QUEEN_X) / sizeof(int);
const int ROOK_NO = sizeof(ROOK_X) / sizeof(int);
const int KNIGHT_NO = sizeof(KNIGHT_X) / sizeof(int);
const int PAWN_NO = sizeof(PAWN_X) / sizeof(int);

// char *dbNameFromLvl(int levl)
// {
//     char ball[9];
//     levl++;
//     SDL_snprintf(ball, sizeof(ball), "../bot");
//     int nextIndex = SDL_strlen(ball);
//     ball[nextIndex] = '0' + levl;
//     return ball;
// }
