//
// Created by tmsz on 2023/1/10.
//

#ifndef SKI_DEC_H
#define SKI_DEC_H

#include "SDL2/SDL.h"
#include <stdlib.h>
#include "SDL2/SDL_image.h"
#include "PLR.h"

struct DEC_Decoration
{
    uint32_t Type;
    int x;
    int y;
};

enum DEC_Type {
    DEC_TREE_1,
    DEC_TREE_2
};

struct DEC_Decoration* DEC_CreateDecoration();

void DEC_DestroyDecoration(struct DEC_Decoration* D);

#endif //SKI_DEC_H
