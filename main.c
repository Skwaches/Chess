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

pieceNode *blackHeadPiece = NULL;
pieceNode *whiteHeadPiece = NULL;
int colour = 0;

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
    char buffer[MAX_ASSET_PATH];
    pieceNode *tempPiece;
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
        printf("%c%d\n", chessX(mouse_pos->x), mouse_pos->y);
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