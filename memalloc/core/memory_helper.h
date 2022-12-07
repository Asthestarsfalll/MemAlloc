#pragma once

#include <malloc.h>
#include <stdio.h>

struct Timer {
  int startTime;
  int endTime;
};

struct MemoryBlock {
  int begin, blockSize, pid, is_fragmentation;
  struct Timer *lifeSpan;
  struct MemoryBlock *nextBlock, *lastBlock;
};

struct Memory {
  int totalSize;
  char *memory;
  struct MemoryBlock *memoryBlocks;
};
typedef struct Memory Memory;
typedef struct MemoryBlock MemoryBlock;
typedef struct Timer Timer;
typedef MemoryBlock *(*process)(MemoryBlock *, int);
typedef int (*condition)(MemoryBlock *);

#define alloc(type, size) (type *)malloc(sizeof(type) * size)

void printMemoryBlock(MemoryBlock *block);
void printMemory(Memory *memory);

MemoryBlock *initMemoryBlock(int startTime, int endTime, int begin,
                             int blockSize, int pid, int is_fragmentation);

Memory *initMemory(int totalSize);

int isSpareBlock(MemoryBlock *block);

int isBusyBlock(MemoryBlock *block);

int isValidSpareBlock(MemoryBlock *block);

MemoryBlock *nextCondBlock(MemoryBlock *block, condition cond);

MemoryBlock *lastCondBlock(MemoryBlock *block, condition cond);

MemoryBlock *nextSpareBlock(MemoryBlock *block);

MemoryBlock *nextBusyBlock(MemoryBlock *block);

MemoryBlock *nextValidSpareBlock(MemoryBlock *block);

MemoryBlock *lastValidSpareBlock(MemoryBlock *block);

void insert(MemoryBlock *newBlock, MemoryBlock *targetBlock, int blockSize);

void _free(MemoryBlock *lastBlock, MemoryBlock *targetBlock);

int fit(Memory *memory, int startTime, int endTime, int blockSize, int pid,
        process operation);

void merge(Memory *memory);

int free_block(Memory *memory, int pid);
