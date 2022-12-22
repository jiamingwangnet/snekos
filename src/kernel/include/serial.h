#pragma once

#include "io.h"

int init_serial();

int is_transmit_empty();
int serial_received();

void serial_char(char c);
void serial_str(const char * str);

uint8_t iserial_char();