#ifndef FRAME_HPP_INCLUDE
#define FRAME_HPP_INCLUDE

#include "common.hpp"

class Frame {
public:
  Frame(const char *path);
  Mat resized;
  bool isIndoor;
  int phase;
  int number;
  string filename;

private:
  void CheckIsIndoor(Mat &image);
  void CalcPhase(Mat &image);
};

#endif /* end of include guard: FRAME_HPP_INCLUDE */
