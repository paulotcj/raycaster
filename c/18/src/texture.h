#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdio.h>
#include <stdint.h>
#include "constants.h"
#include "upng.h"

#define NUM_TEXTURES 8

typedef struct
{
    upng_t* upngTexture;
    int width;
    int height;
    uint32_t* texture_buffer;
} texture_t;

texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures(void);
void freeWallTextures(void);

#endif

