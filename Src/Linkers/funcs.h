#ifndef FUNCS_H
#define FUNCS_H

// Link all other header files
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "classes.h"
#include "constants.h"
#include "movesSDK.h"
#include "sounds.h"
#include "rules.h"

Tile TileFromPos(SDL_FPoint *pos);
SDL_FRect rectFromTile(Tile cords);
void freePieces(PieceNode *Headnode);
void freeTileNodes(TileNode *HeadNode);
bool renderPieces(SDL_Renderer *renderer, PieceNode *HeadPiece);
bool renderTileNodes(SDL_Renderer *renderer, TileNode *HeadTile, Uint8 *colors);

Piece pieceFromPos(PieceNode *HeadPiece, SDL_FPoint *pos); // Checks against Pieces rect ie. where it is visually.
SDL_FRect centerRectAroundPos(SDL_FPoint *pos);

void trackMouse(Piece fake_PIECE, SDL_FPoint *mouse_pos);
void untrackMouse(Piece fake_PIECE);
void movePiece(Piece fakePIECE, Tile destCoordinates);
void movePieceFromPos(Piece fake_PIECE, SDL_FPoint *pos);
void deletePiece(Piece fakePiece);

TileNode *nodeFromTile(Tile coords, TileNode *light, TileNode *dark);

/*Set board*/
TileNode *setTiles(bool start_Offset);
PieceNode *setPieces(SDL_Renderer *renderer, bool colour, const char *Path);
void resetPieces(PieceNode *colour, bool player);
#endif