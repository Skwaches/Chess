#ifndef MOVES_SDK
#define MOVES_SDK
bool openDataBase(void);
void closeDataBase(void);
bool createTable(void);
bool recordMove(const char *moveWhite, const char *moveBlack);
char chessX(int number);
#endif