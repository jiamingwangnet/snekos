#include "../include/drivers/ata.h"
#include "../include/io/io.h"
#include "../include/console/console.h"
#include "../include/drivers/timer.h"
#include "../include/stdlib/stdlib.h"

uint8_t ide_buffer[2048] = {0};
volatile static uint8_t irq_invoked = 0;
static uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct ide_device ide_devices[4];
static size_t ndrives = 0;

struct ide_channel {
    uint16_t base;
    uint16_t control;
    uint16_t bus_master_ide;
    uint8_t no_int;
} channels[2];

void init_ata()
{
    kprintf("Checking for ATA devices.\n");

    pci_common_t device;
    for(size_t i = 0; i < MAX_DEVICES; i++)
    {
        device = get_device(i);
        if( PCI_COMBINE_CLASS(device.class_code, device.subclass) == 0x0101 )
            break;
    }

    if( PCI_COMBINE_CLASS(device.class_code, device.subclass) != 0x0101 )
    {
        kprintf("%hATA Error:%h No IDE Controller available.\n", RED, DEFAULT_FG);
        return;
    }

    // read bar TODO: use pci_header0_t header
    pci_header0_t bars = pci_get_bars(device);

    ide_init(bars.bar0, bars.bar1, bars.bar2, bars.bar3, bars.bar4);

    kprintf("ATA init finished. %h%d%h drive(s) found.\n", ORANGE, ndrives, DEFAULT_FG);
}

uint8_t ide_read(uint8_t channel, uint8_t reg)
{
    uint8_t res;
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].no_int);
    if (reg < 0x08)
        res = in(channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        res = in(channels[channel].base + reg - 0x06);
    else if (reg < 0x0E)
        res = in(channels[channel].control + reg - 0x0A);
    else if (reg < 0x16)
        res = in(channels[channel].bus_master_ide + reg - 0x0E);
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].no_int);
    return res;
}

void ide_write(uint8_t channel, uint8_t reg, uint8_t data)
{
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].no_int);
    if (reg < 0x08)
        out(channels[channel].base + reg - 0x00, data);
    else if (reg < 0x0C)
        out(channels[channel].base + reg - 0x06, data);
    else if (reg < 0x0E)
        out(channels[channel].control + reg - 0x0A, data);
    else if (reg < 0x16)
        out(channels[channel].bus_master_ide + reg - 0x0E, data);
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].no_int);
}

void ide_read_buffer(uint8_t channel, uint8_t reg, uint32_t *buf, uint8_t quads)
{
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].no_int);
    if (reg < 0x08)
        insl(channels[channel].base + reg - 0x00, buf, quads);
    else if (reg < 0x0C)
        insl(channels[channel].base + reg - 0x06, buf, quads);
    else if (reg < 0x0E)
        insl(channels[channel].control + reg - 0x0A, buf, quads);
    else if (reg < 0x16)
        insl(channels[channel].bus_master_ide + reg - 0x0E, buf, quads);
    if(reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].no_int);
}

uint8_t ide_polling(uint8_t channel, uint32_t adv_check) // poll and wait until deivce is not busy
{
    for(int i = 0; i < 4; i++) // delay for 400ns
        ide_read(channel, ATA_REG_ALTSTATUS); // reading alternative status wastes 100ns

    while(ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY); // wait for busy to be 0

    if(adv_check)
    {
        uint8_t state = ide_read(channel, ATA_REG_STATUS);

        if(state & ATA_SR_ERR) return 2; // error
        if(state & ATA_SR_DF) return 1; // device fault
        if((state & ATA_SR_DRQ) == 0) return 3; // DRQ should be set
    }

    return 0; // no error
}

