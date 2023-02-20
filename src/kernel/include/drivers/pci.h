#pragma once

#include "../stdlib/types.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define N_BUS 256
#define DEVICE_PER_BUS 32

#define MAX_DEVICES 1024

typedef struct 
{
    uint8_t bus;
    uint8_t device;
    uint8_t function;
} dev_loc_t;

typedef struct 
{
    dev_loc_t location;

    // offset 0x0
    uint16_t vendor_id;
    uint16_t device_id;
    // offset 0x4
    uint16_t command;
    uint16_t status;
    // offset 0x8
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    //offset 0xC
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
} pci_common_t;

typedef struct
{
    pci_common_t common;

    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;

    uint32_t cardbus_cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t ex_rom_addr;
    uint8_t capabilities_ptr;

    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} pci_header0_t;

pci_common_t get_device(size_t index);
uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

uint16_t pci_read_vendor(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_device(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_read_class(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_read_subclass(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_read_header_type(uint8_t bus, uint8_t slot, uint8_t func);

// pci_common_t pci_read_header_common(uint8_t bus, uint8_t slot);

const char *pci_get_device_name(uint8_t class, uint8_t subclass);

void pci_check_device(uint8_t bus, uint8_t device);
void pci_check_bus(uint8_t bus);
void pci_check_all_busses();
void pci_check_function(uint8_t bus, uint8_t device, uint8_t func);

pci_common_t *pci_get_device_list();
pci_common_t *pci_get_device_end();