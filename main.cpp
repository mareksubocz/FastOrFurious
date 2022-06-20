#include "Car.cpp"
#include "Client.cpp"
#include "Server.cpp"
#include "algebra.cpp"
#include "utils.cpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <dirent.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::string;
using std::vector;

/*TODO: list
  - save states and responses to csv

  - send DEAD to dead client
  - convert part of lateral force to vertical force
  - add boost and animation
  - use delta time NOT NEEDED?
*/

// class GameController {
//   // TODO: get closest car should belong here, as well as handling collisions
// private:
//   bool gameEnded = false;
//   int winnernum = -1;
//   int playersLeft;
//   Configuration config;
//   Server server;
//   string basePath;
//   sf::Font endFont;
//   sf::Text endText;
//   sf::Sprite background;
//   sf::RenderWindow window;
//   vector<pid_t> clientIDs;
//   vector<std::unique_ptr<Car>> cars;
//
//
// public:
//   GameController(Configuration config, char *argv[0])
//   : window(sf::VideoMode(1000,1000), "Fast or Furious"), server(this->config) {
//     this->basePath = string(argv[0]);
//     this->basePath = this->basePath.substr(0, this->basePath.find_last_of("/") + 1);
//     this->endFont.loadFromFile(basePath + "/font.ttf");
//     this->config = config;
//     this->clientIDs = this->server.spawnClients({
//         //TODO: move this to config
//         Version::human
//         }, basePath);
//
//     sf::Texture backgroundTexture;
//     backgroundTexture.loadFromFile(this->basePath +
//                                    "/img/PNG/Background_Tiles/Soil_Tile.png");
//     this->background.setTexture(backgroundTexture);
//     this->background.setScale(
//         (double)window.getSize().x / backgroundTexture.getSize().x,
//         (double)window.getSize().y / backgroundTexture.getSize().y
//         );
//     this->playersLeft = this->config.numOfPlayers;
//   }
//
//   void start(){
//     this->server.waitForConnections();
//     while(window.isOpen()){
//
//     }
//   }
//
// private:
//   void draw(){
//
//   }
// };
PlayerState getClosest(PlayerState p, vector<PlayerState> &gameState){
  float minDist = INFINITY;
  PlayerState closest;
  for (auto & player : gameState){
    float dist = distance(p.pos, player.pos);
    if (dist < 0.1) continue;
    if (dist < minDist){
      minDist = dist;
      closest = player;
    }
  }
  return closest;
}

void this_is_the_end(int &winnerNum, vector<std::unique_ptr<Car>> &cars,
                     sf::RenderWindow &window, sf::Sprite &background,
                     string &path) {
  sf::Font font;
  font.loadFromFile(path + "/font.ttf");
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(100);
  text.setStyle(sf::Text::Bold | sf::Text::Underlined);
  text.setFillColor(cars[winnerNum]->getBody().getColor());
  text.setString("Player " + to_string(winnerNum + 1) + " won!");
  sf::FloatRect textRect = text.getLocalBounds();
  text.setPosition(window.getView().getCenter() -
                   sf::Vector2f(textRect.width / 2, textRect.height / 2));
  background.setColor(sf::Color(100, 100, 100));
  window.draw(text);
}

