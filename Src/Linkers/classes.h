#ifndef CLASSES_H
#define CLASSES_H
#include "funcs.h"
typedef struct Tile
{
    int x;
    int y;
} Tile;

typedef struct TileNode
{
    Tile pos;
    SDL_FRect rect;
    bool selected;
    struct TileNode *next;
} TileNode;

typedef struct PieceNode
{
    char type;
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

typedef struct CastlingOptions
{
    bool queenside;
    bool kingside;
} CastlingOptions;

#endif