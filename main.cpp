#include "Car.cpp"
#include "Client.cpp"
#include "Server.cpp"
#include "utils.cpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <dirent.h>
#include <filesystem>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <signal.h>
#include <vector>

#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::string;
using std::vector;

Configuration config;

/*TODO:
- use delta time
- add boost and animation
- car collisions
- car damage
*/

int main(int argc, char *argv[0]) {
  srand(time(NULL));

  PlayerState gameState[config.numOfPlayers];

  vector<Car *> cars;
  for (int i = 0; i < config.numOfPlayers; i++) {
    Car *car = new Car(200 + 200 * i, 200 + 200 * i, config);
    cars.push_back(car);
  }

  Server server = Server(config);
  // spawn clients
  // std::vector<thread> threads;
  vector<pid_t> clientIDS;
  for (int i = 0; i < config.numOfPlayers; i++) {
    string address = sf::IpAddress::getLocalAddress().toString();
    string port = std::to_string(server.getPlayerSockets()[i]->getLocalPort());
    string result = "/Users/mareksubocz/it/FastOrFurious/probaClient "+address+" "+port;
    const char* char_array = result.c_str();
    clientIDS.push_back(fork());
    for (auto &id: clientIDS){
      cout<<id<<" ";
    }
    if (clientIDS[clientIDS.size()-1] == 0){
      setpgid(getpid(), getpid());
      system(char_array);
      return 0;
    }
  }
  server.waitForConnections();


  sf::RenderWindow window(sf::VideoMode(1000, 1000), "Fast or Furious");
  window.setVerticalSyncEnabled(true); // for artifacts when moving fast
  // window.setFramerateLimit(60);

  // setting random background image (grass, soil, water)
  sf::Texture backgroundTexture;
  sf::Sprite background;
  vector<string> backgroundPaths =
      getAllFilesInDirectory((char *)"./img/PNG/Background_Tiles/");
  int backgroundNumber = rand() % backgroundPaths.size();
  backgroundTexture.loadFromFile("./img/PNG/Background_Tiles/" +
                                 (string)backgroundPaths[backgroundNumber]);
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
      gameState[i] = cars[i]->getPlayerState();
    }
    server.sendGameState(gameState);
    server.waitForResponse();
    vector<Response> playerResponses = server.getPlayerResponses();
    for (int i = 0; i < config.numOfPlayers; i++) {
      cars[i]->handleResponse(playerResponses[i]);
      window.draw(cars[i]->getBody());
    }

    window.display();
  }
  for (auto &id: clientIDS){
    kill(-id, SIGKILL);
  }

  return 0;
}
