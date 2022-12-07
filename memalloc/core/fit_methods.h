#pragma once

#include "memory_helper.h"
#include <malloc.h>
#include <stdio.h>

MemoryBlock *firstfit(MemoryBlock *, int);

MemoryBlock *worstfit(MemoryBlock *, int);

MemoryBlock *bestfit(MemoryBlock *, int);

MemoryBlock *nextfit(MemoryBlock *, int);

MemoryBlock *fastfit(MemoryBlock *, int);
