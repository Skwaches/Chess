#include "Linkers/funcs.h"

#pragma region Starting values.
bool leftMouseHeld = false;
bool leftMouseRelease = false;
bool leftMouseclick = false;
bool rightMouseclick = false;
bool middleClick = false;
bool successfulLaunch = true;
bool running = true;
bool player = true; // WHITE :D
bool checkedMate = true;
bool mate = false;
bool stale = false;
bool checkStatus;
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

bool resetGame(void)
{
    playGameStartAudio();
    player = true;
    mate = false;
    stale = false;
    checkedMate = true;
    resetStorage();
    unselectAll(headDarkTile, headLightTile);
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
    int fps = getFPS(100000);
    if (fps)
        SDL_Log("%d frames per second", fps);

    if (middleClick && !resetGame())
        return false;

    if (LIMIT_FPS)
        SDL_Delay(WAIT_TIME);

    // Cursor Position
    SDL_GetMouseState(&mousepos.x, &mousepos.y);
    static Tile destTile, originalTile;
    static int result = INVALID;

    // Select tile
    if (rightMouseclick)
    {
        Tile mouseTile = TileFromPos(mousepos);
        TileNode *selectedTile = nodeFromTile(mouseTile, headLightTile, headDarkTile);
        if (selectedTile != NULL)
            selectedTile->selected = !(selectedTile->selected);
    }

    /*Select a piece.*/
    if (leftMouseclick)
    {
        Tile mouseTile = TileFromPos(mousepos);
        playerPiece = pieceFromTile(mouseTile, *playerPieces, NULL);
        opponentPiece = pieceFromTile(mouseTile, *opponentPieces, NULL); // COUNTS AS INVALID MOVE.
        unselectAll(headDarkTile, headLightTile);

        if (playerPiece.ptr && !(mate || stale))
        { /*Possible moves*/
            int balls;
            Tile *foundMoves = validMoves(playerPiece, *playerPieces, *opponentPieces, player, &balls);

            for (int x = 0; x < balls; x++)
            {
                TileNode *foundTile = nodeFromTile(foundMoves[x], headLightTile, headDarkTile);
                if (foundTile)
                    foundTile->selected = true;
            }
            SDL_free(foundMoves);
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
        // No evaluation occurs
        if (!playerPiece.ptr ||
            mate || stale)
        {
            if (mate)
                SDL_Log("MATE");
            if (stale)
                SDL_Log("Stale");
            result = INVALID;
        }

        // Evaluation
        else
        {
            destTile = TileFromPos(mousepos);
            originalTile = playerPiece.ptr->pos[playerPiece.index];
            initMove(playerPiece, originalTile, destTile, player, *playerPieces, *opponentPieces);
            result = finalizeMove(true, &checkStatus);
        }

        if (result == INVALID)
        {
            if (playerPiece.ptr)
                untrackMouse(playerPiece);
            if (opponentPiece.ptr)
                untrackMouse(opponentPiece);
            return true;
        }

        playRightSound(checkStatus, result);
        int yValueOfPiece = player ? WHITE_Y : BLACK_Y;

        /*Handle result*/

        Tile rooksTile, rookDest;
        Piece rookPiece;
        char knownPiece = ROOK_NAME;
        if (result != PROMOTION && result != PROMOTION_CAPTURE)
            movePiece(playerPiece, destTile);
        else
            promotePiece(playerPiece, 'Q', *playerPieces, destTile);
        switch (result)
        {
        case VALID: // Move no capture
            break;
        case VALID_CAPTURE: // Move + capture
            deletePiece(pieceFromTile(destTile, *opponentPieces, NULL));
            break;
        case KINGSIDE_CASTLING: // Castle KingSide
            rooksTile = (Tile){ROOK_X[1], yValueOfPiece};
            rookDest = (Tile){6, yValueOfPiece};

            rookPiece = pieceFromTile(rooksTile, *playerPieces, &knownPiece);
            movePiece(rookPiece, rookDest);
            break;
        case QUEENSIDE_CASTLING: // Castle QueenSide
            rooksTile = (Tile){ROOK_X[0], yValueOfPiece};
            rookDest = (Tile){4, yValueOfPiece};
            rookPiece = pieceFromTile(rooksTile, *playerPieces, &knownPiece);
            movePiece(rookPiece, rookDest);
            break;
        case ENPASSANT: // enpassant
            Tile niceEn = (Tile){destTile.x, destTile.y + (player ? -1 : 1)};
            deletePiece(pieceFromTile(niceEn, *opponentPieces, NULL));
            break;
        case PROMOTION:
            break;
        case PROMOTION_CAPTURE:
            deletePiece(pieceFromTile(destTile, *opponentPieces, NULL));
            break;
        default:
            SDL_Log("That move has not been set up yet\n");
            break;
        }
        checkedMate = false;
        unselectAll(headDarkTile, headLightTile);
    }

    /**Check if your opponent has a move */
    if (!checkedMate)
    {
        bool enemyBool = !player;
        bool cantPlay = checkMate(*playerPieces, *opponentPieces, enemyBool);
        mate = cantPlay && checkStatus;
        stale = cantPlay && !checkStatus;
        recordMovesyntax(playerPiece, originalTile, destTile, *playerPieces,
                         *opponentPieces, result, checkStatus, mate, enemyBool);
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