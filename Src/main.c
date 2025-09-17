#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "constants.h"
#include "classes.h"
#include "funcs.h"

#pragma region Globals
// Frame measurements

int starttime = 0;
float totaltime = 0;
int frames = 0;
float fps = 0;
// DELETE THIS
void clearTerminal(int num)
{
    for (int k = 0; k < num; k++)
    {
        SDL_Log("\n");
    }
}

bool leftMouseHeld = false;
bool leftMouseRelease = false;
bool leftMouseclick = false;

bool successfulLaunch = true;
bool running = true;

bool player = true; // WHITE :D

PieceNode **playerPieces = NULL;
PieceNode **opponentPieces = NULL;

Piece playerPiece = {NULL, -1};
Piece opponentPiece = {NULL, -1};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

TileNode *headLightTile = NULL;

PieceNode *blackHeadPiece = NULL;
PieceNode *whiteHeadPiece = NULL;

SDL_FPoint *mousepos = NULL;

#pragma endregion Globals

// FUNCTIONS

// Returns Current FPS Since Launch
void getFPS(void)
{
    static Uint32 starttime = 0;
    static int frames = 0;

    if (starttime == 0)
    {
        starttime = SDL_GetTicks();
        return;
    }

    frames++;
    Uint32 current = SDL_GetTicks();
    double elapsed = (current - starttime) / 1000.0;

    if (elapsed >= 1.0)
    {
        fps = frames / elapsed;
        frames = 0;
        starttime = current;
    }
}

void accident()
{
    successfulLaunch = false;
    running = false;
    SDL_Log("Something went wrong :%s\n", SDL_GetError());
}

void launch()
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        accident();
        return;
    }
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        accident();
        return;
    }

    window = SDL_CreateWindow("CHESS", SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_BORDERLESS);
    if (window == NULL)
    {
        accident();
        return;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        accident();
        return;
    }

    return;
}

// CLEANING FUNCTIONS
void launchClean()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    accident();
}
void clean_tile()
{
    launchClean();
    SDL_free(mousepos);
    accident();
}
void clean_pieces()
{
    clean_tile();
    freeTileNodes(headLightTile);
    accident();
}

void setup(void)
{
#pragma region mouse

    mousepos = SDL_malloc(sizeof(SDL_FPoint));
    if (mousepos == NULL)
    {
        launchClean();
        return;
    }

#pragma endregion mouseSpace

#pragma region LightTiles
    int maxTileX = SCREENWIDTH - TILE_WIDTH;
    int maxTileY = SCREENHEIGHT - TILE_HEIGHT;
    int start_X = 0;
    int start_Y = 0;
    TileNode *tempTile;
    size_t tileNode_size = sizeof(TileNode);
    bool offset;
    headLightTile = SDL_malloc(tileNode_size);
    if (headLightTile == NULL)
    {
        clean_tile();
        accident();
        return;
    }
    tempTile = headLightTile;
    offset = false;
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
                clean_tile();
                freeTileNodes(headLightTile);
                return;
            }
            tempTile = tempTile->next;
        }
    }
#pragma endregion LightTiles

    // ASSETS
    char buffer[MAX_ASSET_PATH];
    int itemcount = 0;

#pragma region Pieces
    char **pieces = NULL;

    PieceNode *tempPiece;
    PieceNode *prevPiece = NULL;

