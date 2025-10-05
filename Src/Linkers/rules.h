#ifndef RULES_H
#define RULES_H
#include "funcs.h"
// Moves
Piece pieceFromTile(Tile dest, PieceNode *pieceFamily); // Checks against Piece coordinate.
bool TileHasOccupant(Tile dest, PieceNode *pieceFamily);
bool initMove(Piece selectedPiece, Tile originalTile /*It doesn't recalculate without this.*/, Tile globalDest, // Doesn't include check logic///
              bool Currentplayer /*True if white.*/,
              PieceNode *family, PieceNode *enemy);
void resetStorage(void);
Tile *fakeMove(Piece piece, Tile dest);
Tile *fakeDelete(Piece piece);
void unfakeMove(Piece piece, Tile *tmpHolder);
int finalizeMove(bool updateState, bool *causeCheck);
bool setCheck(void);
#endif