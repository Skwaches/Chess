#include "Linkers/funcs.h"
/**
 * \param selectedPiece The piece to validate.
 * \param family Players pieces.
 * \param enemy Other players pieces.
 * \param player The player making a move.
 * \param noPaths The memory address to store the number of tiles returned.
 * Can be NULL
 * \returns Tile* of valid tiles. Null if no moves are found.
 *  that the supplied piece can get to.
 * Free the output
 */
Tile *validMoves(Piece selectedPiece, PieceNode *family,
                 PieceNode *enemy, bool player, int *noPaths, int *valids)
{
    if (noPaths)
        *noPaths = 0;
    if (!selectedPiece.ptr)
    {
        SDL_Log("Selected Piece is NULL: validMoves");
        return NULL;
    }
    Tile originalTile = selectedPiece.ptr->pos[selectedPiece.index];
    if (originalTile.x == SHADOW_REALM.x || originalTile.y == SHADOW_REALM.y)
        return NULL;
    Tile *hopes = SDL_malloc(sizeof(Tile) * MAX_POSSIBLE_MOVES);
    if (!hopes)
    {
        SDL_Log("Allocation Failed BOT: hopes");
        return NULL;
    }
    int hopePaths = 0;
    Tile destTile;

    int minY = -originalTile.y + 1;
    int minX = -originalTile.x + 1;
    int realMin = (minX <= minY) ? minX : minY;
    int maxY = Y_TILES - originalTile.y;
    int maxX = X_TILES - originalTile.x;
    int realMax = (maxX >= maxY) ? maxX : maxY;
    switch (selectedPiece.ptr->type)
    {
    case PAWN_NAME:
        int dir = player ? 1 : -1;
        for (int y = dir; player ? (y < 3) : (y > -3); y += dir)
        {
            if (y < minY || y > maxY)
                break;
            if ((y == 2 * dir))
            {
                if (originalTile.y == (player ? WPAWNY : BPAWNY))
                    hopes[hopePaths++] = (Tile){0, y};
                continue;
            }
            for (int x = -1; x < 2; x++)
            {
                if (x >= minX && x <= maxX)
                    hopes[hopePaths++] = (Tile){x, y};
            }
        }
        break;
    case KING_NAME:
        int actualYmin = (-1 > minY) ? -1 : minY;
        int actualYmax = (1 <= maxY) ? 1 : maxY;
        int actualXmin = (-2 > minX) ? -2 : minX;
        int actualXmax = (2 <= maxX) ? 2 : maxX;
        for (int y = actualYmin; y <= actualYmax; y++)
            for (int x = actualXmin; x <= actualXmax; x++)
            {
                if (x == 2 || x == -2)
                {
                    if (y ||
                        originalTile.x != KING_X[0] ||
                        originalTile.y != (player ? WHITE_Y : BLACK_Y))
                        continue;
                }
                if (!y && !x)
                    continue;
                hopes[hopePaths++] = (Tile){x, y};
            }
        break;

    case QUEEN_NAME:
        /*Bishop*/
        for (int c = realMin; c <= realMax; c++)
        {
            if (!c || !(c >= minX && c <= maxX))
                continue;
            if ((-c >= minY) && -c <= maxY)
                hopes[hopePaths++] = (Tile){c, -c};

            if ((c >= minY && c <= maxY))
                hopes[hopePaths++] = (Tile){c, c};
        }
        /*Rook*/
        for (int x = minX; x <= maxX; x++)
            if (x)
                hopes[hopePaths++] = (Tile){x, 0};

        for (int y = minY; y <= maxY; y++)
            if (y)
                hopes[hopePaths++] = (Tile){0, y};
        break;
    case KNIGHT_NAME:
        for (int i = -1; i <= 1; i += 2)
        {
            for (int j = -2; j <= 2; j += 4)
            {
                if (i >= minX && i <= maxX &&
                    j >= minY && j <= maxY)
                {
                    Tile pos1 = {i, j};
                    hopes[hopePaths++] = pos1;
                }
                if (j >= minX && j <= maxX &&
                    i >= minY && i <= maxY)
                {
                    Tile pos2 = {j, i};
                    hopes[hopePaths++] = pos2;
                }
            }
        }
        break;
    case BISHOP_NAME:
        for (int c = realMin; c <= realMax; c++)
        {
            if (!c || !(c >= minX && c <= maxX))
                continue;
            if ((-c >= minY) && -c <= maxY)
                hopes[hopePaths++] = (Tile){c, -c};

            if ((c >= minY && c <= maxY))
                hopes[hopePaths++] = (Tile){c, c};
        }
        break;
    case ROOK_NAME:
        for (int x = minX; x <= maxX; x++)
            if (x)
                hopes[hopePaths++] = (Tile){x, 0};

        for (int y = minY; y <= maxY; y++)
            if (y)
                hopes[hopePaths++] = (Tile){0, y};
        break;
    default:
        SDL_Log("Unknown Piece :validMoves");
        break;
    }

    int truths = 0;
    Tile *dreamer = SDL_malloc(sizeof(Tile) * MAX_POSSIBLE_MOVES);
    if (!dreamer)
    {
        SDL_Log("Allocation Error Bot : dreamer");
        return NULL;
    }
    for (int l = 0; l < hopePaths; l++)
    {
        destTile = (Tile){originalTile.x + hopes[l].x, originalTile.y + hopes[l].y};
        initMove(selectedPiece, originalTile, destTile, player, family, enemy, PROMODEFAULT /*Promotion Value is irrelevant here I believe*/);
        int resulter = finalizeMove();
        if (resulter)
        {
            if (valids)
                valids[truths] = resulter;
            dreamer[truths++] = destTile;
        }
    }

    if (noPaths)
        *noPaths = truths;

    SDL_free(hopes);
    if (truths)
    {
        SDL_realloc(dreamer, sizeof(Tile) * truths);
        if (!dreamer)
        {
            SDL_Log("ReAlloc failed BOT: dreamer");
            return NULL;
        }
    }
    else
    {
        SDL_free(dreamer);
        dreamer = NULL;
    }
    return dreamer;
}

