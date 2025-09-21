#include "Linkers/funcs.h"

// Take global values
static Piece movedPiece;
static bool playerBool; /*True if white.*/
static PieceNode *playerFamily;
static PieceNode *opponentFamily;
static int valid = false;
static Tile destTile, origTile, *KingsTile;
static Tile whiteKingsTile = {5, 1};
static Tile blackKingsTile = {5, 8};
static bool friendlyFire = false;
static bool capturing = false;
static bool KingChecked = false;
static int xVector, yVector, yDisplacement, xDisplacement;

static CastlingOptions castlingPossibleWhite = {true, true};
static CastlingOptions castlingPossibleBlack = {true, true};
static CastlingOptions *castlingPossible = NULL;

// Helper functions

// Wrapper on SDL_strcmp
bool match_Piece(Piece test, const char *str2)
{
    return SDL_strcmp(test.ptr->type, str2) ? false : true;
}

// True if you're NOT skipping a piece
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

// Returns {NULL -1} if no piece is found
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

// Checks if the destination tile has an occupant from the given family.
// i.e Black or White
// Returns true if it does and zero otherwise.
bool TileHasOccupant(Tile dest, PieceNode *pieceFamily)
{
    return pieceFromTile(dest, pieceFamily).index != -1;
}

bool init_Locals(Piece global_piece, Tile global_dest, // Doesn't include check logic///
                 bool global_player /*True if white.*/,
                 PieceNode *global_playerFamily, PieceNode *global_opponentFamily)
{
    if (global_playerFamily == NULL || global_opponentFamily == NULL)
    {
        SDL_Log("One of the piece families is NULL\n");
        return false;
    }
    // Give locals global values.
    movedPiece = global_piece;
    playerBool = global_player;
    playerFamily = global_playerFamily;
    opponentFamily = global_opponentFamily;
    destTile = global_dest;
    return true;
}

// Move Logic.

// Rules for all pieces
void validateGeneralMoverules(void)
{
    // Move has to be within board (0-0)::::: DONE
    if (destTile.x > X_TILES || destTile.y > Y_TILES || destTile.y < 1)
    {
        // SDL_Log("Move is out Of range.\n");
        valid = false;
        return;
    }

    // You cannot capture your own piece.
    if (friendlyFire)
    {
        // SDL_Log("Friendly Fire is not Allowed!\n");
        valid = false;
        return;
    }

    // King should not be captured.
    if (capturing)
    {
        if (match_Piece(pieceFromTile(destTile, opponentFamily), KING_NAME))
        {
            valid = false;
            return;
        }
    }
    valid = true;
    return;
}

#pragma region Piece Specific

void validatePawnMove()
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

            valid = false;
            return;
        }
    }

    if (yDisplacement < 0)
    {

        valid = false;
        return;
    }

    // CheckSkipping
    if (yDisplacement == 2)
    {
        valid = checkSkip();
        if (!valid)
            return;
    }

    // Diagonal Movement // Capturing
    if (xDisplacement != 0)
    {
        // validate possible moves.
        if (yDisplacement == 0)
        {

            valid = false;
            return;
        }
        if (xDisplacement > 1)
        {

            valid = false;
            return;
        }
        if (yDisplacement == 2)
        {

            valid = false;
            return;
        }
        if (xDisplacement == 1 && !capturing)
        {

            valid = false;
            return;
        }

        // ensure valid capture type moves.
    }

    // Can't capture in a straight line
    if (xDisplacement == 0 && capturing)
    {

        valid = false;
        return;
    }
    valid = true + capturing;
    return;
}

void validateKingMove()
{
    // Castling conditions:::::
    //      1. No skipping
    //      2. No capturing
    //      3. No previous king or rook move
    //      4. No y Displacement
    if (checkSkip() && xDisplacement == 2 && yDisplacement == 0 && origTile.x == KING_X[0] && !capturing)
    {
        // Castle;
        if (xVector > 0 && castlingPossible->kingside)
        {
            castlingPossible->kingside = false;
            castlingPossible->queenside = false;
            *KingsTile = destTile;
            valid = 3;
            return;
        }
        Tile uncheckedTile = (Tile){2, playerBool ? WHITE_Y : BLACK_Y};
        if (xVector < 0 && castlingPossible->queenside && !TileHasOccupant(uncheckedTile, opponentFamily) && !TileHasOccupant(uncheckedTile, playerFamily))
        {
            castlingPossible->kingside = false;
            castlingPossible->queenside = false;
            *KingsTile = destTile;
            valid = 4;
            return;
        }
    }

    // 1 Move in any Direction.
    if (yDisplacement > 1 || xDisplacement > 1)
    {

        valid = false;
        return;
    }

    *KingsTile = destTile;
    valid = true + capturing;
    return;
}

