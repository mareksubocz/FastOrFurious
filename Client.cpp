#ifndef CLIENT
#define CLIENT

#include "utils.cpp"
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

class Client {
private:
  sf::IpAddress serverIP;
  unsigned short serverPort;
  sf::UdpSocket *socket;
  Configuration config;
  vector<PlayerState> playerStates;
  bool isBot;
  int keysNum;

public:
  Client(sf::IpAddress serverIP, unsigned short serverPort,
         bool isBot = false, int keysNum = 0) {
    this->serverIP = serverIP;
    this->serverPort = serverPort;
    this->socket = new sf::UdpSocket();
    this->socket->bind(sf::Socket::AnyPort);
    this->isBot = isBot;
    this->keysNum = keysNum;
  }

  void connectToServer() {
    sf::Packet packet;
    char ipAddress[20];
    unsigned int port = this->socket->getLocalPort();

    // send ip and port to server
    strcpy(ipAddress, sf::IpAddress::getLocalAddress().toString().c_str());
    packet << ipAddress << port;
    if (this->socket->send(packet, this->serverIP, this->serverPort) !=
        sf::Socket::Done) {
      cout << "Can't connect to server" << endl;
    }

    sf::IpAddress senderIP;
    unsigned short senderPort;

    // receive config from server
    packet.clear();
    this->socket->receive(packet, senderIP, senderPort);
    packet >> this->config;
  }

  void waitForGameState() {
    sf::IpAddress senderIP;
    unsigned short senderPort;
    sf::Packet packet;
    vector<PlayerState> playerStates;
    PlayerState playerState;
    playerStates.clear();
    this->socket->receive(packet, senderIP, senderPort);
    for (int i = 0; i < config.numOfPlayers; i++) {
      packet >> playerState;
      playerStates.push_back(playerState);
    }
  }

  Response calculateResponse() {
    sf::Clock clock;
    clock.restart();
    sf::Time timeout = sf::milliseconds(config.timeout);
    sf::Time elapsed = clock.getElapsedTime();
    vector<vector<sf::Keyboard::Key>> keys;
    keys.push_back({sf::Keyboard::Up, sf::Keyboard::Left, sf::Keyboard::Right});
    keys.push_back({sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::D});
    Response response;
    if (this->isBot) {
      response.gas = 1;
    }
    // human is steering, use keyboard
    else {
      bool pressed = false;
      while (elapsed < timeout) {
        if (sf::Keyboard::isKeyPressed(keys[this->keysNum][0])) {
          response.gas = 1;
          pressed = true;
        }
        if (sf::Keyboard::isKeyPressed(keys[this->keysNum][1])) {
          response.rotate = -1;
          pressed = true;
        }
        if (sf::Keyboard::isKeyPressed(keys[this->keysNum][2])) {
          response.rotate = 1;
          pressed = true;
        }
        if (pressed) {
          break;
        }
        elapsed = clock.getElapsedTime();
      }
    }
    return response;
  }

  void sendResponse(Response response) {
    sf::Packet packet;
    packet << response;
    this->socket->send(packet, this->serverIP, this->serverPort);
  }

  // void run() {
  //   waitForGameState();
  //   Response response = calculateResponse();
  //   sendResponse(response);
  // }
};

#endif
