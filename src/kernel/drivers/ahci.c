#include "../include/drivers/ahci.h"
#include "../include/drivers/pci.h"
#include "../include/console/console.h"
#include "../include/stdlib/stdlib.h"
#include "../include/memory/memory.h"
#include "../include/io/serial.h"
#include "../include/memory/kmalloc.h"

port_info_t ports[32];
size_t nports = 0;

size_t sector_size = 512; // change to the drive's sector size

HBA_MEM *abar;

size_t get_nports()
{
	return nports;
}

port_info_t *active_ports()
{
	return ports;
}

void init_ahci()
{
    kprintf("Checking for AHCI device.\n");
    pci_common_t device;
    for(size_t i = 0; i < MAX_DEVICES; i++)
    {
        device = get_device(i);
        //                                         SERIAL ATA CONTROLLER
        if(PCI_COMBINE_CLASS(device.class_code, device.subclass) == 0x0106)
            break;
    }
    if(PCI_COMBINE_CLASS(device.class_code, device.subclass) != 0x0106)
    {
        kprintf("%hAHCI Error:%h No Serial ATA Controller found.\n", RED, DEFAULT_FG);
        return;
    }

    void* first = request_page((void*)(PAGE_ALIGN(pci_get_bars(device).bar5))); // FIXME: WHAT THE FUCK make an actual working page manager. Temporary fix: move the mappings to a higher address
    request_page((void*)(PAGE_ALIGN(pci_get_bars(device).bar5) + 0x200000));
    abar = (HBA_MEM*)(pci_get_bars(device).bar5 - PAGE_ALIGN(pci_get_bars(device).bar5) + (uint64_t)first); // 0xbf1000
    // (uint32_t)(pci_get_bars(device).bar5) 0xfebf1000
    probe_ports(abar);

    for(int i = 0; i < nports; i++)
    {
        if(ports[i].type == AHCI_DEV_SATA)
        {
            port_rebase(ports[i].port, ports[i].number);

            device_info_t device;
            if(ahci_identify(ports[i].port, &device))
            {
                size_t size = device.size / 2;
                char unit[4];
                if (size >= 1024 * 1024)
                {
                    size /= (1024 * 1024);
                    memcpy((void*)unit, (void*)"GiB", 4);
                }
                else if(size >= 1024)
                {
                    size /= 1024;
                    memcpy((void*)unit, (void*)"MiB", 4);
                }
                else
                {
                    memcpy((void*)unit, (void*)"KiB", 4);
                }
                kprintf("Initalised drive: %h%s%h\n\tport %h%d%h, size: %h%d%s%h\n", DODGERBLUE, device.model, DEFAULT_FG, ORANGE, i, DEFAULT_FG, ORANGE, size, unit, DEFAULT_FG);
            }
            break;
        }
    }

	kprintf("AHCI init finished. %h%d%h port(s) found.\n", ORANGE, nports, DEFAULT_FG);
}

void probe_ports(HBA_MEM *abar)
{
    for(uint32_t i = 0, pi = abar->pi; i < 32; i++, pi >>= 1)
    {
        if(pi & 1)
        {
            int dt = check_type(&abar->ports[i]);
            // switch(dt)
            // {
            //     case AHCI_DEV_SATA:
            //         kprintf("SATA drive found at port %d\n", i);
            //         break;
            //     case AHCI_DEV_SATAPI:
            //         kprintf("SATAPI drive found at port %d\n", i);
            //         break;
            //     case AHCI_DEV_SEMB:
            //         kprintf("SEMB drive found at port %d\n", i);
            //         break;
            //     case AHCI_DEV_PM:
            //         kprintf("PM drive found at port %d\n", i);
            //         break;
            //     default:
            //         kprintf("No drive found at port %d\n", i);
            //         break;
            // }
            
            if(dt == AHCI_DEV_SATA)
            {
                ports[i].port = &abar->ports[i];
                ports[i].number = i;
                ports[i].type = dt;
                nports ++;
            }
        }
    }
}

