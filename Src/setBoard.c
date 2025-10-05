#include "Linkers/funcs.h"

/**Creates a linked-list of TileNodes.
 * The given bool dictates if the first tile should be offset.
 * True makes an offset.
 * False does not.
 */
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
    if (!headTile)
        return NULL;

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
            tempTile->pos = (Tile){(int)x / TILE_WIDTH + 1, Y_TILES - (int)y / TILE_HEIGHT};
            tempTile->selected = false;
            if (y + TILE_HEIGHT > maxTileY && x + 2 * TILE_WIDTH > maxTileX)
            {
                tempTile->next = NULL;
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

bool setRenderColor(SDL_Renderer *renderer, SDL_Color color)
{
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void unselectAll(TileNode *family, TileNode *opp)
{
    for (TileNode *tempHead = family; tempHead; tempHead = tempHead->next)
        tempHead->selected = false;
    for (TileNode *tempHead = opp; tempHead; tempHead = tempHead->next)
        tempHead->selected = false;
}
// renders TileNode objects connected to the given pointer
// Returns true on success and false otherwise
// Call SDL_GetError for info
bool renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile, SDL_Color colors)
{
    TileNode *TempTile = HeadTile;
    SDL_Color colorToUse;
    while (TempTile != NULL)
    {
        colorToUse = TempTile->selected ? SELECTED_TILE_COLOR : colors;
        if (!SDL_SetRenderDrawColor(renderer, colorToUse.r, colorToUse.b, colorToUse.g, colorToUse.a))
            return false;
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

SDL_Texture *maketexture(SDL_Renderer *renderer, const char *path)
{
    SDL_IOStream *io = SDL_IOFromFile(path, "rb");
    if (!io)
        return NULL;
    if (IMG_isSVG(io))
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

PieceNode *makePieceNode(SDL_Renderer *renderer,
                         char *buffer,
                         PieceNode *tempPiece,
                         const int appearances,
                         const char name,
                         const int *x,
                         const int y)
{
    // tempPiece->noInPlay = appearances;
    tempPiece->appearances = appearances;
    tempPiece->type = name;
    Tile *positions = SDL_malloc(appearances * sizeof(Tile));
    if (positions == NULL)
        return NULL;

    for (int p = 0; p < appearances; p++)
        positions[p] = (Tile){x[p], y};

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
        tempPiece->rect[k] = rectFromTile(tempPiece->pos[k]);

    return tempPiece;
}

PieceNode *setPieces(SDL_Renderer *renderer, bool colour, const char *Path)
{
    char **pieces = NULL;
    char buffer[MAX_ASSET_PATH];
    int itemcount = 0;
    int sideY = colour ? WHITE_Y : BLACK_Y;
    int pawnY = colour ? WPAWNY : BPAWNY;
    PieceNode *HeadPiece = SDL_malloc(sizeof(PieceNode));

    if (HeadPiece == NULL)
        return NULL;
    size_t pieceNodeSize = sizeof(PieceNode);
    PieceNode *tempPiece = HeadPiece;
    PieceNode *prevPiece = NULL;
    // BASE VALUES FOR FAILURE
    tempPiece->next = NULL;
    tempPiece->prev = prevPiece;
    tempPiece->appearances = 1;
    tempPiece->texture = NULL;

    pieces = SDL_GlobDirectory(Path, NULL, 0, &itemcount);
    if (pieces == NULL)
    {
        SDL_free(HeadPiece);
        return NULL;
    }

    for (int i = 0; i < itemcount; i++)
    {
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", Path, pieces[i]);
        if (SDL_strcmp(pieces[i], BISHOP_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, BISHOP_NO, BISHOP_NAME, BISHOP_X, sideY) == NULL)
            {
                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            };
        }
        else if (SDL_strcmp(pieces[i], KING_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KING_NO, KING_NAME, KING_X, sideY) == NULL)
            {
                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
        }
        else if (SDL_strcmp(pieces[i], KNIGHT_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KNIGHT_NO,
                              KNIGHT_NAME, KNIGHT_X, sideY) == NULL)
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
        }
        else if (SDL_strcmp(pieces[i], PAWN_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, PAWN_NO, PAWN_NAME, PAWN_X, pawnY) == NULL)
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
        }
        else if (SDL_strcmp(pieces[i], QUEEN_FILE_NAME) == 0)
        {

            if (makePieceNode(renderer, buffer, tempPiece, QUEEN_NO, QUEEN_NAME, QUEEN_X, sideY) == NULL)
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
        }
        else if (SDL_strcmp(pieces[i], ROOK_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, ROOK_NO, ROOK_NAME, ROOK_X, sideY) == NULL)
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
        }
        else
        {
            continue;
        }

        // Previous Piece
        tempPiece->prev = prevPiece;
        prevPiece = tempPiece;
        // Next Piece?
        if (i == itemcount - 1)
        {
            SDL_free(pieces);
            tempPiece->next = NULL;
            break;
        }

        tempPiece->next = SDL_malloc(pieceNodeSize);
        if (tempPiece->next == NULL)
        {

            SDL_free(pieces);
            freePieces(HeadPiece);
            return NULL;
        }
        tempPiece = tempPiece->next;
    }
    return HeadPiece;
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
        SDL_free(Tempnode);
        Tempnode = Nextnode;
    }
}

// renders all Pieces
// Returns true on success and false otherwise
// Call SDL_GetError for info
bool renderPieces(SDL_Renderer *renderer, PieceNode *HeadPiece)
{
    PieceNode *TempPiece = HeadPiece;
    while (TempPiece != NULL)
    {
        for (int a = 0; a < TempPiece->appearances; a++)
        {
            /** Won't render if:
             * Texture is NULL
             * Item in SHADOW_REALM
             * Stops if render fails*/
            if ((TempPiece->texture != NULL) &&
                (TempPiece->pos[a].x != SHADOW_REALM.x) &&
                (TempPiece->pos[a].y != SHADOW_REALM.y) &&
                (!SDL_RenderTexture(renderer, TempPiece->texture, NULL, &TempPiece->rect[a])))
                return false;
        }
        TempPiece = TempPiece->next;
    }
    return true;
}

void redrawFamily(PieceNode *family)
{
    for (PieceNode *header = family; header; header = header->next)
        for (int p = 0; p < header->appearances; p++)
        {
            Piece tmpPiece = {header, p};
            untrackMouse(tmpPiece);
        }
}

// Sets the piece to the original position.
void resetPieces(PieceNode *family, bool player)
{
    const int yVal = player ? WHITE_Y : BLACK_Y;
    const int pVal = player ? WPAWNY : BPAWNY;
    int *x_Vals;
    int sel_Y;
    for (PieceNode *Head = family; Head; Head = Head->next)
    {
        sel_Y = yVal;
        switch (Head->type)
        {
        case PAWN_NAME:
            x_Vals = PAWN_X;
            sel_Y = pVal;
            break;
        case KING_NAME:
            x_Vals = KING_X;
            break;
        case QUEEN_NAME:
            x_Vals = QUEEN_X;
            break;
        case ROOK_NAME:
            x_Vals = ROOK_X;
            break;
        case BISHOP_NAME:
            x_Vals = BISHOP_X;
            break;
        case KNIGHT_NAME:
            x_Vals = KNIGHT_X;
            break;
        default:
            SDL_Log("Unknown Piece");
            break;
        }
        for (int p = 0; p < Head->appearances; p++)
            Head->pos[p] = (Tile){x_Vals[p], sel_Y};
        redrawFamily(family);
    }
}