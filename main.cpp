#include "Car.cpp"
#include "utils.cpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <dirent.h>
#include <filesystem>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::string;
using std::vector;


configuration config;

/*TODO:
- add delta time
- add boost animation
*/

int main() {
  srand(time(NULL));

  sf::RenderWindow window(sf::VideoMode(1000, 1000), "Fast or Furious");
  // window.setVerticalSyncEnabled(true); // for artifacts when moving fast
  window.setFramerateLimit(60);

  // setting random background image (grass, soil, water)
  sf::Texture backgroundTexture;
  sf::Sprite background;
  vector<string> backgroundPaths = getAllFilesInDirectory((char *)"./img/PNG/Background_Tiles/");
  int backgroundNumber = rand() % backgroundPaths.size();
  backgroundTexture.loadFromFile("./img/PNG/Background_Tiles/" +
                                 (string)backgroundPaths[backgroundNumber]);
  background.setTexture(backgroundTexture);
  background.setScale(
      (double)window.getSize().x / backgroundTexture.getSize().x,
      (double)window.getSize().y / backgroundTexture.getSize().y);

  sf::Keyboard::Key keys1[] = {
      sf::Keyboard::Up,
      sf::Keyboard::Down,
      sf::Keyboard::Left,
      sf::Keyboard::Right
      };
  sf::Keyboard::Key keys2[] = {
      sf::Keyboard::W,
      sf::Keyboard::S,
      sf::Keyboard::A,
      sf::Keyboard::D
      };
  Car c = Car(500, 500, keys1, config);
  Car c2 = Car(700, 700, keys2, config);
  vector<Car*> cars;
  cars.push_back(&c);
  cars.push_back(&c2);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.draw(background);
    for(auto car: cars){
      car->checkMove();
      window.draw(car->getBody());
    }
    window.display();
  }

  return 0;
}
