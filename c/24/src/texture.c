#include "texture.h"

upng_t* textures[NUM_TEXTURES];

static const char* textureFileNames[NUM_TEXTURES] = 
{
    "./images/redbrick.png",    // [0]
    "./images/purplestone.png", // [1]
    "./images/mossystone.png",  // [2]
    "./images/graystone.png",   // [3]
    "./images/colorstone.png",  // [4]
    "./images/bluestone.png",   // [5]
    "./images/wood.png",        // [6]
    "./images/eagle.png",       // [7]
    "./images/barrel.png",      // [8]
    "./images/light.png",       // [9]
    "./images/table.png",       // [10]
    "./images/guard.png",       // [11]
    "./images/armor.png"        // [12]
};

//-----

void loadTextures(void)
{
    for(int i = 0 ; i < NUM_TEXTURES ; i++)
    {
        upng_t* upng = upng_new_from_file(textureFileNames[i]);

        if(upng != NULL)
        {
            upng_decode(upng);
            if(upng_get_error(upng) == UPNG_EOK) //UPNG_EOK = success
            {
                // wallTextures[i].upngTexture = upng;
                // wallTextures[i].width = upng_get_width(upng);
                // wallTextures[i].height = upng_get_height(upng);
                // wallTextures[i].texture_buffer = (color_t*) upng_get_buffer(upng);

                textures[i] = upng;
            }
            else 
            {
                printf("Error decoding texture file %s \n", textureFileNames[i]);
            }
        }
        else 
        {
            printf("Error loading texture %s \n", textureFileNames[i]);
        }
    }
}



//-------
void freeTextures(void)
{
    for(int i = 0 ; i < NUM_TEXTURES ; i++)
    {
        upng_free(textures[i]);
    }
}



