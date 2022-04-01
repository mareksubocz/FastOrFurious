#ifndef UTILS
#define UTILS

#include <SFML/Network.hpp>
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define CLIP(x, d, u) min(max(x, d), u)
#define len(a) sizeof a / sizeof a[0];

enum PlayerType { human, bot };

// 28 bytes
struct Configuration {
  float frontFriction = .2;
  float sideFriction = .99;
  float acc = 0.5;
  float dec = 0.5;
  float rotationSpeed = 5.0;
  float maxVelocity = 1.;
  int timeout = 10;
  int framesPerAnswer = 1;
  int numOfPlayers = 3;

  friend sf::Packet &operator<<(sf::Packet &os, const Configuration &c) {
    os << c.frontFriction << c.sideFriction << c.acc << c.dec << c.rotationSpeed
       << c.maxVelocity << c.timeout << c.framesPerAnswer << c.numOfPlayers;
    return os;
  }

  friend sf::Packet &operator>>(sf::Packet &os, Configuration &c) {
    return os >> c.frontFriction >> c.sideFriction >> c.acc >> c.dec >>
           c.rotationSpeed >> c.maxVelocity >> c.timeout >> c.framesPerAnswer >>
           c.numOfPlayers;
  }

  friend ostream &operator<<(ostream &os, const Configuration &c) {
    os << " " << c.frontFriction << " " << c.sideFriction << " " << c.acc << " "
       << c.dec << " " << c.rotationSpeed << " " << c.maxVelocity << " "
       << c.timeout << " " << c.framesPerAnswer << " " << c.numOfPlayers;
    return os;
  }
};

// 28 bytes
struct PlayerState {
  float x;
  float y;
  float rotation;
  float xSpeed;
  float ySpeed;
  int lap;
  int checkpoint;

  friend sf::Packet &operator<<(sf::Packet &os, const PlayerState &p) {
    os << p.x << p.y << p.rotation << p.xSpeed << p.ySpeed << p.lap
       << p.checkpoint;
    return os;
  }

  friend sf::Packet &operator>>(sf::Packet &os, PlayerState &p) {
    return os >> p.x >> p.y >> p.rotation >> p.xSpeed >> p.ySpeed >> p.lap >>
           p.checkpoint;
  }

  friend ostream &operator<<(ostream &os, const PlayerState &p) {
    os << p.x << " " << p.y << " " << p.rotation << " " << p.xSpeed << " "
       << p.ySpeed << " " << p.lap << " " << p.checkpoint;
    return os;
  }
};

struct Response {
  int gas = 0;    // 0-100
  int rotate = 0; //{-1, 0, 1}
  bool boost = 0;

  friend sf::Packet &operator<<(sf::Packet &os, const Response &r) {
    os << r.gas << r.rotate << r.boost;
    return os;
  }

  friend sf::Packet &operator>>(sf::Packet &os, Response &r) {
    return os >> r.gas >> r.rotate >> r.boost;
  }

  friend ostream &operator<<(ostream &os, const Response &r) {
    os << r.gas << " " << r.rotate << " " << r.boost;
    return os;
  }
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
