#ifndef UTILS
#define UTILS

#include <string>
#include <vector>
#include <dirent.h>
#include <iostream>
using namespace std;

#define CLIP(x, d, u) min(max(x, d), u)
#define len(a) sizeof a / sizeof a[0];

struct configuration {
  string pathCarModels = "";
  string pathBackgrounds = "./img/PNG/Background_Tiles/";
  string pathBoost = "";
  double frontFriction = .2;
  double sideFriction = .99;
  double acc = 0.5;
  double dec = 0.5;
  double rotationSpeed = 5.0;
  double maxVelocity = 1.;
};

vector<string> getAllFilesInDirectory(char *path) {
  DIR *dir;
  struct dirent *diread;
  vector<string> files;

  if ((dir = opendir(path)) != nullptr) {
    while ((diread = readdir(dir)) != nullptr) {
      files.push_back(diread->d_name);
    }
    closedir(dir);
  } else {
    perror("opendir");
    cout << "Folder " << path << " not found." << endl;
    return files;
  }
  files.erase(files.begin()); // . directory
  files.erase(files.begin()); // .. directory
  return files;
}

#endif
