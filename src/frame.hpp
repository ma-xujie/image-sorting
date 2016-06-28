#ifndef FRAME_HPP_INCLUDE
#define FRAME_HPP_INCLUDE

#include "common.hpp"

// 帧的结构体实现，每一个输入的图像都是一帧
class Frame {
public:
  Frame(const char *path); // 读取该帧，做预处理
  Mat resized;             // 只存储每张图像放缩后的结果
  bool isIndoor;           // 是否为演播室场景，在初始化时计算
  int phase;               // 根据小地球计算出的相位信息
  int number;              // 图片文件名中的序号

private:
  void CheckIsIndoor(Mat &image); // 初始化时调用
  void CalcPhase(Mat &image);     // 初始化时调用
};

#endif /* end of include guard: FRAME_HPP_INCLUDE */
