#include "Linkers/funcs.h"

/**Local holders.
 * This is to let me use the same variables in different functions without any parameters.
 */
static Piece movedPiece;
static bool playerBool; /*True if white.*/
static PieceNode *playerFamily, *opponentFamily;
static Tile destTile;

/**
 * Derived variables
 * Their values are entirely dependent on the initiate values.
 * They change whenever initMove is called.
 */
static bool capturing = false;
static int xVector, yVector, yDisplacement, xDisplacement;
static Tile
    *tmpFakeMove1,
    *tmpFakeMove2,
    origTile, *playerKingsTile, tmpKingsTile,
    *enemyKingsTile;

static CastlingOptions *playerCastling;
static Piece tmpPieceFromTile;

/**
 * Storage items.
 * These should only be changed when the move is valid.
 * Reset these values to default if game is reset.*/
static CastlingOptions whiteCastling = {true, true};
static CastlingOptions blackCastling = {true, true};
static Tile whiteKingsTile = {5, WHITE_Y};
static Tile blackKingsTile = {5, BLACK_Y};
static Piece enPawn = {NULL, -1};
static bool noCastling = false;
static bool enpassable = false;

/*Save movedPiece for fakeplay loaded values*/
static Piece tmpMovedPiece;
static Tile tmpOrigTile, tmpDestTile;
static bool tmpPlayerBool;
static PieceNode *tmpPlayerFamily, *tmpOpponentFamily;

void saveInit(void)
{
    tmpMovedPiece = movedPiece;
    tmpOrigTile = origTile;
    tmpDestTile = destTile;
    tmpPlayerBool = playerBool;
    tmpPlayerFamily = playerFamily;
    tmpOpponentFamily = opponentFamily;
    tmpKingsTile = playerBool ? whiteKingsTile : blackKingsTile;
    /*Assign storage values.*/
    playerKingsTile = playerBool ? &whiteKingsTile : &blackKingsTile;
    enemyKingsTile = !playerBool ? &whiteKingsTile : &blackKingsTile;
    playerCastling = playerBool ? &whiteCastling : &blackCastling;
}

void resetInit(void)
{
    initMove(tmpMovedPiece, tmpOrigTile, tmpDestTile, tmpPlayerBool, tmpPlayerFamily, tmpOpponentFamily);
}

/*Resets logic local storage to initial conditions*/
void resetStorage(void)
{
    whiteCastling = (CastlingOptions){true, true};
    blackCastling = (CastlingOptions){true, true};
    whiteKingsTile = (Tile){5, WHITE_Y};
    blackKingsTile = (Tile){5, BLACK_Y};
    enPawn = (Piece){NULL, -1};
    noCastling = false;
    enpassable = false;
}
/**
 * Returns false if values are invalid.
 * This supplies values for validation.
 * true otherwise
 */

bool initMove(Piece globalPiece, Tile globalOrigTile /*It doesn't recalculate without this.*/, Tile globalDest, // Doesn't include check logic///
              bool globalPlayer /*True if white.*/,
              PieceNode *globalPlayerFamily, PieceNode *globalOpponentFamily)
{
    if (globalPlayerFamily == NULL || globalOpponentFamily == NULL)
    {
        SDL_Log("One of the piece families is NULL\n");
        return false;
    }
    // Give locals global values.
    movedPiece = globalPiece;
    origTile = globalOrigTile;
    destTile = globalDest;
    playerBool = globalPlayer;
    playerFamily = globalPlayerFamily;
    opponentFamily = globalOpponentFamily;
    if (movedPiece.ptr == NULL || movedPiece.index < 0 ||
        movedPiece.index >= movedPiece.ptr->appearances)
    {
        SDL_Log("moveCalculations: movedPiece invalid\n");
        return false;
    }

    // Castling

    capturing = TileHasOccupant(destTile, opponentFamily); // Piece specific.
    xVector = destTile.x - origTile.x;
    yVector = destTile.y - origTile.y;
    yDisplacement = SDL_abs(yVector);
    xDisplacement = SDL_abs(xVector); // Take abs

    return true;
}

// Wrapper on SDL_strcmp
bool match_Piece(Piece test, const char *str2)
{
    if (test.ptr == NULL)
        return false;
    return SDL_strcmp(test.ptr->type, str2) ? false : true;
}

// True if you're skipping a piece
bool skippingPiece(void)
{
    int tilesSkipped = xDisplacement ? xDisplacement : yDisplacement;
    if (tilesSkipped <= 0)
        return false; // Nothing to skip
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
            return true;
        }
        TileBeingChecked += 1;
    }
    return false;
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
// Returns true if it does and false otherwise.
bool TileHasOccupant(Tile dest, PieceNode *pieceFamily)
{
    return pieceFromTile(dest, pieceFamily).index != -1;
}

