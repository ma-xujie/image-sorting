#ifndef TOOLS_HPP_INCLUDE
#define TOOLS_HPP_INCLUDE

#include "common.hpp"
#include "frame.hpp"

vector<string> ListDir(const char *dir_path);
bool EndsWith(const string &ending, const string &src);

void Init();
void InitAbsdiff(const vector<Frame> &frames);

deque<int> SortFrames(vector<int> frames);
deque<int> SortOutdoorFrames(vector<int> outdoor_frames,
                             const vector<Frame> &frames);
vector<vector<int>> Clustering(vector<int> frames);
deque<vector<int>> OutdoorFramesReorder(vector<vector<int>> scenes,
                                        const vector<Frame> &frames);

#endif /* end of include guard: TOOLS_HPP_INCLUDE */
