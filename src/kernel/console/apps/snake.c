#include "snake.h"
#include "../../include/drivers/timer.h"
#include "../../include/stdlib/stdlib.h"
#include "../../include/drivers/keyboard.h"
#include "../../include/io/serial.h"
#include "../../include/memory/kmalloc.h"

static const int32_t NORTH[2] = {0, -1};
static const int32_t EAST[2] = {1, 0};
static const int32_t SOUTH[2] = {0, 1};
static const int32_t WEST[2] = {-1, 0};
static game_t *game_bd;
static uint32_t x_offset;
static uint32_t y_offset;
uint32_t score = 0;

uint32_t get_score()
{
    return score;
}

void init_game()
{
    game_bd = (game_t*)kmalloc(sizeof(game_t));
    memset((void*)game_bd, 0, sizeof(game_t));
    game_bd->frames = 0;
    game_bd->keypress = '\0';
    game_bd->return_flag = false;
    score = 0;

    x_offset = SCRN_WIDTH / 2 - (GB_WIDTH * (GB_TILE_SIZE + TILE_PAD)) / 2;
    y_offset = SCRN_HEIGHT / 2 - (GB_HEIGHT * (GB_TILE_SIZE + TILE_PAD)) / 2;

    make_snake(&game_bd->snake, 3, 0x00ff00, 4, 0);

    for(size_t y = 0; y < GB_HEIGHT; y++)
    {
        for(size_t x = 0; x < GB_WIDTH; x++)
        {
            make_tile(&game_bd->tiles[y * GB_WIDTH + x], x, y, 0x0f0f0f);
        }
    }

    srand(get_time());
}

void handle_key_input(Key_Info key)
{
    game_bd->keypress = key.key;
}

void snake_change_dir()
{
    switch(game_bd->keypress)
    {
        case 'w':
            if(game_bd->snake.move_dir == DSOUTH) break;
            game_bd->snake.move_dir = DNORTH;
            break;
        case 'a':
            if(game_bd->snake.move_dir == DEAST) break;
            game_bd->snake.move_dir = DWEST;
            break;
        case 's':
            if(game_bd->snake.move_dir == DNORTH) break;
            game_bd->snake.move_dir = DSOUTH;
            break;
        case 'd':
            if(game_bd->snake.move_dir == DWEST) break;
            game_bd->snake.move_dir = DEAST;
            break;
    }
}

void update()
{
    snake_change_dir();
    game_bd->snake.move_snake(&game_bd->snake);
}

void draw()
{
    if(game_bd->snake.head_x < 0 || game_bd->snake.head_y < 0) return;

    for(size_t y = 0; y < GB_HEIGHT; y++)
    {
        for(size_t x = 0; x < GB_WIDTH; x++)
        {
            tile_t tile = game_bd->tiles[y * GB_WIDTH + x];
            draw_rect(x * (GB_TILE_SIZE + TILE_PAD) + x_offset, y * (GB_TILE_SIZE + TILE_PAD) + y_offset, GB_TILE_SIZE, GB_TILE_SIZE, tile.color);
        }
    }

    for(size_t i = 0; i < game_bd->snake.current_len; i++)
    {
        tile_t tile = game_bd->snake.data[i];
        draw_rect(tile.x * (GB_TILE_SIZE + TILE_PAD) + x_offset, tile.y * (GB_TILE_SIZE + TILE_PAD) + y_offset, GB_TILE_SIZE, GB_TILE_SIZE, tile.color);
    }

    draw_str(20, 20, 0xffffff, 0x000000, "Score: ");
    char cscore[16];
    itoa(score, cscore, 10);
    draw_str(7 * PSF1_WIDTH + 20, 20, 0xffffff, 0x000000, cscore);

    update_buffer();
}

void put_apple(int32_t x, int32_t y, bool put)
{
    game_bd->tiles[y * GB_WIDTH + x].has_food = put;
    game_bd->tiles[y * GB_WIDTH + x].color = (put ? 0xff0000 : 0x0f0f0f);
}

void rand_apple()
{
    int32_t x = rand() % GB_WIDTH;
    int32_t y = rand() % GB_HEIGHT;

    put_apple(x, y, true);
}

void main()
{
    init_game();
    attach_keyboard(&handle_key_input);

    fill_screen(0x000000);
    update_buffer();

    rand_apple();

    while(!game_bd->return_flag)
    {
        if(get_time() % 80 == 0)
        {
            update();
            if(game_bd->return_flag) break;
            draw();
        }

        game_bd->frames++;
    }

    wait_ticks(300);
    kfree((void*)game_bd);
}

void make_snake(snake_t* snake, size_t current_len, uint32_t color, int32_t x, int32_t y)
{
    snake->color = color;
    snake->current_len = current_len;
    snake->move_snake = &snake_move_snake;
    snake->move_dir = DEAST;
    snake->head_x = x;
    snake->head_y = y;

    for(size_t len = 0; len < current_len; len++)
    {
        snake->data[len].color = color;
        snake->data[len].x = x - len;
    }
}

void snake_move_snake(snake_t *self)
{
    int32_t tile_last_x = self->head_x;
    int32_t tile_last_y = self->head_y;

    switch(self->move_dir)
    {
        case DNORTH:
            self->head_x += NORTH[0];
            self->head_y += NORTH[1];
            break;
        case DEAST:
            self->head_x += EAST[0];
            self->head_y += EAST[1];
            break;
        case DSOUTH:
            self->head_x += SOUTH[0];
            self->head_y += SOUTH[1];
            break;
        case DWEST:
            self->head_x += WEST[0];
            self->head_y += WEST[1];
            break;
    }
    self->data[0].x = self->head_x;
    self->data[0].y = self->head_y;

    for(size_t i = 1; i < self->current_len; i++)
    {
        int32_t tmpx;
        int32_t tmpy;
        tmpx = self->data[i].x;
        tmpy = self->data[i].y;

        self->data[i].x = tile_last_x;
        self->data[i].y = tile_last_y;

        tile_last_x = tmpx;
        tile_last_y = tmpy;
    }

    if(self->head_x < 0 || self->head_x >= GB_WIDTH || self->head_y < 0 || self->head_y >= GB_HEIGHT)
        game_bd->return_flag = true;

    for(size_t i = 1; i < self->current_len; i++)
    {
        if(self->head_x == self->data[i].x && self->head_y == self->data[i].y)
        {
            game_bd->return_flag = true;
            break;
        }
    }

    if(game_bd->tiles[self->head_y * GB_WIDTH + self->head_x].has_food)
    {
        put_apple(self->head_x, self->head_y, false);
        self->data[self->current_len].color = 0x00ff00;
        self->data[self->current_len].x = self->data[self->current_len - 1].x;
        self->data[self->current_len].y = self->data[self->current_len - 1].y;

        self->current_len ++;
        score++;

        rand_apple();
    }
}

void make_tile(tile_t* tile, int32_t x, int32_t y, uint32_t color)
{
    tile->x = x;
    tile->y = y;
    tile->color = color;
}