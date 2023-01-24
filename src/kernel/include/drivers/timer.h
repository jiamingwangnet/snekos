#pragma once

#include "../interrupts/handlers.h"
#include "../stdlib/types.h"

void init_timer();
void wait_ticks(uint64_t ticks);
uint64_t get_time();
void inc_time();