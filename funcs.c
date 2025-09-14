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
                         int appearances,
                         char *name,
                         coords *position)
{
    tempPiece->appearances = appearances;
    tempPiece->pos = position;
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
        if (Tempnode->pos)
        {
            SDL_free(Tempnode->pos);
        }
        SDL_free(Tempnode->rect);
        SDL_DestroyTexture(Tempnode->texture);
        for (int f = 0; f < Tempnode->appearances; f++)
        {
            SDL_free(Tempnode->name[f]);
        }
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