/**
 * Checks if your opponent has any valid moves.
 * \param playerFamily Your pieces.
 * \param enemyFamily Enemy's pieces.
 * \param Enemy's playerbool
 * \returns true if no Valid moves are found.
 */
bool checkMate(PieceNode *playerFamily, PieceNode *enemyFamily, bool playerBool)
{
    int noForPiece = 0;
    for (PieceNode *fam = enemyFamily; fam; fam = fam->next)
        for (int a = 0; a < fam->appearances; a++)
        {
            Piece tmpPiece = {fam, a};
            Tile *forPiece = validMoves(tmpPiece, enemyFamily,
                                        playerFamily, playerBool, &noForPiece, NULL);
            if (forPiece)
            {
                SDL_free(forPiece);
                return false;
            }
        }
    return true;
}

Move *selectionPool(PieceNode *botFamily, PieceNode *humanFamily, bool player)
{
    Move *allMoves = SDL_calloc(PIECETYPES, sizeof(Move));
    int noForPiece = 0;
    int types = 0;
    PieceNode *botty = botFamily;
    while (types < PIECETYPES)
    {
        allMoves[types].pieces = botty;
        allMoves[types].dreams = SDL_calloc(botty->appearances, sizeof(Tile *));
        allMoves[types].valids = SDL_calloc(botty->appearances, sizeof(int *));
        allMoves[types].no = SDL_calloc(botty->appearances, sizeof(int));
        for (int a = 0; a < botty->appearances; a++)
        {
            Piece tmpPiece = {botty, a};
            int *validers = SDL_malloc(MAX_POSSIBLE_MOVES * sizeof(int));
            Tile *forPiece = validMoves(tmpPiece, botFamily,
                                        humanFamily, player, &noForPiece, validers);
            SDL_realloc(validers, sizeof(int) * noForPiece);
            allMoves[types].valids[a] = validers;
            allMoves[types].dreams[a] = forPiece;
            allMoves[types].no[a] = noForPiece;
            noForPiece = 0;
        }
        types++;
        botty = botty->next;
        if (!botty)
            break;
    }
    return allMoves;
}

Tile *tileFromPool(Piece piece, Move *pool, int *listLength, int **validers)
{
    for (int a = 0; a < PIECETYPES; a++)
    {
        if (pool[a].pieces->type != piece.ptr->type)
            continue;

        if (validers)
            *validers = pool[a].valids[piece.index];
        if (listLength)
            *listLength = pool[a].no[piece.index];
        return pool[a].dreams[piece.index];
    }
    return NULL;
}

/*This is in dire need of optimisation.*/
Tile randomMoveFromPool(Move *pool, Piece *pieceholder, int *valid)
{
    Sint32 pieceNodeChosen = SDL_rand(PIECETYPES);
    Sint32 indexChosen = SDL_rand(pool[pieceNodeChosen].pieces->appearances);

    int options = pool[pieceNodeChosen].no[indexChosen];
    while (!options)
    {
        pieceNodeChosen = SDL_rand(PIECETYPES);
        indexChosen = SDL_rand(pool[pieceNodeChosen].pieces->appearances);
        options = pool[pieceNodeChosen].no[indexChosen];
    }

    if (pieceholder)
        *pieceholder = (Piece){pool[pieceNodeChosen].pieces, indexChosen};
    Sint32 destTileChosen = SDL_rand(pool[pieceNodeChosen].no[indexChosen]);

    if (valid)
        *valid = pool[pieceNodeChosen].valids[indexChosen][destTileChosen];
    Tile destTile = pool[pieceNodeChosen].dreams[indexChosen][destTileChosen];
    return destTile;
}

void freeMoves(Move *pool)
{
    for (int a = 0; a < PIECETYPES; a++)
    {
        int overall = pool[a].pieces->appearances;
        for (int b = 0; b < overall; b++)
        {
            SDL_free(pool[a].dreams[b]);
            SDL_free(pool[a].valids[b]);
        }
        SDL_free(pool[a].dreams);
        SDL_free(pool[a].no);
    }
    SDL_free(pool);
    pool = NULL;
}

void printMoves(Move *pool)
{
    if (!pool)
        return;
    SDL_Log("Your Moves :");
    for (int a = 0; a < PIECETYPES; a++)
    {
        SDL_Log("%c", pool[a].pieces->type);
        for (int b = 0; b < pool[a].pieces->appearances; b++)
        {
            SDL_Log("%d", b);
            for (int c = 0; c < pool[a].no[b]; c++)
            {
                SDL_Log("%c%d--> %d", chessX(pool[a].dreams[b][c].x),
                        pool[a].dreams[b][c].y, pool[a].valids[b][c]);
            }
            SDL_Log("\n");
        }
        SDL_Log("\n");
    }
}
