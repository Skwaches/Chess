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

// CLEANING FUNCTIONS
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

    headLightTile = setTiles(false);
    headDarkTile = setTiles(true);
    if (headDarkTile == NULL || headLightTile == NULL)
    {
        clean_tile();
        accident();
        SDL_Log("Problem with Tiles");
    }
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

        // MAKE MOVE
        if (leftMouseRelease)
        {
            Tile mouseTile = TileFromPos(mousepos);
            initMove(playerPiece, mouseTile, player, *playerPieces, *opponentPieces);
            int result = finalizeMove();

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
                deletePiece(pieceFromTile(mouseTile, *opponentPieces), opponentPieces);
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
                deletePiece(pieceFromTile(niceEn, *opponentPieces), opponentPieces);
                SDL_Log("Nice\n\n\n");
            default:
                // SDL_Log("That move has not been set up yet\n");
                break;
            }

            if (result)
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
                SDL_Log("White : %d", WhiteCheck);
                SDL_Log("Black : %d\n\n", BlackCheck);
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
}

bool render(void)
{
    // Background
    if (!SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255))
        return false;
    if (!SDL_RenderClear(renderer))
        return false;

    // Light Tiles
    if (!SDL_SetRenderDrawColor(renderer, LIGHT_TILE_COLOR[0], LIGHT_TILE_COLOR[1], LIGHT_TILE_COLOR[2], LIGHT_TILE_COLOR[3]))
        return false;

    if (!renderTileNodes(renderer, headLightTile))
        return false;

    // Dark Tiles
    if (!SDL_SetRenderDrawColor(renderer, DARK_TILE_COLOR[0], DARK_TILE_COLOR[1], DARK_TILE_COLOR[2], DARK_TILE_COLOR[3]))
        return false;

    if (!renderTileNodes(renderer, headDarkTile))
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
        update();
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