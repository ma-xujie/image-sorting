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
