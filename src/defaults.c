#include "funcs.h"
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

bool STARTSIDE = true;
bool HUMAN = true;
bool BOT = false;
bool LIMIT_MOVES_PER_GAME = false;
bool AUTOSTART_NEWGAME = false;
int BOT_DELAY = 1000;
int MAX_MOVES_PER_GAME = 100;

/*Board Colors*/
SDL_Color BACKGROUND_COLOR = {10,10,10,10};
SDL_Color LIGHT_TILE_COLOR =     {26, 230, 179, 255}; // rgba(26, 230, 179, 1)
SDL_Color DARK_TILE_COLOR = 	 {51, 26, 77, 255};   // rgba(51, 26, 77, 1)
SDL_Color SELECTED_TILE_COLOR =  {204, 51, 179, 255}; // rgba(204, 51, 179, 1) 
SDL_Color ORIG_COLOR = 			 {51, 128, 128, 255};  // rgba(51, 128, 128, 1)
SDL_Color DEST_COLOR = 			 {128, 128, 179, 255};// rgba(128, 128, 179, 1)
SDL_Color POSS_DEST_COLOR= 		 {179, 51, 204, 255}; // rgba(179, 51, 204, 1)

