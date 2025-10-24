#include "../links/funcs.h"
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
    return (SDL_FRect){pos.x - (TILE_WIDTH + PIECE_ZOOM)/ 2,
                       pos.y - (TILE_HEIGHT+ PIECE_ZOOM) / 2,
                       TILE_WIDTH + PIECE_ZOOM, TILE_WIDTH + PIECE_ZOOM};
}

/**
 * Since a Piece is drawn on its rect value.
 * The rect is set to a rect built around the mouse position.
 * Piece position is not altered.
 * This is because it is used to reset the rect if a move is not made.*/
void trackMouse(Piece fakePIECE, SDL_FPoint mouse_pos)
{
    fakePIECE.ptr->rect[fakePIECE.index] = centerRectAroundPos(mouse_pos);
}

/**Rebuilds the rect of the piece from the position.
 * Passing in NULL_PIECE does nothing
 */
void untrackMouse(Piece fakePIECE)
{
    if (!fakePIECE.ptr)
        return;
    fakePIECE.ptr->rect[fakePIECE.index] = rectFromTile(fakePIECE.ptr->pos[fakePIECE.index]);
}

/**
 * Changes Piece pos to destination.
 * Rebuilds Piece rect from Position.
 */
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
    case VALID: 
        break;
    case VALID_CAPTURE: 
        deletePiece(pieceFromTile(destTile, *opponentPieces, NULL));
        break;
    case KINGSIDE_CASTLING:
        rooksTile = (Tile){ROOK_X[1], yValueOfPiece};
        rookDest = (Tile){6, yValueOfPiece};
        rookPiece = pieceFromTile(rooksTile, *playerPieces, &knownPiece);
        movePiece(rookPiece, rookDest);
        break;
    case QUEENSIDE_CASTLING:
        rooksTile = (Tile){ROOK_X[0], yValueOfPiece};
        rookDest = (Tile){4, yValueOfPiece};
        rookPiece = pieceFromTile(rooksTile, *playerPieces, &knownPiece);
        movePiece(rookPiece, rookDest);
        break;
    case ENPASSANT: 
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


int approveSelection(int destOptions, Tile destTile, Tile *validDest, int *validCounters)
{
    if (destOptions)
        for (int k = 0; k < destOptions; k++)
        {
            if (destTile.x == validDest[k].x &&
                destTile.y == validDest[k].y)
            {
                return validCounters[k];
            }
        }
    return INVALID;
}
void highlightValidTiles(int destOptions, Tile *validDest, TileNode *headDarkTile, TileNode *headLightTile)
{
    if (validDest)
        for (int a = 0; a < destOptions; a++)
        {
            TileNode *foundNode = nodeFromTile(validDest[a], headDarkTile, headLightTile);
            if (foundNode)
                foundNode->selected = SELECTED_TILE;
        }
}
void flipBoard(PieceNode** bottom,PieceNode** top){
	for(PieceNode* bott = *bottom;bott; bott = bott->next)
		for(int a = 0; a < bott->appearances; a++)
			bott->pos[a] =(Tile) {X_TILES + 1 - bott->pos[a].x, Y_TILES + 1 -  bott->pos[a].y};
		
	for(PieceNode* topp = *top;topp; topp = topp->next)
		for(int a = 0; a < topp->appearances; a++)
			topp->pos[a] =(Tile) {X_TILES + 1 - topp->pos[a].x, Y_TILES + 1 -  topp->pos[a].y};
		

	redrawFamily(*bottom);
	redrawFamily(*top);
}


