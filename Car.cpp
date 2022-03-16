#ifndef CAR
#define CAR

#include "utils.cpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Car {
private:
  int version;
  unsigned short port;
  string name;
  sf::Vector2f velocity;
  sf::Texture bodyTexture;
  sf::Sprite body;
  sf::Keyboard::Key *keys;
  configuration config;

public:
  Car(double x, double y, sf::Keyboard::Key keys[], configuration config,
      bool smooth = false) {
    this->keys = keys;
    this->config = config;
    this->velocity = {0.0, 0.0};
    vector<string> carFolderPaths =
        getAllFilesInDirectory((char *)"./img/PNG/Cars/");
    carFolderPaths.erase(carFolderPaths.begin());
    int carNumber = rand() % carFolderPaths.size();
    this->bodyTexture.loadFromFile(
        "./img/PNG/Cars/" + string(carFolderPaths[carNumber]) + "/01.png");
    this->bodyTexture.setSmooth(true);
    this->body.setTexture(bodyTexture);
    this->body.setScale((double)0.1 / this->body.getScale().x,
                        (double)0.1 / this->body.getScale().y);
    this->body.setOrigin(this->body.getTextureRect().width / 2,
                         this->body.getTextureRect().height / 2);
    this->body.setPosition(x, y);
  }

  void checkMove() {
    if (sf::Keyboard::isKeyPressed(this->keys[0])) {
      this->moveBody(100.0);
    }
    if (sf::Keyboard::isKeyPressed(this->keys[1])) {
      this->moveBody(-5.0);
    }
    if (sf::Keyboard::isKeyPressed(this->keys[2])) {
      this->turn(-1.0);
    }
    if (sf::Keyboard::isKeyPressed(this->keys[3])) {
      this->turn(1.0);
    }
  }

  void turn(double rotation) {
    this->body.rotate(rotation * config.rotationSpeed);
  }

  void moveBody(double speed) {
    sf::Vector2f diff = sf::Vector2f(0., -speed);
    this->body.setPosition(this->body.getTransform().transformPoint(
        this->body.getOrigin() + diff));
    this->body.setPosition(CLIP((double)this->body.getPosition().x, 0., 1000.),
                           CLIP((double)this->body.getPosition().y, 0., 1000.));
  }

  sf::Sprite getBody() { return this->body; }
  unsigned short getPort() { return this->port; }
  void setPort(unsigned short port) { this->port = port; }
  string getName() { return this->name; }
};

#endif
