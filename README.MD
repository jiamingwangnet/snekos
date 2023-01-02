# SnekOS

A 64-bit operating system that only runs snake and snake related games.

## Build

### Requirements
- nasm
- gcc
- grub-mkrescue
- xorriso

Open a terminal and run: 
`make build`

## Run
Make sure [Qemu](https://www.qemu.org/download/) is installed.

Open a terminal and run: 
`qemu-system-x86_64 -drive file=dist/SnekOS.iso`

## Updates
- Basic keyboard input
- Displays text on screen