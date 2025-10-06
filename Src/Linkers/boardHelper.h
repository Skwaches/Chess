#ifndef HELP_H
#define HELP_H
#include "funcs.h"
Tile TileFromPos(SDL_FPoint pos);
SDL_FRect rectFromTile(Tile cords);

SDL_FRect centerRectAroundPos(SDL_FPoint pos);
void trackMouse(Piece fake_PIECE, SDL_FPoint mouse_pos);
void untrackMouse(Piece fake_PIECE);
void movePiece(Piece fakePIECE, Tile destCoordinates);
void deletePiece(Piece fakePiece);
TileNode *nodeFromTile(Tile coords, TileNode *light, TileNode *dark);
void promotePiece(Piece fakePiece, char chosenOne, PieceNode *Family, Tile destTile);
int getFPS(Uint64 time);
#endif