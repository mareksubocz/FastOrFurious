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
  int lap = 0;
  int checkpoint = 0;
  unsigned short port;
  sf::Vector2f velocity;
  sf::Texture bodyTexture;
  sf::Sprite body;
  Configuration config;

public:
  Car(double x, double y, Configuration config,
      bool smooth = false) {
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

  void turn(double rotation) {
    this->body.rotate(rotation * config.rotationSpeed);
  }

  void moveBody(bool gas) {
    sf::Vector2f diff = sf::Vector2f(0., -100*gas);
    this->body.setPosition(this->body.getTransform().transformPoint(
        this->body.getOrigin() + diff));
    this->body.setPosition(CLIP((double)this->body.getPosition().x, 0., 1000.),
                           CLIP((double)this->body.getPosition().y, 0., 1000.));
  }

  void handleResponse(Response response){
    moveBody(response.gas);
    turn(response.rotate);
  }

  PlayerState getPlayerState(){
    PlayerState state;
    state.x = this->body.getPosition().x;
    state.y = this->body.getPosition().y;
    state.rotation = this->body.getRotation();
    state.xSpeed = this->velocity.x;
    state.ySpeed = this->velocity.y;
    state.lap = this->lap;
    state.checkpoint = this->checkpoint;
    return state;
  }

  sf::Sprite getBody() { return this->body; }
  unsigned short getPort() { return this->port; }
  void setPort(unsigned short port) { this->port = port; }
};

#endif
