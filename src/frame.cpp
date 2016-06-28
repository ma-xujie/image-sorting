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
    ++i;
  }
  number[j] = 0;

  int ret;
  sscanf(number, "%d", &ret);
  return ret;
}

Frame::Frame(const char *path) {
  Mat image = imread(path); // 读取图片
  this->number = GetNumberFromPath(path);
  CheckIsIndoor(image);
  if (!this->isIndoor) {
    CalcPhase(image);
  }
  resize(image, this->resized, Size(160, 120)); // 将图片缩小
}

void Frame::CheckIsIndoor(Mat &image) { // 根据帧下方的标题判断是否是演播室场景
  Mat title_area = image(Range(400, 431), Range(50, 401));
  Mat dst;
  absdiff(title_area, TITLE, dst);
  Scalar_<double> diffsum = sum(dst);
  this->isIndoor = (diffsum[0] < 8e5 && diffsum[1] < 8e5 && diffsum[2] < 8e5);
}

void Frame::CalcPhase(Mat &image) { // 用小地球的信息计算相位
  long long min_diff = 1e10;
  Mat earth_area = image(Range(395, 421), Range(65, 111));
  Mat dst;
  for (int i = 0; i != EARTH_NUMBER; ++i) {
    absdiff(EARTH[i], earth_area, dst);
    auto s = sum(dst);
    int t = s.val[0] + s.val[1] + s.val[2];
    if (t < min_diff) {
      min_diff = t;
      this->phase = i;
    }
  }
}