uint8_t ide_print_error(uint32_t drive, uint8_t err)
{
    if (err == 0)
        return err;

    kprintf("%hIDE ERROR: %h", RED, DEFAULT_FG);
    if (err == 1) {kprintf("- Device Fault\n    "); err = 19;}
    else if (err == 2)
    {
        uint8_t st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
        if (st & ATA_ER_AMNF)  {kprintf("- No Address Mark Found\n  "); err = 7;}
        if (st & ATA_ER_TK0NF) {kprintf("- No Media or Media Error\n  "); err = 3;}
        if (st & ATA_ER_ABRT)  {kprintf("- Command Aborted\n  "); err = 20;}
        if (st & ATA_ER_MCR)   {kprintf("- No Media or Media Error\n  "); err = 3;}
        if (st & ATA_ER_IDNF)  {kprintf("- ID mark not Found\n  "); err = 21;}
        if (st & ATA_ER_MC)    {kprintf("- No Media or Media Error\n  "); err = 3;}
        if (st & ATA_ER_UNC)   {kprintf("- Uncorrectable Data Error\n  "); err = 22;}
        if (st & ATA_ER_BBK)   {kprintf("- Bad Sectors\n  "); err = 13;}
    } else if (err == 3)       {kprintf("- Reads Nothing\n  "); err = 23;}
      else if (err == 4)       {kprintf("- Write Protected\n    "); err = 8;}
    
    kprintf("- [");
    kprintf((const char *[]){"Primary", "Secondary"}[ide_devices[drive].channel]);
    kprintf(" ");;
    kprintf((const char *[]){"Master", "Slave"}[ide_devices[drive].drive]);
    kprintf("] ");
    kprintf(ide_devices[drive].model);

    return err;
}

void ide_init(uint32_t bar0, uint32_t bar1, uint32_t bar2, uint32_t bar3, uint32_t bar4)
{
    int i, j, k, count = 0;

    // detect IDE controller interface IO port
    channels[ATA_PRIMARY  ].base           = (bar0 & 0xFFFFFFFC) + 0x1F0 * (!bar0);
    channels[ATA_PRIMARY  ].control        = (bar1 & 0xFFFFFFFC) + 0x3F6 * (!bar1);
    channels[ATA_SECONDARY].base           = (bar2 & 0xFFFFFFFC) + 0x170 * (!bar2);
    channels[ATA_SECONDARY].control        = (bar3 & 0xFFFFFFFC) + 0x376 * (!bar3);
    channels[ATA_PRIMARY  ].bus_master_ide = (bar4 & 0xFFFFFFFC) + 0;
    channels[ATA_SECONDARY].bus_master_ide = (bar4 & 0xFFFFFFFC) + 8;

    // disasble IRQs
    ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

    // detect ata-atapi devices

    for (i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            uint8_t err = 0, type = IDE_ATA, status;
            ide_devices[count].reserved = 0; // assuming no drive
            
            ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // select drive
            wait_ticks(1);

            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY); // send identify command
            wait_ticks(1);

            if(ide_read(i, ATA_REG_STATUS) == 0) continue; // if status is 0, there is no device

            while(1)
            {
                status = ide_read(i, ATA_REG_STATUS);
                if ((status & ATA_SR_ERR)) {err = 1; break;} // error, device is not ATA
                if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break; // all good
            }
            
            // probe for devices
            if (err != 0)
            {
                uint8_t cl = ide_read(i, ATA_REG_LBA1);
                uint8_t ch = ide_read(i, ATA_REG_LBA2);

                if (cl == 0x14 && ch == 0xEB)
                    type = IDE_ATAPI; // ????????
                else if (cl == 0x69 && ch == 0x96)
                    type = IDE_ATAPI; // ????????
                else
                    continue; // unknown type
                
                ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                wait_ticks(1);
            }

            // read identification space
            ide_read_buffer(i, ATA_REG_DATA, (uint32_t*)ide_buffer, 128);

            // read device params
            ide_devices[count].reserved = 1;
            ide_devices[count].type = type;
            ide_devices[count].channel = i;
            ide_devices[count].drive = j;
            ide_devices[count].signature = *((uint16_t*)(ide_buffer + ATA_IDENT_DEVICETYPE));
            ide_devices[count].capabilities = *((uint16_t*)(ide_buffer + ATA_IDENT_CAPABILITIES));
            ide_devices[count].cmdSets = *((uint32_t*)(ide_buffer + ATA_IDENT_COMMANDSETS));

            // get size
            if (ide_devices[count].cmdSets & (1 << 26))
                // uses 48bit addressing
                ide_devices[count].size = *((uint32_t*)(ide_buffer + ATA_IDENT_MAX_LBA_EXT));
            else
                // uses CHS or 28bit addressing
                ide_devices[count].size = *((uint32_t*)(ide_buffer + ATA_IDENT_MAX_LBA));

            // get device model string
            for(k = 0; k < 40; k += 2)
            {
                ide_devices[count].model[k] = ide_buffer[ATA_IDENT_MODEL + k + 1];
                ide_devices[count].model[k + 1] = ide_buffer[ATA_IDENT_MODEL + k];
            }
            ide_devices[count].model[40] = 0; // add null terminator

            count++;
            ndrives++;
        }

        int devs = 0;
        // print devices
        for (i = 0; i < 4; i++)
        {
            if(ide_devices[i].reserved == 1)
            {
                devs ++;
                size_t size = ide_devices[i].size / 2;
                kprintf(" Found %h%s%h Drive %h%d%s %h%s%h\n", PURPLE, (const char *[]){"ATA", "ATAPI"}[ide_devices[i].type], DEFAULT_FG, ORANGE, CONVERT_MEM_UNIT(size), GET_MEM_UNIT(size), DODGERBLUE, ide_devices[i].model, DEFAULT_FG);
            }
        }

        if(devs == 0) kprintf("%hNo ATA drives found.%h\n", RED, DEFAULT_FG);
    }
}

