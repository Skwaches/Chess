#include <SDL3/SDL.h>
#include "classes.h"
#include "funcs.h"
#include "constants.h"

// Helper functions
bool match_Piece(Piece test, const char *str2)
{
    return SDL_strcmp(test.ptr->type, str2) ? false : true;
}

Piece pieceFromTile(Tile dest, PieceNode *pieceFamily)
{
    PieceNode *friend = pieceFamily;
    Piece pieceFoundOnTile = {NULL, -1};
    while (friend != NULL)
    {
        for (int k = 0; k < friend->appearances; k++)
        {
            if (dest.x == friend->pos[k].x && dest.y == friend->pos[k].y)
            {
                pieceFoundOnTile = (Piece){friend, k};
                return pieceFoundOnTile;
            }
        }
        friend = friend->next;
    }
    return pieceFoundOnTile;
}

bool tileHasOccupant(Tile dest, PieceNode *pieceFamily)
{
    return pieceFromTile(dest, pieceFamily).index != -1;
}
// CHESS RULES
int validateMove(Piece test, SDL_FPoint *dest,
                 bool player /*True if white.*/,
                 PieceNode *playerFamily, PieceNode *opponentFamily)
{

    bool valid = true;
    Tile destTile = TileFromPos(dest);
    Tile origTile = test.ptr->pos[test.index];
    bool friendlyFire = tileHasOccupant(destTile, playerFamily);
    bool capturing = tileHasOccupant(destTile, opponentFamily); // Piece specific.

    int yDisplacement = player ? destTile.y - origTile.y : origTile.y - destTile.y;
    int xDisplacement = SDL_abs(destTile.x - origTile.x); // Take abs
    // GENERAL RULES

    // Move has to be within board (0-0)::::: DONE
    if (destTile.x > X_TILES || destTile.y > Y_TILES || destTile.y < 1)
    {
        SDL_Log("Move is out Of range.\n");
        valid = false;
        return valid;
    }

    // Move can't be the same position as any of YOUR pieces (0w0)::::DONE
    if (friendlyFire)
    {
        SDL_Log("Friendly Fire is not Allowed!\n");
        valid = false;
        return valid;
    }

    // PIECE SPECIFIC RULES
    // PAWN
    if (match_Piece(test, PAWN_NAME)) //:::DONE
    {
        // can't move more than two spaces forward.
        // Forward for white and black is different
        // Indexing works like cartesian plane. Origin is (1,1) {white left}.

        const int OrigPawnY = player ? WPAWNY : BPAWNY;

        // Forward movement :::: DONE
        if (yDisplacement >= 2)
        {
            if (destTile.y != (player ? OrigPawnY + 2 : OrigPawnY - 2))
            {
                SDL_Log("Pawn can't move more than 2 spaces if not on the starting tile\n");
                valid = false;
                return valid;
            }
        }

        if (yDisplacement < 0)
        {
            SDL_Log("This little soldier must March Forward!\n");
            valid = false;
            return valid;
        }

        if (yDisplacement == 2)
        {
            Tile tileBetween = {origTile.x, origTile.y + (player ? 1 : -1)};
            bool pieceInBetween = tileHasOccupant(tileBetween, playerFamily) || tileHasOccupant(tileBetween, opponentFamily);
            if (pieceInBetween)
            {
                SDL_Log("A pawn cannot jump over other pieces.");
                valid = false;
                return valid;
            }
        }

        // Diagonal Movement // Capturing
        if (xDisplacement != 0)
        {
            // validate possible moves.
            if (yDisplacement == 0)
            {
                SDL_Log("A pawn can only move ahead or diagonally if capturing.\n");
                valid = false;
                return valid;
            }
            if (xDisplacement > 1)
            {
                SDL_Log("A pawn can only move 1 diagonal away when capturing.\n");
                valid = false;
                return valid;
            }
            if (yDisplacement == 2)
            {
                SDL_Log("A pawn can only move 1 diagonal away when capturing\n");
                valid = false;
                return valid;
            }
            if (xDisplacement == 1 && !capturing)
            {
                SDL_Log("A pawn can only move diagonally when capturing\n");
                valid = false;
                return valid;
            }

            // ensure valid capture type moves.
        }

        // Can't capture in a straight line
        if (xDisplacement == 0 && capturing)
        {
            SDL_Log("A pawn can only capture diagonally.\n");
            valid = false;
            return valid;
        }

        // True case.
        return valid + capturing;
    }

    // KING
    if (match_Piece(test, KING_NAME))
    {
        return valid + capturing;
    }

    // QUEEN
    if (match_Piece(test, QUEEN_NAME))
    {
        return valid + capturing;
    }

    // BISHOP
    if (match_Piece(test, BISHOP_NAME))
    {
        return valid + capturing;
    }

    // KNIGHT
    if (match_Piece(test, KNIGHT_NAME))
    {
        return valid + capturing;
    }

    // ROOK
    if (match_Piece(test, ROOK_NAME))
    {
        return valid + capturing;
    }
    return valid + capturing;
}