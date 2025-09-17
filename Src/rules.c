#include <SDL3/SDL.h>
#include "classes.h"
#include "funcs.h"
#include "constants.h"

// GLOBALS
Piece test;
bool playerBool; /*True if white.*/
PieceNode *playerFamily;
PieceNode *opponentFamily;
Tile destTile, origTile;
bool friendlyFire, capturing;
int xVector, yVector, yDisplacement, xDisplacement;

// Helper functions
bool match_Piece(Piece test, const char *str2)
{
    return SDL_strcmp(test.ptr->type, str2) ? false : true;
}

bool checkSkip(void)
{

    int tilesSkipped = xDisplacement ? xDisplacement : yDisplacement;
    int yUnit = yVector / tilesSkipped;
    int xUnit = xVector / tilesSkipped;
    int TileBeingChecked = 1;
    while (TileBeingChecked < tilesSkipped)
    {
        Tile currentTile = {origTile.x + TileBeingChecked * xUnit,
                            origTile.y + TileBeingChecked * yUnit};
        if (TileHasOccupant(currentTile, opponentFamily) ||
            TileHasOccupant(currentTile, playerFamily))
        {
            return false;
        }
        TileBeingChecked += 1;
    }
    return true;
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

bool TileHasOccupant(Tile dest, PieceNode *pieceFamily)
{
    return pieceFromTile(dest, pieceFamily).index != -1;
}

// CHESS RULES
int validateMove(Piece global_piece, SDL_FPoint *global_dest,
                 bool global_player /*True if white.*/,
                 PieceNode *global_playerFamily, PieceNode *global_opponentFamily)
{
    bool valid = true;
    // Give locals global values.
    test = global_piece;
    playerBool = global_player;
    playerFamily = global_playerFamily;
    opponentFamily = global_opponentFamily;

    destTile = TileFromPos(global_dest);
    origTile = test.ptr->pos[test.index];
    friendlyFire = TileHasOccupant(destTile, playerFamily);
    capturing = TileHasOccupant(destTile, opponentFamily); // Piece specific.
    xVector = destTile.x - origTile.x;
    yVector = destTile.y - origTile.y;
    yDisplacement = SDL_abs(yVector);
    xDisplacement = SDL_abs(xVector); // Take abs
    // GENERAL RULES

    // Move has to be within board (0-0)::::: DONE
    if (destTile.x > X_TILES || destTile.y > Y_TILES || destTile.y < 1)
    {
        SDL_Log("Move is out Of range.\n");
        valid = false;
        return valid;
    }

    if (friendlyFire)
    {
        SDL_Log("Friendly Fire is not Allowed!\n");
        valid = false;
        return valid;
    }

    // King should not be captured.
    if (capturing)
        if (match_Piece(pieceFromTile(destTile, opponentFamily), KING_NAME))
        {
            SDL_Log("Don't touch his highness!");
            valid = false;
            return valid;
        }

    // Friendlies shouldn't let King be in check. INCLUDING the king.
    if (false)
    {
        ////LATER
    }

    // PIECE SPECIFIC RULES

    // PAWN
    if (match_Piece(test, PAWN_NAME)) //:::DONE
    {
        // can't move more than two spaces forward.
        // Forward for white and black is different
        // Indexing works like cartesian plane. Origin is (1,1) {white left}.

        const int OrigPawnY = playerBool ? WPAWNY : BPAWNY;

        // Forward movement :::: DONE
        if (yDisplacement >= 2)
        {
            if (destTile.y != (playerBool ? OrigPawnY + 2 : OrigPawnY - 2))
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

        // CheckSkipping
        if (yDisplacement == 2)
        {
            valid = checkSkip();
            if (!valid)
            {
                SDL_Log("A pawn can't jump over pieces.\n");
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

        return valid + capturing;
    }

    // KING
    if (match_Piece(test, KING_NAME))
    {
        // 1 Move in any Direction.
        if (yDisplacement > 1 || xDisplacement > 1)
        {
            SDL_Log("His majesty grumbles...\n");
            valid = false;
            return valid;
        }

        return valid + capturing;
    }

    // ROOK
    if (match_Piece(test, ROOK_NAME))
    {
        // Vertical and Horizontal Paths
        // Can't Jump Pieces
        if (xVector && yDisplacement)
        { // Straight paths
            SDL_Log("Think more about straight lines maybe...\n");
            valid = false;
            return valid;
        }

        bool checkSkipping = xDisplacement >= 2 || yDisplacement >= 2;

        if (checkSkipping)
        {
            valid = checkSkip();
            if (!valid)
            {
                SDL_Log("A rook cannot jump pieces");
                return valid;
            }
        }
        return valid + capturing;
    }

    // BISHOP
    if (match_Piece(test, BISHOP_NAME))
    {
        // Diagonal Movement
        if (xDisplacement != yDisplacement)
        {
            SDL_Log("Bishops can only move diagonally");
            valid = false;
            return valid;
        }

        // Check Jumping
        if (xDisplacement >= 2)
        {
            valid = checkSkip();
            if (!valid)
            {
                SDL_Log("A Bishop can't jump pieces.\n");
                return valid;
            }
        }
        return valid + capturing;
    }

    // KNIGHT
    if (match_Piece(test, KNIGHT_NAME))
    {
        if (xDisplacement != 1 && yDisplacement != 1)
        {
            valid = false;
            SDL_Log("A knight can only move in an L-shape");
            return valid;
        }
        if ((xDisplacement == 1 && yDisplacement != 2) ||
            (yDisplacement == 1 && xDisplacement != 2))
        {
            valid = false;
            SDL_Log("A knight can only move in an L-shape");
            return valid;
        }
        return valid + capturing;
    }

    // QUEEN
    if (match_Piece(test, QUEEN_NAME))
    { // Diagonals and Straight any distance on the board
        if (xDisplacement >= 2 || yDisplacement >= 2)
        {
            valid = checkSkip();
            if (!valid)
            {
                SDL_Log("A queen can't jump pieces");
                return valid;
            }
        }
        if ((xDisplacement != yDisplacement) &&
            (xDisplacement != 0 && yDisplacement != 0))
        {
            valid = false;
            SDL_Log("A quenn can only move in a straight or diagonal path.\n");
        }
        return valid + capturing;
    }

    return valid + capturing;
}