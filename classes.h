#ifndef CLASSES_H
#define CLASSES_H

#include <SDL3/SDL.h>
typedef struct Tile
{
    int x;
    int y;
} Tile;

typedef struct TileNode
{
    Tile pos;
    SDL_FRect rect;
    struct TileNode *next;
} TileNode;

typedef struct PieceNode
{
    const char *type;
    int noInPlay;
    int appearances;
    Tile *pos;
    SDL_FRect *rect;
    SDL_Texture *texture;
    struct PieceNode *prev;
    struct PieceNode *next;
} PieceNode;

typedef struct Piece
{
    PieceNode *ptr;
    int index;
} Piece;
#endif