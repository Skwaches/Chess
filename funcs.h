#ifndef FUNCS_H
#define FUNCS_H
#include <SDL3/SDL.h>
#include "classes.h"
SDL_Texture *maketexture(SDL_Renderer *renderer, char *path);
pieceNode *makePieceNode(SDL_Renderer *renderer,
                         char *buffer,
                         pieceNode *tempPiece,
                         int appearances,
                         char *name,
                         coords *position);
void freePieces(pieceNode *Headnode);
void freeTiles(tileNode *HeadNode);
#endif