#include "Linkers/funcs.h"
#include "Linkers/bot.h"

Tile *getMoves(Piece chosenPiece, PieceNode *family, PieceNode *enemy, bool player)
{
    Tile originalTile = chosenPiece.ptr->pos[chosenPiece.index];
    Tile destTile;
    initMove(chosenPiece, originalTile, destTile, player, family, enemy);
    finalizeMove(false, NULL);
}