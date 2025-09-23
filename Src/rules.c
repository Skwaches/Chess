#include "Linkers/funcs.h"

// Take global values
static Piece movedPiece, tmpPieceFromTile;
static Piece enPawn = {NULL, -1};

static bool playerBool; /*True if white.*/
static PieceNode *playerFamily;
static PieceNode *opponentFamily;
static int valid = false;
Tile destTile, origTile, *playerKingsTile, *opponentKingsTile;
static Tile whiteKingsTile = {5, 1};
static Tile blackKingsTile = {5, 8};
static Tile *fakeMove1, *fakeMove2;
static int enpassble = 0;
static bool whiteCheck = false;
static bool blackCheck = false;

static bool *kingsCheck;
static bool friendlyFire = false;
static bool capturing = false;
// static bool KingChecked = false;
static int xVector, yVector, yDisplacement, xDisplacement;

static CastlingOptions castlingPossibleWhite = {true, true};
static CastlingOptions castlingPossibleBlack = {true, true};
static CastlingOptions *castlingPossible = NULL;

// Helper functions
// Wrapper on SDL_strcmp
bool match_Piece(Piece test, const char *str2)
{
    if (test.ptr == NULL)
    {
        return false;
    }
    return SDL_strcmp(test.ptr->type, str2) ? false : true;
}