#pragma region Piece Rules
bool validateGeneralMoverules(void)
{
    // Move has to be within board (0-0)::::: DONE
    if (destTile.x > X_TILES || destTile.y > Y_TILES ||
        destTile.y < 1 || destTile.x < 1)
        return false;

    // Only Knight can jump pieces
    if (!match_Piece(movedPiece, KNIGHT_NAME) && skippingPiece())
        return false;

    if (TileHasOccupant(destTile, playerFamily))
        return false;
    /**Move has to be X_tiles && Y-tiles or less....
     * This is a shit work around for the fakedelete.
     * Might not be necessarry.
     * Don't delete preferrably.
     */
    if (xDisplacement > X_TILES || yDisplacement > Y_TILES)
        return false;
    // You cannot capture your own piece.

    return true;
}
int validatePawnMove(void)
{
    /**can't move more than two spaces forward.
     * Forward for white and black is differentIndexing works like cartesian plane.
     * Origin is (1,1) {white's left}.
     */

    const int OrigPawnY = playerBool ? WPAWNY : BPAWNY;
    // Forward movement or One Diagonal
    if (xDisplacement > 1 || yDisplacement > 2 || (playerBool ? (yVector <= 0) : (yVector >= 0)))
        return INVALID;
    /*Capture must be diagonal.*/
    if (capturing && !xDisplacement)
        return INVALID;
    if (yDisplacement == 2 && (xDisplacement || (origTile.y != OrigPawnY)))
        return INVALID;

    // yDisplacement == 1
    // Capturing
    if (xDisplacement)
    {
        if (!capturing) /*Check enpassant*/
        {
            if (enPawn.ptr != NULL)
            {
                int dir = playerBool ? 1 : -1;
                Tile enDest = {enPawn.ptr->pos[enPawn.index].x, enPawn.ptr->pos[enPawn.index].y + dir};
                if (enpassable && (destTile.x == enDest.x) && (destTile.y == enDest.y))
                    return ENPASSANT;
                return INVALID;
            }
            return INVALID;
        }
        return VALID_CAPTURE;
    }
    // Can't capture without xDisplacement
    else if (capturing)
        return INVALID;
    return VALID + capturing;
}
int validateKingMove(void)
{
    /**
     * Castling conditions:::::
     * No skipping.
     * No capturing.
     * No previous king or rook move.
     * No y Displacement.
     * Can't castle through check.
     * Can't be in check.
     */
    if (xDisplacement == 2 && yDisplacement == 0 && origTile.x == KING_X[0] && !capturing && !noCastling)
    {
        // Castle;
        if (playerCastling->kingside && xVector > 0)
        {
            tmpKingsTile = destTile;
            return KINGSIDE_CASTLING;
        }

        else if (playerCastling->queenside && xVector < 0 &&
                 !TileHasOccupant((Tile){destTile.x - 1, destTile.y}, playerFamily) &&
                 !TileHasOccupant((Tile){destTile.x - 1, destTile.y}, opponentFamily))
        {
            tmpKingsTile = destTile;
            return QUEENSIDE_CASTLING;
        }
    }
    // 1 Move in any Direction.
    if (yDisplacement > 1 || xDisplacement > 1)
        return INVALID;

    tmpKingsTile = destTile;
    return VALID + capturing;
}
int validateRookMove(void)
{
    // Vertical and Horizontal Paths
    // Can't Jump Pieces
    if ((xDisplacement && yDisplacement))
        return INVALID;

    return VALID + capturing;
}
int validateBishopMove(void)
{
    // Diagonal Movement
    if (xDisplacement != yDisplacement)
        return INVALID;
    return VALID + capturing;
}
int validateKnightMove(void)
{
    if ((xDisplacement == 2 && yDisplacement == 1) || (xDisplacement == 1 && yDisplacement == 2))
        return (VALID + capturing);
    return INVALID;
}
int validateQueenMove(void)
{
    if ((xDisplacement != yDisplacement) &&
        (xDisplacement != 0 && yDisplacement != 0))
        return INVALID;
    return VALID + capturing;
}
#pragma endregion

