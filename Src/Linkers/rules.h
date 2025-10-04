#ifndef RULES_H
#define RULES_H
#include "funcs.h"
// Moves
Piece pieceFromTile(Tile dest, PieceNode *pieceFamily); // Checks against Piece coordinate.
bool TileHasOccupant(Tile dest, PieceNode *pieceFamily);
bool initMove(Piece globalPiece, Tile globalOrigTile, Tile globalDest, // Doesn't include check logic///
              bool globalPlayer /*True if white.*/,
              PieceNode *globalPlayerFamily, PieceNode *globalOpponentFamily);
int performValidation();
bool match_Piece(Piece test, const char *str2);

void resetStorage(void);
Tile *fakeMove(Piece piece, Tile dest);
Tile *fakeDelete(Piece piece);
void unfakeMove(Piece piece, Tile *tmpHolder);
int finalizeMove(bool updateState);
bool setCheck(void);
#endif