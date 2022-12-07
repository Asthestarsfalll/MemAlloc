#include "init.h"

void randomInitializeMemory(Memory *memory) {
  int times = 1 + rand() % 3, begin, blockSize, size = 0;
  float ratio, length = (float)memory->totalSize / 4;
  MemoryBlock *block, *spareBlock,
      *targetBlock = memory->memoryBlocks->nextBlock;
  for (int i = 0; i < times; i++) {
    ratio = (float)(rand() % 25) / 100;
    begin = (int)((i * 0.25 + ratio) * memory->totalSize);
    blockSize = (int)((float)(3 + rand() % 5) / 10 * length);
    block = initMemoryBlock(-1, -1, begin, blockSize, 999 - i, 0);
    targetBlock->blockSize = begin - size;
    targetBlock->nextBlock = block;
    size = block->begin + block->blockSize;
    spareBlock = initMemoryBlock(-1, -1, size, memory->totalSize - size, -1, 0);
    block->nextBlock = spareBlock;
    block->lastBlock = targetBlock;
    spareBlock->lastBlock = block;
    targetBlock = spareBlock;
  }
}
