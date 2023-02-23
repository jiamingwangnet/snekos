#include "../include/drivers/disk.h"
// count is in sectors (512 bytes)
uint8_t disk_read(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer)
{
    // there are only 4 drives so check if its below 3
    // a drive is reserved if it exists
    if(drive < 3 && ide_devices[drive].reserved)
    {
        if(ide_ata_access(0, drive, sector, count, 0, (uint64_t)buffer))
        {
            return 1; // error
        }
        return 0;
    }
    return 1; // no drive found
}

uint8_t disk_write(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer)
{
    // there are only 4 drives so check if its below 3
    // a drive is reserved if it exists
    if(drive < 3 && ide_devices[drive].reserved)
    {
        if(ide_ata_access(1, drive, sector, count, 0, (uint64_t)buffer))
        {
            return 1; // error
        }
        return 0;
    }
    return 1; // no drive found
}