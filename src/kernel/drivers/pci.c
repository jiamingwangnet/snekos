#include "../include/drivers/pci.h"
#include "../include/io/io.h"

// TODO: make dynamic
pci_common_t device_list[1024];
pci_common_t *list_ptr = device_list;

uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);

    return (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint16_t pci_read_vendor(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_read_word(bus, slot, func, 0);
}

uint16_t pci_read_device(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_read_word(bus, slot, func, 2);
}

uint8_t pci_read_header_type(uint8_t bus, uint8_t slot, uint8_t func)
{
    return (uint8_t)(pci_read_word(bus, slot, func, 14) & 0xff);
}

uint8_t pci_read_class(uint8_t bus, uint8_t slot, uint8_t func)
{
    return (uint8_t)((pci_read_word(bus, slot, func, 10) & 0xff00) >> 8); // Offset must point to consecutive DWORDs
}

uint8_t pci_read_subclass(uint8_t bus, uint8_t slot, uint8_t func)
{
    return (uint8_t)(pci_read_word(bus, slot, func, 10) & 0xff);
}

void pci_check_device(uint8_t bus, uint8_t device)
{
    uint8_t function = 0;

    uint16_t vendorId = pci_read_vendor(bus, device, function);
    if (vendorId == 0xffff) return; // returns all 1s when the device doesnt exist
    pci_check_function(bus, device, function);
    uint8_t header_type = pci_read_header_type(bus, device, function);
    if((header_type & 0x80) != 0) // if this is true then it is a multifunction device
    {
        for(function = 1; function < 8; function++) // check all the functions
        {
            if(pci_read_vendor(bus, device, function) != 0xffff)
            {
                pci_check_function(bus, device, function);
            }
        }
    }
}

void pci_check_bus(uint8_t bus)
{
    for(uint8_t device = 0; device < DEVICE_PER_BUS; device++)
    {
        pci_check_device(bus, device);
    }
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t func)
{
    uint8_t class = pci_read_class(bus, device, func);
    uint8_t subclass = pci_read_subclass(bus, device, func);
    uint8_t secondary_bus;

    *list_ptr++ = (pci_common_t){
        .location = (dev_loc_t){
            .bus = bus,
            .device = device,
            .function = func
        },

        .vendor_id = pci_read_vendor(bus, device, func),
        .device_id = pci_read_device(bus, device, func),

        .command = pci_read_word(bus, device, func, 4),
        .status = pci_read_word(bus, device, func, 6),

        .revision_id = pci_read_word(bus, device, func, 8) & 0xff,
        .prog_if = (pci_read_word(bus, device, func, 8) & 0xff00) >> 8,
        .subclass = subclass,
        .class_code = class,

        .cache_line_size = pci_read_word(bus, device, func, 12) & 0xff,
        .latency_timer = (pci_read_word(bus, device, func, 12) & 0xff00) >> 8,
        .header_type = pci_read_word(bus, device, func, 14) & 0xff,
        .bist = (pci_read_word(bus, device, func, 14) & 0xff00) >> 8
    };

    if(class == 0x6 && subclass == 0x4)
    {
        // TODO: fix, crashes on laptop and vmware
        // secondary_bus = pci_read_word(bus, device, func, 25);
        // pci_check_bus(secondary_bus);
    }
}

void pci_check_all_busses()
{
    uint8_t header_type = pci_read_header_type(0,0,0);
    if((header_type & 0x80) == 0)
    {
        // single pci host controller
        pci_check_bus(0);
    }
    else
    {
        // multiple pci host controllers
        for(uint8_t func = 0; func < 8; func++)
        {
            if(pci_read_vendor(0,0,func) != 0xffff) break;
            pci_check_bus(func);
        }
    }
}

pci_common_t *pci_get_device_list()
{
    return device_list;
}

pci_common_t *pci_get_device_end()
{
    return list_ptr;
}

const char *pci_get_device_name(uint8_t class, uint8_t subclass)
{
    uint16_t both = ((uint16_t)class << 8) + (uint16_t)subclass;
    switch(both)
    {
        case 0x0000: return "Non-VGA-Compatible Unclassified Device";
        case 0x0001: return "VGA-Compatible Unclassified Device";

        case 0x0100: return "SCSI Bus Controller";
        case 0x0101: return "IDE Controller";
        case 0x0102: return "Floppy Disk Controller";
        case 0x0103: return "IPI Bus Controller";
        case 0x0104: return "RAID Controller";
        case 0x0105: return "ATA Controller";
        case 0x0106: return "Serial ATA Controller";
        case 0x0107: return "Serial Attached SCSI Controller";
        case 0x0108: return "Non-Volatile Memory Controller";
        case 0x0180: return "Other";

        case 0x0200: return "Ethernet Controller";
        case 0x0201: return "Token Ring Controller";
        case 0x0202: return "FDDI Controller";
        case 0x0203: return "ATM Controller";
        case 0x0204: return "ISDN Controller";
        case 0x0205: return "WorldFip Controller";
        case 0x0206: return "PICMG 2.14 Multi Computing Controller";
        case 0x0207: return "Infiniband Controller";
        case 0x0208: return "Fabric Controller";
        case 0x0280: return "Other";

        case 0x0300: return "VGA Compatible Controller";
        case 0x0301: return "XGA Controller";
        case 0x0302: return "3D Controller";
        case 0x0380: return "Other";

        case 0x0400: return "Multimedia Video Controller";
        case 0x0401: return "Multimedia Audio Controller";
        case 0x0402: return "Computer Telephony Device";
        case 0x0403: return "Audio Device";
        case 0x0480: return "Other";

        case 0x0500: return "RAM Controller";
        case 0x0501: return "Flash Controller";
        case 0x0580: return "Other";
        
        case 0x0600: return "Host Bridge";
        case 0x0601: return "ISA Bridge";
        case 0x0602: return "EISA Bridge";
        case 0x0603: return "MCA Bridge";
        case 0x0604: return "PCI-to-PCI Bridge";
        case 0x0605: return "PCMCIA Bridge";
        case 0x0606: return "NuBus Bridge";
        case 0x0607: return "CardBus Bridge";
        case 0x0608: return "RACEway Bridge";
        case 0x0609: return "PCI-to-PCI Bridge";
        case 0x060A: return "InfiniBand-to-PCI Host Bridge";
        case 0x0680: return "Other";

        case 0x0700: return "Serial Controller";
        case 0x0701: return "Parallel Controller";
        case 0x0702: return "Multiport Serial Controller";
        case 0x0703: return "Modem";
        case 0x0704: return "IEEE 488.1/2 (GPIB) Controller";
        case 0x0705: return "Smart Card Controller";
        case 0x0780: return "Other";

        case 0x0800: return "PIC";
        case 0x0801: return "DMA Controller";
        case 0x0802: return "Timer";
        case 0x0803: return "RTC Controller";
        case 0x0804: return "PCI Hot-Plug Controller";
        case 0x0805: return "SD Host controller";
        case 0x0806: return "IOMMU";
        case 0x0880: return "Other";
        
        case 0x0900: return "Keyboard Controller";
        case 0x0901: return "Digitizer Pen";
        case 0x0902: return "Mouse Controller";
        case 0x0903: return "Scanner Controller";
        case 0x0904: return "Gameport Controller";
        case 0x0980: return "Other";
        
        case 0x0A00: return "Generic";
        case 0x0A80: return "Other";
        
        case 0x0B00: return "386";
        case 0x0B01: return "486";
        case 0x0B02: return "Pentium";
        case 0x0B03: return "Pentium Pro";
        case 0x0B10: return "Alpha";
        case 0x0B20: return "PowerPC";
        case 0x0B30: return "MIPS";
        case 0x0B40: return "Co-Processor";
        case 0x0B80: return "Other";
        
        case 0x0C00: return "FireWire (IEEE 1394) Controller";
        case 0x0C01: return "ACCESS Bus Controller";
        case 0x0C02: return "SSA";
        case 0x0C03: return "USB Controller";
        case 0x0C04: return "Fibre Channel";
        case 0x0C05: return "SMBus Controller";
        case 0x0C06: return "InfiniBand Controller";
        case 0x0C07: return "IPMI Interface";
        case 0x0C08: return "SERCOS Interface (IEC 61491)";
        case 0x0C09: return "CANbus Controller";
        case 0x0C80: return "Other";
        
        case 0x0D00: return "iRDA Compatible Controller";
        case 0x0D01: return "Consumer IR Controller";
        case 0x0D10: return "RF Controller";
        case 0x0D11: return "Bluetooth Controller";
        case 0x0D12: return "Broadband Controller";
        case 0x0D20: return "Ethernet Controller (802.1a)";
        case 0x0D21: return "Ethernet Controller (802.1b)";
        case 0x0D80: return "Other";
        
        case 0x0E00: return "I20";
        
        case 0x0F01: return "Satellite TV Controller";
        case 0x0F02: return "Satellite Audio Controller";
        case 0x0F03: return "Satellite Voice Controller";
        case 0x0F04: return "Satellite Data Controller";
        
        case 0x1000: return "Network and Computing Encrpytion/Decryption";
        case 0x1010: return "Entertainment Encryption/Decryption";
        case 0x1080: return "Other";
        
        case 0x1100: return "DPIO Modules";
        case 0x1101: return "Performance Counters";
        case 0x1110: return "Communication Synchronizer";
        case 0x1120: return "Signal Processing Management";
        case 0x1180: return "Other";
        
        case 0x1200: return "Processing Accelerator";
        
        case 0x1300: return "Non-Essential Instrumentation";
        
        case 0x4000: return "Co-Processor";

        case 0xFF00: return "Unassigned Class (Vendor specific)";

        default: return "None";
    }
}