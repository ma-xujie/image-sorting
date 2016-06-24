#include "common.hpp"
#include "tools.hpp"

using namespace std;
using namespace cv;

int main(int argc, char const *argv[]) {
  if (argc < 1) {
    cout << "ERROR: No input argument" << endl;
    cout << "USEAGE: image-sorting IMAGE_DIR_PATH" << endl;
    return 0;
  }
  const char *dir = argv[1];
  vector<string> images_filename = ListDir(dir);

  for (string filename : images_filename) {
    if (EndsWith(".jpg", filename)) {
      string image_path = string(dir) + "/" + filename;
      Mat image = imread(image_path.c_str());
      imshow("image-sorting", image);
      waitKey(0);
    }
  }
}
