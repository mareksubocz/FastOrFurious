#ifndef CAR
#define CAR

#include "algebra.cpp"
#include "utils.cpp"
#include <SFML/Graphics.hpp>
#include <math.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

#define dlugosc(v) sqrt(v.x *v.x + v.y * v.y)

/*
   A class containing information about the car, model to display
   and car physics

parameters:
  - x, y: starting position of the car
  - config: configuration of the game
  - smooth: whether to use smoothing or not
*/
class Car {
private:
  int lap = 0;
  int checkpoint = 0;
  unsigned short port;
  sf::Vector2f velocity;
  sf::Texture bodyTexture;
  sf::Sprite body;
  sf::CircleShape collider;
  Configuration config;

public:
  Car(double x, double y, Configuration config, bool smooth = true) {
    this->config = config;
    this->velocity = {0.0, 0.0};
    vector<string> carFolderPaths = getAllFilesInDirectory(
        (char *)"/Users/mareksubocz/it/FastOrFurious/img/PNG/Cars/");
    carFolderPaths.erase(carFolderPaths.begin()); // remove first element
    this->bodyTexture.loadFromFile(
        "/Users/mareksubocz/it/FastOrFurious/img/PNG/Cars/" +
        string(carFolderPaths[rand() % carFolderPaths.size()]) + "/01.png");
    this->bodyTexture.setSmooth(smooth);
    this->body.setTexture(bodyTexture);
    this->body.setScale((double)0.1 / this->body.getScale().x,
                        (double)0.1 / this->body.getScale().y);
    this->body.setOrigin(this->body.getTextureRect().width / 2,
                         this->body.getTextureRect().height / 2);
    this->body.setPosition(x, y);
    this->collider.setRadius(40);
    this->collider.setFillColor(sf::Color(255, 0, 0, 100));
    this->collider.setOrigin(this->collider.getRadius(),
                             this->collider.getRadius());
  }

  void turn(double rotation) {
    this->body.rotate(rotation * dlugosc(this->velocity) * 0.4);
    // sf::Transform t = this->body.getTransform();
    // sf::Vector2f rotAxis = {this->getPosition().x, this->getPosition().y/2};
    // t.rotate(rotation * dlugosc(this->velocity) * 0.4, rotAxis);
    // this->body.setPosition(this->getPosition() +
    // t.transformPoint(this->body.getOrigin() + this->getPosition()));
  }

  void move(bool gas) {
    float y = 3.5 * gas;
    sf::Vector2f diff = sf::Vector2f(0., y);
    sf::Vector2f diffRotated =
        this->body.getPosition() -
        this->body.getTransform().transformPoint(this->body.getOrigin() + diff);
    this->velocity += rotateToShape(diff, this->body);
    this->body.move(velocity);
    if (this->body.getPosition().x <= 0 or this->body.getPosition().x > 1000) {
      this->setVelocity(
          sf::Vector2f(-this->getVelocity().x, this->getVelocity().y));
    }
    if (this->body.getPosition().y <= 0 or this->body.getPosition().y > 1000) {
      this->setVelocity(
          sf::Vector2f(this->getVelocity().x, -this->getVelocity().y));
    }
    this->body.setPosition(CLIP((double)this->body.getPosition().x, 0., 1000.),
                           CLIP((double)this->body.getPosition().y, 0., 1000.));
  }

  // TODO: convert part of lateral force to vertical force
  void updateState() {
    this->velocity.x *= 0.97;
    this->velocity.y *= 0.97;
    this->collider.setPosition(this->getBody().getPosition());
  }

  void display(sf::RenderWindow &window) {
    window.draw(this->body);
    window.draw(this->collider);
  }

  void handleResponse(Response response) {
    move(response.gas);
    turn(response.rotate);
    updateState();
  }

  PlayerState getPlayerState() {
    PlayerState state;
    state.pos = this->body.getPosition();
    state.rotation = this->body.getRotation();
    state.vel = this->velocity;
    state.lap = this->lap;
    state.checkpoint = this->checkpoint;
    return state;
  }

  static void handleCollisions(vector<Car *> &cars,
                               vector<PlayerState> &gameState,
                               Configuration &config) {
    for (int i = 0; i < config.numOfPlayers; i++) {
      for (int j = i + 1; j < config.numOfPlayers; j++) {
        // check if collision happened
        float dist = distance(gameState[i].pos, gameState[j].pos);
        if (dist < 80) { // collision happened
          sf::Vector2f v1d =
              dot(gameState[i].vel - gameState[j].vel,
                  gameState[i].pos - gameState[j].pos) /
              (float)pow(distance(gameState[i].pos, gameState[j].pos), 2) *
              (gameState[i].pos - gameState[j].pos);
          sf::Vector2f v2d =
              dot(gameState[j].vel - gameState[i].vel,
                  gameState[j].pos - gameState[i].pos) /
              (float)pow(distance(gameState[j].pos, gameState[i].pos), 2) *
              (gameState[j].pos - gameState[i].pos);
          float diffx = gameState[i].pos.x - gameState[j].pos.x;
          float diffy = gameState[i].pos.y - gameState[j].pos.y;
          cars[i]->setPosition(cars[i]->getBody().getPosition().x -
                                   0.5 * (dist - 80) * diffx / dist,
                               cars[i]->getBody().getPosition().y -
                                   0.5 * (dist - 80) * diffy / dist);
          cars[j]->setPosition(cars[j]->getBody().getPosition().x +
                                   0.5 * (dist - 80) * diffx / dist,
                               cars[j]->getBody().getPosition().y +
                                   0.5 * (dist - 80) * diffy / dist);

          cars[i]->setVelocity(cars[i]->getVelocity() - v1d);
          cars[j]->setVelocity(cars[j]->getVelocity() - v2d);
        }
      }
    }
  }

  // TODO: car position is center, rotate using transform.rotate(angle, center);

  sf::Sprite getBody() { return this->body; }
  unsigned short getPort() { return this->port; }
  sf::Vector2f getVelocity() { return this->velocity; }
  void setPosition(float x, float y) { this->body.setPosition(x, y); }
  void setPort(unsigned short port) { this->port = port; }
  void setVelocity(sf::Vector2f velocity) { this->velocity = velocity; }
};

#endif
