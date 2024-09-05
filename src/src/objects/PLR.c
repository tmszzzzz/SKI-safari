//
// Created by tmsz on 2023/1/7.
//

#include "PLR.h"

struct PLR_Player* PLR_Create()
{
    struct PLR_Player* P = malloc(sizeof(struct PLR_Player));
    return P;
}

void PLR_Destroy(struct PLR_Player* P)
{
    free(P);
}