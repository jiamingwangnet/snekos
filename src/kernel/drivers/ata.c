#include "../include/drivers/ata.h"
#include "../include/io/io.h"
#include "../include/console/console.h"
#include "../include/drivers/timer.h"
#include "../include/stdlib/stdlib.h"

uint8_t ide_buffer[2048] = {0};
volatile static uint8_t irq_invoked = 0;
static uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct ide_device 
{
    uint8_t reseved;        // 0 or 1
    uint8_t channel;        // 0 primary channel, 1 secondary channel
    uint8_t drive;          // 0 master drive, 1 slave drive
    uint16_t type;          // 0 ata, 1 atapi
    uint16_t signature;     // drive signature
    uint16_t capabilities;  // features
    uint32_t cmdSets;       // supported command sets
    uint32_t size;          // size in sectors (iso file size * 2 KiB)
    uint8_t model[41];      // model as string
} ide_devices[4];

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
        if( (((uint16_t)device.class_code << 8) + (uint16_t)device.subclass) == 0x0101 )
            break;
    }

    if( (((uint16_t)device.class_code << 8) + (uint16_t)device.subclass) != 0x0101 )
    {
        kprintf("ATA Error: No IDE Controller available.\n");
        return;
    }
    uint8_t bus = device.location.bus, dev = device.location.device, func = device.location.function;

    // read bar TODO: use pci_header0_t header
    uint32_t bar0, bar1, bar2, bar3, bar4;
    bar0 = (pci_read_word(bus, dev, func, 18) << 16) | pci_read_word(bus, dev, func, 16);
    bar1 = (pci_read_word(bus, dev, func, 22) << 16) | pci_read_word(bus, dev, func, 20);
    bar2 = (pci_read_word(bus, dev, func, 26) << 16) | pci_read_word(bus, dev, func, 24);
    bar3 = (pci_read_word(bus, dev, func, 30) << 16) | pci_read_word(bus, dev, func, 28);
    bar4 = (pci_read_word(bus, dev, func, 34) << 16) | pci_read_word(bus, dev, func, 32);

    ide_init(bar0, bar1, bar2, bar3 , bar4);

    kprintf("ATA init finished.\n");
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

    kprintf("IDE ERROR: ");
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
    kprintch(' ');
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
            ide_devices[count].reseved = 0; // assuming no drive
            
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
            ide_devices[count].reseved = 1;
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
        }

        // print devices
        for (i = 0; i < 4; i++)
        {
            if(ide_devices[i].reseved == 1)
            {
                size_t size = ide_devices[i].size / 2;

                kprintf(" Found ");
                kprintf((const char *[]){"ATA", "ATAPI"}[ide_devices[i].type]);
                kprintf(" Drive ");

                char csize[16];

                if (size >= 1024 * 1024)
                {
                    size /= (1024 * 1024);
                    itoa(size, csize, 10);

                    kprintf(csize);
                    kprintf("GiB - ");
                }
                else if(size >= 1024)
                {
                    size /= 1024;
                    itoa(size, csize, 10);

                    kprintf(csize);
                    kprintf("MiB - ");
                }
                else
                {
                    itoa(size, csize, 10);

                    kprintf(csize);
                    kprintf("KiB - ");
                }

                kprintf(ide_devices[i].model);
                kprintch('\n');
            }
        }
    }
}
