#include "frame.hpp"
#include <ctype.h>

int GetNumberFromPath(const char *path) {
  char number[10];
  int i = 0, j = 0;
  while (path[i] != 0) {
    if (isdigit(path[i])) {
      if (i == 0 || !isdigit(path[i - 1])) {
        j = 0;
        number[j++] = path[i];
      } else {
        number[j++] = path[i];
      }
    }
  }
  number[j] = 0;

  int ret;
  sscanf(number, "%d", &ret);
  return ret;
}

Frame::Frame(const char *path) {
  this->raw = imread(path);
  this->original_number = GetNumberFromPath(path);
  CheckIsIndoor();
  if (!this->isIndoor) {
    CalcPhase();
  }
}

void Frame::CheckIsIndoor() {}

void Frame::CalcPhase() {}

void Frame::CalcHist() {}
