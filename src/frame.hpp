#ifndef FRAME_HPP_INCLUDE
#define FRAME_HPP_INCLUDE

#include "common.hpp"

class Frame {
public:
  Frame(const char *path);
  Mat raw;
  Mat resized;
  bool isIndoor;
  int phase;
  int original_number;
  int number;

private:
  void CheckIsIndoor();
  void CalcPhase();
  void CalcHist();
  MatND hist;
};

#endif /* end of include guard: FRAME_HPP_INCLUDE */