int performValidation(void)
{
    if (!validateGeneralMoverules())
        return INVALID;

    // Pawn
    if (match_Piece(movedPiece, PAWN_NAME))
        return validatePawnMove();

    // KING
    if (match_Piece(movedPiece, KING_NAME))
        return validateKingMove();

    // ROOK
    if (match_Piece(movedPiece, ROOK_NAME))
        return validateRookMove();

    // BISHOP
    if (match_Piece(movedPiece, BISHOP_NAME))
        return validateBishopMove();

    // KNIGHT
    if (match_Piece(movedPiece, KNIGHT_NAME))
        return validateKnightMove();

    // QUEEN
    if (match_Piece(movedPiece, QUEEN_NAME))
        return validateQueenMove();

    // To handle weird piece names
    return INVALID;
}

/**
 * Moves Piece to destination location.
 * Free is called when calling unfakemove()
 *
 * \param piece The piece to be moved temporarily.
 * \param dest The destination to move the piece to.
 *
 * \returns Tile* holding the original position of the piece. Can be NULL
 *
 */
Tile *fakeMove(Piece piece, Tile dest)
{
    Tile *tmpForMove = SDL_malloc(sizeof(Tile));
    *tmpForMove = piece.ptr->pos[piece.index];
    movePiece(piece, dest);
    return tmpForMove;
}
/**
 * Moves piece to Shadow Realm.
 * This is simply abusing the fact that a valid move can't have a displacement>8.
 * So the piece is effectively deleted
 *
 * \param piece The piece to delete
 *
 * \returns Tile* holding original position of the piece. Can be NULL.
 */
Tile *fakeDelete(Piece piece)
{
    Tile *tmpForMove = SDL_malloc(sizeof(Tile));
    *tmpForMove = piece.ptr->pos[piece.index];
    deletePiece(piece);
    return tmpForMove;
}

/**
 * Free is called here.
 * Resets piece positions from tmp local variable
 * \param piece The piece to move back.
 * \param tmpHolder The Tile holding the original position of the piece.
 */
void unfakeMove(Piece piece, Tile *tmpHolder)
{
    movePiece(piece, *tmpHolder);
    SDL_free(tmpHolder);
}

/**
 * Performs the move supplied.
 * Ensure to call unfakePlay.
 * It stores the moved piece.
 */
void fakePlay(int OrigValid)
{
    int yValueOfPiece = playerBool ? WHITE_Y : BLACK_Y; // For Castling
    switch (OrigValid)
    {
    case INVALID:
        tmpFakeMove1 = NULL;
        tmpFakeMove2 = NULL;
        break;
    case VALID: // Move no capture
        tmpFakeMove1 = fakeMove(movedPiece, destTile);
        tmpFakeMove2 = NULL;
        break;
    case VALID_CAPTURE: // Move + capture
        tmpFakeMove1 = fakeMove(movedPiece, destTile);
        tmpPieceFromTile = pieceFromTile(destTile, opponentFamily);
        tmpFakeMove2 = fakeDelete(tmpPieceFromTile);
        break;
    case KINGSIDE_CASTLING: // Castle KingSide
        tmpFakeMove1 = fakeMove(movedPiece, destTile);
        tmpPieceFromTile = pieceFromTile((Tile){ROOK_X[1], yValueOfPiece}, playerFamily);
        tmpFakeMove2 = fakeMove(tmpPieceFromTile, (Tile){6, yValueOfPiece});
        break;
    case QUEENSIDE_CASTLING: // Castle QueenSide
        tmpFakeMove1 = fakeMove(movedPiece, destTile);
        tmpPieceFromTile = pieceFromTile((Tile){ROOK_X[0], yValueOfPiece}, playerFamily);
        tmpFakeMove2 = fakeMove(tmpPieceFromTile, (Tile){4, yValueOfPiece});
        break;
    case ENPASSANT: // enpassant
        tmpFakeMove1 = fakeMove(movedPiece, destTile);
        Tile niceEn = (Tile){destTile.x, destTile.y + (playerBool ? -1 : 1)};
        tmpPieceFromTile = pieceFromTile(niceEn, opponentFamily);
        tmpFakeMove2 = fakeDelete(tmpPieceFromTile);
        break;
    default:
        SDL_Log("%d", OrigValid);
        tmpFakeMove1 = NULL;
        tmpFakeMove2 = NULL;
        SDL_Log("That move has not been set up yet\n");
        break;
    }
}

/**
 * Undo fake moves.
 * Just rewrite the code in binary at this point #holyDogWater
 * Make sure the value of valid and moved Piece is CORRECT
 * \returns true if the unfake play succeeded.
 * false otherwise.
 */
void unfakePlay(void)
{
    if (tmpFakeMove1 != NULL)
        unfakeMove(movedPiece, tmpFakeMove1);
    if (tmpFakeMove2 != NULL)
        unfakeMove(tmpPieceFromTile, tmpFakeMove2);
    tmpFakeMove1 = NULL;
    tmpFakeMove2 = NULL;
}

