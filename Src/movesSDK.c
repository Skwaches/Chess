#include <sqlite3.h>
#include <SDL3/SDL.h>
#include "Linkers/movesSDK.h"

// EXCLUSIVE TO THIS FILE
static sqlite3 *DATABASE = NULL; // Creates a static memory address for the object.
static unsigned int GAMENUMBER = 0;
static char *errorMessage = NULL;
static sqlite3_stmt *statement;

// DataBase is opened and closed from the Main file:
bool openDataBase(void)
{
    if (sqlite3_open("../Moves.db", &DATABASE) != SQLITE_OK)
    {
        sqlite3_close(DATABASE);
        SDL_Log("DataBase could not be opened :%s", sqlite3_errmsg(DATABASE));
        return false;
    }

    // Set GAMENUMBER to next table;
    char *command = "SELECT COUNT(*) FROM sqlite_master WHERE type='table';";

    sqlite3_prepare_v2(DATABASE, command, -1, &statement, 0);
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        GAMENUMBER = sqlite3_column_int(statement, 0);
    }
    sqlite3_finalize(statement);

    SDL_Log("DataBase Opened");
    return true;
}

void closeDataBase(void)
{
    sqlite3_close(DATABASE);
    SDL_Log("DataBase Closed");
    DATABASE = NULL; // AVOID HANGING POINTERS;
}

bool createTable(void)
{
    GAMENUMBER++;
    char command[56 + 1 + 10];
    // size of command=56 + NULL terminator and 10 digits from GAMENUMBER;
    // max of 10^10-1 for GAMENUMBER which is pretty damn big;
    SDL_snprintf(command, sizeof(command), "CREATE TABLE IF NOT EXISTS [%d](White TEXT,Black TEXT);", GAMENUMBER);
    if (sqlite3_exec(DATABASE, command, 0, 0, &errorMessage) != SQLITE_OK)
    {
        SDL_Log("SQL ERROR : %s", errorMessage);
        sqlite3_free(errorMessage);
        errorMessage = NULL; // Avoid dangling pointer
        return false;
    }
    return true;
}

bool recordMove(const char *moveWhite, const char *moveBlack)
{
    char command[128];
    SDL_snprintf(command, sizeof(command), "INSERT INTO [%d](White,Black) VALUES(?,?)", GAMENUMBER);
    // Prepare statement
    if (sqlite3_prepare_v2(DATABASE, command, -1, &statement, 0) != SQLITE_OK)
    {
        SDL_Log("SQL ERROR :%s", sqlite3_errmsg(DATABASE));
        return false;
    }

    // Bind Value
    sqlite3_bind_text(statement, 1, moveWhite, -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, moveBlack, -1, SQLITE_STATIC);
    // Execute
    if (sqlite3_step(statement) != SQLITE_DONE)
    {
        SDL_Log("SQL ERROR :%s", sqlite3_errmsg(DATABASE));
        sqlite3_finalize(statement);
        return false;
    }
    SDL_Log("Move recorded: %s, %s", moveWhite, moveBlack);
    sqlite3_finalize(statement);
    return true;
}
// Invalid returns i
char chessX(int number)
{
    switch (number)
    {
    case 1:
        return 'a';
        break;
    case 2:
        return 'b';
        break;
    case 3:
        return 'c';
        break;
    case 4:
        return 'd';
        break;
    case 5:
        return 'e';
        break;
    case 6:
        return 'f';
        break;
    case 7:
        return 'g';
        break;
    case 8:
        return 'h';
        break;

    default:
        return 'i';
        break;
    }
}

// Invalid returns 0
int realX(char letter)
{
    switch (letter)
    {
    case 'a':
        return 1;
        break;
    case 'b':
        return 2;
        break;
    case 'c':
        return 3;
        break;
    case 'd':
        return 4;
        break;
    case 'e':
        return 5;
        break;
    case 'f':
        return 6;
        break;
    case 'g':
        return 7;
        break;
    case 'h':
        return 8;
        break;
    default:
        return 0;
        break;
    }
}
