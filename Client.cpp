#ifndef CLIENT
#define CLIENT

#include "algebra.cpp"
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

enum class Version { human, simple, adversary };

class Client {
private:
  sf::IpAddress serverIP;
  unsigned short serverPort;
  sf::UdpSocket *socket;
  Configuration config;
  vector<PlayerState> playerStates = {};
  Version version;
  int num;

public:
  Client(sf::IpAddress serverIP, unsigned short serverPort, Version version,
         int num = 0) {
    this->serverIP = serverIP;
    this->serverPort = serverPort;
    this->socket = new sf::UdpSocket();
    this->socket->bind(sf::Socket::AnyPort);
    this->version = version;
    this->num = num;
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
    PlayerState playerState;
    playerStates.clear();
    if (this->socket->receive(packet, senderIP, senderPort) !=
        sf::Socket::Done) {
      cout << "error receiving game state" << endl;
    }
    for (int i = 0; i < config.numOfPlayers; i++) {
      packet >> playerState;
      this->playerStates.push_back(playerState);
    }
  }

  Response calculateResponseBot() {
    Response response;
    sf::Vector2f nextCheckpoint =
        this->config.checkpoints[this->playerStates[this->num].checkpoint];
    sf::Vector2f myPos = this->playerStates[this->num].pos;
    sf::Vector2f relVec = nextCheckpoint - myPos;
    relVec.y = -relVec.y;
    float myRot = this->playerStates[this->num].rotation;

    float turn = myRot - vectorRotation(relVec);
    if (abs(turn) > 180.f) {
      turn = -turn;
    }
    if (turn == 0) turn = 10;
    float gas = abs(100.f / turn);

    response.rotate = -sign(turn, 1.f);
    response.gas = gas;
    return response;
  }

  Response calculateResponseAdversary() {

    // drive around if you won
    int lastLeft = -1;
    for (auto const &p : this->playerStates){
      if (p.health > 0){
        lastLeft++;
      }
    }
    if (lastLeft > 0){ return calculateResponseBot(); }

    Response response;
    int targetI = -1;
    sf::Vector2f target;
    float minDistance = MAXFLOAT;
    for (int i = 0; i<this->config.numOfPlayers; i++){
      if (i == this->num or this->playerStates[i].health <= 0){ continue; }
      float tmp = distance(this->playerStates[this->num].pos, this->playerStates[i].pos);
      if (tmp < minDistance){
        minDistance = tmp;
        target = this->playerStates[i].pos;
        targetI = i;
      }
    }
    sf::Vector2f myPos = this->playerStates[this->num].pos;
    sf::Vector2f relVec = target - myPos;
    relVec.y = -relVec.y;
    float myRot = this->playerStates[this->num].rotation;

    float turn = myRot - vectorRotation(relVec);
    if (abs(turn) > 180.f) {
      turn = -turn;
    }
    if (turn == 0) turn = 10;

    response.rotate = -sign(turn, 1.f);
    response.gas = 100;
    return response;
  }

  Response calculateResponse() {
    sf::Clock clock;
    clock.restart();
    sf::Time timeout = sf::milliseconds(config.timeout);
    sf::Time elapsed = clock.getElapsedTime();
    vector<vector<sf::Keyboard::Key>> keys;
    keys.push_back({sf::Keyboard::Up, sf::Keyboard::Left, sf::Keyboard::Right});
    keys.push_back({sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::D});
    keys.push_back({sf::Keyboard::I, sf::Keyboard::J, sf::Keyboard::L});
    Response response;
    bool pressed = false;
    while (elapsed < timeout) {
      if (sf::Keyboard::isKeyPressed(keys[this->num][0])) {
        response.gas = 100;
        pressed = true;
      }
      if (sf::Keyboard::isKeyPressed(keys[this->num][1])) {
        response.rotate = -1;
        pressed = true;
      }
      if (sf::Keyboard::isKeyPressed(keys[this->num][2])) {
        response.rotate = 1;
        pressed = true;
      }
      if (pressed) {
        break;
      }
      elapsed = clock.getElapsedTime();
    }
    return response;
  }

  void sendResponse(Response response) {
    sf::Packet packet;
    packet << response;
    this->socket->send(packet, this->serverIP, this->serverPort);
  }

  void run() {
    connectToServer();
    Response response;
    while (true) {
      waitForGameState();
      switch(this->version){
        case Version::human:
          response = calculateResponse();
          break;
        case Version::simple:
          response = calculateResponseBot();
          break;
        case Version::adversary:
          response = calculateResponseAdversary();
      }
      sendResponse(response);
    }
  }
};

#endif
