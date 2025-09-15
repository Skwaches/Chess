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
    SDL_Surface *surf = IMG_LoadSizedSVG_IO(io, SVG_WIDTH, SVG_HEIGHT);
    if (surf == NULL)
    {
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    return texture;
}

pieceNode *makePieceNode(SDL_Renderer *renderer,
                         char *buffer,
                         pieceNode *tempPiece,
                         const int appearances,
                         const char *name,
                         const int *x,
                         const int y)
{
    tempPiece->appearances = appearances;
    coords *positions = SDL_malloc(appearances * sizeof(coords));
    if (positions == NULL)
    {
        return NULL;
    }
    for (int p = 0; p < appearances; p++)
    {
        positions[p] = (coords){x[p], y};
    }
    tempPiece->pos = positions;
    tempPiece->texture = maketexture(renderer, buffer);
    tempPiece->rect = SDL_malloc(sizeof(SDL_FRect) * tempPiece->appearances);
    tempPiece->name = SDL_malloc(sizeof(char *) * tempPiece->appearances);
    if (tempPiece->texture == NULL || tempPiece->rect == NULL || tempPiece->name == NULL)
    {
        return NULL;
    }

    for (int k = 0; k < tempPiece->appearances; k++)
    {
        tempPiece->name[k] = SDL_malloc(sizeof(char) * MAX_NAME_LENGTH);
        if (tempPiece->name[k] == NULL)
        {
            return NULL;
        }
        SDL_snprintf(tempPiece->name[k], sizeof(char) * MAX_NAME_LENGTH, "%s-%d", name, k);

        float tempPosx = (tempPiece->pos[k].x - 1) * TILE_WIDTH;
        float tempPosy = (8 - tempPiece->pos[k].y) * TILE_HEIGHT;
        tempPiece->rect[k] = (SDL_FRect){tempPosx, tempPosy, TILE_WIDTH, TILE_HEIGHT};
    }
    return tempPiece;
}

void freePieces(pieceNode *Headnode)
{
    pieceNode *Tempnode = Headnode;
    pieceNode *Nextnode;
    while (Tempnode != NULL)
    {
        Nextnode = Tempnode->next;
        for (int f = 0; f < Tempnode->appearances; f++)
        {
            SDL_free(Tempnode->name[f]);
        }
        SDL_free(Tempnode->pos);
        SDL_free(Tempnode->rect);
        SDL_DestroyTexture(Tempnode->texture);
        SDL_free(Tempnode->name);
        SDL_free(Tempnode);
        Tempnode = Nextnode;
    }
}
void freeTiles(tileNode *HeadNode)
{
    tileNode *TempNode = HeadNode;
    tileNode *NextNode;
    while (TempNode != NULL)
    {
        NextNode = TempNode->next;
        SDL_free(TempNode);
        TempNode = NextNode;
    }
}
void renderPieces(SDL_Renderer *renderer, pieceNode *HeadPiece)
{
    pieceNode *TempPiece = HeadPiece;
    while (TempPiece != NULL)
    {
        for (int a = 0; a < TempPiece->appearances; a++)
        {
            SDL_RenderTexture(renderer, TempPiece->texture, NULL, &TempPiece->rect[a]);
        }
        TempPiece = TempPiece->next;
    }
}
void renderTiles(SDL_Renderer *renderer, tileNode *HeadTile)
{
    tileNode *TempTile = HeadTile;
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