#include "Linkers/funcs.h"

#pragma region Starting values.
bool leftMouseHeld = false;
bool leftMouseRelease = false;
bool leftMouseclick = false;
bool rightMouseclick = false;
bool middleClick = false;
bool F10clicked = false;
bool F4clicked = false;
bool successfulLaunch = true;
bool running = true;
bool player = STARTSIDE;
bool checkedMate = true;
bool gameOver = false;
bool mate = false;
bool stale = false;
bool checkStatus = false;
Tile (*BOT_MAIN)(Move *, Piece *, int *) = &bot4;
static Tile destTile, originalTile, *validDest;
static int destOptions, *validCounters;
static char pawnoGo = PROMODEFAULT;
static bool selfplayer = SELFPLAY;
static bool accelBots = false;
static Uint64 savedTime = 0;
static int totalMoves = 0;

int result = INVALID;
Move *allMoves = NULL;
PieceNode **playerPieces = NULL;
PieceNode **opponentPieces = NULL;
Piece playerPiece = NULL_PIECE;
Piece opponentPiece = NULL_PIECE;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TileNode *headLightTile = NULL;
TileNode *headDarkTile = NULL;
PieceNode *blackHeadPiece = NULL;
PieceNode *whiteHeadPiece = NULL;
SDL_FPoint mousepos = {0, 0};
#pragma endregion

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
    /*For side selection*/
    playerPieces = player ? &whiteHeadPiece : &blackHeadPiece;
    opponentPieces = !player ? &whiteHeadPiece : &blackHeadPiece;
    allMoves = selectionPool(*playerPieces, *opponentPieces, player);
    playGameStartAudio();
}

void process_input(void)
{
    leftMouseRelease = false;
    leftMouseclick = false;
    middleClick = false;
    rightMouseclick = false;
    F10clicked = false;
    F4clicked = false;
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
    case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_F10)
            F10clicked = true;
        if (event.key.scancode == SDL_SCANCODE_F4)
            F4clicked = true;
        break;
    default:
        break;
    }
}

bool newGame(void)
{
    totalMoves = 0;
    playGameStartAudio();
    player = STARTSIDE;
    mate = false;
    stale = false;
    checkedMate = true;
    result = INVALID;
    gameOver = false;
    resetStorage();
    unselectAll(headDarkTile, headLightTile);
    resetPieces(whiteHeadPiece, true);
    resetPieces(blackHeadPiece, false);
    playerPieces = player ? &whiteHeadPiece : &blackHeadPiece;
    opponentPieces = !player ? &whiteHeadPiece : &blackHeadPiece;
    freeMoves(allMoves);
    allMoves = selectionPool(*playerPieces, *opponentPieces, player);
    int currentTable = createTable();
    if (!currentTable || (currentTable >= MAX_GAMES))
        return false;
    return true;
}

