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
  int number;

private:
  void CheckIsIndoor();
  void CalcPhase();
};

#endif /* end of include guard: FRAME_HPP_INCLUDE */
