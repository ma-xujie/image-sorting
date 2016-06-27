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

double PhaseBonus(int phase1, int phase2, bool is_back) {
  int phase_diff = phase2 - phase1;
  if (phase_diff < -290) {
    phase_diff += 302;
  }
  if (phase_diff > 290) {
    phase_diff -= 302;
  }
  if (0 <= phase_diff && phase_diff <= 3 && is_back) {
    return 0.75;
  } else if (-3 <= phase_diff && phase_diff <= 0 && !is_back) {
    return 0.75;
  }
  return 1;
}

int NearestFrameIndex(int src, const vector<int> &range,
                      const vector<Frame> &frames, bool is_back) {
  double min_absdiff = 1e9;
  int nearest_index = -1;
  for (int i = 0; i != range.size(); ++i) {
    int absdiff = ABSDIFF[src][range[i]] *
                  PhaseBonus(frames[src].phase, frames[i].phase, is_back);
    if (absdiff < min_absdiff) {
      min_absdiff = absdiff;
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

deque<int> SortOutdoorFrames(vector<int> outdoor_frames,
                             const vector<Frame> &frames) {
  deque<int> sorted_frames;
  sorted_frames.push_back(outdoor_frames.back());
  outdoor_frames.pop_back();
  int front_nearest = -1, back_nearest = -1;
  double front_absdiff, back_absdiff;
  while (!outdoor_frames.empty()) {
    if (front_nearest == -1) {
      front_nearest = NearestFrameIndex(sorted_frames.front(), outdoor_frames,
                                        frames, false);
      front_absdiff =
          PhaseBonus(frames[sorted_frames.front()].phase,
                     frames[front_nearest].phase, false) *
          ABSDIFF[sorted_frames.front()][outdoor_frames[front_nearest]];
    }
    if (back_nearest == -1) {
      back_nearest =
          NearestFrameIndex(sorted_frames.back(), outdoor_frames, frames, true);
      back_absdiff =
          PhaseBonus(frames[sorted_frames.back()].phase,
                     frames[back_nearest].phase, true) *
          ABSDIFF[sorted_frames.back()][outdoor_frames[back_nearest]];
    }
    if (front_nearest == back_nearest) {
      if (PhaseBonus(frames[sorted_frames.front()].phase,
                     frames[front_nearest].phase, false) > 1.01) {
        sorted_frames.push_front(outdoor_frames[front_nearest]);
        outdoor_frames[front_nearest] = outdoor_frames.back();
        outdoor_frames.pop_back();
      } else {
        sorted_frames.push_back(outdoor_frames[back_nearest]);
        outdoor_frames[back_nearest] = outdoor_frames.back();
        outdoor_frames.pop_back();
      }
      front_nearest = -1;
      back_nearest = -1;
    } else if (front_absdiff < back_absdiff) {
      sorted_frames.push_front(outdoor_frames[front_nearest]);
      outdoor_frames[front_nearest] = outdoor_frames.back();
      outdoor_frames.pop_back();
      front_nearest = -1;
    } else {
      sorted_frames.push_back(outdoor_frames[back_nearest]);
      outdoor_frames[back_nearest] = outdoor_frames.back();
      outdoor_frames.pop_back();
      back_nearest = -1;
    }
  }
  return sorted_frames;
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
  const int threshold = 2e6;
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
    for (int i = 0; i < scenes.size() - 1; ++i) {
      if (scenes[i].empty()) {
        continue;
      }
      for (int j = i + 1; j < scenes.size(); ++j) {
        if (scenes[j].empty()) {
          continue;
        }
        for (int k = 0; k < scenes[i].size(); ++k) {
          for (int l = 0; l < scenes[j].size(); ++l) {
            if (ABSDIFF[scenes[i][k]][scenes[j][l]] < threshold) {
              scenes[i].insert(scenes[i].end(), scenes[j].begin(),
                               scenes[j].end());
              scenes[j].clear();
              scene_adjustment_flag = 1;
              break;
            }
            if (scene_adjustment_flag) {
              break;
            }
          }
        }
      }
    }
  }

  scene_adjustment_flag = 1;
  while (scene_adjustment_flag) {
    scene_adjustment_flag = 0;
    for (int i = 0; i != scenes.size(); ++i) {
      for (int j = 0; j != scenes.size(); ++j) {
        if (j == i || scenes[i].empty() || scenes[j].empty() ||
            scenes[j].size() > 25) {
          continue;
        }
        int min_scene = 0;
        int min_absdiff = 1e9;
        for (int k = 0; k != scenes[i].size(); ++k) {
          for (int l = 0; l != scenes[j].size(); ++l) {
            if (ABSDIFF[scenes[i][k]][scenes[j][l]] < min_absdiff) {
              min_absdiff = ABSDIFF[scenes[i][k]][scenes[j][l]];
              min_scene = i;
              scene_adjustment_flag = 1;
            }
          }
        }
        scenes[i].insert(scenes[i].end(), scenes[j].begin(), scenes[j].end());
        scenes[j].clear();
        break;
      }
    }
  }

  bool scene_remove_flag = 1;
  while (scene_remove_flag) {
    scene_remove_flag = 0;
    for (int i = 0; i != scenes.size(); ++i) {
      if (scenes[i].empty()) {
        scenes[i] = scenes.back();
        scenes.pop_back();
        scene_remove_flag = 1;
        break;
      }
    }
  }

  return scenes;
}
