#include "Linkers/funcs.h"

#pragma region Starting values.
bool leftMouseHeld = false;
bool leftMouseRelease = false;
bool leftMouseclick = false;
bool rightMouseclick = false;
bool middleClick = false;
bool successfulLaunch = true;
bool running = true;

bool player = STARTSIDE; /*White Starts*/
bool checkedMate = true;
bool mate = false;
bool stale = false;
bool checkStatus = false;
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

bool newGame(void)
{
    playGameStartAudio();
    player = STARTSIDE;
    mate = false;
    stale = false;
    allMoves = NULL;
    checkedMate = true;
    resetStorage();
    unselectAll(headDarkTile, headLightTile);
    resetPieces(whiteHeadPiece, true);
    resetPieces(blackHeadPiece, false);
    playerPieces = player ? &whiteHeadPiece : &blackHeadPiece;
    opponentPieces = !player ? &whiteHeadPiece : &blackHeadPiece;
    if (!createTable())
        return false;
    return true;
}

bool update(void)
{
    if (middleClick && !newGame())
        return false;
    if (LIMIT_FPS)
        SDL_Delay(WAIT_TIME);
    int fps = getFPS(100);
    if (fps)
    {
        SDL_Log("%d\n", fps);
    }
    // Cursor Position
    SDL_GetMouseState(&mousepos.x, &mousepos.y);
    static Tile destTile, originalTile, *validDest;
    static int result = INVALID;
    static int destOptions, *validCounters;
    static char pawnoGo = PROMODEFAULT;
    static Uint64 savedTime = 0;
    if (!allMoves)
        allMoves = selectionPool(*playerPieces, *opponentPieces, player);

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
        if (playerPiece.ptr && !(mate || stale) && allMoves)
        {
            validDest = tileFromPool(playerPiece, allMoves, &destOptions, &validCounters);
            for (int a = 0; a < destOptions; a++)
            {
                TileNode *foundNode = nodeFromTile(validDest[a], headDarkTile, headLightTile);
                if (foundNode)
                    foundNode->selected = true;
            }
        }
    }

    /**Track mouse */
    if (leftMouseHeld)
    {
        if (playerPiece.ptr)
            trackMouse(playerPiece, mousepos);
        if (opponentPiece.ptr)
            trackMouse(opponentPiece, mousepos);
    }

    /**Validate and make move */
    if (leftMouseRelease)
    {
        if (!playerPiece.ptr ||
            mate || stale || player == BOT)
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

    if (player == BOT)
    {
        if (mate || stale)
            return true;
        if (!savedTime)
            savedTime = SDL_GetTicks();
        if (SDL_GetTicks() - savedTime < BOT_DELAY)
            return true;
        savedTime = 0;
        destTile = randomMoveFromPool(allMoves, &playerPiece, &result);
        originalTile = playerPiece.ptr->pos[playerPiece.index];
    }

    if (result)
    {
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
        unselectAll(headDarkTile, headLightTile);
        freeMoves(allMoves);
        allMoves = NULL;
        bool enemyBool = !player;
        bool cantPlay = checkMate(*playerPieces, *opponentPieces, enemyBool);
        mate = cantPlay && checkStatus;
        stale = cantPlay && !checkStatus;
        recordMovesyntax(playerPiece, originalTile, destTile, *playerPieces,
                         *opponentPieces, result, checkStatus, mate, enemyBool, pawnoGo);
        if (mate || stale)
        {
            playGameEndAudio();
            if (mate)
                SDL_Log("%d WON!", player);
            if (stale)
                SDL_Log("%d caused stalemate....", player);
        }
        checkedMate = true;
        player = !player;
        /*For side selection*/
        playerPieces = player ? &whiteHeadPiece : &blackHeadPiece;
        opponentPieces = !player ? &whiteHeadPiece : &blackHeadPiece;
        playerPiece = NULL_PIECE;
    }

    return true;
}

// Returns True if all Items where rendered successfully
bool render(void)
{
    // Background
    if (!setRenderColor(renderer, BACKGROUND_COLOR))
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