bool update(void)
{
    SDL_GetMouseState(&mousepos.x, &mousepos.y);

    if (LIMIT_FPS)
    {
        SDL_Delay(WAIT_TIME);
    }

    if (F10clicked)
    {
        accelBots = !accelBots;
    }

    if ((mate || stale || gameOver) && selfplayer)
    {
        if (!newGame())
            return false;
    }

    if (F4clicked)
    {
        newGame();
    }

    if (middleClick)
    {
        selfplayer = !selfplayer;
    }

    if (rightMouseclick)
    {
        Tile mouseTile = TileFromPos(mousepos);
        TileNode *selectedTile = nodeFromTile(mouseTile, headLightTile, headDarkTile);
        if (selectedTile)
            selectedTile->selected = !(selectedTile->selected);
    }

    if (leftMouseclick)
    {
        /*Select a piece.*/
        unselectAll(headDarkTile, headLightTile);
        Tile mouseTile = TileFromPos(mousepos);
        if (player == HUMAN)
            playerPiece = pieceFromTile(mouseTile, *playerPieces, NULL);
        opponentPiece = pieceFromTile(mouseTile, *opponentPieces, NULL);
        /*Highlight destination options.*/
        if (playerPiece.ptr && !(mate || stale))
        {
            validDest = tileFromPool(playerPiece, allMoves, &destOptions, &validCounters);
            if (validDest)
                for (int a = 0; a < destOptions; a++)
                {
                    TileNode *foundNode = nodeFromTile(validDest[a], headDarkTile, headLightTile);
                    if (foundNode)
                        foundNode->selected = true;
                }
        }
    }

    if (leftMouseHeld)
    {
        if (playerPiece.ptr)
            trackMouse(playerPiece, mousepos);
        if (opponentPiece.ptr)
            trackMouse(opponentPiece, mousepos);
    }

    if (leftMouseRelease)
    {
        if (!playerPiece.ptr ||
            mate || stale || selfplayer)
        {
            if (mate)
                SDL_Log("MATE");
            if (stale)
                SDL_Log("Stale");
        }
        // Evaluation
        else
        {
            destTile = TileFromPos(mousepos);
            originalTile = playerPiece.ptr->pos[playerPiece.index];
            if (destOptions)
                for (int k = 0; k < destOptions; k++)
                {
                    if (destTile.x == validDest[k].x &&
                        destTile.y == validDest[k].y)
                    {
                        result = validCounters[k];
                        break;
                    }
                }
        }
        if (result == INVALID)
        {
            if (playerPiece.ptr)
                untrackMouse(playerPiece);
            if (opponentPiece.ptr)
                untrackMouse(opponentPiece);
        }
    }

    if (selfplayer || player == BOT)
    {
        if (mate || stale)
            return true;
        if (!savedTime)
            savedTime = SDL_GetTicks();
        Uint64 botmoveDelay = !accelBots ? BOT_DELAY : (BOT_DELAY / ((BOT_ACCEL > 0) ? BOT_ACCEL : 1));
        if (SDL_GetTicks() - savedTime < botmoveDelay)
            return true;
        savedTime = 0;
        destTile = (*BOT_MAIN)(allMoves, &playerPiece, &result);
        originalTile = playerPiece.ptr->pos[playerPiece.index];
    }

    if (result)
    {
        initMove(playerPiece, originalTile, destTile, player,
                 *playerPieces, *opponentPieces, pawnoGo);
        fullLogicUpdate(result, &checkStatus);
        if (!accelBots)
            playRightSound(checkStatus, result);
        performMove(result, playerPiece, destTile, pawnoGo, playerPieces, opponentPieces, player);
        checkedMate = false;
        result = INVALID;
    }

    /**Check if your opponent has a move */
    if (!checkedMate)
    {
        if (LIMIT_MOVES_PER_GAME)
        {
            gameOver = totalMoves > MAX_MOVES_PER_GAME;
        }
        unselectAll(headDarkTile, headLightTile);
        bool enemyBool = !player;
        freeMoves(allMoves);
        allMoves = selectionPool(*opponentPieces, *playerPieces, enemyBool);
        bool cantPlay = checkMate(allMoves);
        if (cantPlay)
        {
            mate = checkStatus;
            stale = checkstale(*playerPieces, *opponentPieces);
            if (!stale)
                stale = !checkStatus;
        }
        recordMovesyntax(playerPiece, originalTile, destTile, *playerPieces,
                         *opponentPieces, result, checkStatus, mate, enemyBool,
                         pawnoGo, stale, gameOver);

        if (mate || stale || gameOver)
        {
            totalMoves = 0;
            playGameEndAudio();
            if (mate)
                SDL_Log("%d WON!", player);
            if (stale)
                SDL_Log("%d caused stalemate....", player);
            if (gameOver)
            {
                SDL_Log("Game was terminated.");
            }
            if (selfplayer)
            {
                if (!newGame())
                {
                    return false;
                }
            }
        }
        player = !player;
        playerPieces = player ? &whiteHeadPiece : &blackHeadPiece;
        opponentPieces = !player ? &whiteHeadPiece : &blackHeadPiece;
        playerPiece = NULL_PIECE;
        checkedMate = true;
        totalMoves++;
    }

    return true;
}

bool render(void)
{
    // Background
    if (!setRenderColor(renderer, BACKGROUND_COLOR))
    {
        SDL_Log("Render Fail : 0");
        return false;
    }

    if (!SDL_RenderClear(renderer))
    {
        SDL_Log("Render Fail : 1");
        return false;
    }

    if (accelBots)
    {
        if (!SDL_RenderPresent(renderer))
        {
            SDL_Log("Render Fail : 6");
            return false;
        }
        return true;
    }

    // Light Tiles
    if (!renderTileNodes(renderer, headLightTile, LIGHT_TILE_COLOR))
    {
        SDL_Log("Render Fail : 2");
        return false;
    }

    // Dark Tiles
    if (!renderTileNodes(renderer, headDarkTile, DARK_TILE_COLOR))
    {
        SDL_Log("Render Fail : 3");
        return false;
    }

    // Black Pieces
    if (!renderPieces(renderer, blackHeadPiece))
    {
        SDL_Log("Render Fail : 4");
        return false;
    }

    // White Pieces
    if (!renderPieces(renderer, whiteHeadPiece))
    {
        SDL_Log("Render Fail : 5");
        return false;
    }

    // Present
    if (!SDL_RenderPresent(renderer))
    {
        SDL_Log("Render Fail : 6");
        return false;
    }
    return true;
}

void clean(void)
{
    freeMoves(allMoves);
    cleanAudio();
    closeDataBase();
    freePieces(blackHeadPiece);
    freeTileNodes(headLightTile);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// MAIN
int main()
{
    launch();
    if (running)
        setup();

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