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
                if (hopePaths >= MAX_POSSIBLE_MOVES)
                    break;
                if (originalTile.y == (player ? WPAWNY : BPAWNY))
                    hopes[hopePaths++] = (Tile){0, y};

                continue;
            }
            for (int x = -1; x < 2; x++)
            {
                if (hopePaths >= MAX_POSSIBLE_MOVES)
                    break;
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
                if (hopePaths >= MAX_POSSIBLE_MOVES)
                    break;
                hopes[hopePaths++] = (Tile){x, y};
            }
        break;
    case QUEEN_NAME:
        for (int c = realMin; c <= realMax; c++)
        {
            if (!c || !(c >= minX && c <= maxX))
                continue;
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
            if ((-c >= minY) && -c <= maxY)
                hopes[hopePaths++] = (Tile){c, -c};
            if ((c >= minY && c <= maxY))
                hopes[hopePaths++] = (Tile){c, c};
        }
        /*Rook*/
        for (int x = minX; x <= maxX; x++)
        {
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
            if (x)
                hopes[hopePaths++] = (Tile){x, 0};
        }

        for (int y = minY; y <= maxY; y++)
        {
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
            if (y)
                hopes[hopePaths++] = (Tile){0, y};
        }
        break;
    case BISHOP_NAME:
        for (int c = realMin; c <= realMax; c++)
        {
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
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
        {
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
            if (x)
                hopes[hopePaths++] = (Tile){x, 0};
        }

        for (int y = minY; y <= maxY; y++)
        {
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
            if (y)
                hopes[hopePaths++] = (Tile){0, y};
        }
        break;
    case KNIGHT_NAME:
        for (int i = -1; i <= 1; i += 2)
        {
            if (hopePaths >= MAX_POSSIBLE_MOVES)
                break;
            for (int j = -2; j <= 2; j += 4)
            {
                if (hopePaths >= MAX_POSSIBLE_MOVES)
                    break;
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

    default:
        SDL_Log("Unknown Piece :validMoves");
        break;
    }

    int truths = 0;
    Tile *dreamer = SDL_malloc(sizeof(Tile) * MAX_POSSIBLE_MOVES);
    if (!dreamer)
    {
        SDL_free(hopes);
        SDL_Log("Allocation Error Bot : dreamer");
        return NULL;
    }

    for (int l = 0; l < hopePaths; l++)
    {
        destTile = (Tile){originalTile.x + hopes[l].x, originalTile.y + hopes[l].y};
        initMove(selectedPiece, originalTile, destTile, player, family, enemy, PROMODEFAULT /*Promotion Value is irrelevant here I believe*/);
        int resulter = finalizeMove();
        if (resulter && (truths < MAX_POSSIBLE_MOVES))
        {
            if (valids)
                valids[truths] = resulter;
            dreamer[truths++] = destTile;
        }
    }

    SDL_free(hopes);
    if (!truths)
    {
        SDL_free(dreamer);
        return NULL;
    }
    Tile *tmpLucid = SDL_realloc(dreamer, sizeof(Tile) * truths);
    if (!tmpLucid)
    {
        SDL_Log("ReAlloc failed BOT: dreamer");
        SDL_free(dreamer);
        return NULL;
    }
    if (noPaths)
        *noPaths = truths;
    dreamer = tmpLucid;
    return dreamer;
}

void freeMoves(Move *pool)
{
    if (!pool)
        return;
    for (int a = 0; a < PIECETYPES; a++)
    {
        if (!pool[a].pieces)
            continue;
        int overall = pool[a].pieces->appearances;
        for (int b = 0; b < overall; b++)
        {
            if (pool[a].no && pool[a].no[b])
            {
                if (pool[a].dreams)
                    SDL_free(pool[a].dreams[b]);
                if (pool[a].valids)
                    SDL_free(pool[a].valids[b]);
            }
        }
        SDL_free(pool[a].no);
        SDL_free(pool[a].dreams);
        SDL_free(pool[a].valids);
    }
    SDL_free(pool);
}

Move *selectionPool(PieceNode *botFamily, PieceNode *humanFamily, bool player)
{
    Move *allMoves = SDL_calloc(PIECETYPES, sizeof(Move));
    if (!allMoves)
        return NULL;
    int noForPiece = 0;
    int types = 0;
    PieceNode *botty = botFamily;
    while (types < PIECETYPES && botty)
    {
        allMoves[types].pieces = botty;
        allMoves[types].dreams = SDL_calloc(botty->appearances, sizeof(Tile *));
        allMoves[types].valids = SDL_calloc(botty->appearances, sizeof(int *));
        allMoves[types].no = SDL_calloc(botty->appearances, sizeof(int));
        if (!allMoves[types].dreams || !allMoves[types].valids || !allMoves[types].no)
        {
            freeMoves(allMoves);
            SDL_free(allMoves);
            return NULL;
        }
        for (int a = 0; a < botty->appearances; a++)
        {
            Piece tmpPiece = {botty, a};
            int *validers = SDL_malloc(MAX_POSSIBLE_MOVES * sizeof(int));
            if (!validers)
            {
                allMoves[types].valids[a] = NULL;
                allMoves[types].dreams[a] = NULL;
                allMoves[types].no[a] = 0;
                continue;
            }
            Tile *forPiece = validMoves(tmpPiece, botFamily,
                                        humanFamily, player, &noForPiece, validers);
            if (forPiece && noForPiece > 0)
            {
                int *tmpValiders = SDL_realloc(validers, sizeof(int) * noForPiece);
                if (tmpValiders)
                    validers = tmpValiders;
                allMoves[types].valids[a] = validers;
                allMoves[types].dreams[a] = forPiece;
                allMoves[types].no[a] = noForPiece;
            }
            else
            {
                SDL_free(validers);
                allMoves[types].valids[a] = NULL;
                allMoves[types].dreams[a] = NULL;
                allMoves[types].no[a] = 0;
            }
            noForPiece = 0;
        }
        types++;
        botty = botty->next;
    }
    return allMoves;
}

/**
 * Checks if any moves are available in the pool.
 * \returns true if no Valid moves are found.
 */
bool checkMate(Move *pool)
{
    if (!pool)
        return false;
    for (int a = 0; a < PIECETYPES; a++)
        for (int b = 0; b < pool[a].pieces->appearances; b++)
            if (pool[a].no[b])
                return false;
    return true;
}

bool checkstale(PieceNode *playerFamily, PieceNode *enemyFamily)
{
    PieceNode *player = playerFamily;
    PieceNode *enemy = enemyFamily;
    while (player && enemy)
    {
        if (player->type != KING_NAME)
            for (int a = 0; a < player->appearances; a++)
                if (player->pos[a].x != SHADOW_REALM.x &&
                    player->pos[a].y != SHADOW_REALM.y)
                    return false;

        if (enemy->type != KING_NAME)
            for (int a = 0; a < enemy->appearances; a++)
                if (enemy->pos[a].x != SHADOW_REALM.x &&
                    enemy->pos[a].y != SHADOW_REALM.y)
                    return false;

        player = player->next;
        enemy = enemy->next;
    }
    return true;
}

Tile *tileFromPool(Piece piece, Move *pool, int *listLength, int **validers)
{
    for (int a = 0; a < PIECETYPES; a++)
    {
        if (pool[a].pieces->type != piece.ptr->type)
            continue;
        if (listLength)
            *listLength = pool[a].no[piece.index];
        if (!pool[a].no[piece.index])
            return NULL;
        if (validers)
            *validers = pool[a].valids[piece.index];

        return pool[a].dreams[piece.index];
    }
    return NULL;
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
