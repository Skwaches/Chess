#ifndef FUNCS_H
#define FUNCS_H
#include <SDL3/SDL.h>
#include "classes.h"

Tile TileFromPos(SDL_FPoint *pos);
SDL_FRect rectFromTile(Tile cords);
PieceNode *makePieceNode(SDL_Renderer *renderer,
                         char *buffer,
                         PieceNode *tempPiece,
                         const int appearances,
                         const char *name,
                         const int *x,
                         const int y);
void freePieces(PieceNode *Headnode);
void freeTileNodes(TileNode *HeadNode);
void renderPieces(SDL_Renderer *renderer, PieceNode *HeadPiece);
void renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile);
char chessX(int number);
int realX(char letter);
Piece pieceFromPos(PieceNode *HeadPiece, SDL_FPoint *pos); // Checks against Pieces rect ie. where it is visually.
SDL_FRect centerRectAroundPos(SDL_FPoint *pos);
void trackMouse(Piece fake_PIECE, SDL_FPoint *mouse_pos);
void untrackMouse(Piece fake_PIECE);
void movePiece(Piece fake_PIECE, SDL_FPoint *pos);
void deletePiece(Piece fakePiece, PieceNode **FakeFamily);

Piece pieceFromTile(Tile dest, PieceNode *pieceFamily); // Checks against Piece coordinate.
bool tileHasOccupant(Tile dest, PieceNode *pieceFamily);
int validateMove(Piece test, SDL_FPoint *dest,
                 bool player /*True if white.*/,
                 PieceNode *playerFamily, PieceNode *enemyFamily);

#endif