#include "Linkers/funcs.h"

// Creates a linked-list of TileNodes.
// The given bool dictates if the first tile should be offset.
// True makes an offset.
// False does not.
TileNode *setTiles(bool startOffset)
{
    int maxTileX = SCREENWIDTH - TILE_WIDTH;
    int maxTileY = SCREENHEIGHT - TILE_HEIGHT;
    int start_X = 0;
    int start_Y = 0;
    TileNode *tempTile;
    size_t tileNode_size = sizeof(TileNode);
    bool offset;
    TileNode *headTile = SDL_malloc(tileNode_size);
    if (headTile == NULL)
    {
        return NULL;
    }
    tempTile = headTile;
    offset = startOffset;
    for (int y = start_Y; y <= maxTileY; y += TILE_HEIGHT)
    {

        if (offset)
        {
            start_X = TILE_WIDTH;
        }
        else
        {
            start_X = 0;
        }
        offset = !offset;

        for (int x = start_X; x <= maxTileX; x += 2 * TILE_WIDTH)
        {
            SDL_FRect temp_rect = {x, y, TILE_WIDTH, TILE_HEIGHT};
            tempTile->rect = temp_rect;
            tempTile->pos = (Tile){(int)x / TILE_WIDTH + 1, 8 - (int)y / TILE_HEIGHT};
            tempTile->selected = false;
            if (y + TILE_HEIGHT > maxTileY && x + 2 * TILE_WIDTH > maxTileX)
            {
                tempTile->next = NULL;
                tempTile = tempTile->next;
                break;
            }
            tempTile->next = SDL_malloc(tileNode_size);
            if (tempTile->next == NULL)
            {
                freeTileNodes(headTile);
                break;
            }
            tempTile = tempTile->next;
        }
    }
    return headTile;
}

// renders TileNode objects connected to the given pointer
// Returns true on success and false otherwise
// Call SDL_GetError for info
bool renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile)
{
    TileNode *TempTile = HeadTile;
    while (TempTile != NULL)
    {
        if (!SDL_RenderFillRect(renderer, &TempTile->rect))
        {
            return false;
        }
        TempTile = TempTile->next;
    }
    return true;
}

// Frees all the tileNodes
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
