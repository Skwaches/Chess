// #include "Linkers/boardHelper.h"
#include "Linkers/funcs.h"
Tile TileFromPos(SDL_FPoint pos)
{
    Tile mousetile = (Tile){pos.x / TILE_WIDTH + 1, Y_TILES - pos.y / TILE_HEIGHT + 1};
    return mousetile;
}

SDL_FRect rectFromTile(Tile cords)
{
    float tempPosx = (cords.x - 1) * TILE_WIDTH;
    float tempPosy = (Y_TILES - cords.y) * TILE_HEIGHT;
    return (SDL_FRect){tempPosx, tempPosy, TILE_WIDTH, TILE_HEIGHT};
}

// Center Rect around pos
SDL_FRect centerRectAroundPos(SDL_FPoint pos)
{
    return (SDL_FRect){pos.x - TILE_WIDTH / 2,
                       pos.y - TILE_HEIGHT / 2,
                       TILE_WIDTH, TILE_WIDTH};
}

//  Since a Piece is drawn on its rect value.
//  The rect is set to a rect built around the mouse position.
//  Piece position is not altered.
// This is because it is used to reset the rect if a move is not made.
void trackMouse(Piece fakePIECE, SDL_FPoint mouse_pos)
{
    fakePIECE.ptr->rect[fakePIECE.index] = centerRectAroundPos(mouse_pos);
}

// Rebuilds the rect of the piece from the position.
void untrackMouse(Piece fakePIECE)
{
    fakePIECE.ptr->rect[fakePIECE.index] = rectFromTile(fakePIECE.ptr->pos[fakePIECE.index]);
}

// Changes Piece pos to destination.
// Rebuilds Piece rect from Position.
void movePiece(Piece fakePIECE, Tile destCoordinates)
{
    fakePIECE.ptr->pos[fakePIECE.index] = destCoordinates;
    untrackMouse(fakePIECE);
}

TileNode *nodeFromTile(Tile coords, TileNode *light, TileNode *dark)
{
    TileNode *tempLight = light;
    TileNode *tempDark = dark;
    while (tempDark != NULL && tempLight != NULL)
    {
        if (tempDark->pos.x == coords.x && tempDark->pos.y == coords.y)
            return tempDark;
        if (tempLight->pos.x == coords.x && tempLight->pos.y == coords.y)
            return tempLight;
        tempDark = tempDark->next;
        tempLight = tempLight->next;
    }
    return NULL;
}

// This sends it to the SHADOW_REALM
void deletePiece(Piece fakePiece)
{
    if (fakePiece.ptr == NULL)
        return;
    fakePiece.ptr->pos[fakePiece.index] = SHADOW_REALM;
}

void promotePiece(Piece fakePiece, char chosenOne, PieceNode *Family, Tile destTile)
{
    if (fakePiece.ptr->type != PAWN_NAME || !chosenOne || chosenOne == KING_NAME)
    {
        SDL_Log("Invalid selection");
        return;
    }
    for (PieceNode *tmpPiece = Family; tmpPiece; tmpPiece = tmpPiece->next)
    {
        if (tmpPiece->type != chosenOne)
            continue;
        tmpPiece->appearances++;
        int last_index = tmpPiece->appearances - 1;
        tmpPiece->pos[last_index] = destTile;
        untrackMouse((Piece){tmpPiece, last_index});
        /*Remove original*/
        deletePiece(fakePiece);
        return;
    }
}

void performMove(int result, Piece playerPiece, Tile destTile, char pawnoGo,
                 PieceNode **playerPieces, PieceNode **opponentPieces, bool player)
{
    int yValueOfPiece = player ? WHITE_Y : BLACK_Y;
    Tile rooksTile, rookDest;
    Piece rookPiece;
    char knownPiece = ROOK_NAME;
    if (result != PROMOTION && result != PROMOTION_CAPTURE)
        movePiece(playerPiece, destTile);
    else
        promotePiece(playerPiece, pawnoGo, *playerPieces, destTile);

    switch (result)
    {
    case VALID: // Move no capture
        break;
    case VALID_CAPTURE: // Move + capture
        deletePiece(pieceFromTile(destTile, *opponentPieces, NULL));
        break;
    case KINGSIDE_CASTLING: // Castle KingSide
        rooksTile = (Tile){ROOK_X[1], yValueOfPiece};
        rookDest = (Tile){6, yValueOfPiece};
        rookPiece = pieceFromTile(rooksTile, *playerPieces, &knownPiece);
        movePiece(rookPiece, rookDest);
        break;
    case QUEENSIDE_CASTLING: // Castle QueenSide
        rooksTile = (Tile){ROOK_X[0], yValueOfPiece};
        rookDest = (Tile){4, yValueOfPiece};
        rookPiece = pieceFromTile(rooksTile, *playerPieces, &knownPiece);
        movePiece(rookPiece, rookDest);
        break;
    case ENPASSANT: // enpassant
        Tile niceEn = (Tile){destTile.x, destTile.y + (player ? -1 : 1)};
        deletePiece(pieceFromTile(niceEn, *opponentPieces, NULL));
        break;
    case PROMOTION:
        break;
    case PROMOTION_CAPTURE:
        deletePiece(pieceFromTile(destTile, *opponentPieces, NULL));
        break;
    default:
        SDL_Log("That move has not been set up yet\n");
        break;
    }
}
/**
 * \returns Average Fps after every {time} seconds
 * since game launch.
 *
 * \param time time to take average for in ms.
 * Defaults to 10000 if 0 is supplied.
 */
int getFPS(Uint64 time)
{
    if (!time)
        time = 10000;
    int fps = 0;
    static int frames = 0;
    static Uint64 timePassed = 0;
    static Uint64 startTime = 0;
    if (startTime)
    {
        timePassed += SDL_GetTicks() - startTime;
        startTime = SDL_GetTicks();
        frames++;
        if (timePassed >= time)
        {
            fps = frames / timePassed;
            fps /= 1000;
            frames = 0;
            return fps;
        }
    }
    else
        startTime = SDL_GetTicks();

    return 0;
}
