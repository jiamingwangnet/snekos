#pragma once

#include "../../include/graphics/graphics.h"

#define GB_TILE_SIZE 20
#define GB_WIDTH 30
#define GB_HEIGHT 20
#define MAX_SNAKE_LEN 1000
#define TILE_PAD 2

enum DIRS {
    DNORTH, DEAST, DSOUTH, DWEST
};

typedef struct tile {
    int32_t x;
    int32_t y;
    uint32_t color;
    
    bool has_food;
} tile_t;
void make_tile(tile_t* tile, int32_t x, int32_t y, uint32_t color);

typedef struct snake {
    tile_t data[MAX_SNAKE_LEN];
    size_t current_len;
    uint32_t color;

    int32_t head_x;
    int32_t head_y;

    enum DIRS move_dir;

    void(*move_snake)(struct snake *self);
} snake_t;
void snake_move_snake(snake_t *self);
void make_snake(snake_t* snake, size_t current_len, uint32_t color, int32_t x, int32_t y);

typedef struct snake_game {
    tile_t tiles[GB_WIDTH * GB_HEIGHT]; // TODO: Make dynamic
    snake_t snake;
    uint64_t frames;
    char keypress;
    bool return_flag;   
} game_t;
void init_game();

void main();
uint32_t get_score();