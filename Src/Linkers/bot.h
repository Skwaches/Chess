#ifndef BOT_H
#define BOT_H
Tile *validMoves(Piece selectedPiece, PieceNode *family,
                 PieceNode *enemy, bool player, int *noPaths, int *valids);
bool checkMate(PieceNode *playerFamily, PieceNode *enemyFamily, bool playerBool);
Move *selectionPool(PieceNode *botFamily, PieceNode *humanFamily, bool player);
Tile *tileFromPool(Piece piece, Move *pool, int *listLength, int **validers);
void freeMoves(Move *pool);
void printMoves(Move *pool);
Tile randomMoveFromPool(Move *pool, Piece *pieceholder, int *valid);
#endif