#ifndef MOVES_SDK
#define MOVES_SDK
bool openDataBase(void);
void closeDataBase(void);
bool createTable(void);
bool recordMovesyntax(Piece moved, Tile destTile, int result, bool check);
#endif