#include "fit_methods.h"
#include "memory_helper.h"
#include <math.h>
static int last_index = 0;

MemoryBlock *firstfit(MemoryBlock *block, int blockSize) {
  MemoryBlock *spareBlock = nextSpareBlock(block);
  while (spareBlock != NULL) {
    if (blockSize <= spareBlock->blockSize)
      return spareBlock;
    spareBlock = nextSpareBlock(spareBlock->nextBlock);
  }
  return NULL;
}

MemoryBlock *worstfit(MemoryBlock *block, int blockSize) {
  MemoryBlock *spareBlock = nextSpareBlock(block), *targetBlock = NULL;
  int maxSize = 0;
  while (spareBlock != NULL) {
    if (blockSize <= spareBlock->blockSize && maxSize < spareBlock->blockSize) {
      maxSize = spareBlock->blockSize;
      targetBlock = spareBlock;
    }
    spareBlock = nextSpareBlock(spareBlock->nextBlock);
  }
  printMemoryBlock(targetBlock);
  return targetBlock;
}

MemoryBlock *bestfit(MemoryBlock *block, int blockSize) {
  MemoryBlock *spareBlock = nextSpareBlock(block), *targetBlock = NULL;
  int minSize = 10000;
  while (spareBlock != NULL) {
    if (blockSize <= spareBlock->blockSize && minSize > spareBlock->blockSize) {
      minSize = spareBlock->blockSize;
      targetBlock = spareBlock;
    }
    spareBlock = nextSpareBlock(spareBlock->nextBlock);
  }
  return targetBlock;
}

MemoryBlock *nextfit(MemoryBlock *block, int blockSize) {
  MemoryBlock *spareBlock = nextSpareBlock(block);
  while (spareBlock != NULL) {
    if (spareBlock->begin >= last_index)
      break;
    spareBlock = nextSpareBlock(spareBlock->nextBlock);
  }
  spareBlock = firstfit(spareBlock, blockSize);
  if (spareBlock == NULL)
    spareBlock = firstfit(block, blockSize);
  if (spareBlock != NULL)
    last_index = spareBlock->begin;
  else
    last_index = 0;
  return spareBlock;
}

MemoryBlock *fastfit(MemoryBlock *block, int blockSize) {
  MemoryBlock *spareBlock = nextValidSpareBlock(block), *newBlock;
  int targetSise = (int)pow(2., (int)log2(blockSize) + 1), min = 10000, size;

  while (spareBlock != NULL) {
    size = spareBlock->blockSize - targetSise;
    if (size < min && size >= 0) {
      min = size;
      newBlock = spareBlock;
    }
    spareBlock = nextValidSpareBlock(spareBlock->nextBlock);
  }
  spareBlock = newBlock;
  int curSize = spareBlock->blockSize;

  while (curSize > targetSise) {
    curSize /= 2;
    newBlock = initMemoryBlock(-1, -1, spareBlock->begin, curSize, -1, 1);
    insert(newBlock, spareBlock, curSize);
    spareBlock->is_fragmentation = 0;
    spareBlock = newBlock;
  }
  if (curSize == targetSise)
    spareBlock->is_fragmentation = 1;
  if (spareBlock->blockSize > blockSize)
    return spareBlock;
  return NULL;
}
