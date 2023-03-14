#include "../include/drivers/disk.h"

// count is in sectors (512 bytes)
uint8_t disk_read(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer) // drive is port num (for ahci) or drive (for ide)
{
    if(get_nports() > 0) // prioritise ahci over ide
    {
        HBA_PORT *port = active_ports()[drive].port;
        if(!ahci_read(port, sector, count, buffer))
        {
            return 1;
        }
        return 0;
    }
    // there are only 4 drives so check if its below 3
    // a drive is reserved if it exists
    else if(drive < 3 && ide_devices[drive].reserved)
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
     if(get_nports() > 0) // prioritise ahci over ide
    {
        HBA_PORT *port = active_ports()[drive].port;
        if(!ahci_write(port, sector, count, buffer))
        {
            return 1;
        }
        return 0;
    }
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