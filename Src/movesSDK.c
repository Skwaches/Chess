#include <sqlite3.h>
#include <SDL3/SDL.h>
#include "Linkers/funcs.h"
#include "Linkers/movesSDK.h"
// EXCLUSIVE TO THIS FILE
static sqlite3 *DATABASE = NULL;
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
        GAMENUMBER = sqlite3_column_int(statement, 0);
    sqlite3_finalize(statement);
    return true;
}

void closeDataBase(void)
{
    sqlite3_close(DATABASE);
    DATABASE = NULL;
}

bool createTable(void)
{
    GAMENUMBER++;
    char command[56 + 1 + 10];
    // size of command=56 + NULL terminator and 10 digits from GAMENUMBER;
    // max of 10^10-1 for GAMENUMBER which is pretty damn big;
    SDL_snprintf(command, sizeof(command), "CREATE TABLE IF NOT EXISTS [%d](Moves);", GAMENUMBER);
    if (sqlite3_exec(DATABASE, command, 0, 0, &errorMessage) != SQLITE_OK)
    {
        SDL_Log("SQL ERROR : %s", errorMessage);
        sqlite3_free(errorMessage);
        errorMessage = NULL; // Avoid dangling pointer
        return false;
    }
    return true;
}

bool recordMove(const char *move)
{
    char command[MAX_COMMAND_LENGTH];
    SDL_snprintf(command, sizeof(command), "INSERT INTO [%d](Moves) VALUES(?)", GAMENUMBER);
    // Prepare statement
    if (sqlite3_prepare_v2(DATABASE, command, -1, &statement, 0) != SQLITE_OK)
    {
        SDL_Log("SQL ERROR :%s", sqlite3_errmsg(DATABASE));
        return false;
    }

    // Bind Value
    sqlite3_bind_text(statement, 1, move, -1, SQLITE_STATIC);
    // Execute
    if (sqlite3_step(statement) != SQLITE_DONE)
    {
        SDL_Log("SQL ERROR :%s", sqlite3_errmsg(DATABASE));
        sqlite3_finalize(statement);
        return false;
    }
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

bool recordMovesyntax(Piece peace, Tile originalTile, Tile destTile, int result, bool check, bool mate)
{
    char moveMade[MAX_MOVE_SYNTAX];
    Tile origTile = originalTile;
    bool capture = (result == VALID_CAPTURE || result == ENPASSANT || result == PROMOTION_CAPTURE);

    const char pieceName = peace.ptr->type;
    char origFile = '\0';
    char origRank = '\0';

    char captiver = capture ? 'x' : '\0';

    char destFile = '\0';
    char destRank = '\0';

    char promotion = '\0';
    char promoRes = '\0';

    char materive = mate ? '#' : '\0';
    char checkive = (!mate && check) ? '+' : '\0';

    switch (result)
    {
    case KINGSIDE_CASTLING:
        SDL_snprintf(moveMade, sizeof(moveMade), "0-0");
        break;
    case QUEENSIDE_CASTLING:
        SDL_snprintf(moveMade, sizeof(moveMade), "0-0-0");
        break;
    default:
        SDL_snprintf(moveMade, sizeof(moveMade), "%c", pieceName);
        destFile = chessX(destTile.x);
        destRank = '0' + destTile.y;
        if (pieceName == KING_NAME)
            break;
        if (pieceName == PAWN_NAME)
        {
            if (capture)
                origFile = chessX(origTile.x);
            if (result == PROMOTION)
            {
                promotion = '=';
                promoRes = 'Q';
            }
            break;
        }

        origFile = chessX(origTile.x);
        origRank = '0' + origTile.y;
    }
    /*Build move.*/
    int currIndex = SDL_strlen(moveMade);
    if (origFile)
        moveMade[currIndex++] = origFile;
    if (origRank)
        moveMade[currIndex++] = origRank;
    if (captiver)
        moveMade[currIndex++] = captiver;
    if (destFile)
        moveMade[currIndex++] = destFile;
    if (destRank)
        moveMade[currIndex++] = destRank;
    if (promotion)
        moveMade[currIndex++] = promotion;
    if (promoRes)
        moveMade[currIndex++] = promoRes;
    if (checkive)
        moveMade[currIndex++] = checkive;
    if (materive)
        moveMade[currIndex++] = materive;
    moveMade[currIndex] = '\0';
    return recordMove(moveMade);
}