#ifndef SERVER
#define SERVER

#include "utils.cpp"
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <unistd.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct PlayerInfo {
  int num;
  sf::IpAddress ip;
  unsigned short port;
  sf::UdpSocket *socket;
};

class Server {
private:
  Configuration config;
  vector<PlayerInfo> players;
  vector<Response> playerResponses;

  void assignSockets() {
    for (int i = 0; i < this->config.numOfPlayers; i++) {
      sf::UdpSocket *socket = new sf::UdpSocket;

      // socket is available
      if (socket->bind(sf::Socket::AnyPort) == sf::Socket::Done) {
        players[i].socket = socket;
        players[i].num = i + 1;
        printf("Player %d assigned port %d.\n", i + 1, socket->getLocalPort());
      }
      // socket is unavailable (weird, Socket::AnyPort looks for available ones)
      else {
        printf("Port %d binding failed.\n", socket->getLocalPort());
        delete socket;
        i--;
      }
    }
  }

public:
  Server(const Configuration & config) {
    this->config = config;
    players.resize(config.numOfPlayers);
    playerResponses.resize(config.numOfPlayers);
    assignSockets();
    cout << "Server's IP address: " << sf::IpAddress::getLocalAddress() << endl;
  }

  // Wait endlessly until all players send information about their port.
  void waitForConnections() {
    char ipAddress[20];
    unsigned int port;
    sf::Packet packet;
    sf::SocketSelector selector;

    for (auto player : this->players) {
      selector.add(*player.socket);
    }

    int playersLeft = this->config.numOfPlayers;
    while (playersLeft > 0) {
      if (selector.wait()) {
        for (auto &player : this->players) {
          if (selector.isReady(*player.socket)) {
            // receive ip and port from client
            player.socket->receive(packet, player.ip, player.port);
            if (!(packet >> ipAddress >> port)) {
              printf("Player %d didn't connect properly.\n", player.num);
              throw std::invalid_argument("Player didn't connect properly.");
              break;
            }
            player.ip = ipAddress;
            player.port = port;
            printf("Player %d connected.\n", player.num);

            packet.clear();
            packet << config;
            player.socket->send(packet, player.ip, player.port);
            selector.remove(*player.socket);
            playersLeft--;
          }
        }
      }
    }
    printf("All players connected.\n");
  }

  void sendGameState(vector<PlayerState> &playerStates) {
    sf::Packet packet;
    for (int i = 0; i < config.numOfPlayers; i++) {
      packet << playerStates[i];
    }
    for (auto player : this->players) {
      player.socket->send(packet, player.ip, player.port);
    }
  }

  void waitForResponse() {
    sf::Clock clock;
    clock.restart();
    sf::Time elapsed;
    sf::SocketSelector selector;
    sf::Packet packet;
    Response response;

    for (auto player : this->players) {
      selector.add(*player.socket);
    }

    int playersLeft = this->config.numOfPlayers;
    sf::Time timeout = sf::milliseconds(this->config.timeout);
    while (playersLeft > 0 and elapsed < timeout) {
      elapsed = clock.getElapsedTime();
      if (selector.wait(timeout - elapsed)) {
        for (auto player : this->players) {
          if (selector.isReady(*player.socket)) {
            // receive ip and port from client
            packet.clear();
            player.socket->receive(packet, player.ip, player.port);
            if (!(packet >> playerResponses[player.num - 1])) {
              printf("Player %d sent corrupted message.\n", player.num);
              break;
            }
            selector.remove(*player.socket);
            playersLeft--;
          }
        }
      }
    }
    sf::sleep(timeout - elapsed);
  }

  vector<pid_t> spawnClients(int n) {
    vector<pid_t> clientIDs;
    for (int i = 0; i < n; i++) {
      string address = sf::IpAddress::getLocalAddress().toString();
      string port = std::to_string(this->getPlayerSockets()[i]->getLocalPort());
      string result = "/Users/mareksubocz/it/FastOrFurious/runClient " +
                      address + " " + port + " " + std::to_string(i);
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

  vector<Response> getPlayerResponses() { return this->playerResponses; }

  vector<sf::UdpSocket *> getPlayerSockets() {
    vector<sf::UdpSocket *> playerSockets;
    for (auto &player : this->players) {
      playerSockets.push_back(player.socket);
    }
    return playerSockets;
  }
};

#endif
