#ifndef RESOURCES_H
#define RESOURCES_H
#include <SDL2/SDL.h>
#include <toolbox/toolbox.h>
typedef enum
{
  TEX_BIG_DEMON,
  TEX_KNIGHT,
  TEX_AXE,
  TEX_ANIME_SWORD,
  TEX_BLOOD,
  NUM_TEXS
} TextureId;
BOOL         resources_load();
void         resources_unload();
SDL_Texture* get_texture(int texture_id);
#endif // RESOURCES_H
