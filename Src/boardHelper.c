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
    {
        return;
    }
    fakePiece.ptr->pos[fakePiece.index] = SHADOW_REALM;
}

void promotePiece(Piece fakePiece, char chosenOne, PieceNode *Family, Tile destTile)
{
    if (fakePiece.ptr->type != PAWN_NAME || !chosenOne)
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
    static int frames = 0;
    static int fps = 0;
    static Uint64 timePassed = 0;
    static Uint64 startTime = 0;
    if (startTime)
    {
        timePassed += SDL_GetTicks() - startTime;
        startTime = SDL_GetTicks();
        frames++;
        if (timePassed >= time)
        {
            fps = frames / (timePassed / 1000);
            frames = 0;
            timePassed = 0;
        }
    }
    else
        startTime = SDL_GetTicks();
    return fps;
}
