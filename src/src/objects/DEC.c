//
// Created by tmsz on 2023/1/10.
//

#include "DEC.h"

struct DEC_Decoration* DEC_CreateDecoration()
{
    struct DEC_Decoration* D = malloc(sizeof(struct DEC_Decoration));
    D->Type = rand()%2;
    return D;
}

void DEC_DestroyDecoration(struct DEC_Decoration* D)
{
    free(D);
}