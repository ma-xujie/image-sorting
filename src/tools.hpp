#ifndef TOOLS_HPP_INCLUDE
#define TOOLS_HPP_INCLUDE

#include "common.hpp"
#include "frame.hpp"
#include "scene.hpp"

vector<string> ListDir(const char *dir_path);
bool EndsWith(const string &ending, const string &src);

void Init(); // 读取演播室场景的特征，读取小地球 :-)
void InitAbsdiff(const vector<Frame> &frames); // 计算所有帧两两之间的绝对差

deque<int> SortFrames(vector<int> frames); // 用贪心方法给 vector<int>
                                           // 中的所有帧排序，容器中的数值是
                                           // Frame ID，由读取图片的顺序决定
deque<Scene> SortOutdoorFrames(vector<int> outdoor_frames,
                               const vector<Frame> &frames); // 分镜头排序

#endif /* end of include guard: TOOLS_HPP_INCLUDE */
