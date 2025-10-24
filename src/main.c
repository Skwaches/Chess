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
bool player;
bool checkedMate = true;
bool checkStatus = false;
bool validationComplete = false;

/*Game ending */
bool gameOver = false;
bool movesOver = false;
bool mate = false;
bool stale = false;

Tile (*BOT_MAIN)(Move *, Piece *, int *) = &bot4;
static Tile destTile, originalTile, *validDest;
static int destOptions = 0;
static int *validCounters;
static char pawnoGo = PROMODEFAULT;
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

/*Variables for highlighting previous moves*/
TileNode *prevHoverTile = NULL;
TileNode *fromTileNode = NULL;
TileNode *toTileNode = NULL;

PieceNode *topHeadPiece = NULL;
PieceNode *bottomHeadPiece = NULL;
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
		SDL_Log("Video sub-system couldn't be initialised.");
        accident();
        return;
    }
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
		SDL_Log("Audio sub-system couldn't be initialised.");
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
		closeDataBase();
        accident();
		return;
    }

    window = SDL_CreateWindow("CHESS", SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
		SDL_Log("Window couldn't be created.");
        closeDataBase();
        accident();
        return;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
		SDL_Log("Renderer could not be created.");
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
	const char* bottomPath = HUMAN? WHITE_PIECES_PATH: BLACK_PIECES_PATH;
	const char* topPath    = !HUMAN? WHITE_PIECES_PATH: BLACK_PIECES_PATH;
	bottomHeadPiece = setPieces(renderer,true,bottomPath);
   	topHeadPiece = setPieces(renderer,false,topPath);
    if (topHeadPiece == NULL || bottomHeadPiece == NULL)
    {
        clean_pieces();
        accident();
        SDL_Log("Problem with pieces");
        return;
    }
    player = STARTSIDE;
	/*For side selection*/
   	playerPieces   = player ?  &bottomHeadPiece : &topHeadPiece;
	opponentPieces = !player ? &bottomHeadPiece : &topHeadPiece;
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
    while(SDL_PollEvent(&event)){
    switch (event.type){
	 	case SDL_EVENT_QUIT:
        	running = false;
       		break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
    		switch(event.button.button){
       			case 1: 
					leftMouseclick = true;
            		leftMouseHeld = true;
					break;
				case 2:
            		middleClick = true;
					break;
				case 3:
            		rightMouseclick = true;
					break;
        		default: break;}
       		 break;
    
		case SDL_EVENT_MOUSE_BUTTON_UP:
    	    switch(event.button.button){
				case 1:
            		leftMouseHeld = false;
            		leftMouseRelease = true;
        			break;
				default:break;}
       		 break;

   		 case SDL_EVENT_KEY_DOWN:
        	switch(event.key.scancode){
				case SDL_SCANCODE_F10:
            		F10clicked = true;
					break;
				case SDL_SCANCODE_F4:
            		F4clicked = true;
					break;
        		case SDL_SCANCODE_ESCAPE:
            		running = false;
					break;
				default:break;}
        	break;
    	default:break;
 	   }

	}
}

bool newGame(void){
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
    unselectAll(headDarkTile, headLightTile,true);
    resetPieces(bottomHeadPiece, true);
    resetPieces(topHeadPiece, false);
    playerPieces = player ? &bottomHeadPiece : &topHeadPiece;
    opponentPieces = !player ? &bottomHeadPiece : &topHeadPiece;
    freeMoves(allMoves);
    allMoves = selectionPool(*playerPieces, *opponentPieces, player);
    int currentTable = createTable();
	return true;
}
/**
 * I want to stop renderering when there is no need.
 * Will make update return special values for rendering. Add this to defaults.h or something.*/
