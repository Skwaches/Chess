#include "Linkers/funcs.h"

#pragma region Globals
// Frame measurements
int starttime = 0;
float totaltime = 0;
int frames = 0;
float fps = 0;

bool leftMouseHeld = false;
bool leftMouseRelease = false;
bool leftMouseclick = false;

bool rightMouseclick = false;
bool middleClick = false;

bool successfulLaunch = true;
bool running = true;

bool player = true; // WHITE :D
bool checkStatus;

bool WhiteCheck = false;
bool BlackCheck = false;

PieceNode **playerPieces = NULL;
PieceNode **opponentPieces = NULL;

Piece playerPiece = {NULL, -1};
Piece opponentPiece = {NULL, -1};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

TileNode *headLightTile = NULL;
TileNode *headDarkTile = NULL;
PieceNode *blackHeadPiece = NULL;
PieceNode *whiteHeadPiece = NULL;

SDL_FPoint *mousepos = NULL;

char moveMadeWhite[40];
char moveMadeBlack[40];
#pragma endregion Globals

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
    if (!openDataBase())
    {
        closeDataBase();
        accident();
        return;
    }
    if (!createTable())
    {
        closeDataBase();
        accident();
        return;
    }
    if (!Init_Audio())
    {
        accident();
    }

    window = SDL_CreateWindow("CHESS", SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        closeDataBase();
        accident();
        return;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
        closeDataBase();
        SDL_DestroyWindow(window);
        accident();
        return;
    }

    return;
}

#pragma region Cleaning Functions for failed launch
void launchClean()
{
    closeDataBase();
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
#pragma endregion

void setup(void)
{

    mousepos = SDL_malloc(sizeof(SDL_FPoint));
    if (mousepos == NULL)
    {
        launchClean();
        return;
    }

    // Tiles
    headLightTile = setTiles(false);
    headDarkTile = setTiles(true);
    if (headDarkTile == NULL || headLightTile == NULL)
    {
        clean_tile();
        accident();
        SDL_Log("Problem with Tiles");
    }
    // Pieces
    whiteHeadPiece = setPieces(renderer, player, WHITE_PIECES_PATH);
    blackHeadPiece = setPieces(renderer, !player, BLACK_PIECES_PATH);
    if (blackHeadPiece == NULL || whiteHeadPiece == NULL)
    {
        clean_pieces();
        accident();
        SDL_Log("Problem with pieces");
        return;
    }
    playGameStartAudio();
}

void process_input(void)
{
    leftMouseRelease = false;
    leftMouseclick = false;
    middleClick = false;
    rightMouseclick = false;
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
        if (event.button.button == 2)
        {
            middleClick = true;
        }
        else if (event.button.button == 3)
        {
            rightMouseclick = true;
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

bool resetGame(void)
{
    playGameStartAudio();
    player = true;
    resetStorage();
    resetPieces(whiteHeadPiece, true);
    resetPieces(blackHeadPiece, false);
    if (!createTable())
    {
        return false;
    }
    return true;
}

bool update(void)
{
    if (middleClick && !resetGame())
        return false;

    if (LIMIT_FPS)
        SDL_Delay(WAIT_TIME);

    // Cursor Position
    SDL_GetMouseState(&mousepos->x, &mousepos->y);
    Tile mouseTile = TileFromPos(mousepos);
    // Set up for player
    playerPieces = player ? &whiteHeadPiece : &blackHeadPiece;
    opponentPieces = !player ? &whiteHeadPiece : &blackHeadPiece;
    // Select tile
    if (rightMouseclick)
    {
        TileNode *selectedTile = nodeFromTile(mouseTile, headLightTile, headDarkTile);
        if (selectedTile != NULL)
            selectedTile->selected = !(selectedTile->selected);
    }

    // Select piece
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

        // VALIDATE AND MAKE MOVE
        if (leftMouseRelease)
        {
            initMove(playerPiece, playerPiece.ptr->pos[playerPiece.index], mouseTile, player, *playerPieces, *opponentPieces);
            int result = finalizeMove(true);

            // VALID MOVE
            int yValueOfPiece = player ? WHITE_Y : BLACK_Y; // For Castling

            switch (result)
            {
            case INVALID: // Invalid move
                untrackMouse(playerPiece);
                break;
            case VALID: // Move no capture
                playMoveSound();
                movePiece(playerPiece, mouseTile);
                break;
            case VALID_CAPTURE: // Move + capture
                playCaptureSound();
                deletePiece(pieceFromTile(mouseTile, *opponentPieces));
                movePiece(playerPiece, mouseTile);
                break;
            case KINGSIDE_CASTLING: // Castle KingSide
                playCastleSound();
                movePiece(playerPiece, mouseTile);
                movePiece(pieceFromTile((Tile){ROOK_X[1], yValueOfPiece}, *playerPieces), (Tile){6, yValueOfPiece});
                break;
            case QUEENSIDE_CASTLING: // Castle QueenSide
                playCastleSound();
                movePieceFromPos(playerPiece, mousepos);
                movePiece(pieceFromTile((Tile){ROOK_X[0], yValueOfPiece}, *playerPieces), (Tile){4, yValueOfPiece});
                break;
            case ENPASSANT: // enpassant
                playCaptureSound();
                movePiece(playerPiece, mouseTile);
                Tile niceEn = (Tile){mouseTile.x, mouseTile.y + (player ? -1 : 1)};
                deletePiece(pieceFromTile(niceEn, *opponentPieces));
                break;
            default:
                SDL_Log("That move has not been set up yet\n");
                break;
            }

            if (result != INVALID)
            {
                bool checkStatus = setCheck();
                if (player)
                {
                    BlackCheck = checkStatus;
                    WhiteCheck = false;
                }
                else
                {
                    WhiteCheck = checkStatus;
                    BlackCheck = false;
                }
                // Sfx
                if (checkStatus)
                {
                    playCheckSound();
                }

                player = !player;
                // RECORDING MOVE
                if (player)
                {
                    SDL_snprintf(moveMadeWhite, sizeof(moveMadeWhite), "%s to (%c%d)", playerPiece.ptr->type, chessX(mouseTile.x), mouseTile.y);
                }
                else
                {
                    SDL_snprintf(moveMadeBlack, sizeof(moveMadeBlack), "%s to (%c%d)", playerPiece.ptr->type, chessX(mouseTile.x), mouseTile.y);
                    recordMove(moveMadeWhite, moveMadeBlack);
                }
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

    return true;
}

// Returns True if all Items where rendered successfully
bool render(void)
{
    // Background
    if (!SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], BACKGROUND_COLOR[3]))
        return false;
    if (!SDL_RenderClear(renderer))
        return false;

    // Light Tiles
    if (!renderTileNodes(renderer, headLightTile, LIGHT_TILE_COLOR))
        return false;

    // Dark Tiles
    if (!renderTileNodes(renderer, headDarkTile, DARK_TILE_COLOR))
        return false;

    // Black Pieces
    if (!renderPieces(renderer, blackHeadPiece))
        return false;

    // White Pieces
    if (!renderPieces(renderer, whiteHeadPiece))
        return false;

    // Present
    if (!SDL_RenderPresent(renderer))
        return false;
    return true;
}

void clean(void)
{
    cleanAudio();
    closeDataBase();
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
        if (!update())
            running = false;
        if (!render())
            running = false;
    }

    // Error during launch
    if (!successfulLaunch)
        return 1;

    // Success
    clean();
    return 0;
}