#pragma region BLACK
    blackHeadPiece = SDL_malloc(sizeof(PieceNode));
    if (blackHeadPiece == NULL)
    {
        clean_pieces();
        return;
    }
    size_t pieceNodeSize = sizeof(PieceNode);
    tempPiece = blackHeadPiece;

    // BASE VALUES FOR FAILURE
    tempPiece->next = NULL;
    tempPiece->prev = prevPiece;
    tempPiece->appearances = 1;
    tempPiece->texture = NULL;

    pieces = SDL_GlobDirectory(BLACK_PIECES_PATH, NULL, 0, &itemcount);
    if (pieces == NULL)
    {
        SDL_free(blackHeadPiece);
        clean_pieces();
        return;
    }

    for (int i = 0; i < itemcount; i++)
    {
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", BLACK_PIECES_PATH, pieces[i]);
        if (SDL_strcmp(pieces[i], BISHOP_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, BISHOP_NO, BISHOP_NAME, BISHOP_X, BLACK_Y) == NULL)
            {
                clean_pieces();
                SDL_free(pieces);
                freePieces(blackHeadPiece);
            };
        }
        else if (SDL_strcmp(pieces[i], KING_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KING_NO, KING_NAME, KING_X, BLACK_Y) == NULL)
            {
                clean_pieces();
                SDL_free(pieces);
                freePieces(blackHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], KNIGHT_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KNIGHT_NO,
                              KNIGHT_NAME, KNIGHT_X, BLACK_Y) == NULL)
            {
                clean_pieces();
                SDL_free(pieces);
                freePieces(blackHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], PAWN_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, PAWN_NO, PAWN_NAME, PAWN_X, BPAWNY) == NULL)
            {
                clean_pieces();
                SDL_free(pieces);
                freePieces(blackHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], QUEEN_FILE_NAME) == 0)
        {

            if (makePieceNode(renderer, buffer, tempPiece, QUEEN_NO, QUEEN_NAME, QUEEN_X, BLACK_Y) == NULL)
            {
                clean_pieces();
                SDL_free(pieces);
                freePieces(blackHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], ROOK_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, ROOK_NO, ROOK_NAME, ROOK_X, BLACK_Y) == NULL)
            {
                clean_pieces();
                SDL_free(pieces);
                freePieces(blackHeadPiece);
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
            clean_pieces();
            SDL_free(pieces);
            freePieces(blackHeadPiece);
            return;
        }
        tempPiece = tempPiece->next;
    }

#pragma endregion BLACK

#pragma region WHITE
    whiteHeadPiece = SDL_malloc(sizeof(PieceNode));
    prevPiece = NULL;
    if (whiteHeadPiece == NULL)
    {
        clean_pieces();
        freePieces(blackHeadPiece);
        return;
    }

    tempPiece = whiteHeadPiece;

    // BASE VALUES FOR FAILURE
    tempPiece->next = NULL;
    tempPiece->prev = prevPiece;
    tempPiece->appearances = 1;
    tempPiece->texture = NULL;

    pieces = SDL_GlobDirectory(WHITE_PIECES_PATH, NULL, 0, &itemcount);
    if (pieces == NULL)
    {
        clean_pieces();
        freePieces(blackHeadPiece);
        SDL_free(whiteHeadPiece);
        return;
    }
    for (int i = 0; i < itemcount; i++)
    {
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", WHITE_PIECES_PATH, pieces[i]);
        if (SDL_strcmp(pieces[i], BISHOP_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, BISHOP_NO, BISHOP_NAME, BISHOP_X, WHITE_Y) == NULL)
            {
                clean_pieces();
                freePieces(blackHeadPiece);
                freePieces(whiteHeadPiece);
            };
        }
        else if (SDL_strcmp(pieces[i], KING_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KING_NO, KING_NAME, KING_X, WHITE_Y) == NULL)
            {
                clean_pieces();
                freePieces(blackHeadPiece);
                freePieces(whiteHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], KNIGHT_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, KNIGHT_NO, KNIGHT_NAME, KNIGHT_X, WHITE_Y) == NULL)
            {
                clean_pieces();
                freePieces(blackHeadPiece);
                freePieces(whiteHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], PAWN_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, PAWN_NO, PAWN_NAME, PAWN_X, WPAWNY) == NULL)
            {
                clean_pieces();
                freePieces(blackHeadPiece);
                freePieces(whiteHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], QUEEN_FILE_NAME) == 0)
        {

            if (makePieceNode(renderer, buffer, tempPiece, QUEEN_NO, QUEEN_NAME, QUEEN_X, WHITE_Y) == NULL)
            {
                clean_pieces();
                freePieces(blackHeadPiece);
                freePieces(whiteHeadPiece);
            }
        }
        else if (SDL_strcmp(pieces[i], ROOK_FILE_NAME) == 0)
        {
            if (makePieceNode(renderer, buffer, tempPiece, ROOK_NO, ROOK_NAME, ROOK_X, WHITE_Y) == NULL)
            {
                clean_pieces();
                freePieces(blackHeadPiece);
                freePieces(whiteHeadPiece);
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
            freePieces(blackHeadPiece);
            freePieces(whiteHeadPiece);
            clean_pieces();
            return;
        }
        tempPiece = tempPiece->next;
    }

