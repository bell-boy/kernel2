#pragma once
#define PAGE_SIZE 4096

class Frame {
    Frame* next;
    char padding[PAGE_SIZE - sizeof(Frame*)];
};

extern void init_falloc();
extern Frame* falloc();
extern void ffree(Frame* frame);