// Check device type
int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void port_rebase(HBA_PORT *port, size_t portno)
{
    stop_cmd(port); // stop command engine
    void *newBase = ALIGN_ADDR( (uint64_t)virt_to_phys(kmalloc(sizeof(HBA_CMD_HEADER) * 32 + 0x1000)), 0x1000);

	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = newBase;
	port->clbu = (uint32_t)((uint64_t)newBase >> 32);
	memset((void*)port->clb, 0, 1024);

    void *fisBase = ALIGN_ADDR( (uint64_t)virt_to_phys(kmalloc(256 * 32 + 0x1000)), 0x1000);
	// FIS entry size = 256 bytes per port
	port->fb = fisBase;
	port->fbu = (uint32_t)((uint64_t)fisBase >> 32);
	memset((void*)port->fb, 0, 256);

    // Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)((uint64_t)port->clb + ((uint64_t)port->clbu << 32));
	for (int i = 0; i < 32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
        void *ctblAddr = ALIGN_ADDR((uint64_t)virt_to_phys(kmalloc(256 + 0x1000)), 0x1000);
		// 256 bytes per command table, 64+16+48+16*8
		cmdheader[i].ctba = ctblAddr;
		cmdheader[i].ctbau = (uint32_t)((uint64_t)ctblAddr >> 32);
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
 
	start_cmd(port);	// Start command engine
}

// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
}

bool send_command(HBA_PORT *port, HBA_PRDT_ENTRY *prdt, uint16_t prdtl, FIS_REG_H2D *cmdfis)
{
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return false;
 
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(uint64_t)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = prdtl;	// PRDT entries count

    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)cmdheader->ctba;
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

    memcpy((void*)cmdtbl->prdt_entry, (void*)prdt, sizeof(HBA_PRDT_ENTRY) * cmdheader->prdtl);

    // Setup command
	FIS_REG_H2D *tblcmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

    memcpy((void*)tblcmdfis, (void*)cmdfis, sizeof(FIS_REG_H2D));

    // The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_SR_BSY | ATA_SR_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return false;
	}
 
	port->ci = 1<<slot;	// Issue command

	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Disk error\n");
			return false;
		}
	}
 
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		kprintf("Disk error\n");
		return false;
	}
 
	return true;
}

bool ahci_identify(HBA_PORT *port, device_info_t *device)
{
    uint8_t buffer[2048];
    uint64_t phys_buf = virt_to_phys((void*)buffer);

    uint16_t prdtl = 1;
    HBA_PRDT_ENTRY *entries = (HBA_PRDT_ENTRY*)kmalloc(sizeof(HBA_PRDT_ENTRY) * prdtl);

    entries[0].dba = (uint32_t)phys_buf;
    entries[0].dbau = (uint32_t)((uint64_t)phys_buf >> 32);
    entries[0].dbc = 2048;
    entries[0].i = 1;

    FIS_REG_H2D cmdfis;

    cmdfis.fis_type = FIS_TYPE_REG_H2D;
	cmdfis.c = 1;	// Command
	cmdfis.command = ATA_CMD_IDENTIFY;

    if(send_command(port, entries, prdtl, &cmdfis))
    {
        device->signature = *((uint16_t*)(buffer + ATA_IDENT_DEVICETYPE));
        device->capabilities = *((uint16_t*)(buffer + ATA_IDENT_CAPABILITIES));
        device->cmdSets = *((uint32_t*)(buffer + ATA_IDENT_COMMANDSETS));

        if (device->cmdSets & (1 << 26))
            // uses 48bit addressing
            device->size = *((uint32_t*)(buffer + ATA_IDENT_MAX_LBA_EXT));
        else
            // uses CHS or 28bit addressing
            device->size = *((uint32_t*)(buffer + ATA_IDENT_MAX_LBA));

        for(int i = 0; i < 40; i += 2)
        {
            device->model[i] = buffer[ATA_IDENT_MODEL + i + 1];
            device->model[i + 1] = buffer[ATA_IDENT_MODEL + i];
        }
        device->model[40] = 0; // add null terminator

        return true;
    }

    kfree((void*)entries);
    return false;
}

