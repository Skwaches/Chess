#include "../links/funcs.h"

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

/**First available move*/
Tile bot1(Move *pool, Piece *pieceholder, int *valid)
{
    for (int a = 0; a < PIECETYPES; a++)
        for (int b = 0; b < pool[a].pieces->appearances; b++)
        {
            if (!pool[a].no[b])
                continue;
            if (valid && pool[a].valids[b])
                *valid = pool[a].valids[b][0];
            if (pieceholder)
                *pieceholder = (Piece){pool[a].pieces, b};
            return pool[a].dreams[b][0];
        }
    return SHADOW_REALM;
}

/**
 * Random piece Type.
 */
Tile bot2(Move *pool, Piece *pieceholder, int *valid)
{
    int searchd[PIECETYPES];
    int poss = 0;
    while (poss < PIECETYPES)
    {
        searchd[poss] = poss;
        poss++;
    }
    shuffle(searchd, PIECETYPES);
    for (int a = 0; a < PIECETYPES; a++)
    {
        int Typeindex = searchd[a];
        PieceNode *chosenType = pool[Typeindex].pieces;
        int **validResults = pool[Typeindex].valids;
        Tile **dreamBox = pool[Typeindex].dreams;
        int *nos = pool[Typeindex].no;
        int apers[chosenType->appearances];
        poss = 0;
        while (poss < chosenType->appearances)
        {
            apers[poss] = poss;
            poss++;
        }
        shuffle(apers, chosenType->appearances);
        for (int b = 0; b < chosenType->appearances; b++)
        {
            int pieceChosen = apers[b];
            int choices = nos[pieceChosen];
            if (!choices)
                continue;
            if (valid && validResults[pieceChosen])
                *valid = validResults[pieceChosen][0];
            if (pieceholder)
                *pieceholder = (Piece){chosenType, pieceChosen};
            return dreamBox[pieceChosen][0];
        }
    }
    return SHADOW_REALM;
}

/**
 * Random piece chosen.
 */
Tile bot3(Move *pool, Piece *pieceholder, int *valid)
{
    int searchd[PIECETYPES];
    int poss = 0;
    while (poss < PIECETYPES)
    {
        searchd[poss] = poss;
        poss++;
    }
    shuffle(searchd, PIECETYPES);
    for (int a = 0; a < PIECETYPES; a++)
    {
        int Typeindex = searchd[a];
        PieceNode *chosenType = pool[Typeindex].pieces;
        int **validResults = pool[Typeindex].valids;
        Tile **dreamBox = pool[Typeindex].dreams;
        int apers[chosenType->appearances];
        poss = 0;
        while (poss < chosenType->appearances)
        {
            apers[poss] = poss;
            poss++;
        }
        shuffle(apers, chosenType->appearances);
        for (int b = 0; b < chosenType->appearances; b++)
        {
            int pieceChosen = apers[b];
            if (valid && validResults[pieceChosen])
                *valid = validResults[pieceChosen][0];
            if (pieceholder)
                *pieceholder = (Piece){chosenType, pieceChosen};
            return dreamBox[pieceChosen][0];
        }
    }
    return SHADOW_REALM;
}

/*Every thing about this bot is random*/
Tile bot4(Move *pool, Piece *pieceholder, int *valid)
{
    int searchd[PIECETYPES];
    int poss = 0;
    while (poss < PIECETYPES)
    {
        searchd[poss] = poss;
        poss++;
    }
    shuffle(searchd, PIECETYPES);
    for (int a = 0; a < PIECETYPES; a++)
    {
        int Typeindex = searchd[a];
        PieceNode *chosenType = pool[Typeindex].pieces;
        int *posNumbers = pool[Typeindex].no;
        int **validResults = pool[Typeindex].valids;
        Tile **dreamBox = pool[Typeindex].dreams;
        int apers[chosenType->appearances];
        poss = 0;
        while (poss < chosenType->appearances)
        {
            apers[poss] = poss;
            poss++;
        }
        shuffle(apers, chosenType->appearances);
        for (int b = 0; b < chosenType->appearances; b++)
        {
            int pieceChosen = apers[b];
            int choices = posNumbers[pieceChosen];
            if (!choices)
                continue;
            int destChosen = SDL_rand(choices);
            if (valid && validResults[pieceChosen])
                *valid = validResults[pieceChosen][destChosen];
            if (pieceholder)
                *pieceholder = (Piece){chosenType, pieceChosen};
            return dreamBox[pieceChosen][destChosen];
        }
    }
    return SHADOW_REALM;
}