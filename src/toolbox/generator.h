#ifndef GENERATOR_H
#define GENERATOR_H

enum
{
  GENERATOR_PARAM_WIDTH,
  GENERATOR_PARAM_HEIGHT,
  GENERATOR_PARAM_MAX_ROOM_SIZE,
  GENERATOR_PARAM_MIN_ROOM_SIZE
};
void set_generator_param(int pram, int value);
void generate_new_dungeon();

#ifdef DEBUG
void draw_rooms(int s);
void draw_graph(int s);
void draw_tree(int s);
#endif // DEBUG
#endif // GENERATOR_H
