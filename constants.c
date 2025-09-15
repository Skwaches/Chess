#include "constants.h"
#include "classes.h"
const int PAWN_X[] = {1, 2, 3, 4, 5, 6, 7, 8};
const int BISHOP_X[] = {3, 6};
const int QUEEN_X[] = {4};
const int KING_X[] = {5};
const int ROOK_X[] = {1, 8};
const int KNIGHT_X[] = {2, 7};

const int KING_NO = sizeof(KING_X) / sizeof(int);
const int BISHOP_NO = sizeof(BISHOP_X) / sizeof(int);
const int QUEEN_NO = sizeof(QUEEN_X) / sizeof(int);
const int ROOK_NO = sizeof(ROOK_X) / sizeof(int);
const int KNIGHT_NO = sizeof(KNIGHT_X) / sizeof(int);
const int PAWN_NO = sizeof(PAWN_X) / sizeof(int);