void validateRookMove()
{
    // Vertical and Horizontal Paths
    // Can't Jump Pieces
    if (xDisplacement && yDisplacement)
    {
        // Straight paths

        valid = false;
        return;
    }

    bool checkSkipping = xDisplacement >= 2 || yDisplacement >= 2;

    if (checkSkipping)
    {
        valid = checkSkip();
        if (!valid)
        {

            return;
        }
    }

    // SET SPECIFIC CASTLING SIDE TO FALSE
    if (castlingPossible->queenside)
    {
        if (origTile.x == ROOK_X[0])
        {

            castlingPossible->queenside = false;
        }
    }
    if (castlingPossible->kingside)
    {
        if (origTile.x == ROOK_X[1])
        {
            castlingPossible->kingside = false;
        }
    }
    valid = true + capturing;
    return;
}

void validateBishopMove()
{
    // Diagonal Movement
    if (xDisplacement != yDisplacement)
    {

        valid = false;
        return;
    }

    // Check Jumping
    if (xDisplacement >= 2)
    {
        valid = checkSkip();
        if (!valid)
        {

            return;
        }
    }
    valid = true + capturing;
    return;
}

void validateKnightMove()
{
    if (xDisplacement != 1 && yDisplacement != 1)
    {
        valid = false;

        return;
    }
    if ((xDisplacement == 1 && yDisplacement != 2) ||
        (yDisplacement == 1 && xDisplacement != 2))
    {
        valid = false;

        return;
    }
    valid = true + capturing;
    return;
}

void validateQueenMove()
{
    // Diagonals and Straight any distance on the board
    if (xDisplacement >= 2 || yDisplacement >= 2)
    {
        valid = checkSkip();
        if (!valid)
        {

            return;
        }
    }

    if ((xDisplacement != yDisplacement) &&
        (xDisplacement != 0 && yDisplacement != 0))
    {
        valid = false;
        return;
    }

    valid = true + capturing;
    return;
}
#pragma endregion

void validateMove()
{
    // ALL MOVES
    validateGeneralMoverules();
    if (!valid)
    {
        return;
    }

    // Piece Specific

    // Pawn
    if (match_Piece(movedPiece, PAWN_NAME))
    {
        validatePawnMove();
    }

    // KING
    if (match_Piece(movedPiece, KING_NAME)) // Update King's local position if moved
    {
        validateKingMove();
    }

    // ROOK
    if (match_Piece(movedPiece, ROOK_NAME))
    {
        validateRookMove();
    }

    // BISHOP
    if (match_Piece(movedPiece, BISHOP_NAME))
    {
        validateBishopMove();
    }

    // KNIGHT
    if (match_Piece(movedPiece, KNIGHT_NAME))
    {
        validateKnightMove();
    }

    // QUEEN
    if (match_Piece(movedPiece, QUEEN_NAME))
    {
        validateQueenMove();
    }

    return;
}

//  CHESS RULES
// Returns 0 for invalid move.
// Returns non-zero for valid move
// 1 for no capture.
// 2 for capture.
// 3 for castling kingSide.
// 4 for castling queensSide.

int moveCalculations()
{
    // Check
    static int initKingPos = 0;
    KingsTile = playerBool ? &whiteKingsTile : &blackKingsTile;
    // This is swapped because the one playing is the enemy
    // to the other team and we want check to refer to them ($o$)

    if (initKingPos < 2) // Set original position of the king and update on movement.
    {
        *KingsTile = (Tile){KING_X[0], playerBool ? WHITE_Y : BLACK_Y};
        initKingPos++;
    }

    // Castling
    castlingPossible = playerBool ? &castlingPossibleWhite : &castlingPossibleBlack;

    origTile = movedPiece.ptr->pos[movedPiece.index];
    friendlyFire = TileHasOccupant(destTile, playerFamily);
    capturing = TileHasOccupant(destTile, opponentFamily); // Piece specific.

    xVector = destTile.x - origTile.x;
    yVector = destTile.y - origTile.y;

    yDisplacement = SDL_abs(yVector);
    xDisplacement = SDL_abs(xVector); // Take abs

    validateMove();
    return valid;
}

// Check if any piece from the given team can get to this given tile in ONE MOVE.
// True if possible false otherwise:
bool TileUnderAttack(Tile tileToCheck, PieceNode *opps)
{
    PieceNode *tempPieceNode = opps;
    Piece tempPiece;
    while (tempPieceNode != NULL)
    {
        for (int k = 0; k < tempPieceNode->appearances; k++)
        {
            tempPiece = (Piece){tempPieceNode, k};
            init_Locals(tempPiece, tileToCheck, playerBool, playerFamily, opps);
            if (moveCalculations() != 0)
            {
                return true;
            }
        }
        tempPieceNode = tempPieceNode->next;
    }
    return false;
}

void setCheck()
{

    bool KingChecked = TileUnderAttack(*KingsTile, opponentFamily);
    SDL_Log("%d, %d ", KingsTile->x, KingsTile->y);
    if (KingChecked)
    {
        SDL_Log("A check has occured!");
    }
}
