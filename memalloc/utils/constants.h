#pragma once

#include <stdio.h>
#include <string.h>

const char LABLE = '|';
const char CHAR_TYPE[3] = "%c";
const char STRING_TYPE[3] = "%s";
const char UNSET[5] = "\e[0m";
const char CLEAR[3] = "2J";

struct ColorManager {
  char black[7];
  char red[7];
  char green[7];
  char yellow[7];
  char blue[7];
  char purple[7];
  char deep_green[7];
  char white[7];
};

typedef struct ColorManager ColorManager;
