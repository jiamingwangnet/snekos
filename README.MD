# SnekOS

A 64-bit operating system that only runs snake and snake related games.

It has two boot modes, snake and terminal. The snake mode will boot into the snake game and run the game forever, the terminal mode will boot into a terminal for running commands including the snake game.

## Build

### Requirements
- nasm
- gcc
- grub-mkrescue
- xorriso

Open a terminal and run: 
`make build`

## Run in QEMU
Make sure [QEMU](https://www.qemu.org/download/) is installed.

Open a terminal and run: 
`qemu-system-x86_64 -drive file=dist/SnekOS.iso`

## Updates
- Added snake game
