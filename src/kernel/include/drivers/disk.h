#pragma once

#include "../stdlib/types.h"
#include "ata.h"
#include "ahci.h"

uint8_t disk_read(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);
uint8_t disk_write(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);