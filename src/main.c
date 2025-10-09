#include "library/links/funcs.h"

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
bool checkStatus = false;
bool validationComplete = false;

/*Game ending */
bool gameOver = false;
bool movesOver = false;
bool mate = false;
bool stale = false;

Tile (*BOT_MAIN)(Move *, Piece *, int *) = &bot2;
static Tile destTile, originalTile, *validDest;
static int destOptions, *validCounters;
static char pawnoGo = PROMODEFAULT;
static bool botSim = SELFPLAY;
static bool accelBots = false;
static Uint64 savedTime = 0;
static int totalMoves = 0;

int result = INVALID;
Move *allMoves = NULL;
PieceNode **playerPieces = NULL;
PieceNode **opponentPieces = NULL;
Piece playerPiece = NULL_PIECE;
Piece *premoveBuffer = NULL; /*Implement this, big boy*/
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

void process_input(void){
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
        if (event.key.scancode == SDL_SCANCODE_ESCAPE)
            running = false;
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
    movesOver = false;
    checkedMate = true;
    result = INVALID;
    gameOver = false;
    validationComplete = false;
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

    if (botSim)
    {
        if (F10clicked)
            accelBots = !accelBots;

        if (gameOver && botSim)
            if (!newGame())
                return false;
    }

    if (F4clicked)
    {
        newGame();
    }

    if (middleClick)
    {
        botSim = !botSim;
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
        Tile mouseTile = TileFromPos(mousepos);
        /*Select a piece.*/
        Piece foundPiece = pieceFromTile(mouseTile, *playerPieces, NULL);
        if (foundPiece.ptr)
        {
            unselectAll(headDarkTile, headLightTile);
            playerPiece = foundPiece.ptr ? foundPiece : playerPiece;
        }
        if (playerPiece.ptr)
        {
            validDest = tileFromPool(playerPiece, allMoves, &destOptions, &validCounters);
            highlightValidTiles(destOptions, validDest, headDarkTile, headLightTile);
            if (!foundPiece.ptr)
            {
                destTile = mouseTile;
                originalTile = playerPiece.ptr->pos[playerPiece.index];
                result = approveSelection(destOptions, destTile, validDest, validCounters);
                validationComplete = true;
            }
        }
    }

    if (leftMouseHeld)
    {
        if (playerPiece.ptr)
            trackMouse(playerPiece, mousepos);
    }

    if (leftMouseRelease)
    {
        if (!playerPiece.ptr || gameOver || botSim || validationComplete)
        {
            if (mate)
                SDL_Log("MATE");
            if (stale)
                SDL_Log("Stale");
            untrackMouse(playerPiece);
        }
        // Evaluation
        else
        {
            originalTile = playerPiece.ptr->pos[playerPiece.index];
            destTile = TileFromPos(mousepos);
            bool justSelection = destTile.x == originalTile.x && destTile.y == originalTile.y;
            if (!justSelection)
            {
                result = approveSelection(destOptions, destTile, validDest, validCounters);
                validationComplete = true;
            }
            else
                untrackMouse(playerPiece);
        }
    }

    /*Bot selects move*/
    if ((botSim || player == BOT) && !LOCAL_GAME)
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
        validationComplete = true;
    }

    /*Make move*/
    if (validationComplete)
    {
        validationComplete = false;
        if (!result)
        {
            untrackMouse(playerPiece);
            unselectAll(headDarkTile, headLightTile);
            playerPiece = NULL_PIECE;
            return true;
        }
        initMove(playerPiece, originalTile, destTile, player,
                 *playerPieces, *opponentPieces, pawnoGo);
        fullLogicUpdate(result, &checkStatus);
        playRightSound(checkStatus, result);
        performMove(result, playerPiece, destTile, pawnoGo, playerPieces, opponentPieces, player);
        checkedMate = false;
        result = INVALID;
    }

    /**Check if your opponent has a move */
    if (!checkedMate)
    {
        if (LIMIT_MOVES_PER_GAME)
            movesOver = totalMoves > MAX_MOVES_PER_GAME;

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
                         pawnoGo, stale, movesOver);
        gameOver = mate || stale || movesOver;
        if (gameOver)
        {
            totalMoves = 0;
            playGameEndAudio();
            if (mate)
                SDL_Log("%d WON!", player);
            if (stale)
                SDL_Log("%d caused stalemate....", player);
            if (movesOver)
            {
                SDL_Log("Game was terminated.");
            }
            if (AUTOSTART_NEWGAME)
            {
                if (!newGame())
                {
                    checkedMate = true;
                    return false;
                }
            }
            checkedMate = true;
            return true;
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