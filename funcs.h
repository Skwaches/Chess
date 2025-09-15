#ifndef FUNCS_H
#define FUNCS_H
#include <SDL3/SDL.h>
#include "classes.h"
SDL_Texture *maketexture(SDL_Renderer *renderer, char *path);
pieceNode *makePieceNode(SDL_Renderer *renderer,
                         char *buffer,
                         pieceNode *tempPiece,
                         const int appearances,
                         const char *name,
                         const int *x,
                         const int y);
void freePieces(pieceNode *Headnode);
void freeTiles(tileNode *HeadNode);
void renderPieces(SDL_Renderer *renderer, pieceNode *HeadPiece);
void renderTiles(SDL_Renderer *renderer, tileNode *HeadTile);
Piece *pieceFromPos(SDL_FPoint *pos, pieceNode *HeadPiece);
SDL_FRect rectFromPos(SDL_FPoint *pos);
char chessX(int number);
int realX(char letter);
#endif