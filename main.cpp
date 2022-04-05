#include "Car.cpp"
#include "Client.cpp"
#include "Server.cpp"
#include "algebra.cpp"
#include "utils.cpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <dirent.h>
#include <filesystem>
#include <iostream>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::string;
using std::vector;

/*TODO:
- use delta time NOT NEEDED?
- add boost and animation
- car collisions
- car damage
*/

vector<pid_t> spawnClients(Configuration &config, Server &server) {
  vector<pid_t> clientIDs;
  for (int i = 0; i < config.numOfPlayers; i++) {
    string address = sf::IpAddress::getLocalAddress().toString();
    string port = std::to_string(server.getPlayerSockets()[i]->getLocalPort());
    string result = "/Users/mareksubocz/it/FastOrFurious/runClient " + address +
                    " " + port + " " + std::to_string(i);
    const char *char_array = result.c_str();
    clientIDs.push_back(fork());
    if (clientIDs[clientIDs.size() - 1] == 0) {
      setpgid(getpid(), getpid());
      system(char_array);
      return clientIDs;
    }
  }
  return clientIDs;
}

int main(int argc, char *argv[0]) {
  vector<pid_t> clientIDs;
  try {
    Configuration config;
    srand(time(NULL));

    vector<PlayerState> gameState(config.numOfPlayers);

    vector<Car *> cars;
    for (int i = 0; i < config.numOfPlayers; i++) {
      Car *car = new Car(200 + 200 * i, 200 + 200 * i, config);
      cars.push_back(car);
    }

    Server server = Server(config);
    // spawn clients
    clientIDs = spawnClients(config, server);
    server.waitForConnections();

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Fast or Furious");
    window.setVerticalSyncEnabled(true); // for artifacts when moving fast
    // window.setFramerateLimit(60);

    // setting random background image (grass, soil, water)
    sf::Texture backgroundTexture;
    sf::Sprite background;
    vector<string> backgroundPaths = getAllFilesInDirectory((
        char *)"/Users/mareksubocz/it/FastOrFurious/img/PNG/Background_Tiles/");
    int backgroundNumber = rand() % backgroundPaths.size();
    backgroundTexture.loadFromFile(
        "/Users/mareksubocz/it/FastOrFurious/img/PNG/Background_Tiles/" +
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
      // handle collisions
      Car::handleCollisions(cars, gameState, config);
      for (int i = 0; i < config.numOfPlayers; i++) {
        cars[i]->handleResponse(playerResponses[i]);
        cars[i]->display(window);
      }

      window.display();
    }
    for (auto &id : clientIDs) {
      kill(-id, SIGKILL);
    }
  } catch (const std::exception &ex) {
    cout << ex.what() << endl;
    for (auto &id : clientIDs) {
      kill(-id, SIGKILL);
    }
  }

  return 0;
}
