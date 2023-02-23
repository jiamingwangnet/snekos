#pragma once

#include "ata.h"

uint8_t disk_read(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);
uint8_t disk_write(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);