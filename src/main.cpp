#include "common.hpp"
#include "frame.hpp"
#include "scene.hpp"
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
  // 实际上计算是否为演播室场景在 Frame 类
  // 的构造函数中就完成了，这里只是单纯将它们收集起来
  vector<int> indoor_frames, outdoor_frames;
  for (int i = 0; i != frames.size(); ++i) {
    if (frames[i].isIndoor) {
      indoor_frames.push_back(i);
    } else {
      outdoor_frames.push_back(i);
    }
  }

  cout << "Sorting Indoor Frames..." << endl;
  // 给演播室的场景排序
  deque<int> indoor_sorted = SortFrames(indoor_frames);

  cout << "Sorting Outdoor Frames..." << endl;
  // 完成分镜头（Scene）和镜头内排序的工作
  deque<Scene> outdoor_sorted = SortOutdoorFrames(outdoor_frames, frames);

  cout << "Writing Results..." << endl;

  // (a)
  ofstream f11("11.txt", ios_base::out);
  ofstream f12("12.txt", ios_base::out);
  for (int i = 0; i != frames.size(); ++i) {
    if (frames[i].isIndoor) {
      f11 << frames[i].number << endl;
    } else {
      f12 << frames[i].number << endl;
    }
  }
  f11.close();
  f12.close();

  // (b)
  ofstream f2("2.txt", ios_base::out);
  for (auto i : indoor_sorted) {
    f2 << frames[i].number << endl;
  }
  f2.close();

  // (c)
  ofstream f3("3.txt", ios_base::out);
  int scene_number = 1;
  for (auto &scene : outdoor_sorted) {
    for (auto i : scene.frames) {
      f3 << frames[i].number << ", " << scene_number << endl;
    }
    ++scene_number;
  }
  f3.close();

  cout << "Done!" << endl;

  cout << "Show All Sorted Frames?(y/N)";
  string choice;
  cin >> choice;
  if (choice == "y" || choice == "Y") {
    cout << "Scene Indoor" << endl;
    ifstream f4("2.txt", ios_base::in);
    string file_number;
    while (f4 >> file_number) {
      string filepath(dir);
      filepath = filepath + "/" + file_number + ".jpg";
      auto t = imread(filepath.c_str());
      imshow("I Love Signal & System", t);
      waitKey(40);
    }
    f4.close();

    cout << "Scene Outdoor" << endl;
    ifstream f5("3.txt", ios_base::in);
    string scene;
    while (f5 >> file_number >> scene) {
      file_number.pop_back();
      string filepath(dir);
      filepath = filepath + "/" + file_number + ".jpg";
      auto t = imread(filepath.c_str());
      imshow("I Love Signal & System", t);
      waitKey(40);
    }
    f5.close();
  }
}
