#ifndef BOARD_H
#define BOARD_H
#include "funcs.h"
TileNode *setTiles(bool start_Offset);
void unselectAll(TileNode *family, TileNode *opp, bool force);
bool setRenderColor(SDL_Renderer *renderer, SDL_Color color);
bool renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile, SDL_Color colors);
void freeTileNodes(TileNode *HeadNode);
PieceNode *setPieces(SDL_Renderer *renderer, bool colour, const char *Path);
bool renderPieces(SDL_Renderer *renderer, PieceNode *HeadPiece);
void resetPieces(PieceNode *colour, bool player);
void freePieces(PieceNode *Headnode);
#endif
