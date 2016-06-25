#include "common.hpp"
#include "frame.hpp"
#include "tools.hpp"
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[]) {
  if (argc < 1) {
    cout << "ERROR: No input argument" << endl;
    cout << "USEAGE: image-sorting IMAGE_DIR_PATH" << endl;
    return 0;
  }

  Init();

  cout << "Loading Images..." << endl;
  const char *dir = argv[1];
  vector<string> images_filename = ListDir(dir);
  vector<Frame> frames;

  for (string filename : images_filename) {
    if (EndsWith(".jpg", filename) || EndsWith(".jpeg", filename)) {

      string image_path = string(dir) + "/" + filename;
      frames.emplace_back(image_path.c_str());
    }
  }

  cout << "Calculating Absdiff..." << endl;
  InitAbsdiff(frames);

  cout << "Sorting Frames..." << endl;
  vector<int> indoor_frames, outdoor_frames;
  for (int i = 0; i != frames.size(); ++i) {
    if (frames[i].isIndoor) {
      indoor_frames.push_back(i);
    } else {
      outdoor_frames.push_back(i);
    }
  }

  cout << "Sorting Indoor Frames..." << endl;
  deque<int> indoor_sorted = SortFrames(indoor_frames);

  cout << "Sorting Outdoor Frames..." << endl;
  deque<int> outdoor_presorted = SortFrames(outdoor_frames);
  deque<vector<int>> outdoor_sorted =
      OutdoorFramesReorder(outdoor_presorted, frames);

  cout << "Writing Results..." << endl;

  // (a)
  ofstream f11("A11.txt", ios_base::out);
  ofstream f12("A12.txt", ios_base::out);
  for (int i = 0; i != frames.size(); ++i) {
    if (frames[i].isIndoor) {
      f11 << frames[i].filename << endl;
    } else {
      f12 << frames[i].filename << endl;
    }
  }
  f11.close();
  f12.close();

  // (b)
  ofstream f2("A2.txt", ios_base::out);
  for (auto i : indoor_sorted) {
    f2 << frames[i].filename << endl;
  }
  f2.close();

  // (c)
  ofstream f3("A3.txt", ios_base::out);
  int scene_number = 1;
  for (auto &scene : outdoor_sorted) {
    for (auto i : scene) {
      f3 << frames[i].filename << ' ' << scene_number << endl;
      cout << scene_number << endl;
      imshow("haha", frames[i].raw);
      waitKey(0);
    }
    ++scene_number;
  }
  f3.close();
}