#pragma endregion WHITE

#pragma endregion Pieces

    // #pragma Audio

    //     char **songs = NULL;
    //     char **music = NULL;

    //     songs = SDL_GlobDirectory(MUSIC_PATH, "*.mp3", NULL, &itemcount);
    //     SDL_Free(songs);
}

void process_input(void)
{
    leftMouseRelease = false;
    leftMouseclick = false;

    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_EVENT_QUIT:
        running = false;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == 1)
        {
            leftMouseclick = true;
            leftMouseHeld = true;
        }

        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == 1)
        {
            leftMouseHeld = false;
            leftMouseRelease = true;
        }
        break;

    default:
        break;
    }
}

void update(void)
{
    // // FPS
    getFPS(); // frames stored in fps variable
    if (fps < 500 && SDL_GetTicks() > 2000)
    {
        SDL_Log("%f fps? You're pc's chopped lil' bro...\n", fps);
        clearTerminal(44);
    }
    if (LIMIT_FPS)
    {
        SDL_Delay(WAIT_TIME);
    }

    // UPDATES
    // Cursor Position
    SDL_GetMouseState(&mousepos->x, &mousepos->y);

    // Set up for player
    if (player)
    {
        playerPieces = &whiteHeadPiece;
        opponentPieces = &blackHeadPiece;
    }
    else
    {
        playerPieces = &blackHeadPiece;
        opponentPieces = &whiteHeadPiece;
    }

    if (leftMouseclick)
    {
        playerPiece = pieceFromPos(*playerPieces, mousepos);
        opponentPiece = pieceFromPos(*opponentPieces, mousepos); // COUNTS AS INVALID MOVE.
    }

    // Player
    if (playerPiece.ptr != NULL)
    {
        // TRACKMOUSE
        if (leftMouseHeld)
        {
            trackMouse(playerPiece, mousepos);
        }

        // MAKE MOVE
        if (leftMouseRelease)
        {
            // VALID MOVE
            int result = validateMove(playerPiece, mousepos, player, *playerPieces, *opponentPieces);
            if (result) // Valid no capture
            {

                if (result == 2)
                {
                    Tile mouseTile = TileFromPos(mousepos);
                    deletePiece(pieceFromTile(mouseTile, *opponentPieces), opponentPieces);
                }
                movePiece(playerPiece, mousepos);
                player = !player;
            }

            // INVALID MOVE
            else // Invalid
            {
                untrackMouse(playerPiece);
            }
        }
    }
    // Opponent -> For fidgeting when it's not your turn :D
    else if (opponentPiece.ptr != NULL)
    {
        if (leftMouseHeld)
        {
            trackMouse(opponentPiece, mousepos);
        }
        if (leftMouseRelease)
        {
            untrackMouse(opponentPiece);
        }
    }
}

void render(void)
{
    // DARK TILES
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    // LIGHT TILES
    SDL_SetRenderDrawColor(renderer, 10, 150, 150, 255);
    renderTileNodes(renderer, headLightTile);
    renderPieces(renderer, blackHeadPiece);
    renderPieces(renderer, whiteHeadPiece);

    SDL_RenderPresent(renderer);
}

void clean(void)
{
    freePieces(blackHeadPiece);
    freeTileNodes(headLightTile);
    SDL_free(mousepos);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// MAIN
int main()
{
    launch();
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

    if (successfulLaunch)
    {
        clean();
    };
    return 0;
}