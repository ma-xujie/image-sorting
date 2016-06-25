#ifndef TOOLS_HPP_INCLUDE
#define TOOLS_HPP_INCLUDE

#include "common.hpp"
#include "frame.hpp"

vector<string> ListDir(const char *dir_path);
bool EndsWith(const string &ending, const string &src);

void Init();
void InitSimilarity(const vector<Frame> &frames);
#endif /* end of include guard: TOOLS_HPP_INCLUDE */
