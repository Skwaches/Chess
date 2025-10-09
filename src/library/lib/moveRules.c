#include "../links/funcs.h"

/**Local holders.
 * This is to let me use the same variables in different functions without any parameters.
 */
static Piece movedPiece;
static bool playerBool; /*True if white.*/
static PieceNode *playerFamily, *opponentFamily;
static Tile destTile;
static char chosenPromo = PROMODEFAULT;
/**
 * Derived variables
 * Their values are entirely dependent on the initiate values.
 * They change whenever initMove is called.
 */
static bool capturing = false;
static int xVector, yVector, yDisplacement, xDisplacement;
static Tile
    origTile,
    *playerKingsTile, tmpKingsTile,
    *enemyKingsTile;
static Tile *tmpFakeMove1 = NULL;
static Tile *tmpFakeMove2 = NULL;
static Tile *tmpFakeMove3 = NULL;
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
static Piece enPawn = NULL_PIECE;
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
    initMove(tmpMovedPiece, tmpOrigTile, tmpDestTile, tmpPlayerBool, tmpPlayerFamily, tmpOpponentFamily, chosenPromo);
}

/*Resets logic local storage to initial conditions*/
void resetStorage(void)
{
    whiteCastling = (CastlingOptions){true, true};
    blackCastling = (CastlingOptions){true, true};
    whiteKingsTile = (Tile){5, WHITE_Y};
    blackKingsTile = (Tile){5, BLACK_Y};
    enPawn = NULL_PIECE;
    noCastling = false;
    enpassable = false;
}
/**
 * Returns false if values are invalid.
 * This supplies values for validation.
 * true otherwise
 */

