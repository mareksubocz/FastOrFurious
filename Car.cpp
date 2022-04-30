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
  sf::Vector2f vel;
  sf::Texture bodyTexture;
  sf::Sprite body;
  sf::CircleShape collider;
  Configuration config;

public:
  Car(double x, double y, const Configuration &config, bool smooth = true) {
    this->config = config;
    this->vel = {0.0, 0.0};
    // vector<string> carFolderPaths = getAllFilesInDirectory(
    //     (char *)"/Users/mareksubocz/it/FastOrFurious/img/PNG/Cars/");
    // carFolderPaths.erase(carFolderPaths.begin()); // remove first element
    this->bodyTexture.loadFromFile(
        "/Users/mareksubocz/it/FastOrFurious/img/PNG/Cars/Yellow/01.png");
    this->bodyTexture.setSmooth(smooth);
    this->body.setTexture(bodyTexture);
    this->body.setColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
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
    sf::Vector2f axis = this->body.getTransform().transformPoint(
        this->body.getOrigin() + sf::Vector2f(0, -300));
    sf::Transform t;
    // t.rotate(rotation * dlugosc(this->velocity)* 0.3,axis);
    t.rotate(rotation * (1 + 0.3 * (dlugosc(this->vel))), axis);
    t *= this->body.getTransform();
    this->body.setPosition(t.transformPoint(this->body.getOrigin()));
    // this->body.rotate(rotation * dlugosc(this->velocity) * 0.3);
    this->body.rotate(rotation * (1 + 0.3 * dlugosc(this->vel)));
  }

  void move(float gas) {
    gas = clip(gas, 0.f, 100.f);
    gas /= 100;
    float y = 2.0 * gas;
    sf::Vector2f diff = sf::Vector2f(0., y);
    sf::Vector2f diffRotated =
        this->body.getPosition() -
        this->body.getTransform().transformPoint(this->body.getOrigin() + diff);
    this->vel += rotateToShape(diff, this->body);
    this->body.move(vel);
    if (this->body.getPosition().x <= 0 or this->body.getPosition().x > 1000) {
      this->setVelocity(
          sf::Vector2f(-this->getVelocity().x, this->getVelocity().y));
    }
    if (this->body.getPosition().y <= 0 or this->body.getPosition().y > 1000) {
      this->setVelocity(
          sf::Vector2f(this->getVelocity().x, -this->getVelocity().y));
    }
    this->body.setPosition(clip((double)this->body.getPosition().x, 0., 1000.),
                           clip((double)this->body.getPosition().y, 0., 1000.));
  }

  void updateState() {
    this->vel.x *= 0.97;
    this->vel.y *= 0.97;
    this->collider.setPosition(this->getBody().getPosition());
  }

  void display(sf::RenderWindow &window) {
    window.draw(this->body);
    // window.draw(this->collider);
  }

  void handleResponse(Response response) {
    if (this->lap > 0){
      sf::Vector2f relVec = this->config.checkpoints[this->checkpoint] - this->getBody().getPosition();
      relVec.y = -relVec.y;
    }
    move(response.gas);
    turn(response.rotate);
    updateState();
  }

  PlayerState getPlayerState() {
    PlayerState state;
    state.pos = this->body.getPosition();
    state.rotation = this->body.getRotation();
    state.vel = this->vel;
    state.lap = this->lap;
    state.checkpoint = this->checkpoint;
    return state;
  }

  static void handleCollisions(vector<Car *> &cars,
                               const Configuration &config) {
    for (int i = 0; i < config.numOfPlayers; i++) {
      for (int j = i + 1; j < config.numOfPlayers; j++) {
        // check if collision happened
        float dist = distance(cars[i]->getBody().getPosition(), cars[j]->getBody().getPosition());
        if (dist < 80) { // collision happened
          sf::Vector2f v1d =
              dot(cars[i]->vel - cars[j]->vel,
                  cars[i]->getBody().getPosition() - cars[j]->getBody().getPosition()) /
              (float)pow(distance(cars[i]->getBody().getPosition(), cars[j]->getBody().getPosition()), 2) *
              (cars[i]->getBody().getPosition() - cars[j]->getBody().getPosition());
          sf::Vector2f v2d =
              dot(cars[j]->vel - cars[i]->vel,
                  cars[j]->getBody().getPosition() - cars[i]->getBody().getPosition()) /
              (float)pow(distance(cars[j]->getBody().getPosition(), cars[i]->getBody().getPosition()), 2) *
              (cars[j]->getBody().getPosition() - cars[i]->getBody().getPosition());
          float diffx = cars[i]->getBody().getPosition().x - cars[j]->getBody().getPosition().x;
          float diffy = cars[i]->getBody().getPosition().y - cars[j]->getBody().getPosition().y;
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
          sf::Vector2f forcei = cars[i]->vel - cars[i]->getVelocity();
          sf::Vector2f forcej = cars[j]->vel - cars[j]->getVelocity();
        }
      }
    }
  }

  static void handleCheckpoints(vector<Car *> &cars,
                                const Configuration &config) {
    for (int i = 0; i<config.numOfPlayers; i++){
      if (distance(cars[i]->getBody().getPosition(),config.checkpoints[cars[i]->checkpoint]) < 50){
        cars[i]->checkpoint += 1;
        if (cars[i]->checkpoint >= config.numOfCheckpoints){
          cars[i]->checkpoint %= config.numOfCheckpoints;
          cars[i]->lap += 1;
        }
      }
    }
  }

  sf::Sprite getBody() { return this->body; }
  unsigned short getPort() { return this->port; }
  sf::Vector2f getVelocity() { return this->vel; }
  int getLap() { return this->lap; }
  void setPosition(float x, float y) { this->body.setPosition(x, y); }
  void setPort(unsigned short port) { this->port = port; }
  void setVelocity(sf::Vector2f velocity) { this->vel = velocity; }
};

#endif
