#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "constants.h"
#include "graphics.h"



bool mapHasWallAt(float x, float y);
bool isInsideMap(float x, float y);
void renderMap(void);
int getMapAt(int i, int j);

#endif
