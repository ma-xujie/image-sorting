#ifndef COMMON_HPP_INCLUDE
#define COMMON_HPP_INCLUDE

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <queue>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

const int MAX_FRAME_NUMBER = 6000;
extern int ABSDIFF[MAX_FRAME_NUMBER][MAX_FRAME_NUMBER];

const int EARTH_NUMBER = 302;
extern Mat EARTH[EARTH_NUMBER];
extern Mat TITLE;

#endif /* end of include guard: COMMON_HPP_INCLUDE */
