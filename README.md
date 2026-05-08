# another operating system

THIS time is different trust

## Installation Instructions

`uv` is a fast, single-tool package manager.
To install `uv`, run
```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

`clang` is our C++ compiler of choice.
`lld` is our linker of choice.
`qemu-system-arm` is our emulator of choice. Note that we are targeting the ARM ISA.
To install these tools, run
```bash
sudo apt install clang lld qemu-system-arm
```

to build + run qemu

```bash
uv run buildtool qemu
```

Note the buildtool uses clang + ld.lld, so make sure you have these installed! 

## TODO:
- [ ] get to user mode:
    - [ ] be able to load programs into memory from disk
        - [ ] disk driver
            - [ ] parse the device tree
                - [ ] create a linked-list data-strcture
                    - [ ] get allocation working (in-progress)
            - [ ] do driver discovery 
            - [ ] write virtio-disk driver
                - [ ] handle interrupts
        - [ ] ext2
        - [ ] elf loader
    - [ ] be able to swtich to el0
    - [ ] create tool chain for building c programs 
