#include "map.h"

static const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1}, // 0
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}, // 1
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1}, // 2
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 3
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1}, // 4
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 5
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 6
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1}, // 7
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // 8
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5}, // 9
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5}, // 10
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5}, // 11
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}  // 12
  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,20 (21 columns) 
};


bool mapHasWallAt(float x, float y) 
{
    if (x < 0 || x >= MAP_NUM_COLS * TILE_SIZE || 
        y < 0 || y >= MAP_NUM_ROWS * TILE_SIZE) 
    {
        return true;
    }
    
    //note: X and Y are given in pixels, we need to translate this to tiles coordinates
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}



bool isInsideMap(float x, float y) 
{
    return (
        x >= 0 && x <= MAP_NUM_COLS * TILE_SIZE && 
        y >= 0 && y <= MAP_NUM_ROWS * TILE_SIZE
    );
}

int getMapAt(int i, int j) 
{
    return map[i][j];
}

void renderMap(void)
{
    for(int i = 0; i < MAP_NUM_ROWS; i++)
    {
        for(int j = 0; j < MAP_NUM_COLS; j++)
        {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            color_t tileColor = map[i][j] != 0 ? 0xFFFFFFFF : 0x00000000;
            drawRect
	    (
                tileX * MINIMAP_SCALE_FACTOR,
                tileY * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                tileColor
            );

        }
    }

    //------------------
    //test coordinates - start
    // SDL_Rect mapTileRect2 = 
    // {
    //     0 + 800, //x
    //     0 + 200 ,//y
    //     TILE_SIZE, //width
    //     TILE_SIZE  //height
    // };

    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // SDL_RenderFillRect(renderer , &mapTileRect2);

    //test coordinates - end
    //------------------
}

