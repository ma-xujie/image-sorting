#ifndef SCENE_HPP_INCLUDE
#define SCENE_HPP_INCLUDE

#include "common.hpp"
#include "frame.hpp"

// 场景类，一个场景包含一个有序的帧序列
class Scene {
public:
  Scene(const vector<int> &frames_, const vector<Frame> &all_frames_);
  Scene(const Scene &scene);

  // 在当前帧序列的前面或后面粘帖另一个帧序列
  void ConcatFront(const Scene &another_scene);
  void ConcatBack(const Scene &another_scene);

  // 将当前帧序列尾/头与另一帧序列头/尾进行比较
  int BackAbsdiff(const Scene &another_scene);
  int FrontAbsdiff(const Scene &another_scene);

  // 计算当前帧序列是否与另一个帧序列相位连续
  bool BackPhaseContinue(Scene &another_scene, int n);
  bool FrontPhaseContinue(Scene &another_scene, int n);
  const vector<Frame> *all_frames; // all_frames 中含有所有帧的相位信息
  vector<int> frames;              // 有序帧序列

private:
  // 根据相位信息检查当前帧序列是否是逆序的，在初始化时调用
  bool IsReversed();
};

#endif /* end of include guard: SCENE_HPP_INCLUDE */
