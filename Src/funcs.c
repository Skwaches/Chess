#include "Linkers/funcs.h"

SDL_Texture *maketexture(SDL_Renderer *renderer, const char *path)
{

    SDL_IOStream *io = SDL_IOFromFile(path, "rb");
    if (io == NULL)
    {
        return NULL;
    }
    if (IMG_isSVG(io) != 0)
    {
        SDL_Surface *surf = IMG_LoadSizedSVG_IO(io, (int)TILE_WIDTH, (int)TILE_HEIGHT);
        if (surf == NULL)
        {
            SDL_CloseIO(io);
            return NULL;
        }

        SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);
        SDL_CloseIO(io);
        return text;
    }
    else
    {
        SDL_Texture *text = IMG_LoadTexture_IO(renderer, io, true);
        return text;
    }
}

Tile TileFromPos(SDL_FPoint *pos)
{
    Tile mousetile = (Tile){pos->x / TILE_WIDTH + 1, Y_TILES - pos->y / TILE_HEIGHT + 1};
    return mousetile;
}

SDL_FRect rectFromTile(Tile cords)
{
    float tempPosx = (cords.x - 1) * TILE_WIDTH;
    float tempPosy = (8 - cords.y) * TILE_HEIGHT;
    return (SDL_FRect){tempPosx, tempPosy, TILE_WIDTH, TILE_HEIGHT};
}

PieceNode *makePieceNode(SDL_Renderer *renderer,
                         char *buffer,
                         PieceNode *tempPiece,
                         const int appearances,
                         const char *name,
                         const int *x,
                         const int y)
{
    // tempPiece->noInPlay = appearances;
    tempPiece->appearances = appearances;
    tempPiece->type = name;
    Tile *positions = SDL_malloc(appearances * sizeof(Tile));
    if (positions == NULL)
    {
        return NULL;
    }
    for (int p = 0; p < appearances; p++)
    {
        positions[p] = (Tile){x[p], y};
    }

    tempPiece->pos = positions;
    tempPiece->texture = maketexture(renderer, buffer);
    tempPiece->rect = SDL_malloc(sizeof(SDL_FRect) * tempPiece->appearances);
    if (tempPiece->texture == NULL)
    {
        SDL_free(positions);
        return NULL;
    }
    if (tempPiece->rect == NULL)
    {
        SDL_DestroyTexture(tempPiece->texture);
        SDL_free(positions);
        return NULL;
    }
    for (int k = 0; k < tempPiece->appearances; k++)
    {
        tempPiece->rect[k] = rectFromTile(tempPiece->pos[k]);
    }
    return tempPiece;
}

void freePieces(PieceNode *Headnode)
{
    PieceNode *Tempnode = Headnode;
    PieceNode *Nextnode;
    while (Tempnode != NULL)
    {
        Nextnode = Tempnode->next;
        SDL_free(Tempnode->pos);
        SDL_free(Tempnode->rect);
        if (Tempnode->texture != NULL)
        {
            SDL_DestroyTexture(Tempnode->texture);
        }
        else
        {
            SDL_Log("You didn't have a %s? LOL", Tempnode->type);
        }
        SDL_free(Tempnode);
        Tempnode = Nextnode;
    }
}

void freeTileNodes(TileNode *HeadNode)
{
    TileNode *TempNode = HeadNode;
    TileNode *NextNode;
    while (TempNode != NULL)
    {
        NextNode = TempNode->next;
        SDL_free(TempNode);
        TempNode = NextNode;
    }
}

void renderPieces(SDL_Renderer *renderer, PieceNode *HeadPiece)
{
    PieceNode *TempPiece = HeadPiece;
    while (TempPiece != NULL)
    {
        for (int a = 0; a < TempPiece->appearances; a++)
        {
            if (TempPiece->texture != NULL && TempPiece->pos[a].x != 0) // Won't render if x coord is 0
            {
                SDL_RenderTexture(renderer, TempPiece->texture, NULL, &TempPiece->rect[a]);
            }
        }
        TempPiece = TempPiece->next;
    }
}

void renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile)
{
    TileNode *TempTile = HeadTile;
    while (TempTile != NULL)
    {
        SDL_RenderFillRect(renderer, &TempTile->rect);
        TempTile = TempTile->next;
    }
}

// {NULL ,-1} if not found
Piece pieceFromPos(PieceNode *HeadPiece, SDL_FPoint *pos)
{
    PieceNode *TempPiece = HeadPiece;

    while (TempPiece != NULL)
    {
        for (int k = 0; k < TempPiece->appearances; k++)
        {
            if (SDL_PointInRectFloat(pos, &TempPiece->rect[k]))
            {
                Piece *selection = SDL_malloc(sizeof(Piece));
                selection->index = k;
                selection->ptr = TempPiece;
                return *selection;
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

// Make these linked lists to make this easier;;;
void deletePiece(Piece fakePiece, PieceNode **FakeFamily)
{
    if (fakePiece.ptr == NULL)
    {
        return;
    }
    int index = fakePiece.index;
    PieceNode *fakeNode = fakePiece.ptr;

    // Shift all pieces after index left by one
    for (int i = index; i < fakeNode->appearances - 1; i++)
    {
        fakeNode->pos[i] = fakeNode->pos[i + 1];
        fakeNode->rect[i] = fakeNode->rect[i + 1];
    }
    // fakeNode->noInPlay--;
    fakeNode->appearances--;
    // Reallocate arrays to new size if not empty
    if (fakeNode->appearances > 0)
    {
        Tile *newPos = SDL_realloc(fakeNode->pos, sizeof(Tile) * fakeNode->appearances);
        SDL_FRect *newRect = SDL_realloc(fakeNode->rect, sizeof(SDL_FRect) * fakeNode->appearances);
        if (newPos != NULL)
            fakeNode->pos = newPos;
        if (newRect != NULL)
            fakeNode->rect = newRect;
    }
    else
    {
        // Free Items
        SDL_free(fakeNode->pos);
        SDL_free(fakeNode->rect);
        if (fakeNode->texture != NULL)
            SDL_DestroyTexture(fakeNode->texture);

        PieceNode *nextNode = fakeNode->next;
        PieceNode *prevNode = fakeNode->prev;

        // Unlink Node
        if (nextNode != NULL)
            nextNode->prev = prevNode;
        if (prevNode != NULL)
            prevNode->next = nextNode;
        else
            // Node is head**Change stored Head
            *FakeFamily = nextNode;

        SDL_free(fakeNode);
    }
}