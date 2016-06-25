#include "tools.hpp"
#include <dirent.h>
#include <sys/types.h>

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

void InitSimilarity(const vector<Frame> &frames) {
  Mat t;
  for (int i = 0; i != frames.size() - 1; ++i) {
    for (int j = i + 1; j != frames.size(); ++j) {
      const Frame &f1 = frames[i], &f2 = frames[j];
      multiply(f1.resized, f2.resized, t);
      Scalar_<double> s = sum(t);
      SIMILARITY[f1.number][f2.number] = SIMILARITY[f2.number][f1.number] =
          (s.val[0] + s.val[1] + s.val[2]) /
          (f1.resized_norm * f2.resized_norm);
    }
  }
}

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