/**
 * Checks if your move puts you in check.
 * To set it up to validate a move, call fakeplay first.
 * \returns true if possible, false otherwise
 */
bool setBadCheck(Tile kingersTile)
{
    bool savedPlayer = !playerBool;
    PieceNode *friendlies = playerFamily;
    PieceNode *enemies = opponentFamily;
    for (PieceNode *Head = enemies; Head; Head = Head->next)
    {
        for (int a = 0; a < Head->appearances; a++)
        {
            Piece tempPiece = {Head, a};
            if (initMove(tempPiece, Head->pos[a], kingersTile, savedPlayer, enemies, friendlies))
                if (performValidation() != INVALID)
                    return true;
        }
    }
    return false;
}

/**Checks if your move puts the opponent in check.
 * True if it does false otherwiseD
 * This should be called after the play has been made (or faked).
 */
bool setCheck(void)
{
    Tile savedKingsTile = *enemyKingsTile;
    bool savedPlayer = playerBool;
    PieceNode *friendlies = playerFamily;
    PieceNode *enemies = opponentFamily;
    for (PieceNode *Head = friendlies; Head; Head = Head->next)
    {
        for (int a = 0; a < Head->appearances; a++)
        {
            Piece tempPiece = {Head, a};
            if (initMove(tempPiece, Head->pos[a], savedKingsTile, savedPlayer, friendlies, enemies))
                if (performValidation() != INVALID)
                    return true;
        }
    }
    return false;
}

void updateStorage(void)
{
    /**Update stored values when move is valid */
    if (match_Piece(movedPiece, ROOK_NAME))
    {
        // SET SPECIFIC CASTLING SIDE TO FALSE
        int rookYval = playerBool ? WHITE_Y : BLACK_Y;
        if (origTile.y == rookYval)
        {
            if (origTile.x == ROOK_X[0]) /*Queenside*/
                playerCastling->queenside = false;
            else if (origTile.x == ROOK_X[1])
                playerCastling->kingside = false;
        }
    }

    else if (match_Piece(movedPiece, PAWN_NAME))
    {
        if (yDisplacement == 2)
        {
            enPawn = movedPiece;
        }
    }

    else if (match_Piece(movedPiece, KING_NAME))
    {
        tmpKingsTile = destTile;
        *playerKingsTile = destTile;
        *playerCastling = (CastlingOptions){false, false};
    }
}

void enpassantTimer(void)
{
    /*Set enpassant timing.*/
    if (enpassable)
    {
        enPawn = (Piece){NULL, -1};
        enpassable = false;
    }
    if (enPawn.ptr != NULL)
        enpassable = true;
}

// True if castling path crosses a checked square
bool castleThroughCheck(int Originalvalid)
{
    int dir;
    switch (Originalvalid)
    {
    case KINGSIDE_CASTLING:
        dir = 1;
        break;

    case QUEENSIDE_CASTLING:
        dir = -1;
        break;
    default:
        return false;
        break;
    }
    Tile tileJumped = {KING_X[0] + dir, origTile.y};
    saveInit();
    initMove(movedPiece, origTile, tileJumped, playerBool, playerFamily, opponentFamily);
    fakePlay(VALID);
    bool HopperScotch = setBadCheck(tileJumped);
    resetInit();
    unfakePlay();
    return HopperScotch;
}

/**
 * CHESS RULES
 * It runs base of the most recently supplied values.
 * \returns
 * INVALID for invalid move.
 * VALID for no capture.
 * VALID_CAPTURE for capture.
 * KINGSIDE_CASTLING for castling kingSide.
 * QUEENSIDE_CASTLING for castling queensSide.
 * ENPASSANT for enpassant
 * \param updateState determines whether the validation storage should be updated.
 * i.e Whether there the move will be made on the board or not.
 * If set to false the board state will not change on the validation end.
 * Don't change state on front-end if this is set to false.*/
int finalizeMove(bool updateState)
{
    saveInit();
    int calculatedvalid = performValidation();
    if (calculatedvalid == INVALID)
        return INVALID;
    SDL_Log("%d, %d", tmpKingsTile.x, tmpKingsTile.y);

    fakePlay(calculatedvalid);
    if (updateState)
        noCastling = setCheck(); /*Can't castle when in check*/
    bool badCheck = setBadCheck(tmpKingsTile);
    resetInit();
    unfakePlay();

    bool jumpedCheck = castleThroughCheck(calculatedvalid);
    if (badCheck || jumpedCheck)
    {
        playIllegalSound();
        return INVALID;
    }
    if (updateState)
    {
        updateStorage();
        enpassantTimer();
    }
    return calculatedvalid;
}
