#ifndef BOT_H
#define BOT_H
Tile *validMoves(Piece selectedPiece, PieceNode *family,
                 PieceNode *enemy, bool player, int *noPaths);
bool checkMate(PieceNode *playerFamily, PieceNode *enemyFamily, bool playerBool);
#endif