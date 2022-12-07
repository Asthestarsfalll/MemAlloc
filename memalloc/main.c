#include "core/fit_methods.h"
#include "core/memory_helper.h"
#include "utils/display.h"
#include "utils/init.h"
#include <stdio.h>
#include <unistd.h>

process fitFunc = firstfit;
int global_data_num, is_sim = 0, is_all_spare = 1, max_time;
void changeMethod(int index) {
  switch (index) {
  case 1:
    fitFunc = firstfit;
    break;
  case 2:
    fitFunc = worstfit;
    break;
  case 3:
    fitFunc = bestfit;
    break;
  case 4:
    fitFunc = nextfit;
    break;
  case 5:
    fitFunc = fastfit;
    break;
  }
}

int getOpration() {
  int op;
  scanf("%d", &op);
  return op;
}

void showMenu() {
  puts("\n\n\n");
  printf("      1. 适 应 方 法\n");
  printf("      2. 添 加 进 程\n");
  printf("      3. 释 放 进 程\n");
  printf("      4. 查 看 内 存\n");
  if (is_sim)
    printf("      5. 关 闭 模 拟 模 式\n");
  else
    printf("      5. 开 启 模 拟 模 式\n");
  printf("      6. 开 始 模 拟\n");
  printf("      7. 退 出 程 序\n");
}

void showMethods() {
  puts("\n\n\n");
  printf("      1. 首 次 适 应");
  printf("      2. 最 差 适 应");
  printf("      3. 最 佳 适 应");
  printf("      4. 下 次 适 应");
  printf("      5. 快 速 适 应");
}

void showAddPrc() {
  puts("\n\n\n");
  printf("请分别输入进程的pid号、占用内存、开始时间、结束时间（没有则填-1）\n");
}

void timeProcess(Memory *memory, DisplayBar *bar, int (*data)[100]) {
  int time = 0, pid, mem, st, ed;
  MemoryBlock *block = memory->memoryBlocks->nextBlock;
  // getchar();
  while (time <= max_time) {
    printf("                        \e[33mTIME:  %d\e[0m", time);
    for (int i = 0; i < global_data_num; i++) {
      st = data[2][i];
      ed = data[3][i];
      if (time != st && time != ed)
        continue;
      pid = data[0][i];
      mem = data[1][i];
      if (time == st)
        fit(memory, st, ed, mem, pid, fitFunc);
      else
        free_block(memory, pid);
    }
    if (time == max_time)
      getchar();
    displayMemory(memory, bar);
    sleep(1);
    // getchar();
    time++;
  }
}

int main() {
  Memory *memory = initMemory(1024);
  // randomInitializeMemory(memory);
  printMemory(memory);
  DisplayBar *bar = initDisplay(memory->totalSize, 90);
  int operator= 0, pid = -1, mem = 0, st = -1, end = -1, status = 1;

  int data[4][100]; // pid, mem size, st time, ed time
  system("clear");
  while (1) {
    if (operator== 5)
      break;
    else if (operator== 0)
      showMenu();

    operator= getOpration();

    system("clear");
    if (operator== 2) {
      showAddPrc();
      scanf("%d %d %d %d", &pid, &mem, &st, &end);
      if (st > 0)
        is_all_spare = 1;

      if (end > max_time)
        max_time = end;
      if (!is_sim) {
        status = fit(memory, st, end, mem, pid, fitFunc);
        if (!status) {
          printf("申请失败！");
        }
      } else {
        data[0][global_data_num] = pid;
        data[1][global_data_num] = mem;
        data[2][global_data_num] = st;
        data[3][global_data_num] = end;
      }
      global_data_num++;
      system("clear");
    } else if (operator== 3) {
      printf("\n\n\n请输入想要释放的进程号:\n");
      operator= getOpration();
      status = free_block(memory, operator);
      if (!status)
        printf("不存在该进程号： %d", operator);
      else
        system("clear");
    } else if (operator== 4) {
      displayMemory(memory, bar);
      getchar();
      getchar();
      system("clear");
    } else if (operator== 5) {
      is_sim = 1;
      global_data_num = 0;
    } else if (operator== 6) {
      if (is_sim) {
        timeProcess(memory, bar, data);
        printf("模拟已结束，按回车返回主界面");
        getchar();
        getchar();
        system("clear");
      } else {
        printf("请先开启模拟模式！");
        getchar();
        system("clear");
      }
    } else if (operator== 1) {
      showMethods();
      operator= getOpration();
      system("clear");
      changeMethod(operator);
    } else
      printf("再给你一次机会。\n\n");
    operator= 0;
  }
  return 0;
}
