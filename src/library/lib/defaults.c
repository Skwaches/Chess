#include "../links/funcs.h"
int PAWN_X[] = {1, 2, 3, 4, 5, 6, 7, 8};
int BISHOP_X[] = {3, 6};
int QUEEN_X[] = {4};
int KING_X[] = {5};
int ROOK_X[] = {1, 8};
int KNIGHT_X[] = {2, 7};

const int KING_NO = sizeof(KING_X) / sizeof(int);
const int BISHOP_NO = sizeof(BISHOP_X) / sizeof(int);
const int QUEEN_NO = sizeof(QUEEN_X) / sizeof(int);
const int ROOK_NO = sizeof(ROOK_X) / sizeof(int);
const int KNIGHT_NO = sizeof(KNIGHT_X) / sizeof(int);
const int PAWN_NO = sizeof(PAWN_X) / sizeof(int);

bool STARTSIDE = false;
bool HUMAN = true;
bool BOT = false;
bool LIMIT_MOVES_PER_GAME = false;
bool AUTOSTART_NEWGAME = false;
int BOT_DELAY = 1000;
int MAX_MOVES_PER_GAME = 100;

/*Board Colors*/
SDL_Color BACKGROUND_COLOR = {10, 10, 10, 255};
SDL_Color LIGHT_TILE_COLOR ={200, 160, 190, 255};
SDL_Color DARK_TILE_COLOR = {128, 14, 89,255};
SDL_Color SELECTED_TILE_COLOR ={141, 100, 58,70};
SDL_Color ORIG_COLOR = {196, 180, 28,30};
SDL_Color DEST_COLOR = {108, 104, 16,30};
SDL_Color POSS_DEST_COLOR= {130, 190 ,100, 140};

