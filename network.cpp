#ifndef SERVER
#define SERVER
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Car.cpp"

class Server {
private:
  sf::SocketSelector selector;
  vector<sf::UdpSocket *> sockets;
  vector<Car> *cars;

public:
  Server(vector<Car> *cars) {
    this->cars = cars;
    for (auto car: *cars){
      //bind players
    }
    sf::SocketSelector selector;
    vector<sf::Socket *> sockets;
    for (auto car : *cars) {
      sf::UdpSocket *socket = new sf::UdpSocket;
      if (socket->bind(sf::Socket::AnyPort) == sf::Socket::Done) {
        sockets.push_back(socket);
        car.setPort(socket->getLocalPort());
      } else {
        cout << "Socket " << socket->getLocalPort() << " binding failed." << endl;
        delete socket;
      }
      selector.add(*socket);
    }
  }

  void sendGameState(){

  }

  void waitForAnswer(int timeout) {
    char data[100];
    sf::IpAddress sender;
    unsigned short port;
    std::size_t received;

    if (selector.wait(sf::milliseconds(timeout))) {
      for (auto socket : this->sockets) {
        if (selector.isReady(*socket)) {
          socket->receive(data, 100, received, sender, port);
        }
      }
    } else {
      // nikt niczego nie przesłał
    }
  }
};
#endif
