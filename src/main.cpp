#include "common.hpp"
#include "frame.hpp"
#include "tools.hpp"

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
      indoor_frames.push_back(frames[i].number);
    } else {
      outdoor_frames.push_back(frames[i].number);
    }
  }

  cout << "Sorting Indoor Frames..." << endl;
  deque<int> indoor_sorted = SortFrames(indoor_frames);

  cout << "Sorting Outdoor Frames..." << endl;
  deque<int> outdoor_presorted = SortFrames(outdoor_frames);
}