bool initMove(Piece selectedPiece, Tile originalTile /*It doesn't recalculate without this.*/, Tile globalDest, // Doesn't include check logic///
              bool Currentplayer /*True if white.*/,
              PieceNode *family, PieceNode *enemy, char selectPromo)
{
    if (family == NULL || enemy == NULL)
    {
        SDL_Log("One of the piece families is NULL\n");
        return false;
    }
    // Give locals global values.
    movedPiece = selectedPiece;
    origTile = originalTile;
    destTile = globalDest;
    playerBool = Currentplayer;
    playerFamily = family;
    opponentFamily = enemy;
    chosenPromo = selectPromo;
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

/**
 * \param searchPoint The tile to check.
 * \param pieceFamily The pieces to look through.
 * \param the memory address of the expected piece identifier if known.
 * \returns The piece found on the tile.
 * {NULL -1} if no piece is found.
 */
Piece pieceFromTile(Tile searchPoint, PieceNode *pieceFamily, char *pieceName)
{
    Piece pieceFoundOnTile = NULL_PIECE;
    bool pieceKnown = pieceName;
    for (PieceNode *friend = pieceFamily; friend; friend = friend->next)
    {
        if (pieceKnown && friend->type != *pieceName)
            continue;
        for (int k = 0; k < friend->appearances; k++)
        {
            if (searchPoint.x == friend->pos[k].x && searchPoint.y == friend->pos[k].y)
            {
                pieceFoundOnTile = (Piece){friend, k};
                return pieceFoundOnTile;
            }
        }
    }
    return pieceFoundOnTile;
}

// Checks if the destination tile has an occupant from the given family.
// i.e Black or White
// Returns true if it does and false otherwise.
bool TileHasOccupant(Tile dest, PieceNode *pieceFamily)
{
    return pieceFromTile(dest, pieceFamily, NULL).index != -1;
}

#pragma region Piece Rules
bool validateGeneralMoverules(void)
{
    if (movedPiece.ptr->type != KNIGHT_NAME && skippingPiece())
        return false;
    if (destTile.x > X_TILES || destTile.y > Y_TILES ||
        destTile.x < 1 || destTile.y < 1)
        return false;
    if (TileHasOccupant(destTile, playerFamily))
        return false;
    if (xDisplacement > X_TILES || yDisplacement > Y_TILES)
        return false;

    return true;
}
int validatePawnMove(void)
{
    const int OrigPawnY = playerBool ? WPAWNY : BPAWNY;
    if (xDisplacement > 1 || yDisplacement > 2 ||
        (playerBool ? (yVector <= 0) : (yVector >= 0)))
        return INVALID;

    if (yDisplacement == 2 && (xDisplacement || (origTile.y != OrigPawnY)))
        return INVALID;

    if (!xDisplacement && capturing)
        return INVALID;

    if (xDisplacement && !capturing)
    {
        if (enPawn.ptr != NULL)
        {
            int dir = playerBool ? 1 : -1;
            Tile enDest = {enPawn.ptr->pos[enPawn.index].x,
                           enPawn.ptr->pos[enPawn.index].y + dir};
            if (enpassable && (destTile.x == enDest.x) && (destTile.y == enDest.y))
                return ENPASSANT;
        }
        return INVALID;
    }

    if (destTile.y == 1 || destTile.y == Y_TILES)
        return PROMOTION + capturing;

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
    if (xDisplacement == 2 && yDisplacement == 0 &&
        origTile.x == KING_X[0] && !capturing && !noCastling)
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
    if ((xDisplacement == 2 && yDisplacement == 1) ||
        (xDisplacement == 1 && yDisplacement == 2))
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

int performValidation()
{
    if (!validateGeneralMoverules())
        return INVALID;
    switch (movedPiece.ptr->type)
    {
    case PAWN_NAME:
        return validatePawnMove();
    case KING_NAME:
        return validateKingMove();
    case ROOK_NAME:
        return validateRookMove();
    case QUEEN_NAME:
        return validateQueenMove();
    case BISHOP_NAME:
        return validateBishopMove();
    case KNIGHT_NAME:
        return validateKnightMove();
    default:
        SDL_Log("That Piece is unknown : %c", movedPiece.ptr->type);
        return INVALID;
    }
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
    if (!tmpForMove)
        SDL_Log("Allocation Failed : fakeMove");
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
    if (!tmpForMove)
        SDL_Log("Allocation Failed : fakeDelete");
    *tmpForMove = piece.ptr->pos[piece.index];
    deletePiece(piece);
    return tmpForMove;
}
Tile *fakePromotion(Piece piece, char chosenChar)
{
    for (PieceNode *tmpPiece = playerFamily; tmpPiece; tmpPiece = tmpPiece->next)
    {
        if (tmpPiece->type != chosenChar)
            continue;
        tmpPiece->pos[tmpPiece->appearances] = destTile;
        tmpPiece->appearances++;
    }
    return fakeDelete(piece);
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
void unfakePromotion(Piece piece, Tile *promo, char chosenChar)
{
    unfakeMove(piece, promo);
    for (PieceNode *tmpPiece = playerFamily; tmpPiece; tmpPiece = tmpPiece->next)
    {
        if (tmpPiece->type != chosenChar)
            continue;
        tmpPiece->appearances--;
    }
}
/**
 * Performs the move supplied.
 * Ensure to call unfakePlay.
 * It stores the moved piece.
 */
void fakePlay(int OrigValid)
{
    tmpFakeMove1 = NULL;
    tmpFakeMove2 = NULL;
    tmpFakeMove3 = NULL;
    if (OrigValid == INVALID)
        return;
    if (OrigValid != PROMOTION && OrigValid != PROMOTION_CAPTURE)
        tmpFakeMove1 = fakeMove(movedPiece, destTile);
    else
        tmpFakeMove3 = fakePromotion(movedPiece, chosenPromo);
    char knownRook = ROOK_NAME;
    Tile rookTile, rookDest;
    int yValueOfPiece = playerBool ? WHITE_Y : BLACK_Y; // For Castling
    switch (OrigValid)
    {
    case VALID: // Move no capture
        break;
    case VALID_CAPTURE: // Move + capture
        tmpPieceFromTile = pieceFromTile(destTile, opponentFamily, NULL);
        tmpFakeMove2 = fakeDelete(tmpPieceFromTile);
        break;
    case KINGSIDE_CASTLING: // Castle KingSide
        rookTile = (Tile){ROOK_X[1], yValueOfPiece};
        rookDest = (Tile){6, yValueOfPiece};
        tmpPieceFromTile = pieceFromTile(rookTile, playerFamily, &knownRook);
        tmpFakeMove2 = fakeMove(tmpPieceFromTile, rookDest);
        break;
    case QUEENSIDE_CASTLING: // Castle QueenSide
        rookTile = (Tile){ROOK_X[0], yValueOfPiece};
        rookDest = (Tile){4, yValueOfPiece};
        tmpPieceFromTile = pieceFromTile(rookTile, playerFamily, &knownRook);
        tmpFakeMove2 = fakeMove(tmpPieceFromTile, rookDest);
        break;
    case ENPASSANT: // enpassant
        Tile niceEn = (Tile){destTile.x, destTile.y + (playerBool ? -1 : 1)};
        tmpPieceFromTile = pieceFromTile(niceEn, opponentFamily, NULL);
        tmpFakeMove2 = fakeDelete(tmpPieceFromTile);
        break;
    case PROMOTION:
        break;
    case PROMOTION_CAPTURE:
        tmpPieceFromTile = pieceFromTile(destTile, opponentFamily, NULL);
        tmpFakeMove2 = fakeDelete(tmpPieceFromTile);
        break;
    default:
        SDL_Log("%d", OrigValid);
        tmpFakeMove1 = NULL;
        tmpFakeMove2 = NULL;
        tmpFakeMove3 = NULL;
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
    if (tmpFakeMove1)
        unfakeMove(movedPiece, tmpFakeMove1);
    if (tmpFakeMove2)
        unfakeMove(tmpPieceFromTile, tmpFakeMove2);
    if (tmpFakeMove3)
        unfakePromotion(movedPiece, tmpFakeMove3, chosenPromo);
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
    char myPromo = chosenPromo;
    for (PieceNode *Head = enemies; Head; Head = Head->next)
    {
        for (int a = 0; a < Head->appearances; a++)
        {
            Piece tempPiece = {Head, a};
            if (initMove(tempPiece, Head->pos[a], kingersTile,
                         savedPlayer, enemies, friendlies, myPromo))
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
    char myPromo = chosenPromo;
    for (PieceNode *Head = friendlies; Head; Head = Head->next)
    {
        for (int a = 0; a < Head->appearances; a++)
        {
            Piece tempPiece = {Head, a};
            if (initMove(tempPiece, Head->pos[a], savedKingsTile, savedPlayer, friendlies, enemies, myPromo))
                if (performValidation() != INVALID)
                    return true;
        }
    }
    return false;
}

void updateStorage(void)
{
    /**Update stored values when move is valid */
    switch (movedPiece.ptr->type)
    {
    case ROOK_NAME:
        // SET SPECIFIC CASTLING SIDE TO FALSE
        int rookYval = playerBool ? WHITE_Y : BLACK_Y;
        if (origTile.y == rookYval)
        {
            if (playerCastling->queenside && origTile.x == ROOK_X[0]) /*Queenside*/
                playerCastling->queenside = false;
            else if (playerCastling->kingside && origTile.x == ROOK_X[1])
                playerCastling->kingside = false;
            else
                break;
        }
        break;
    case PAWN_NAME:
        if (yDisplacement == 2)
            enPawn = movedPiece;
        break;
    case KING_NAME:
        tmpKingsTile = destTile;
        *playerKingsTile = destTile;
        if (playerCastling->queenside || playerCastling->kingside)
            *playerCastling = (CastlingOptions){false, false};
        break;
    default:
        break;
    }
}

void enpassantTimer(void)
{
    /*Set enpassant timing.*/
    if (enpassable)
    {
        enPawn = NULL_PIECE;
        enpassable = false;
    }
    if (enPawn.ptr != NULL)
        enpassable = true;
}

/**
 * True if castling path crosses a checked square
 * \param bot Whether the bot is validating or not.
 * Set false only for the bot.
 */
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
    initMove(movedPiece, origTile, tileJumped, playerBool, playerFamily, opponentFamily, chosenPromo);
    fakePlay(VALID);
    bool HopperScotch = setBadCheck(tileJumped);
    resetInit();
    unfakePlay();
    return HopperScotch;
}

/**
 * \returns
 * INVALID for invalid move.
 * VALID for no capture.
 * VALID_CAPTURE for capture.
 * KINGSIDE_CASTLING for castling kingSide.
 * QUEENSIDE_CASTLING for castling queensSide.
 * ENPASSANT for enpassant
 * PROMOTION for a promotion.
 * PROMOTION_CAPTURE for capture resulting in promotion.
 * */
int finalizeMove(void)
{
    saveInit();
    int calculatedvalid = performValidation();
    if (calculatedvalid == INVALID)
        return INVALID;

    fakePlay(calculatedvalid);
    bool badCheck = setBadCheck(tmpKingsTile);
    resetInit();
    unfakePlay();
    bool jumpedCheck = castleThroughCheck(calculatedvalid);
    if (badCheck)
    {
        return INVALID;
    }
    if (jumpedCheck)
    {
        return INVALID;
    }
    return calculatedvalid;
}

/*This should be called before the move has been made*/
void fullLogicUpdate(int valid, bool *cheekers)
{
    saveInit();
    fakePlay(valid);
    noCastling = setCheck(); /*Can't castle when in check*/
    resetInit();
    if (cheekers)
        *cheekers = noCastling;
    unfakePlay();
    updateStorage();
    enpassantTimer();
}