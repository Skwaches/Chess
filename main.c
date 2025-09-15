#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include "constants.h"
#include "classes.h"
#include "funcs.h"

#pragma region Globals
bool leftmouseHeld = false;
bool leftclick = false;

bool running = false;

Piece *selected = NULL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

tileNode *headLightTile = NULL;

pieceNode *blackHeadPiece = NULL;
pieceNode *whiteHeadPiece = NULL;

coords *mousetile = NULL;
SDL_FPoint *mousepos = NULL;

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
#pragma region mouse
    mousetile = SDL_malloc(sizeof(coords));
    if (mousetile == NULL)
    {
        accident();
        fprintf(stderr, "Mouse Tile\n");
        return;
    }
    mousepos = SDL_malloc(sizeof(SDL_FPoint *));

#pragma endregion mouseSpace

#pragma region Board
    int maxTileX = SCREENWIDTH - TILE_WIDTH;
    int maxTileY = SCREENHEIGHT - TILE_HEIGHT;
    int start_X = 0;
    int start_Y = 0;
    tileNode *tempTile;
    size_t tileNode_size = sizeof(tileNode);
    bool offset;

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
    char **pieces = NULL;
    int piece_no = 0;
    char buffer[MAX_ASSET_PATH];
    pieceNode *tempPiece;
    pieceNode *prevPiece = NULL;
#pragma region BLACK
    blackHeadPiece = SDL_malloc(sizeof(pieceNode));
    if (blackHeadPiece == NULL)
    {
        accident();
        fprintf(stderr, "blackHeadPiece");
        return;
    }
    size_t pieceNodeSize = sizeof(pieceNode);
    tempPiece = blackHeadPiece;
    pieces = SDL_GlobDirectory(BLACK_PIECES_PATH, "*.svg", 0, &piece_no);
    if (pieces == NULL)
    {
        accident();
        fprintf(stderr, "Black Pieces\n");
        return;
    }

    for (int i = 0; i < piece_no; i++)
    {
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", BLACK_PIECES_PATH, pieces[i]);
        if (SDL_strcmp(pieces[i], BISHOP_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, BISHOP_NO, BISHOP_NAME, BISHOP_X, BLACK_Y) == NULL)
            {
                accident();
                fprintf(stderr, BISHOP_FILE_NAME);
            };
        }
        else if (SDL_strcmp(pieces[i], KING_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KING_NO, KING_NAME, KING_X, BLACK_Y) == NULL)
            {
                accident();
                fprintf(stderr, KING_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], KNIGHT_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KNIGHT_NO, KNIGHT_NAME, KNIGHT_X, BLACK_Y) == NULL)
            {
                accident();
                fprintf(stderr, KNIGHT_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], PAWN_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, PAWN_NO, PAWN_NAME, PAWN_X, BPAWNY) == NULL)
            {
                accident();
                fprintf(stderr, PAWN_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], QUEEN_FILE_NAME) == 0)
        {

            if (makePieceNode(renderer, buffer, tempPiece, QUEEN_NO, QUEEN_NAME, QUEEN_X, BLACK_Y) == NULL)
            {
                accident();
                fprintf(stderr, QUEEN_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], ROOK_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, ROOK_NO, ROOK_NAME, ROOK_X, BLACK_Y) == NULL)
            {
                accident();
                fprintf(stderr, BISHOP_FILE_NAME);
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
        if (i == piece_no - 1)
        {

            tempPiece->next = NULL;
            break;
        }

        tempPiece->next = SDL_malloc(pieceNodeSize);
        tempPiece = tempPiece->next;
    }
#pragma endregion BLACK

#pragma region WHITE
    whiteHeadPiece = SDL_malloc(sizeof(pieceNode));
    prevPiece = NULL;
    if (whiteHeadPiece == NULL)
    {
        accident();
        fprintf(stderr, "whiteHeadPiece\n");
        return;
    }

    tempPiece = whiteHeadPiece;
    pieces = SDL_GlobDirectory(WHITE_PIECES_PATH, "*.svg", 0, &piece_no);
    if (pieces == NULL)
    {
        accident();
        fprintf(stderr, "White Pieces\n");
        return;
    }
    for (int i = 0; i < piece_no; i++)
    {
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", WHITE_PIECES_PATH, pieces[i]);
        if (SDL_strcmp(pieces[i], BISHOP_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, BISHOP_NO, BISHOP_NAME, BISHOP_X, WHITE_Y) == NULL)
            {
                accident();
                fprintf(stderr, BISHOP_FILE_NAME);
            };
        }
        else if (SDL_strcmp(pieces[i], KING_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KING_NO, KING_NAME, KING_X, WHITE_Y) == NULL)
            {
                accident();
                fprintf(stderr, KING_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], KNIGHT_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KNIGHT_NO, KNIGHT_NAME, KNIGHT_X, WHITE_Y) == NULL)
            {
                accident();
                fprintf(stderr, KNIGHT_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], PAWN_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, PAWN_NO, PAWN_NAME, PAWN_X, WPAWNY) == NULL)
            {
                accident();
                fprintf(stderr, PAWN_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], QUEEN_FILE_NAME) == 0)
        {

            if (makePieceNode(renderer, buffer, tempPiece, QUEEN_NO, QUEEN_NAME, QUEEN_X, WHITE_Y) == NULL)
            {
                accident();
                fprintf(stderr, QUEEN_FILE_NAME);
            }
        }
        else if (SDL_strcmp(pieces[i], ROOK_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, ROOK_NO, ROOK_NAME, ROOK_X, WHITE_Y) == NULL)
            {
                accident();
                fprintf(stderr, ROOK_FILE_NAME);
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
        if (i == piece_no - 1)
        {

            tempPiece->next = NULL;
            break;
        }

        tempPiece->next = SDL_malloc(pieceNodeSize);
        tempPiece = tempPiece->next;
    }

#pragma endregion WHITE

#pragma endregion Pieces
}

void process_input(void)
{

    // Cursor Position
    SDL_GetMouseState(&mousepos->x, &mousepos->y);
    mousetile->x = mousepos->x / TILE_WIDTH + 1;
    mousetile->y = 8 - mousepos->y / TILE_HEIGHT;

    leftclick = false;

    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {

    case SDL_EVENT_QUIT:
        SDL_Quit();
        running = false;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == 1)
        {
            leftmouseHeld = true;
            selected = pieceFromPos(mousepos, blackHeadPiece);
        }

        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == 1)
        {
            leftmouseHeld = false;
            SDL_free(selected);
            selected = NULL;
            leftclick = true;
        }
        break;

    default:
        break;
    }
}

void update(void)
{
    // FPS
    SDL_Delay(WAIT_TIME);

    // UPDATE
    if (selected)
    {
        selected->ptr->rect[selected->index] = rectFromPos(mousepos);
    }
}

void render(void)
{
    // DARK TILES
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    // LIGHT TILES
    SDL_SetRenderDrawColor(renderer, 10, 150, 150, 255);
    renderTiles(renderer, headLightTile);
    renderPieces(renderer, blackHeadPiece);
    renderPieces(renderer, whiteHeadPiece);

    SDL_RenderPresent(renderer);
}

void clean(void)
{
    freePieces(blackHeadPiece);
    freeTiles(headLightTile);
    SDL_free(mousetile);
    SDL_free(mousepos);
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