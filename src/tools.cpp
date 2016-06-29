#include "tools.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <vector>

vector<string> ListDir(const char *path) {
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

// 找到 range 容器中与 src 之间相似度最高的一帧
int NearestFrameIndex(int src, const vector<int> &range) {
  double min_absdiff = 1e9;
  int nearest_index = 0;
  for (int i = 0; i != range.size(); ++i) {
    if (ABSDIFF[src][range[i]] < min_absdiff) {
      min_absdiff = ABSDIFF[src][range[i]];
      nearest_index = i;
    }
  }
  return nearest_index;
}

// 用贪心方法给输入容器中的帧排序
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

// 计算一个帧序列中所有相邻两帧之间绝对差的平均值（听起来很绕对吧，不过代码很简单哦）
int AverageAbsdiff(const vector<int> &frames) {
  if (frames.size() <= 1) {
    return 6e5;
  } else {
    int s = 0;
    for (int i = 0; i != frames.size() - 1; ++i) {
      s += ABSDIFF[frames[i]][frames[i + 1]];
    }
    return s / (frames.size() - 1);
  }
}

// 分镜头！输入参数 presorted_frames 是一个分段连续的帧序列（直接用所有非演播室
// 场景贪心排序得到），然后从头开始扫描这个序列，如果相邻两个帧之间的相似度大于一个
// 动态的阈值的话，就认为它们在同一个镜头中，否则将它们拆分为两个镜头。在报告中会详
// 细叙述这个过程
vector<Scene> SplitScenes(deque<int> presorted_frames,
                          const vector<Frame> &all_frames) {
  vector<Scene> scenes;
  vector<int> cur_scene;
  for (int frame : presorted_frames) {
    if (cur_scene.empty() ||
        ABSDIFF[frame][cur_scene.back()] < 2 * AverageAbsdiff(cur_scene)) {
      cur_scene.push_back(frame);
    } else {
      scenes.emplace_back(Scene(cur_scene, all_frames));
      cur_scene.clear();
    }
  }
  return scenes;
}

// 镜头合并！容器 scenes 中有很多零碎的场景，它们可能属于同一个镜头，只是在最开
// 始的处理过程中没有放在一起，于是用这个函数让它们在一起！参数 th 帮助确定两个
// 镜头是否足够相似，参数 phase_bias 决定要拼接的相邻两个场景之间的最大相位偏差
vector<Scene> ReduceScenes(vector<Scene> scenes, int th, int phase_bias) {
  bool scenes_reduce_flag = 1;
  while (scenes_reduce_flag) {
    scenes_reduce_flag = 0;
    for (int i = 0; i != scenes.size() - 1; ++i) {
      auto &scene1 = scenes[i];
      Scene *next_scene = nullptr;
      int min_absdiff = 1e9;
      bool add_back;
      for (int j = i + 1; j != scenes.size(); ++j) {
        auto &scene2 = scenes[j];
        if (scene1.frames.empty() || scene2.frames.empty()) {
          continue;
        }
        // 相似度（绝对差）的阈值为 th * (两个场景的平均绝对差之和)
        int threshold = th * (AverageAbsdiff(scene1.frames) +
                              AverageAbsdiff(scene2.frames));
        if (scene1.BackPhaseContinue(scene2, phase_bias) &&
            scene1.BackAbsdiff(scene2) < threshold) {
          if (scene1.BackAbsdiff(scene2) < min_absdiff) {
            min_absdiff = scene1.BackAbsdiff(scene2);
            next_scene = &scene2;
            add_back = true;
          }
        } else if (scene1.FrontPhaseContinue(scene2, phase_bias) &&
                   scene1.FrontAbsdiff(scene2) < threshold) {
          if (scene1.FrontAbsdiff(scene2) < min_absdiff) {
            min_absdiff = scene1.FrontAbsdiff(scene2);
            next_scene = &scene2;
            add_back = false;
          }
        }
      }
      if (next_scene) {
        if (add_back) {
          scene1.ConcatBack(*next_scene);
          next_scene->frames.clear();
          scenes_reduce_flag = 1;
        } else {
          scene1.ConcatFront(*next_scene);
          next_scene->frames.clear();
          scenes_reduce_flag = 1;
        }
      }
    }
  }

  vector<Scene> scenes_reduced;
  for (auto &scene : scenes) {
    if (!scene.frames.empty()) {
      scenes_reduced.emplace_back(scene);
    }
  }
  return scenes_reduced;
}

// 给场景排序！这一步就比较轻松啦，考虑场景之间的相位关系，尽量将相位上连续的放在一起就好咯
deque<Scene> SortScenes(vector<Scene> scenes) {
  deque<Scene> sorted_scenes(1, scenes.back());
  scenes.pop_back();
  while (!scenes.empty()) {
    bool continue_search = 1;
    int k = 1;
    while (continue_search) {
      continue_search = 1;
      for (int i = 0; i != scenes.size(); ++i) {
        if (sorted_scenes.front().FrontPhaseContinue(scenes[i], k)) {
          sorted_scenes.push_front(scenes[i]);
          scenes[i] = scenes.back();
          continue_search = 0;
          break;
        }
        if (sorted_scenes.back().BackPhaseContinue(scenes[i], k)) {
          sorted_scenes.push_back(scenes[i]);
          scenes[i] = scenes.back();
          continue_search = 0;
          break;
        }
      }
      k++;
    }
    scenes.pop_back();
  }
  return sorted_scenes;
}

// 给非演播室场景排序的主程序
deque<Scene> SortOutdoorFrames(vector<int> outdoor_frames,
                               const vector<Frame> &frames) {
  // 先用贪心方法将所有非演播室场景排序
  deque<int> presorted_frames = SortFrames(outdoor_frames);

  // 然后对分段连续的帧序列进行拆分
  vector<Scene> presplited_scenes = SplitScenes(presorted_frames, frames);

  // 对过度拆分的场景进行合并
  int dd = 1, dp = 3;
  vector<Scene> scenes_reduced = ReduceScenes(presplited_scenes, dd, dp);
  while (scenes_reduced.size() > 70 && dd < 5) { // 改变参数多合并几次
    scenes_reduced = ReduceScenes(scenes_reduced, ++dd, ++dp);
  }

  // 最后给场景之间排序就好啦
  deque<Scene> sorted_scenes = SortScenes(scenes_reduced);

  return sorted_scenes;
}