// direction 0: read, 1: write                                  lba is sector????
uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint32_t lba, uint8_t numsects, uint16_t selector, uint64_t edi) //https://github.com/encounter/osdev/blob/master/kernel/fatfs/diskio.c
{
    uint8_t lba_mode /* 0 CHS, 1 LBA28, 2 LBA48 */, dma /* 0 no dma, 1 dma */, cmd;
    uint8_t lba_io[6];
    uint32_t channel = ide_devices[drive].channel;
    uint32_t slavebit = ide_devices[drive].drive;
    uint32_t bus = channels[channel].base;
    uint32_t words = 256;
    uint16_t cyl, i;
    uint8_t head, sect, err;

    ide_write(channel, ATA_REG_CONTROL, channels[channel].no_int = (irq_invoked = 0x0) + 0x02);

    /*
        if No LBA support:
            use CHS
        else if LBA Sector address > 0x0FFFFFFF
            use LBA48
        else
            use LBA28
    */

    // select from LBA28, LBA48 or CHS
    if (lba >= 0x10000000)
    {
        // LBA48
        lba_mode = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0; // LBA28 is a integer, 32bits are enough to access 2TB.
        lba_io[5] = 0;
        head      = 0; // Lower 4bits of HDDEVSEL are not used
    } 
    else if (ide_devices[drive].capabilities & 0x200) // drive supports LBA
    {
        // LBA28
        lba_mode = 1;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = 0; // registers not used
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba & 0x0F000000) >> 24;
    }
    else
    {
        // CHS:
        lba_mode = 0;
        sect = (lba % 63) + 1;
        cyl = (lba + 1 - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba + 1 - sect) % (16 * 63) / 63; // header number is written to DDEVSEL for lower 4bits
    }

    dma = 0; // dont support dma

    // wait until not busy
    while(ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY);

    if (lba_mode == 0)
        ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
    else
        ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);
    
    // write params
    if (lba_mode == 2)
    {
        ide_write(channel, ATA_REG_SECCOUNT1, 0);
        ide_write(channel, ATA_REG_LBA3, lba_io[3]);
        ide_write(channel, ATA_REG_LBA4, lba_io[4]);
        ide_write(channel, ATA_REG_LBA5, lba_io[5]);
    }
    ide_write(channel, ATA_REG_SECCOUNT0, numsects);
    ide_write(channel, ATA_REG_LBA0, lba_io[0]);
    ide_write(channel, ATA_REG_LBA1, lba_io[1]);
    ide_write(channel, ATA_REG_LBA2, lba_io[2]);

    if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;   
    if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;   
    if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
    if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;
    ide_write(channel, ATA_REG_COMMAND, cmd); // send command

    // dont handle dma because it is not supported
    if (direction == 0)
    {
        // PIO Read
        for (i = 0; i < numsects; i++)
        {
            if(err = ide_polling(channel, 1))
                return err;
            __asm__ volatile("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // reads 256 words or 512 bytes
            edi += (words * 2);
        }
    }
    else
    {
        // PIO write
        for(i = 0; i < numsects; i++)
        {
            ide_polling(channel, 0);
            __asm__ volatile("rep outsw" : : "c"(words), "d"(bus), "S"(edi));
            edi += (words * 2);
        }

        ide_write(channel, ATA_REG_COMMAND, (char[]){
            ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT
        }[lba_mode]);
        ide_polling(channel, 0);
    }

    return 0;
}