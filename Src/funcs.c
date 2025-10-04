#include "Linkers/funcs.h"

Tile TileFromPos(SDL_FPoint *pos)
{
    Tile mousetile = (Tile){pos->x / TILE_WIDTH + 1, Y_TILES - pos->y / TILE_HEIGHT + 1};
    return mousetile;
}

SDL_FRect rectFromTile(Tile cords)
{
    float tempPosx = (cords.x - 1) * TILE_WIDTH;
    float tempPosy = (Y_TILES - cords.y) * TILE_HEIGHT;
    return (SDL_FRect){tempPosx, tempPosy, TILE_WIDTH, TILE_HEIGHT};
}

// {NULL ,-1} if not found
// This was a GUARANTEED MEMORY leak lol. ///Prolly fixed now
// This is sooo Assss
Piece pieceFromPos(PieceNode *HeadPiece, SDL_FPoint *pos)
{
    PieceNode *TempPiece = HeadPiece;

    while (TempPiece != NULL)
    {
        for (int k = 0; k < TempPiece->appearances; k++)
        {
            if (SDL_PointInRectFloat(pos, &TempPiece->rect[k]))
            {
                Piece selection = {TempPiece, k};
                return selection;
            }
        }
        TempPiece = TempPiece->next;
    }
    return (Piece){NULL, -1};
}

// Center Rect around pos
SDL_FRect centerRectAroundPos(SDL_FPoint *pos)
{
    return (SDL_FRect){pos->x - TILE_WIDTH / 2, pos->y - TILE_HEIGHT / 2, TILE_WIDTH, TILE_WIDTH};
}

//  Since a Piece is drawn on its rect value.
//  The rect is set to a rect built around the mouse position.
//  Piece position is not altered.
// This is because it is used to reset the rect if a move is not made.
void trackMouse(Piece fakePIECE, SDL_FPoint *mouse_pos)
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

// Wrapper on MovePiece Function
// Takes a SDL_FPoint instead of a Tile
void movePieceFromPos(Piece fakePIECE, SDL_FPoint *pos)
{
    movePiece(fakePIECE, TileFromPos(pos));
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
    {
        return;
    }
    fakePiece.ptr->pos[fakePiece.index] = SHADOW_REALM;
}