#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stdint.h>
#include "constants.h"
#include "upng.h"

#define NUM_TEXTURES 13

extern upng_t* textures[NUM_TEXTURES];



void loadTextures(void);
void freeTextures(void);

#endif

