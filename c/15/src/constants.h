#define PI 3.14159265
#define TWO_PI 6.28318530
#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20
#define NUM_TEXTURES 8

#define MINIMAP_SCALE_FACTOR 0.2


#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)


#define FOV_ANGLE (60 * (PI / 180))
#define NUM_RAYS WINDOW_WIDTH

//tan(x) = opp/adj -> adj * tan(x) = opp -> adj = opp/ tan(x)
// DIST_PROJ_PLANE = ajd; (WINDOW_WIDTH / 2) = opp; FOV_ANGLE / 2 = desired angle
//
//         window width
//        [___________] --> Opp
//              |    /
//              |   /
// adj side (?) |  /
//  (unkown)    | /
//              .  ----> angle = FOV_ANGLE / 2

#define DIST_PROJ_PLANE ((WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2))

#define FPS 30
#define FRAME_TIME_LENGTH (1000 / FPS)