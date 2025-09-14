#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include "constants.h"
#include "classes.h"
#include "funcs.h"

#pragma region Globals
bool mouseHeld = false;
bool running = false;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

tileNode *headDarkTile = NULL;
tileNode *headLightTile = NULL;

pieceNode *headPiece = NULL;
char **pieces = NULL;
int piece_no = 0;

float mouseX, mouseY;
coords *mouse_pos = NULL;
#pragma endregion Globals

// FUNCTIONS
void initialise_window()
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        running = false;
        return;
    }

    window = SDL_CreateWindow("CHESS", SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_BORDERLESS);
    if (window == NULL)
    {
        fprintf(stderr, "Window FAIL");
        return;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
        fprintf(stderr, "Renderer FAIL");
        return;
    }
    running = true;
    return;
}

void accident()
{
    running = false;

    printf("Something went wrong :%s\n", SDL_GetError());
}

void setup(void)
{
#pragma region mouseSpace
    mouse_pos = SDL_malloc(sizeof(int) * 2);
    if (mouse_pos == NULL)
    {
        accident();
        fprintf(stderr, "Mouse Pos\n");
        return;
    }
#pragma endregion mouseSpace

#pragma region Board
    int maxTileX = SCREENWIDTH - TILE_WIDTH;
    int maxTileY = SCREENHEIGHT - TILE_HEIGHT;
    int start_X = 0;
    int start_Y = 0;
    tileNode *tempTile;
    size_t tileNode_size = sizeof(tileNode);
    bool offset;

#pragma region DarkTiles
    offset = true;
    headDarkTile = SDL_malloc(tileNode_size);
    if (headDarkTile == NULL)
    {
        accident();
        fprintf(stderr, "headDarkTile\n");
        return;
    }
    tempTile = headDarkTile;
    for (int y = start_Y; y <= maxTileY; y += TILE_HEIGHT)
    {
        if (tempTile == NULL)
        {
            break;
        }
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
            tempTile->pos = (coords){(int)x / TILE_WIDTH + 1, 8 - (int)y / TILE_HEIGHT};
            ;
            if (y + TILE_HEIGHT > maxTileY && x + 2 * TILE_WIDTH > maxTileX)
            {
                tempTile->next = NULL;
                tempTile = tempTile->next;
                break;
            }
            tempTile->next = SDL_malloc(tileNode_size);
            if (tempTile == NULL)
            {
                accident();
                fprintf(stderr, "tempTile-Dark");
                return;
            }
            tempTile = tempTile->next;
        }
    }
#pragma endregion DarkTiles

#pragma region LightTiles
    headLightTile = SDL_malloc(tileNode_size);
    if (headLightTile == NULL)
    {
        accident();
        fprintf(stderr, "headLightTile");
        return;
    }
    tempTile = headLightTile;
    offset = false;
    for (int y = start_Y; y <= maxTileY; y += TILE_HEIGHT)
    {
        if (tempTile == NULL)
        {
            break;
        }
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
            tempTile->pos = (coords){(int)x / TILE_WIDTH + 1, 8 - (int)y / TILE_HEIGHT};
            ;
            if (y + TILE_HEIGHT > maxTileY && x + 2 * TILE_WIDTH > maxTileX)
            {
                tempTile->next = NULL;
                tempTile = tempTile->next;
                break;
            }
            tempTile->next = SDL_malloc(tileNode_size);
            if (tempTile->next == NULL)
            {
                accident();
                fprintf(stderr, "tempTile-Light");
                return;
            }
            tempTile = tempTile->next;
        }
    }
#pragma endregion LightTiles

#pragma endregion Board