void update(void)
{
    SDL_GetMouseState(&mousepos.x, &mousepos.y);

    if (LIMIT_FPS)
        SDL_Delay(WAIT_TIME);

    if (F4clicked)
        newGame();
    
	if (rightMouseclick)
    {
        Tile mouseTile = TileFromPos(mousepos);
        TileNode *selectedTile = nodeFromTile(mouseTile, headLightTile, headDarkTile);
        if (selectedTile)
            selectedTile->selected = selectedTile->selected==SELECTED_TILE ? NORMAL_TILE: SELECTED_TILE;
    }

    if (leftMouseclick)
    {
        Tile mouseTile = TileFromPos(mousepos);
        
		/*Select a piece.*/
		Piece foundPiece = pieceFromTile(mouseTile, *playerPieces, NULL);
		Piece opponentPiece = pieceFromTile(mouseTile, *opponentPieces, NULL);	
		if (foundPiece.ptr)
        {
            unselectAll(headDarkTile, headLightTile,false);
			playerPiece = foundPiece;
        	
		}
		if(opponentPiece.ptr){
			
		}
		/*Attempt move if piece is already selected*/
        if (playerPiece.ptr)
        {
            validDest = tileFromPool(playerPiece, allMoves, &destOptions, &validCounters);
            if(false)
				highlightValidTiles(destOptions, validDest, headDarkTile, headLightTile);
            if (!foundPiece.ptr){
                destTile = mouseTile;
                originalTile = playerPiece.ptr->pos[playerPiece.index];
                result = approveSelection(destOptions, destTile, validDest, validCounters);
                validationComplete = true;
            }
        }
    }

    if (leftMouseHeld)
    {
        if (playerPiece.ptr){
			trackMouse(playerPiece, mousepos);
			if(!gameOver){
			Tile mouseTile = TileFromPos(mousepos);	
		    if(prevHoverTile && prevHoverTile != fromTileNode && prevHoverTile != toTileNode)
				prevHoverTile->selected = NORMAL_TILE;
			TileNode* currentTile = nodeFromTile(mouseTile, headLightTile, headDarkTile);
		    if(currentTile && currentTile != fromTileNode && currentTile != toTileNode)
				currentTile->selected = PIECE_HOVER;
				prevHoverTile =	currentTile;			
			}}
	}
    

    if (leftMouseRelease)
    {
        if (!playerPiece.ptr || gameOver || validationComplete)
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
    if (player == BOT)
    {
        if (mate || stale)
            return ;
        if (!savedTime)
            savedTime = SDL_GetTicks();

        if (SDL_GetTicks() - savedTime < BOT_DELAY)
            return ;
        
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
            unselectAll(headDarkTile, headLightTile,false);
            playerPiece = NULL_PIECE;
            return ;
        }
        initMove(playerPiece, originalTile, destTile, player,
                 *playerPieces, *opponentPieces, pawnoGo);
        fullLogicUpdate(result, &checkStatus);
        playRightSound(checkStatus, result);
        performMove(result, playerPiece, destTile, pawnoGo, playerPieces, opponentPieces, player);
        
		/*Remove previous highlight*/
		if(fromTileNode)
			fromTileNode->selected = NORMAL_TILE;
		if(toTileNode)
			toTileNode->selected = NORMAL_TILE;
	
		/*Highlight move made*/
		fromTileNode = nodeFromTile(originalTile, headLightTile, headDarkTile); 
	 	toTileNode = nodeFromTile(destTile, headLightTile, headDarkTile);
		if(fromTileNode)
			fromTileNode->selected = PREV_ORIG;
		if(toTileNode)
			toTileNode->selected = PREV_DEST;

		checkedMate = false;
        result = INVALID;
    }

    /**Check if your opponent has a move */
    if (!checkedMate)
    {
        if (LIMIT_MOVES_PER_GAME)
            movesOver = totalMoves > MAX_MOVES_PER_GAME;

        unselectAll(headDarkTile, headLightTile,false);
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
                SDL_Log("Game was terminated.");
            if (AUTOSTART_NEWGAME)
                newGame();
            checkedMate = true;
            return ;
        }
        player = !player;
        playerPieces = player ? &bottomHeadPiece : &topHeadPiece;
        opponentPieces = !player ? &bottomHeadPiece : &topHeadPiece;
        playerPiece = NULL_PIECE;
        checkedMate = true;
        totalMoves++;
    }
}

bool render(void)
{
    // Background
    if (!setRenderColor(renderer, BACKGROUND_COLOR))
    {
        SDL_Log("Background Render Colour fail");
        return false;
    }

    if (!SDL_RenderClear(renderer)){
        SDL_Log("Background Render Fail.");
        return false;
    }

    // Light Tiles
    if (!renderTileNodes(renderer, headLightTile, LIGHT_TILE_COLOR))
    {
        SDL_Log("LightTiles Render Fail.");
        return false;
    }

    // Dark Tiles
    if (!renderTileNodes(renderer, headDarkTile, DARK_TILE_COLOR))
    {
        SDL_Log("DartTiles Render Fail.");
        return false;
    }

    // Top Pieces
    if (!renderPieces(renderer, topHeadPiece))
    {
        SDL_Log("Black-Pieces Render Fail.");
        return false;
    }

    // Bottom  Pieces
    if (!renderPieces(renderer, bottomHeadPiece))
    {
        SDL_Log("White-Pieces Render Fail.");
        return false;
    }
	
    // Present
    if (!SDL_RenderPresent(renderer))
    {
        SDL_Log("Render Present Fail.");
        return false;
    }
    return true;
}

void clean(void)
{
    freeMoves(allMoves);
    cleanAudio();
    closeDataBase();
    freePieces(topHeadPiece);
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