bool ahci_read(HBA_PORT *port, uint64_t sector, uint32_t nsectors, uint8_t *buf)
{
    buf = (uint8_t*)virt_to_phys((void*)buf);

    uint32_t lsector = (uint32_t)sector;
    uint32_t hsector = (uint32_t)(sector >> 32);
	
	uint16_t prdtl = (uint16_t)((nsectors)>>4) + 1;	// PRDT entries count
	HBA_PRDT_ENTRY *entries = kmalloc(sizeof(HBA_PRDT_ENTRY) * prdtl);
 
	// 8K bytes (16 sectors) per PRDT
    int i;
    uint32_t sects = nsectors;
    
	for (i = 0; i < prdtl - 1; i++)
	{
		entries[i].dba = (uint32_t) buf;
        entries[i].dbau = (uint32_t)((uint64_t)buf >> 32);
		entries[i].dbc = sector_size * 16 - 1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		entries[i].i = 1;
		buf += sector_size * 16;	// 8K bytes
		sects -= 16;	// 16 sectors
	}
	// Last entry
	entries[i].dba = (uint32_t) buf;
	entries[i].dbau = (uint32_t)((uint64_t)buf >> 32);
	entries[i].dbc = (sects << 9) - 1;	// 512 bytes per sector
	entries[i].i = 1;
 
	// Setup command
	FIS_REG_H2D cmdfis;
 
	cmdfis.fis_type = FIS_TYPE_REG_H2D;
	cmdfis.c = 1;	// Command
	cmdfis.command = ATA_CMD_READ_DMA_EXT;
 
	cmdfis.lba0 = (uint8_t)lsector;
	cmdfis.lba1 = (uint8_t)(lsector>>8);
	cmdfis.lba2 = (uint8_t)(lsector>>16); 
	cmdfis.lba3 = (uint8_t)(lsector>>24);
	cmdfis.lba4 = (uint8_t)hsector;
	cmdfis.lba5 = (uint8_t)(hsector>>8);

    cmdfis.device = 1<<6;	// LBA mode
 
	cmdfis.countl = nsectors & 0xFF;
	cmdfis.counth = (nsectors >> 8) & 0xFF;
 
	bool res = send_command(port, entries, prdtl, &cmdfis);
    kfree((void*)entries);

    return res;
}

bool ahci_write(HBA_PORT *port, uint64_t sector, uint32_t nsectors, uint8_t *buf)
{
	buf = (uint8_t*)virt_to_phys((void*)buf);

    uint32_t lsector = (uint32_t)sector;
    uint32_t hsector = (uint32_t)(sector >> 32);
	
	uint16_t prdtl = (uint16_t)((nsectors)>>4) + 1;	// PRDT entries count
	HBA_PRDT_ENTRY *entries = kmalloc(sizeof(HBA_PRDT_ENTRY) * prdtl);
 
	// 8K bytes (16 sectors) per PRDT
    int i;
    uint32_t sects = nsectors;
    
	for (i = 0; i < prdtl - 1; i++)
	{
		entries[i].dba = (uint32_t) buf;
        entries[i].dbau = (uint32_t)((uint64_t)buf >> 32);
		entries[i].dbc = sector_size * 16 - 1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		entries[i].i = 1;
		buf += sector_size * 16;	// 8K bytes
		sects -= 16;	// 16 sectors
	}
	// Last entry
	entries[i].dba = (uint32_t) buf;
	entries[i].dbau = (uint32_t)((uint64_t)buf >> 32);
	entries[i].dbc = (sects << 9) - 1;	// 512 bytes per sector
	entries[i].i = 1;
 
	// Setup command
	FIS_REG_H2D cmdfis;
 
	cmdfis.fis_type = FIS_TYPE_REG_H2D;
	cmdfis.c = 1;	// Command
	cmdfis.command = ATA_CMD_WRITE_DMA_EXT;
 
	cmdfis.lba0 = (uint8_t)lsector;
	cmdfis.lba1 = (uint8_t)(lsector>>8);
	cmdfis.lba2 = (uint8_t)(lsector>>16); 
	cmdfis.lba3 = (uint8_t)(lsector>>24);
	cmdfis.lba4 = (uint8_t)hsector;
	cmdfis.lba5 = (uint8_t)(hsector>>8);

    cmdfis.device = 1<<6;	// LBA mode
 
	cmdfis.countl = nsectors & 0xFF;
	cmdfis.counth = (nsectors >> 8) & 0xFF;
 
	bool res = send_command(port, entries, prdtl, &cmdfis);
    kfree((void*)entries);

    return res;
}
 
// Find a free command list slot
int find_cmdslot(HBA_PORT *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
    int cmdslots = (abar->cap & 0x0f00) >> 8;
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}