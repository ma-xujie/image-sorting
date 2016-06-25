#ifndef TOOLS_HPP_INCLUDE
#define TOOLS_HPP_INCLUDE

#include "common.hpp"
#include "frame.hpp"

vector<string> ListDir(const char *dir_path);
bool EndsWith(const string &ending, const string &src);

void Init();
void InitAbsdiff(const vector<Frame> &frames);

int NearestFrameIndex(int src, const vector<int> &range);
deque<int> SortFrames(vector<int> frames);
deque<vector<int>> OutdoorFramesReorder(deque<int> outdoor_frames,
                                        const vector<Frame> &frames);

#endif /* end of include guard: TOOLS_HPP_INCLUDE */
