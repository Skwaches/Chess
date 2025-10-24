#include "../links/funcs.h"
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
		start_X = offset ? TILE_WIDTH: 0;
        offset = !offset;

        for (int x = start_X; x <= maxTileX; x += 2 * TILE_WIDTH)
        {
            SDL_FRect temp_rect = {x, y, TILE_WIDTH, TILE_HEIGHT};
            tempTile->rect = temp_rect;
            tempTile->pos = (Tile){(int)x / TILE_WIDTH + 1, Y_TILES - (int)y / TILE_HEIGHT};
            tempTile->selected = NORMAL_TILE;
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


void unselectAll(TileNode *family, TileNode *opp,bool force)
{
    for (TileNode *tempHead = family; tempHead; tempHead = tempHead->next)
      	if(force || (tempHead->selected != PREV_ORIG && tempHead->selected != PREV_DEST) )
 			tempHead->selected = NORMAL_TILE;
    for (TileNode *tempHead = opp; tempHead; tempHead = tempHead->next)
      	if(force || (tempHead->selected != PREV_ORIG && tempHead->selected != PREV_DEST) )
        	tempHead->selected = NORMAL_TILE;
}
// renders TileNode objects connected to the given pointer
// Returns true on success and false otherwise
// Call SDL_GetError for info
bool renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile, SDL_Color colors)
{
    for(TileNode *TempTile = HeadTile; TempTile; TempTile=TempTile->next){
        if ( !setRenderColor(renderer,colors) )
            return false;
        if ( !SDL_RenderFillRect(renderer, &TempTile->rect) )
            return false;
		
		SDL_Color chosen = {0,0,0,0};
		switch(TempTile->selected){
			case NORMAL_TILE:
				break;
			case SELECTED_TILE:
				chosen = SELECTED_TILE_COLOR;
				break;
			case PIECE_HOVER:
				chosen = DEST_COLOR;
				break;
			case PREV_ORIG:
				chosen = ORIG_COLOR;
				break;
			case PREV_DEST:
				chosen = DEST_COLOR;
				break;
			default: 
				break;
        }
		if(chosen.a){
			setRenderColor(renderer,chosen);
			SDL_RenderFillRect(renderer, &TempTile->rect);
		}
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
                         const int appearsNo,
                         const char name,
                         const int *x,
                         const int y)
{
    // tempPiece->noInPlay = appearances;
    tempPiece->appearances = appearsNo;
    tempPiece->type = name;
    size_t Maxappearances = (name == PAWN_NAME ||
                             name == KING_NAME)
                                ? appearsNo
                                : (appearsNo + PAWN_NO);
    Tile *positions = SDL_malloc(Maxappearances * sizeof(Tile));
    if (positions == NULL)
        return NULL;
    for (int p = 0; p < appearsNo; p++)
        positions[p] = (Tile){x[p], y};

    tempPiece->pos = positions;
    tempPiece->texture = maketexture(renderer, buffer);
    tempPiece->rect = SDL_malloc(sizeof(SDL_FRect) * (Maxappearances));
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
/*colour determines position true is bottom.*/
PieceNode *setPieces(SDL_Renderer *renderer, bool colour,  const char *Path)
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

    // BASE VALUES FOR FAILURE
    tempPiece->next = NULL;
    tempPiece->appearances = 0;
    tempPiece->texture = NULL;
    tempPiece->pos = NULL;
    tempPiece->rect = NULL;
    tempPiece->type = 'i';

    pieces = SDL_GlobDirectory(Path, NULL, 0, &itemcount);
    if (pieces == NULL)
    {
        SDL_free(HeadPiece);
        return NULL;
    }

    Uint8 noLoaded = 0;
    for (int i = 0; i < itemcount; i++)
    {

        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", Path, pieces[i]);
        if (!SDL_strcmp(pieces[i], BISHOP_FILE_NAME))
        {
            if (!makePieceNode(renderer, buffer, tempPiece,
                               BISHOP_NO, BISHOP_NAME, BISHOP_X, sideY))
            {
                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
            noLoaded++;
            if (noLoaded < PIECETYPES)
            {
                tempPiece->next = SDL_malloc(pieceNodeSize);
                if (!tempPiece->next)
                {
                    freePieces(HeadPiece);
                    break;
                }
                tempPiece = tempPiece->next;
            }
            if (noLoaded == PIECETYPES)
                break;
        }
        if (!SDL_strcmp(pieces[i], KING_FILE_NAME))
        {
            if (!makePieceNode(renderer, buffer, tempPiece,
                               KING_NO, KING_NAME, KING_X, sideY))
            {
                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
            noLoaded++;
            if (noLoaded < PIECETYPES)
            {
                tempPiece->next = SDL_malloc(pieceNodeSize);
                if (!tempPiece->next)
                {
                    freePieces(HeadPiece);
                    break;
                }
                tempPiece = tempPiece->next;
            }
            if (noLoaded == PIECETYPES)
                break;
        }
        if (!SDL_strcmp(pieces[i], KNIGHT_FILE_NAME))
        {
            if (!makePieceNode(renderer, buffer, tempPiece,
                               KNIGHT_NO, KNIGHT_NAME, KNIGHT_X, sideY))
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
            noLoaded++;
            if (noLoaded < PIECETYPES)
            {
                tempPiece->next = SDL_malloc(pieceNodeSize);
                if (!tempPiece->next)
                {
                    freePieces(HeadPiece);
                    break;
                }
                tempPiece = tempPiece->next;
            }
            if (noLoaded == PIECETYPES)
                break;
        }
        if (!SDL_strcmp(pieces[i], PAWN_FILE_NAME))
        {
            if (!makePieceNode(renderer, buffer, tempPiece,
                               PAWN_NO, PAWN_NAME, PAWN_X, pawnY))
            {
                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
            noLoaded++;
            if (noLoaded < PIECETYPES)
            {
                tempPiece->next = SDL_malloc(pieceNodeSize);
                if (!tempPiece->next)
                {
                    freePieces(HeadPiece);
                    break;
                }
                tempPiece = tempPiece->next;
            }
            if (noLoaded == PIECETYPES)
                break;
        }
        if (!SDL_strcmp(pieces[i], QUEEN_FILE_NAME))
        {
            if (!makePieceNode(renderer, buffer, tempPiece,
                               QUEEN_NO, QUEEN_NAME, QUEEN_X, sideY))
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
            noLoaded++;
            if (noLoaded < PIECETYPES)
            {
                tempPiece->next = SDL_malloc(pieceNodeSize);
                if (!tempPiece->next)
                {
                    freePieces(HeadPiece);
                    break;
                }
                tempPiece = tempPiece->next;
            }
            if (noLoaded == PIECETYPES)
                break;
        }
        if (!SDL_strcmp(pieces[i], ROOK_FILE_NAME))
        {
            if (!makePieceNode(renderer, buffer, tempPiece,
                               ROOK_NO, ROOK_NAME, ROOK_X, sideY))
            {

                SDL_free(pieces);
                freePieces(HeadPiece);
                return NULL;
            }
            noLoaded++;
            if (noLoaded < PIECETYPES)
            {
                tempPiece->next = SDL_malloc(pieceNodeSize);
                if (!tempPiece->next)
                {
                    freePieces(HeadPiece);
                    break;
                }
                tempPiece = tempPiece->next;
            }
            if (noLoaded == PIECETYPES)
                break;
        }
    }
    tempPiece->next = NULL;
    SDL_free(pieces);
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
    for (PieceNode *tempie = HeadPiece; tempie; tempie = tempie->next)
        for (int a = 0; a < tempie->appearances; a++)
        {
            if (tempie->pos[a].x == SHADOW_REALM.x ||
                tempie->pos[a].y == SHADOW_REALM.y)
                continue;

            bool ok = SDL_RenderTexture(renderer, tempie->texture, NULL, &tempie->rect[a]);
            if (!ok)
                return false;
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
    int apperances;
    for (PieceNode *Head = family; Head; Head = Head->next)
    {
        sel_Y = yVal;
        switch (Head->type)
        {
        case PAWN_NAME:
            x_Vals = PAWN_X;
            apperances = PAWN_NO;
            sel_Y = pVal;
            break;
        case KING_NAME:
            x_Vals = KING_X;
            apperances = KING_NO;
            break;
        case QUEEN_NAME:
            apperances = QUEEN_NO;
            x_Vals = QUEEN_X;
            break;
        case ROOK_NAME:
            apperances = ROOK_NO;
            x_Vals = ROOK_X;
            break;
        case BISHOP_NAME:
            apperances = BISHOP_NO;
            x_Vals = BISHOP_X;
            break;
        case KNIGHT_NAME:
            apperances = KNIGHT_NO;
            x_Vals = KNIGHT_X;
            break;
        default:
            SDL_Log("Unknown Piece");
            break;
        }
        Head->appearances = apperances;
        for (int p = 0; p < apperances; p++)
            Head->pos[p] = (Tile){x_Vals[p], sel_Y};
        redrawFamily(family);
    }
}
