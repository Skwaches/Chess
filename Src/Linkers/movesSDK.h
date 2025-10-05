#ifndef MOVES_SDK
#define MOVES_SDK
bool openDataBase(void);
void closeDataBase(void);
bool createTable(void);
bool recordMovesyntax(Piece peace, Tile originalTile,
                      Tile destTile, int result,
                      bool check, bool mate);
#endif