#pragma once

#include "handlers.h"
#include "types.h"

void init_timer();
void wait_ticks(uint64_t ticks);
uint64_t get_time();
void inc_time();