#include "tools.hpp"
#include <dirent.h>
#include <sys/types.h>

std::vector<std::string> ListDir(const char *path) {
  DIR *dir;
  dir = opendir(path);
  if (dir == NULL) {
    cout << "ERROR: Can't open dir " << path << endl;
    exit(0);
  }
  dirent *entry;
  vector<string> ret;
  while ((entry = readdir(dir))) {
    ret.emplace_back(entry->d_name);
  }
  closedir(dir);
  return ret;
}

bool EndsWith(const string &ending, const string &src) {
  return src.length() > ending.length() &&
         equal(ending.rbegin(), ending.rend(), src.rbegin());
}

void Init() {
  // load title image
  const char *title_path = "./title.jpg";
  TITLE = imread(title_path);

  // load earth
  const char *earth_dir = "./earth";
  char earth_path[20];
  for (int i = 0; i != EARTH_NUMBER; ++i) {
    sprintf(earth_path, "%s/%03d.jpg", earth_dir, i);
    EARTH[i] = imread(earth_path);
  }
}

void InitAbsdiff(const vector<Frame> &frames) {
  Mat t;
  for (int i = 0; i != frames.size() - 1; ++i) {
    for (int j = i + 1; j != frames.size(); ++j) {
      const Frame &f1 = frames[i], &f2 = frames[j];
      if (f1.isIndoor != f2.isIndoor) {
        continue;
      }
      // multiply(f1.resized, f2.resized, t);
      absdiff(f1.resized, f2.resized, t);
      Scalar_<double> s = sum(t);
      ABSDIFF[f1.number][f2.number] = ABSDIFF[f2.number][f1.number] =
          (s.val[0] + s.val[1] + s.val[2]);
    }
  }
}

int NearestFrameIndex(int src, const vector<int> &range) {
  double min_absdiff = 1e9;
  int nearest_index = -1;
  for (int i = 0; i != range.size(); ++i) {
    if (ABSDIFF[src][range[i]] < min_absdiff) {
      min_absdiff = ABSDIFF[src][range[i]];
      nearest_index = i;
    }
  }
  return nearest_index;
}

deque<int> SortFrames(vector<int> frames) {
  deque<int> sorted_frames;
  sorted_frames.push_back(frames.back());
  frames.pop_back();
  int front_nearest = -1, back_nearest = -1;
  double front_absdiff, back_absdiff;
  while (!frames.empty()) {
    if (front_nearest == -1) {
      front_nearest = NearestFrameIndex(sorted_frames.front(), frames);
      front_absdiff = ABSDIFF[sorted_frames.front()][frames[front_nearest]];
    }
    if (back_nearest == -1) {
      back_nearest = NearestFrameIndex(sorted_frames.back(), frames);
      back_absdiff = ABSDIFF[sorted_frames.back()][frames[back_nearest]];
    }
    if (front_absdiff < back_absdiff) {
      sorted_frames.push_front(frames[front_nearest]);
      frames[front_nearest] = frames.back();
      frames.pop_back();
      if (front_nearest == back_nearest) {
        back_nearest = -1;
      }
      front_nearest = -1;
    } else {
      sorted_frames.push_back(frames[back_nearest]);
      frames[back_nearest] = frames.back();
      frames.pop_back();
      if (back_nearest == front_nearest) {
        front_nearest = -1;
      }
      back_nearest = -1;
    }
  }
  return sorted_frames;
}

bool IsSceneInversed(vector<int> scene) {
  if (scene.size() == 1) {
    return false;
  }
  int s = 0;
  for (int i = 0; i != scene.size() - 1; ++i) {
    if (true) {
    }
  }
}
vector<vector<int>> OutdoorFramesResort(deque<int> frames) {
  const int threshold = 1e5;
  vector<vector<int>> scenes;
  vector<int> cur_scene;
  cur_scene.push_back(frames.front());
  frames.pop_front();
  while (!frames.empty()) {
    if (ABSDIFF[frames.front()][cur_scene.back()] < threshold) {
      cur_scene.push_back(frames.front());
      frames.pop_front();
    } else {
      scenes.push_back(cur_scene);
      cur_scene.clear();
      cur_scene.push_back(frames.front());
      frames.pop_front();
    }
  }
}
