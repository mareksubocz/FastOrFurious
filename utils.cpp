#ifndef UTILS
#define UTILS

#include <SFML/Network.hpp>
#include <array>
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define len(a) sizeof a / sizeof a[0];

// 22 bytes
struct Configuration {
  float frontFriction = .2;
  float sideFriction = .99;
  float acc = 0.5;
  float dec = 0.5;
  float rotationSpeed = 5.0;
  float maxVelocity = 1.;
  int timeout = 10;
  int framesPerAnswer = 1;
  int numOfPlayers = 4;
  array<sf::Vector2f, 3> checkpoints = {
      sf::Vector2f(rand()%900+50, rand()%900+50), sf::Vector2f(rand()%900+50, rand()%900+50), sf::Vector2f(rand()%900+50, rand()%900+50)};
  int numOfCheckpoints = checkpoints.size();

  friend sf::Packet &operator<<(sf::Packet &os, const Configuration &c) {
    os << c.frontFriction << c.sideFriction << c.acc << c.dec << c.rotationSpeed
       << c.maxVelocity << c.timeout << c.framesPerAnswer << c.numOfPlayers
       << c.numOfCheckpoints;
    for (auto &checkpoint : c.checkpoints) {
      os << checkpoint.x << checkpoint.y;
    }
    return os;
  }

  friend sf::Packet &operator>>(sf::Packet &os, Configuration &c) {
    os >> c.frontFriction >> c.sideFriction >> c.acc >> c.dec >>
        c.rotationSpeed >> c.maxVelocity >> c.timeout >> c.framesPerAnswer >>
        c.numOfPlayers >> c.numOfCheckpoints;
    for (auto &checkpoint : c.checkpoints) {
      os >> checkpoint.x >> checkpoint.y;
    }
    return os;
  }

  friend ostream &operator<<(ostream &os, const Configuration &c) {
    os << " " << c.frontFriction << " " << c.sideFriction << " " << c.acc << " "
       << c.dec << " " << c.rotationSpeed << " " << c.maxVelocity << " "
       << c.timeout << " " << c.framesPerAnswer << " " << c.numOfPlayers;
    for (auto &checkpoint : c.checkpoints) {
      os << checkpoint.x << checkpoint.y;
    }
    return os;
  }
};

// 28 bytes
struct PlayerState {
  sf::Vector2f pos;
  sf::Vector2f vel;
  float rotation;
  int lap;
  int checkpoint;
  int health;

  friend sf::Packet &operator<<(sf::Packet &os, const PlayerState &p) {
    os << p.pos.x << p.pos.y << p.vel.x << p.vel.y << p.rotation << p.lap
       << p.checkpoint << p.health;
    return os;
  }

  friend sf::Packet &operator>>(sf::Packet &os, PlayerState &p) {
    return os >> p.pos.x >> p.pos.y >> p.vel.x >> p.vel.y >> p.rotation >>
           p.lap >> p.checkpoint >> p.health;
  }

  friend ostream &operator<<(ostream &os, const PlayerState &p) {
    os << p.pos.x << ", " << p.pos.y << ", " << p.vel.x << ", " << p.vel.y << ", "
       << p.rotation << ", " << p.lap << ", " << p.checkpoint << ", " << p.health;
    return os;
  }
};

struct Response {
  int gas = 0;    // 0-100
  int rotate = 0; // {-1, 0, 1}
  bool boost = 0;

  friend sf::Packet &operator<<(sf::Packet &os, const Response &r) {
    os << r.gas << r.rotate << r.boost;
    return os;
  }

  friend sf::Packet &operator>>(sf::Packet &os, Response &r) {
    return os >> r.gas >> r.rotate >> r.boost;
  }

  friend ostream &operator<<(ostream &os, const Response &r) {
    os << r.gas << ", " << r.rotate << ", " << r.boost;
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
