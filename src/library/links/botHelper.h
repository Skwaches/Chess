#ifndef BOT_H
#define BOT_H
Tile *validMoves(Piece selectedPiece, PieceNode *family,
                 PieceNode *enemy, bool player, int *noPaths, int *valids);
bool checkMate(Move *pool);
Move *selectionPool(PieceNode *botFamily, PieceNode *humanFamily, bool player);
Tile *tileFromPool(Piece piece, Move *pool, int *listLength, int **validers);
void freeMoves(Move *pool);
void printMoves(Move *pool);
bool checkstale(PieceNode *playerFamily, PieceNode *enemyFamily);
#endif
