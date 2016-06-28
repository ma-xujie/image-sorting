#include "scene.hpp"
#include "common.hpp"

// Scene 的具体实现，注释见 scene.hpp

Scene::Scene(const vector<int> &frames_, const vector<Frame> &all_frames_) {
  this->frames = frames_;
  this->all_frames = &all_frames_;
  if (this->IsReversed()) {
    reverse(this->frames.begin(), this->frames.end());
  }
}

Scene::Scene(const Scene &scene) {
  this->all_frames = scene.all_frames;
  this->frames = scene.frames;
}

void Scene::ConcatFront(const Scene &another_scene) {
  this->frames.insert(this->frames.begin(), another_scene.frames.begin(),
                      another_scene.frames.end());
}
void Scene::ConcatBack(const Scene &another_scene) {
  this->frames.insert(this->frames.end(), another_scene.frames.begin(),
                      another_scene.frames.end());
}
int Scene::BackAbsdiff(const Scene &another_scene) {
  return ABSDIFF[this->frames.back()][another_scene.frames.front()];
}
int Scene::FrontAbsdiff(const Scene &another_scene) {
  return ABSDIFF[this->frames.front()][another_scene.frames.back()];
}

bool Scene::BackPhaseContinue(Scene &another_scene, int n) {
  int phase_diff = all_frames->at(another_scene.frames.front()).phase -
                   all_frames->at(this->frames.back()).phase;
  return (0 <= phase_diff && phase_diff <= n) || phase_diff <= (n - 301);
}

bool Scene::FrontPhaseContinue(Scene &another_scene, int n) {
  return another_scene.BackPhaseContinue(*this, n);
}

bool Scene::IsReversed() {
  if (this->frames.size() <= 1) {
    return false;
  } else {
    int s = 0;
    for (int i = 0; i != this->frames.size() - 1; ++i) {
      int phase_diff =
          all_frames->at(frames[i + 1]).phase - all_frames->at(frames[i]).phase;
      s += (0 <= phase_diff && phase_diff < 5) || phase_diff < -295;
      s -= (0 <= (-phase_diff) && (-phase_diff) < 5) || (-phase_diff) < -295;
    }
    return s < 0;
  }
}