#pragma region Pieces
    headPiece = SDL_malloc(sizeof(pieceNode));
    if (headPiece == NULL)
    {
        accident();
        fprintf(stderr, "headPiece");
        return;
    }
    size_t pieceNodeSize = sizeof(pieceNode);
    pieceNode *tempPiece = headPiece;
    char buffer[MAX_ASSET_PATH];
    char *tempName;
    pieces = SDL_GlobDirectory(PIECE_PATH, "*.svg", 0, &piece_no);
    if (pieces == NULL)
    {
        accident();
        fprintf(stderr, "Pieces\n");
        return;
    }

    for (int i = 0; i < piece_no; i++)
    {
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", PIECE_PATH, pieces[i]);
        if (SDL_strcmp(pieces[i], "bishop.svg") == 0)
        {
            coords *pos = SDL_malloc(sizeof(coords) * BISHOP_NO);
            pos[0] = (coords){3, 8};
            pos[1] = (coords){6, 8};
            tempName = "Bishop";
            if (makePieceNode(renderer, buffer, tempPiece, BISHOP_NO, tempName, pos) == NULL)
            {
                accident();
                fprintf(stderr, "bishop.svg");
            };
        }
        else if (SDL_strcmp(pieces[i], "king.svg") == 0)
        {
            coords *pos = SDL_malloc(sizeof(coords) * KING_NO);
            pos[0] = (coords){5, 8};
            tempName = "King";
            if (makePieceNode(renderer, buffer, tempPiece, KING_NO, tempName, pos) == NULL)
                ;
        }
        else if (SDL_strcmp(pieces[i], "knight.svg") == 0)
        {
            coords *pos = SDL_malloc(sizeof(coords) * KNIGHT_NO);
            pos[0] = (coords){2, 8};
            pos[1] = (coords){7, 8};
            tempName = "Knight";
            if (makePieceNode(renderer, buffer, tempPiece, KNIGHT_NO, tempName, pos) == NULL)
                ;
        }
        else if (SDL_strcmp(pieces[i], "pawn.svg") == 0)
        {
            coords *pos = SDL_malloc(sizeof(coords) * PAWN_NO);
            for (int o = 0; o < PAWN_NO; o++)
            {
                pos[o] = (coords){o + 1, 7};
            }
            tempName = "Pawn";
            if (makePieceNode(renderer, buffer, tempPiece, PAWN_NO, tempName, pos) == NULL)
                ;
        }
        else if (SDL_strcmp(pieces[i], "queen.svg") == 0)
        {
            coords *pos = SDL_malloc(sizeof(coords) * QUEEN_NO);
            pos[0] = (coords){4, 8};
            tempName = "Queen";
            if (makePieceNode(renderer, buffer, tempPiece, QUEEN_NO, tempName, pos) == NULL)
                ;
        }
        else if (SDL_strcmp(pieces[i], "rook.svg") == 0)
        {
            coords *pos = SDL_malloc(sizeof(coords) * ROOK_NO);
            pos[0] = (coords){1, 8};
            pos[1] = (coords){8, 8};
            tempName = "Rook";
            if (makePieceNode(renderer, buffer, tempPiece, ROOK_NO, tempName, pos) == NULL)
                ;
        }
        else
        {
            continue;
        }
        // Next Piece?
        if (i == piece_no - 1)
        {
            tempPiece->next = NULL;
            break;
        }
        tempPiece->next = SDL_malloc(pieceNodeSize);
        tempPiece = tempPiece->next;
    }
#pragma endregion Pieces
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_EVENT_QUIT:
        SDL_Quit();
        running = false;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        mouseHeld = true;
        SDL_GetMouseState(&mouseX, &mouseY);
        mouse_pos->x = (int)mouseX / TILE_WIDTH + 1;
        mouse_pos->y = 8 - (int)mouseY / TILE_HEIGHT;
        printf("%d ,%d\n", mouse_pos->x, mouse_pos->y);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        mouseHeld = false;
        break;

    default:
        break;
    }
}

void update(void)
{
    // SOMETHING
}

void render(void)
{
    // Background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // Board
    tileNode *tempTile;
    // Dark Tiles
    SDL_SetRenderDrawColor(renderer, 100, 30, 30, 255);
    tempTile = headDarkTile;
    while (tempTile != NULL)
    {
        SDL_RenderFillRect(renderer, &tempTile->rect);
        tempTile = tempTile->next;
    }

    // Light Tiles
    SDL_SetRenderDrawColor(renderer, 10, 150, 150, 255);
    tempTile = headLightTile;
    while (tempTile != NULL)
    {
        SDL_RenderFillRect(renderer, &tempTile->rect);
        tempTile = tempTile->next;
    }
    // Pieces
    pieceNode *tempPiece = headPiece;

    while (tempPiece != NULL)
    {
        for (int k = 0; k < tempPiece->appearances; k++)
        {
            SDL_RenderTexture(renderer, tempPiece->texture, NULL, &tempPiece->rect[k]);
        }
        tempPiece = tempPiece->next;
    }
    SDL_RenderPresent(renderer);
}

void clean(void)
{
    freePieces(headPiece);
    freeTiles(headDarkTile);
    freeTiles(headLightTile);

    SDL_free(mouse_pos);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// MAIN
int main()
{
    initialise_window();
    if (running)
    {
        setup();
    }

    // Game Loop
    while (running)
    {
        process_input();
        update();
        render();
    }
    clean();
    return 0;
}