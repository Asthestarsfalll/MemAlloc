#include "display.h"
#include "../core/memory_helper.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getColor(ColorManager *manager, int index) {
  char *color;
  switch (index) {
  case -1:
    color = manager->white;
    break;
  case 0:
    color = manager->red;
    break;
  case 1:
    color = manager->green;
    break;
  case 2:
    color = manager->yellow;
    break;
  case 3:
    color = manager->blue;
    break;
  case 4:
    color = manager->purple;
    break;
  case 5:
    color = manager->deep_green;
    break;
  default:
    return manager->black;
  }
  return color;
}

ColorManager *initForeGroundColor() {
  ColorManager *fgc = alloc(ColorManager, 1);
  strcpy(fgc->black, "\e[30m\0");
  strcpy(fgc->red, "\e[31m\0");
  strcpy(fgc->green, "\e[32m\0");
  strcpy(fgc->yellow, "\e[33m\0");
  strcpy(fgc->blue, "\e[34m\0");
  strcpy(fgc->purple, "\e[35m\0");
  strcpy(fgc->deep_green, "\e[36m\0");
  strcpy(fgc->white, "\e[37m\0");
  return fgc;
}

ColorManager *initBackGroundColor() {
  ColorManager *bgc = alloc(ColorManager, 1);
  strcpy(bgc->black, "\e[40m\0");
  strcpy(bgc->red, "\e[41m\0");
  strcpy(bgc->green, "\e[42m\0");
  strcpy(bgc->yellow, "\e[43m\0");
  strcpy(bgc->blue, "\e[44m\0");
  strcpy(bgc->purple, "\e[45m\0");
  strcpy(bgc->deep_green, "\e[46m\0");
  strcpy(bgc->white, "\e[47m\0");
  return bgc;
}

ColorManager *FGColorManager, *BGColorManager;

DisplayBar *initDisplay(int totalSize, int barSize) {
  FGColorManager = initForeGroundColor();
  BGColorManager = initBackGroundColor();
  DisplayBar *bar = alloc(DisplayBar, 1);
  bar->totalSize = totalSize;
  bar->barSize = barSize;
  return bar;
}

char *getFormat(DisplayBar *bar, char *prevFormat, char *fcolor,
                int blockSize) {
  float ratio = (float)blockSize / bar->totalSize;
  int barSize = (int)(bar->barSize * ratio);
  barSize = barSize == 0 ? 1 : barSize;
  char *new_bar = alloc(char, barSize + 1);
  for (int i = 0; i < barSize; i++)
    new_bar[i] = LABLE;
  strcat(prevFormat, fcolor);
  strcat(prevFormat, new_bar);
  return prevFormat;
}

void displayMemory(Memory *memory, DisplayBar *bar) {
  MemoryBlock *block = memory->memoryBlocks->nextBlock;
  int index = 0;
  char *fcolor, *bcolor, *format = alloc(char, bar->barSize + 100);
  format[0] = '\0';
  while (block != NULL) {
    if (isBusyBlock(block)) {
      fcolor = getColor(FGColorManager, block->pid % 6);
    } else if (block->is_fragmentation)
      fcolor = getColor(FGColorManager, -2);
    else
      fcolor = getColor(FGColorManager, -1);

    format = getFormat(bar, format, fcolor, block->blockSize);

    block = block->nextBlock;
  }
  strcat(format, UNSET);
  system("clear");
  printf("\n\n\n");
  printf(format); // just ingnore warning
  printf("\n\n\n");

  char *upper = alloc(char, 89);
  for (int i = 0; i < 88; i++) {
    upper[i] = '-';
  }
  puts(upper);
  printf("\e[43m\e[31m|   PID   |   begin   |   end   |   size   |   start "
         "time   |  "
         " end time   |   color   |\e[0m\n");
  puts(upper);
  int begin, end, size, st, ed;
  char label[] = "■■  ";
  index = 0;
  block = memory->memoryBlocks->nextBlock;
  while (block != NULL) {
    if (isSpareBlock(block)) {
      printf("|    -    ");
      if (block->is_fragmentation)
        fcolor = getColor(FGColorManager, -2);
      else
        fcolor = getColor(FGColorManager, -1);
    } else {
      printf("|   %-6d", block->pid);
      fcolor = getColor(FGColorManager, block->pid % 6);
      index = (index + 1) % 6;
    }
    end = block->begin + block->blockSize;
    printf("|   %-8d|   %-6d|   %-7d|     %-11d|     %-9d|", block->begin, end,
           block->blockSize, block->lifeSpan->startTime,
           block->lifeSpan->endTime);
    printf("    %s%s%s   |\n", fcolor, label, UNSET);
    block = block->nextBlock;
    puts(upper);
  }
}
