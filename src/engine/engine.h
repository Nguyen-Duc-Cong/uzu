#ifndef UZU_ENGINE_H
#define UZU_ENGINE_H
#include <SDL2/SDL.h>
typedef struct {
  void *user_data;
  SDL_bool (*init)(void *);
  void (*loop)(void *, SDL_Renderer *);
  void (*fini)(void *);
  void (*event)(void*, const SDL_Event*);
} GameDelegate;

typedef struct {
  const char *window_title;
  int window_width;
  int window_height;
  int frame_rate;
} GameSetting;

void uzu_run(GameDelegate *game_delegate, const GameSetting *game_setting);
void uzu_stop();
void uzu_set_frame_rate(Uint32 frame_rate);
Uint32 uzu_get_frame_rate();

#endif // UZU_ENGINE_H
