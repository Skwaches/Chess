#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string.h>
#include "funcs.h"
#include "constants.h"
#include "classes.h"

SDL_Texture *maketexture(SDL_Renderer *renderer, char *path)
{
    SDL_IOStream *io = SDL_IOFromFile(path, "rb");
    if (io == NULL)
    {
        return NULL;
    }

    return IMG_LoadTexture_IO(renderer, io, true);
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
    tempPiece->noInPlay = appearances;
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

// Invalid returns i
char chessX(int number)
{
    switch (number)
    {
    case 1:
        return 'a';
        break;
    case 2:
        return 'b';
        break;
    case 3:
        return 'c';
        break;
    case 4:
        return 'd';
        break;
    case 5:
        return 'e';
        break;
    case 6:
        return 'f';
        break;
    case 7:
        return 'g';
        break;
    case 8:
        return 'h';
        break;

    default:
        return 'i';
        break;
    }
}

// Invalid returns 0
int realX(char letter)
{
    switch (letter)
    {
    case 'a':
        return 1;
        break;
    case 'b':
        return 2;
        break;
    case 'c':
        return 3;
        break;
    case 'd':
        return 4;
        break;
    case 'e':
        return 5;
        break;
    case 'f':
        return 6;
        break;
    case 'g':
        return 7;
        break;
    case 'h':
        return 8;
        break;

    default:
        return 0;
        break;
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

void trackMouse(Piece fakePIECE, SDL_FPoint *mouse_pos)
{
    fakePIECE.ptr->rect[fakePIECE.index] = centerRectAroundPos(mouse_pos);
}

void untrackMouse(Piece fakePIECE)
{
    fakePIECE.ptr->rect[fakePIECE.index] = rectFromTile(fakePIECE.ptr->pos[fakePIECE.index]);
}

void movePiece(Piece fakePIECE, SDL_FPoint *pos)
{
    fakePIECE.ptr->pos[fakePIECE.index] = TileFromPos(pos);
    untrackMouse(fakePIECE);
}

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
    fakeNode->noInPlay--;
    fakeNode->appearances--;
    // Reallocate arrays to new size if not empty
    if (fakeNode->noInPlay > 0)
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