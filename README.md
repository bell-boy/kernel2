# another operating system

THIS time is different trust

to build + run qemu

```python
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
