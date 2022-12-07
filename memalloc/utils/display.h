#include "../core/memory_helper.h"

struct DisplayBar {
  int totalSize, barSize;
};

typedef struct DisplayBar DisplayBar;

DisplayBar *initDisplay(int totalSize, int barSize);
void displayMemory(Memory *memory, DisplayBar *bar);
