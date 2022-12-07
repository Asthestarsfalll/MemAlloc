#include "memory_helper.h"
#include <malloc.h>
#include <stdio.h>

void printMemoryBlock(MemoryBlock *block) {
  printf("begin: %d, size: %d, end: %d, st:%d, ed:%d, pid: %d, "
         "is_fragmentation: %d\n",
         block->begin, block->blockSize, block->begin + block->blockSize,
         block->lifeSpan->startTime, block->lifeSpan->endTime, block->pid,
         block->is_fragmentation);
}

void printMemory(Memory *memory) {
  MemoryBlock *block = memory->memoryBlocks->nextBlock;
  while (block != NULL) {
    printMemoryBlock(block);
    block = block->nextBlock;
  }
}

MemoryBlock *initMemoryBlock(int startTime, int endTime, int begin,
                             int blockSize, int pid, int is_fragmentation) {
  MemoryBlock *block = alloc(MemoryBlock, 1);
  Timer *lifeSpan = alloc(Timer, 1);
  lifeSpan->startTime = startTime;
  lifeSpan->endTime = endTime;
  block->blockSize = blockSize;
  block->begin = begin;
  block->pid = pid;
  block->lifeSpan = lifeSpan;
  block->nextBlock = NULL;
  block->lastBlock = NULL;
  block->is_fragmentation = is_fragmentation;
  return block;
}

Memory *initMemory(int totalSize) {
  Memory *memory = alloc(Memory, 1);
  MemoryBlock *block = initMemoryBlock(-1, -1, 0, totalSize, -1, 0),
              *head = initMemoryBlock(-1, -1, 0, 1, -2, 0);
  memory->memory = alloc(char, totalSize);
  memory->totalSize = totalSize;
  head->nextBlock = block;
  block->lastBlock = head;
  memory->memoryBlocks = head;
  return memory;
}

int isSpareBlock(MemoryBlock *block) { return block->pid == -1; }

int isBusyBlock(MemoryBlock *block) { return block->pid != -1; }

int isValidSpareBlock(MemoryBlock *block) {
  return block->pid == -1 && !block->is_fragmentation;
}

MemoryBlock *nextCondBlock(MemoryBlock *block, condition cond) {
  while (block != NULL) {
    if (cond(block))
      return block;
    block = block->nextBlock;
  }
  return NULL;
}

MemoryBlock *nextSpareBlock(MemoryBlock *block) {
  return nextCondBlock(block, isSpareBlock);
}

MemoryBlock *nextBusyBlock(MemoryBlock *block) {
  return nextCondBlock(block, isBusyBlock);
}

MemoryBlock *nextValidSpareBlock(MemoryBlock *block) {
  return nextCondBlock(block, isValidSpareBlock);
}

MemoryBlock *lastCondBlock(MemoryBlock *block, condition cond) {
  while (block != NULL) {
    if (cond(block))
      return block;
    block = block->lastBlock;
  }
  return NULL;
}

MemoryBlock *lastValidSpareBlock(MemoryBlock *block) {
  return lastCondBlock(block, isValidSpareBlock);
}

void insert(MemoryBlock *newBlock, MemoryBlock *targetBlock, int blockSize) {
  newBlock->lastBlock = targetBlock->lastBlock;
  targetBlock->lastBlock->nextBlock = newBlock;
  targetBlock->lastBlock = newBlock;
  targetBlock->blockSize -= blockSize;
  if (targetBlock->blockSize == 0) {
    newBlock->nextBlock = targetBlock->nextBlock;
    if (targetBlock->nextBlock != NULL) {
      // targetBlock->nextBlock->is_fragmentation = 0;
      targetBlock->nextBlock->lastBlock = newBlock;
    }
    free(targetBlock);
  } else {
    newBlock->nextBlock = targetBlock;
    targetBlock->begin += newBlock->blockSize;
  }
}

void _free(MemoryBlock *lastBlock, MemoryBlock *targetBlock) {
  targetBlock->pid = -1;
  targetBlock->lifeSpan->startTime = -1;
  targetBlock->lifeSpan->endTime = -1;
  if (isValidSpareBlock(lastBlock)) {
    if (targetBlock->nextBlock != NULL) {
      targetBlock->nextBlock->lastBlock = lastBlock;
    }
    lastBlock->nextBlock = targetBlock->nextBlock;
    lastBlock->blockSize += targetBlock->blockSize;
    free(targetBlock);
  }
}

int fit(struct Memory *memory, int startTime, int endTime, int blockSize,
        int pid, process operation) {
  MemoryBlock *blockNode = memory->memoryBlocks->nextBlock;
  printMemoryBlock(blockNode);
  MemoryBlock *targetBlock = operation(blockNode, blockSize);
  if (targetBlock != NULL) {
    MemoryBlock *newBlock = initMemoryBlock(
        startTime, endTime, targetBlock->begin, blockSize, pid, 0);
    insert(newBlock, targetBlock, blockSize);
    if (targetBlock->blockSize == 0)
      _free(targetBlock->lastBlock, targetBlock);
    return 1;
  }
  return 0;
}

void merge(Memory *memory) {
  MemoryBlock *block = nextValidSpareBlock(memory->memoryBlocks->nextBlock);
  MemoryBlock *spBlock = nextValidSpareBlock(block->nextBlock);

  while (block != NULL && spBlock != NULL) {
    if (block->nextBlock == spBlock) {
      _free(block, spBlock);
    } else
      block = nextValidSpareBlock(block->nextBlock);
    spBlock = nextValidSpareBlock(block->nextBlock);
  }
}

int free_block(Memory *memory, int pid) {
  int flag = 0;
  MemoryBlock *block = memory->memoryBlocks->nextBlock;
  while (block != NULL) {
    if (block->pid == pid) {
      if (block->nextBlock != NULL)
        block->nextBlock->is_fragmentation = 0;
      block->is_fragmentation = 0;
      _free(block->lastBlock, block);
      merge(memory);
      flag = 1;
      break;
    }
    block = block->nextBlock;
  }

  return flag;
}
