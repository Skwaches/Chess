#ifndef CLASSES_H
#define CLASSES_H

#include <SDL3/SDL.h>
typedef struct coords
{
    int x;
    int y;
} coords;

typedef struct tileNode
{
    coords pos;
    SDL_FRect rect;
    struct tileNode *next;
} tileNode;

typedef struct pieceNode
{
    int appearances;
    char **name;
    coords *pos;
    SDL_FRect *rect;
    SDL_Texture *texture;
    struct pieceNode *prev;
    struct pieceNode *next;
} pieceNode;

typedef struct Piece
{
    pieceNode *ptr;
    int index;
} Piece;
#endif