#ifndef MOVES_SDK
#define MOVES_SDK
bool openDataBase(void);
void closeDataBase(void);
int createTable(void);
char chessX(int number);
bool recordMovesyntax(Piece peace, Tile originalTile, Tile destTile,
                      PieceNode *family, PieceNode *enemy,
                      int result, bool check, bool mate, bool player,
                      char chosenPiece, bool stale, bool gameOver);
#endif
