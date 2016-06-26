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
      ABSDIFF[i][j] = ABSDIFF[j][i] = (s.val[0] + s.val[1] + s.val[2]);
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

bool IsSceneInversed(vector<int> scene, const vector<Frame> &frames) {
  if (scene.size() == 1) {
    return false;
  }
  int s = 0;
  for (int i = 0; i != scene.size() - 1; ++i) {
    int phase_diff = frames[i + 1].phase - frames[i].phase;
    if ((0 < phase_diff && phase_diff < 5) || phase_diff < 295) {
      s += 1;
    } else {
      s -= 1;
    }
  }
  return s < 0;
}

deque<vector<int>> OutdoorFramesReorder(vector<vector<int>> scenes,
                                        const vector<Frame> &frames) {
  deque<vector<int>> sorted_scenes;
  sorted_scenes.push_back(scenes.back());
  scenes.pop_back();
  int all_empty_flag = 0;
  while (!all_empty_flag) {
    int min_phase_diff = 1000;
    int which_scene;
    bool is_back;
    all_empty_flag = 1;
    for (int i = 0; i != scenes.size(); ++i) {
      if (scenes[i].empty()) {
        continue;
      }
      all_empty_flag = 0;
      int back_phase_diff = abs(frames[scenes[i].front()].phase -
                                frames[sorted_scenes.back().back()].phase);
      int front_phase_diff = abs(frames[sorted_scenes.front().front()].phase -
                                 frames[scenes[i].back()].phase);

      if (back_phase_diff > 290) {
        back_phase_diff = 302 - back_phase_diff;
      }
      if (front_phase_diff > 290) {
        front_phase_diff = 302 - front_phase_diff;
      }
      if (back_phase_diff < min_phase_diff) {
        min_phase_diff = back_phase_diff;
        which_scene = i;
        is_back = 1;
      }
      if (front_phase_diff < min_phase_diff) {
        min_phase_diff = front_phase_diff;
        which_scene = i;
        is_back = 0;
      }
    }
    if (!all_empty_flag) {
      if (is_back) {
        sorted_scenes.push_back(scenes[which_scene]);
      } else {
        sorted_scenes.push_front(scenes[which_scene]);
      }
      scenes[which_scene].clear();
    }
  }
  return sorted_scenes;
}

vector<vector<int>> Clustering(vector<int> frames) {
  const int threshold = 25e5;
  vector<vector<int>> scenes;
  scenes.push_back(vector<int>(1, frames.back()));
  frames.pop_back();

  while (!frames.empty()) {
    int frame = frames.back();
    frames.pop_back();
    int min_absdiff = 1e9, min_scene = 0;
    for (int i = 0; i != scenes.size(); ++i) {
      for (int j = 0; j != scenes[i].size(); ++j) {
        if (ABSDIFF[frame][scenes[i][j]] < min_absdiff) {
          min_absdiff = ABSDIFF[frame][scenes[i][j]];
          min_scene = i;
        }
      }
    }
    if (min_absdiff < threshold) {
      scenes[min_scene].push_back(frame);
    } else {
      scenes.push_back(vector<int>(1, frame));
    }
  }

  bool scene_adjustment_flag = 1;
  while (scene_adjustment_flag) {
    scene_adjustment_flag = 0;
    for (int i = 0; i != scenes.size(); ++i) {
      if (scenes[i].size() < 25) {
        int min_scene = -1;
        int min_absdiff = 1e9;
        for (int j = 0; j != scenes.size(); ++j) {
          if (j == i || scenes[j].size() < 25) {
            continue;
          }
          int absdiff = ABSDIFF[scenes[j][0]][scenes[i][0]];
          if (absdiff < min_absdiff) {
            min_absdiff = absdiff;
            min_scene = j;
          }
        }
        if (min_scene != -1) {
          scene_adjustment_flag = 1;
          scenes[min_scene].insert(scenes[min_scene].end(), scenes[i].begin(),
                                   scenes[i].end());
          scenes[i] = scenes.back();
          scenes.pop_back();
          break;
        }
      }
    }
  }
  return scenes;
}