int main(int argc, char *argv[0]) {
  srand(time(NULL));
  vector<pid_t> clientIDs; // macOS

  string base_path = string(argv[0]);
  base_path = base_path.substr(0, base_path.find_last_of("/") + 1);
  int winnerNum = -1;

  try {
    const Configuration config;

    vector<PlayerState> gameState(config.numOfPlayers);
    vector<std::unique_ptr<Car>> cars;
    for (int i = 0; i < config.numOfPlayers; i++) {
      cars.push_back(
          std::make_unique<Car>(200 + 100 * i, 800, config, base_path));
    }

    Server server = Server(config);

    // spawn clients macOS
    clientIDs = server.spawnClients(
        {
        Version::human,
        Version::simple,
        Version::simple,
        },
        base_path);
    server.waitForConnections();

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Fast or Furious");

    // draw background
    sf::Texture backgroundTexture;
    sf::Sprite background;
    backgroundTexture.loadFromFile(base_path +
                                   "/img/PNG/Background_Tiles/Soil_Tile.png");
    background.setTexture(backgroundTexture);
    background.setScale(
        (double)window.getSize().x / backgroundTexture.getSize().x,
        (double)window.getSize().y / backgroundTexture.getSize().y);

    // main loop
    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
          window.close();
      }
      window.draw(background);

      for (int i = 0; i < config.numOfPlayers; i++) {
        // if (cars[i]->getHealth() <= 0) continue;
        gameState[i] = cars[i]->getPlayerState();
      }
      server.sendGameState(gameState);
      server.waitForResponse();
      vector<Response> playerResponses = server.getPlayerResponses();

      //TODO: save gamestate and player responses
      string filename("test.csv");
      ofstream file_out;
      file_out.open(filename, std::ios_base::app);
      PlayerState closest = getClosest(gameState[0], gameState);
      sf::Vector2f relativePos = closest.pos - gameState[0].pos;
      sf::Vector2f relativeVel = closest.vel - gameState[0].vel;
      float relativeRot = vectorRotation(relativePos) - gameState[0].rotation;
      sf::Vector2f relativeChkptPos = config.checkpoints[gameState[0].checkpoint] - gameState[0].pos;
      float relativeChkptRot = vectorRotation(relativeChkptPos) - gameState[0].rotation;
      file_out << gameState[0]       << ", "
               << relativePos.x      << ", "
               << relativePos.y      << ", "
               << relativeVel.x      << ", "
               << relativeVel.y      << ", "
               << relativeRot        << ", "
               << closest.health     << ", "
               << relativeChkptPos.x << ", "
               << relativeChkptPos.y << ", "
               << relativeChkptRot   << ", "
               << playerResponses[0] << endl;

      Car::handleCollisions(cars, config);
      Car::handleCheckpoints(cars, config );
      for (int i = 0; i < config.numOfPlayers; i++) {
        if (cars[i]->getHealth() <= 0) continue;
        cars[i]->handleResponse(playerResponses[i]);
        cars[i]->display(window);
      }

      sf::CircleShape circleShape;
      sf::CircleShape playerCircle;
      playerCircle.setRadius(10);
      playerCircle.setOrigin(10, 10);
      circleShape.setFillColor(sf::Color(0, 150, 0, 100));
      circleShape.setRadius(50);
      circleShape.setOrigin(50, 50);
      for (int i = 0; i < config.numOfCheckpoints; i++) {
        circleShape.setPosition(config.checkpoints[i]);
        window.draw(circleShape);
        for (int j = 0; j < config.numOfPlayers; j++) {
          if (cars[j]->getHealth() <= 0) continue;
          if (gameState[j].checkpoint == i) {
            playerCircle.setFillColor(cars[j]->getBody().getColor());
            playerCircle.setPosition(
                config.checkpoints[i] +
                sf::Vector2f(15 * (2 * j - (config.numOfPlayers - 1)), 60));
            window.draw(playerCircle);
          }
        }
      }

      // check if there's a winner
      int playersLeftCounter = 0;
      int playerLeftNum = -1;
      if (winnerNum == -1) {
        for (int i = 0; i < config.numOfPlayers; i++){
          if (cars[i]->getHealth() > 0){
            playersLeftCounter++;
            playerLeftNum = i;
          }
        }
        if (playersLeftCounter < 2){
          winnerNum = playerLeftNum;
        }

        for (int i = 0; i < config.numOfPlayers; i++) {
          if (cars[i]->getHealth() <= 0) { continue; }
          if (cars[i]->getLap() >= 3) {

            this_is_the_end(i, cars, window, background, base_path);
            winnerNum = i;
          }
        }
      } else {
        this_is_the_end(winnerNum, cars, window, background, base_path);
      }

      window.display();
    }

    // clear auto-spawned clients
    for (auto &id : clientIDs) { // macOS
      kill(-id, SIGKILL);
    }
  } catch (const std::exception &ex) {
    cout << ex.what() << endl;

    for (auto &id : clientIDs) { // macOS
      kill(-id, SIGKILL);
    }
  }

  return 0;
}
