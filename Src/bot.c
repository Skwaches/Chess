#include "Linkers/funcs.h"
#include "Linkers/bot.h"
/**
 * \param selectedPiece The piece to validate.
 * \param family The head* of the player PieceNode.
 * \param enemy The head* of the enemy PieceNode.
 * \param player The player making a move.
 * \param noPaths The memory address to store the number of tiles returned.
 * Can be NULL
 * \returns Tile* of valid tiles. Null if no moves are found.
 *  that the supplied piece can get to.
 * Free the output
 */
Tile *validMoves(Piece selectedPiece, PieceNode *family,
                 PieceNode *enemy, bool player, int *noPaths)
{
    if (noPaths)
        *noPaths = 0;
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
        for (int y = (-1 > minY) ? -1 : minY; y < 2; y++)
            for (int x = (-2 > minX) ? -2 : minX; x <= 2; x++)
            {
                if (x == 2 || x == -2)
                {
                    if (y ||
                        originalTile.x != KING_X[0] ||
                        originalTile.y != (player ? WHITE_Y : BLACK_Y))
                        continue;
                }
                if (y == 0 && x == 0)
                    continue;
                hopes[hopePaths++] = (Tile){x, y};
            }
        break;

    case QUEEN_NAME:
        for (int c = realMin; c <= realMax; c++)
        {
            if (!c || !(c >= minX && c <= maxX))
                continue;
            if ((-c >= minY) && -c <= maxY)
                hopes[hopePaths++] = (Tile){c, -c};

            if ((c >= minY && c <= maxY))
                hopes[hopePaths++] = (Tile){c, c};
        }
        // Rook
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
        // Rook
        for (int x = minX; x <= maxX; x++)
            if (x)
                hopes[hopePaths++] = (Tile){x, 0};

        for (int y = minY; y <= maxY; y++)
            if (y)
                hopes[hopePaths++] = (Tile){0, y};
        break;
    default:
        SDL_Log("Unknown Piece");
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
        initMove(selectedPiece, originalTile, destTile, player, family, enemy);
        if (finalizeMove(false, NULL) != INVALID)
            dreamer[truths++] = destTile;
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
 * Checks if you have Any valid moves.
 * \param playerFamily The pieces of the player whose moves are being evaluated.
 * \param enemyFamily The other player's pieces.
 * \param Your playerbool
 * \returns true if no Valid moves are found.
 */
bool checkMate(PieceNode *playerFamily, PieceNode *enemyFamily, bool playerBool)
{
    int noForPiece = 0;
    for (PieceNode *fam = playerFamily; fam; fam = fam->next)
        for (int a = 0; a < fam->appearances; a++)
        {
            Piece tmpPiece = {fam, a};
            Tile *forPiece = validMoves(tmpPiece, playerFamily,
                                        enemyFamily, playerBool, &noForPiece);
            if (forPiece)
            {
                SDL_free(forPiece);
                return false;
            }
        }
    return true;
}

Tile *selectionPool(PieceNode *botFamily, PieceNode *humanFamily, bool player, int *noMoves)
{
    Tile *allMoves = SDL_malloc(MAX_TOTAL_POSSIBLE_MOVES);
    if (noMoves)
        *noMoves = 0;
    int noForPiece = 0;
    int totalMoves = 0;
    for (PieceNode *botty = botFamily; botty; botty = botty->next)
        for (int a = 0; a < botty->appearances; a++)
        {
            Piece tmpPiece = {botty, a};
            Tile *forPiece = validMoves(tmpPiece, botFamily,
                                        humanFamily, player, &noForPiece);
            for (int k = 0; k < noForPiece; k++)
            {
                allMoves[totalMoves++] = forPiece[k];
            }
            noForPiece = 0;
            SDL_free(forPiece);
        }
    if (noMoves)
        *noMoves = totalMoves;
    return allMoves;
}