// True if you're NOT skipping a piece
bool checkSkip(void)
{

    int tilesSkipped = xDisplacement ? xDisplacement : yDisplacement;
    if (tilesSkipped <= 0)
        return true; // Nothing to skip
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

// Move Logic
// Rules for all pieces
void validateGeneralMoverules(void)
{
    // Move has to be within board (0-0)::::: DONE
    if (destTile.x > X_TILES || destTile.y > Y_TILES ||
        destTile.y < 1 || destTile.x < 1)
    {
        // SDL_Log("Move is out Of range.\n");
        valid = false;
        return;
    }

    // Move has to be 8 pieces or less....
    // This is a shit work around for the fakedelete.
    // Might not be necessarry.
    // Don't delete preferrably.
    if (xDisplacement > 8 || yDisplacement > 8)
    {
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

    ////// ##THIS WAS A BUG. Checking for checks automatically evaluated as false
    /////  ##IT TOOK ME AN UNHEALTHY AMOUNT OF TIME TO FIGURE THIS OUT.
    /////  ##It's 01:36 AM monday. I legit just turned off the lights to sleep.
    /////  ##I don't wanna talk about it.

    // // #King should not be captured.
    // #if (capturing)
    // #{
    //     #if (match_Piece(pieceFromTile(destTile, opponentFamily), KING_NAME))
    //     {
    //         valid = false;
    //         return;
    //     }
    // #}

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
    Tile enPawnDest;

    // Forward movement or One Diagonal
    if (xDisplacement > 1 || yDisplacement > 2 || (playerBool ? (yVector <= 0) : (yVector >= 0)))
    {
        valid = false;
        return;
    }

    if (yDisplacement == 2)
    {
        // Can't capture when moving forward.
        if (capturing)
        {
            valid = false;
            return;
        }
        valid = checkSkip();
        if (!valid)
            return;
        if (xDisplacement || origTile.y != OrigPawnY)
        {
            valid = false;
            return;
        }
        enPawn = movedPiece;
    }

    else // yDisplacement == 1
    {
        // Capturing
        if (xDisplacement)
        {
            // Move has to be capture unless enpassant;
            if (!capturing)
            {
                // enpassant;
                if (enPawn.ptr != NULL)
                {
                    enPawnDest = (Tile){enPawn.ptr->pos[enPawn.index].x, enPawn.ptr->pos[enPawn.index].y + (playerBool ? 1 : -1)};
                    if (destTile.x == enPawnDest.x && destTile.y == enPawnDest.y)
                    {
                        valid = 5;
                        return;
                    }
                }

                // else
                valid = false;
                return;
            }
            valid = true + capturing;
            return;
        }

        // Can't capture without xDisplacement
        else if (capturing)
        {
            valid = false;
            return;
        }
    }
    valid = true + capturing;
    return;
}

void validateKingMove()
{
    // Castling conditions:::::
    //      1. No skipping.
    //      2. No capturing.
    //      3. No previous king or rook move.
    //      4. No y Displacement.
    //      5. Can't castle through check.
    if (checkSkip() && xDisplacement == 2 && yDisplacement == 0 && origTile.x == KING_X[0] && !capturing)
    {
        // Castle;
        if (xVector > 0 && castlingPossible->kingside)
        {
            castlingPossible->kingside = false;
            castlingPossible->queenside = false;
            *playerKingsTile = destTile;
            valid = 3;
            return;
        }

        if (xVector < 0 && castlingPossible->queenside && checkSkip())
        {
            castlingPossible->kingside = false;
            castlingPossible->queenside = false;
            *playerKingsTile = destTile;
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

    // // This part is added to fix bug where pawn's don't seem to affect other check;
    // // Remove this once a proper solution has been found.
    // // This might be a permanent solution lol #holydogwater
    Piece possiblePawn1 = pieceFromTile((Tile){destTile.x + 1, destTile.y + (playerBool ? 1 : -1)}, opponentFamily);
    Piece possiblePawn2 = pieceFromTile((Tile){destTile.x - 1, destTile.y + (playerBool ? 1 : -1)}, opponentFamily);
    if (match_Piece(possiblePawn1, PAWN_NAME) || match_Piece(possiblePawn2, PAWN_NAME))
    {
        valid = false;
        SDL_Log("OH dear God! A pawn!");
        return;
    }

    *playerKingsTile = destTile;
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
    if (!((xDisplacement == 2 && yDisplacement == 1) ||
          (xDisplacement == 1 && yDisplacement == 2)))
    {
        valid = false;
        return;
    }
    valid = true + capturing;
    return;
}

void validateQueenMove()
{
    if ((xDisplacement != yDisplacement) &&
        (xDisplacement != 0 && yDisplacement != 0))
    {
        valid = false;
        return;
    }

    // Diagonals and Straight any distance on the board
    if (xDisplacement >= 2 || yDisplacement >= 2)
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
#pragma endregion

void performValidation()
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
// This checks if a move is valid following BASIC chess rules.
// Checks Pins and enpassant are not checked for.
// A second function MUST be called to finish the validation.
// Returns 0 for invalid move.
// Returns non-zero for valid move
// 1 for no capture.
// 2 for capture.
// 3 for castling kingSide.
// 4 for castling queensSide.
// 5 for enpassant
void initMove(Piece global_piece, Tile global_dest, // Doesn't include check logic///
              bool global_player /*True if white.*/,
              PieceNode *global_playerFamily, PieceNode *global_opponentFamily)
{
    if (global_playerFamily == NULL || global_opponentFamily == NULL)
    {
        SDL_Log("One of the piece families is NULL\n");
        return;
    }

    // Give locals global values.
    movedPiece = global_piece;
    playerBool = global_player;
    playerFamily = global_playerFamily;
    opponentFamily = global_opponentFamily;
    destTile = global_dest;

    // Math part
    if (movedPiece.ptr == NULL || movedPiece.index < 0 ||
        movedPiece.index >= movedPiece.ptr->appearances)
    {
        SDL_Log("moveCalculations: movedPiece invalid\n");
        return;
    }

    // Check
    playerKingsTile = playerBool ? &whiteKingsTile : &blackKingsTile;
    opponentKingsTile = !playerBool ? &whiteKingsTile : &blackKingsTile;

    // to the other team and we want check to refer to them ($o$)
    // Castling
    castlingPossible = playerBool ? &castlingPossibleWhite : &castlingPossibleBlack;

    origTile = movedPiece.ptr->pos[movedPiece.index];
    friendlyFire = TileHasOccupant(destTile, playerFamily);
    capturing = TileHasOccupant(destTile, opponentFamily); // Piece specific.
    xVector = destTile.x - origTile.x;
    yVector = destTile.y - origTile.y;

    yDisplacement = SDL_abs(yVector);
    xDisplacement = SDL_abs(xVector); // Take abs

    return;
}

// Stores original position in a tmp local variable
// Might switch to using the make move function and just undoing it after.....prolly easier.
// But the base logic is the same so I'll leave it like thuis
//  Change of plans...The tmp local variable will be malloced... // For when I want to fake more than one move at once.
//  FREE IS CALLED WHEN UNFAKING;;
// Returns the memory address of the original position
Tile *fakeMove(Piece piece, Tile dest)
{
    Tile *tmpForMove = SDL_malloc(sizeof(Tile));
    *tmpForMove = piece.ptr->pos[piece.index];
    movePiece(piece, dest);
    return tmpForMove;
}

// Resets piece positions from tmp local variable
// Free is called here.
void unfakeMove(Piece piece, Tile *tmpHolder)
{
    movePiece(piece, *tmpHolder);
    SDL_free(tmpHolder);
}

// Moves piece to {-10,-10}
// This is simply abusing the fact that a valid move can't have a displacement>8
// So the piece is effectively deleted
Tile *fakeDelete(Piece piece)
{
    return fakeMove(piece, (Tile){-10, -10});
}

// Performs the appropriate fake move. #holyDogWater
void fakePlay()
{
    int yValueOfPiece = playerBool ? WHITE_Y : BLACK_Y; // For Castling
    switch (valid)
    {
    case 0:
        fakeMove1 = NULL;
        fakeMove2 = NULL;
        break;
    case 1: // Move no capture
        fakeMove1 = fakeMove(movedPiece, destTile);
        fakeMove2 = NULL;
        break;

    case 2: // Move + capture

        fakeMove1 = fakeMove(movedPiece, destTile);
        tmpPieceFromTile = pieceFromTile(destTile, opponentFamily);
        fakeMove2 = fakeDelete(tmpPieceFromTile);
        break;

    case 3: // Castle KingSide
        fakeMove1 = fakeMove(movedPiece, destTile);
        tmpPieceFromTile = pieceFromTile((Tile){ROOK_X[1], yValueOfPiece}, playerFamily);
        fakeMove2 = fakeMove(tmpPieceFromTile, (Tile){6, yValueOfPiece});
        break;
    case 4: // Castle QueenSide
        fakeMove1 = fakeMove(movedPiece, destTile);
        tmpPieceFromTile = pieceFromTile((Tile){ROOK_X[0], yValueOfPiece}, playerFamily);
        fakeMove2 = fakeMove(tmpPieceFromTile, (Tile){4, yValueOfPiece});
        break;
    case 5: // enpassant
        fakeMove1 = fakeMove(movedPiece, destTile);
        Tile niceEn = (Tile){destTile.x, destTile.y + (playerBool ? -1 : 1)};
        tmpPieceFromTile = pieceFromTile(niceEn, opponentFamily);
        fakeMove2 = fakeDelete(tmpPieceFromTile);
        break;
    default:
        fakeMove1 = NULL;
        fakeMove2 = NULL;
        SDL_Log("That move has not been set up yet\n");
        break;
    }
}

// Undo's performed fake move.
// Just rewrite the code in binary at this point #holyDogWater
// Make sure the value of valid is CORRECT
void unfakePlay()
{
    switch (valid)
    {
    case 0: // Invalid move
        break;
    case 1: // Move no capture
        if (fakeMove1 != NULL)
            unfakeMove(movedPiece, fakeMove1);
        break;

    case 2: // Move + capture
        if (fakeMove1 != NULL && fakeMove2 != NULL)
        {
            unfakeMove(movedPiece, fakeMove1);
            unfakeMove(tmpPieceFromTile, fakeMove2);
        }
        break;
    case 3: // Castle KingSide
        if (fakeMove1 != NULL && fakeMove2 != NULL)
        {
            unfakeMove(movedPiece, fakeMove1);
            unfakeMove(tmpPieceFromTile, fakeMove2);
        }
        break;
    case 4: // Castle QueenSide
        if (fakeMove1 != NULL && fakeMove2 != NULL)
        {
            unfakeMove(movedPiece, fakeMove1);
            unfakeMove(tmpPieceFromTile, fakeMove2);
        }
        break;
    case 5: // enpassant
        if (fakeMove1 != NULL && fakeMove2 != NULL)
        {
            unfakeMove(movedPiece, fakeMove1);
            unfakeMove(tmpPieceFromTile, fakeMove2);
        }
        break;
    default:
        fakeMove1 = NULL;
        fakeMove2 = NULL;
        // SDL_Log("Nothing to Undo \n\n");
        break;
    }
}

// Check if any piece from the given team can get to this given tile in ONE MOVE.
// Calculates if your move puts your opponent in Check:
// True if possible false otherwise:
// Pawn don't breaks this logic.
// This function will now be for checking for check.
// It only works if the move is "made".
// Added fakePlay to attempt fake the move and check the output.
// This can be called after the move has been made.
// No harm.
bool setCheck(void)
{
    // Store current valued; //Reset when exiting function
    bool SavedplayerBool = playerBool;
    PieceNode *SavedplayerFamily = playerFamily;
    PieceNode *SavedopponentFamily = opponentFamily;
    Tile SaveddestTile = destTile;
    Piece savedMovedPiece = movedPiece;
    int tmpValid = valid;
    Tile *savedKingsTile = playerKingsTile;
    Tile *SavedOppsKingsTile = opponentKingsTile;

    PieceNode *tempPieceNode = playerFamily;
    kingsCheck = !playerBool ? &whiteCheck : &blackCheck; // Reverse because we are setting Opponent's Check

    while (tempPieceNode != NULL)
    {
        for (int k = 0; k < tempPieceNode->appearances; k++)
        {
            Piece tempPiece = {tempPieceNode, k};
            initMove(tempPiece, *opponentKingsTile, playerBool, playerFamily, opponentFamily);
            performValidation();
            bool simValid = valid;

            // Reset if changed
            valid = tmpValid;
            playerBool = SavedplayerBool;
            playerFamily = SavedplayerFamily;
            opponentFamily = SavedopponentFamily;
            destTile = SaveddestTile;
            movedPiece = savedMovedPiece;
            playerKingsTile = savedKingsTile;
            opponentKingsTile = SavedOppsKingsTile;

            if (simValid)
            {
                *kingsCheck = true;
                return true;
            }
        }
        tempPieceNode = tempPieceNode->next;
    }

    *kingsCheck = false;
    return false;
}

// Calculates if YOUR MOVE puts YOURSELF in check.
// This is an invalid move.
// Fake the play before calling this.
// This should be called BEFORE valid is returned;
// This is one of the general move rules.
// Don't ask me how this shit works
// Simulation shouldn't affect any of the locals;
// It doesn't work with pawns So I just did it manually.... I'm not happy.
bool setBadCheck(void)
{
    // Store current valued; //Reset when exiting function
    bool SavedplayerBool = playerBool;
    PieceNode *SavedplayerFamily = playerFamily;
    PieceNode *SavedopponentFamily = opponentFamily;
    Tile SaveddestTile = destTile;
    Piece savedMovedPiece = movedPiece;
    int tmpValid = valid;
    Tile *savedKingsTile = playerKingsTile;
    Tile *SavedOppsKingsTile = opponentKingsTile;

    PieceNode *tempPieceNode = opponentFamily;

    while (tempPieceNode != NULL)
    {
        for (int k = 0; k < tempPieceNode->appearances; k++)
        {
            Piece tempPiece = {tempPieceNode, k};
            // Special-case pawn attacks
            if (match_Piece(tempPiece, PAWN_NAME) && tmpValid)
            {
                int dir = playerBool ? -1 : +1; // opponent pawn direction
                if ((playerKingsTile->y == tempPieceNode->pos[k].y + dir) &&
                    (playerKingsTile->x == tempPieceNode->pos[k].x + 1 ||
                     playerKingsTile->x == tempPieceNode->pos[k].x - 1))
                {
                    // Reset if changed
                    valid = tmpValid;
                    playerBool = SavedplayerBool;
                    playerFamily = SavedplayerFamily;
                    opponentFamily = SavedopponentFamily;
                    destTile = SaveddestTile;
                    movedPiece = savedMovedPiece;
                    playerKingsTile = savedKingsTile;
                    opponentKingsTile = SavedOppsKingsTile;

                    return true; // pawn attacks king
                }
                continue;
            }

            initMove(tempPiece, *playerKingsTile, playerBool, opponentFamily, playerFamily);
            performValidation();
            bool simValid = valid;

            // Reset if changed
            valid = tmpValid;
            playerBool = SavedplayerBool;
            playerFamily = SavedplayerFamily;
            opponentFamily = SavedopponentFamily;
            destTile = SaveddestTile;
            movedPiece = savedMovedPiece;
            playerKingsTile = savedKingsTile;
            opponentKingsTile = SavedOppsKingsTile;

            if (simValid)
            {
                return true;
            }
        }
        tempPieceNode = tempPieceNode->next;
    }
    return false;
}

// This is setBadCheck customized for castling check;
bool jumpingCheck(void)
{

    if (!(valid == 3 || valid == 4))
    {
        return false;
    }
    // Store current valued; //Reset when exiting function
    bool SavedplayerBool = playerBool;
    PieceNode *SavedplayerFamily = playerFamily;
    PieceNode *SavedopponentFamily = opponentFamily;
    Tile SaveddestTile = destTile;
    Piece savedMovedPiece = movedPiece;
    int tmpValid = valid;
    Tile *savedKingsTile = playerKingsTile;
    Tile *SavedOppsKingsTile = opponentKingsTile;

    PieceNode *tempPieceNode = opponentFamily;
    Tile jumpedForCastling = (Tile){origTile.x + (valid == 3) ? 1 : -1, origTile.y};
    while (tempPieceNode != NULL)
    {
        for (int k = 0; k < tempPieceNode->appearances; k++)
        {
            Piece tempPiece = {tempPieceNode, k};
            // Special-case pawn attacks
            if (match_Piece(tempPiece, PAWN_NAME) && tmpValid)
            {
                int dir = playerBool ? -1 : +1; // opponent pawn direction
                if ((jumpedForCastling.y == tempPieceNode->pos[k].y + dir) &&
                    (jumpedForCastling.x == tempPieceNode->pos[k].x + 1 ||
                     jumpedForCastling.x == tempPieceNode->pos[k].x - 1))
                {
                    // Reset if changed
                    valid = tmpValid;
                    playerBool = SavedplayerBool;
                    playerFamily = SavedplayerFamily;
                    opponentFamily = SavedopponentFamily;
                    destTile = SaveddestTile;
                    movedPiece = savedMovedPiece;
                    playerKingsTile = savedKingsTile;
                    opponentKingsTile = SavedOppsKingsTile;

                    return true; // pawn attacks king
                }
                continue;
            }

            initMove(tempPiece, jumpedForCastling, playerBool, opponentFamily, playerFamily);
            performValidation();
            bool simValid = valid;

            // Reset if changed
            valid = tmpValid;
            playerBool = SavedplayerBool;
            playerFamily = SavedplayerFamily;
            opponentFamily = SavedopponentFamily;
            destTile = SaveddestTile;
            movedPiece = savedMovedPiece;
            playerKingsTile = savedKingsTile;
            opponentKingsTile = SavedOppsKingsTile;

            if (simValid)
            {
                return true;
            }
        }
        tempPieceNode = tempPieceNode->next;
    }
    return false;
}

int finalizeMove(void)
{
    /* Snapshot globals that performValidation() and validators may change */
    Piece savedEnPawn = enPawn;
    CastlingOptions savedWCast = castlingPossibleWhite;
    CastlingOptions savedBCast = castlingPossibleBlack;
    Tile savedWhiteKing = whiteKingsTile;
    Tile savedBlackKing = blackKingsTile;
    int savedValid = valid;
    Piece savedMovedPiece = movedPiece;
    Tile savedDest = destTile;
    PieceNode *savedPlayerFamily = playerFamily;
    PieceNode *savedOpponentFamily = opponentFamily;
    bool savedPlayerBool = playerBool;

    performValidation();

    /* If basic validation failed, restore mutated globals and bail out */
    if (!valid)
    {
        enPawn = savedEnPawn;
        castlingPossibleWhite = savedWCast;
        castlingPossibleBlack = savedBCast;
        whiteKingsTile = savedWhiteKing;
        blackKingsTile = savedBlackKing;
        valid = savedValid;
        movedPiece = savedMovedPiece;
        destTile = savedDest;
        playerFamily = savedPlayerFamily;
        opponentFamily = savedOpponentFamily;
        playerBool = savedPlayerBool;
        return valid;
    }

    fakePlay();
    bool BadCheckMove = setBadCheck();
    bool jumpedCheck = jumpingCheck();
    unfakePlay();

    if (BadCheckMove || jumpedCheck)
    {
        SDL_Log("Don't put yourself in check");
        /* Ensure nothing leaked from validation/simulation */
        enPawn = savedEnPawn;
        castlingPossibleWhite = savedWCast;
        castlingPossibleBlack = savedBCast;
        whiteKingsTile = savedWhiteKing;
        blackKingsTile = savedBlackKing;
        valid = false;
        return valid;
    }

    /* Commit en-passant bookkeeping */
    if (enPawn.ptr != NULL)
        enpassble += 1;

    if (valid == 5 || enpassble == 2)
    {
        enPawn = (Piece){NULL, -1};
        enpassble = 0;
    }

    return valid;
}
