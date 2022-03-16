#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <dirent.h>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  sf::UdpSocket socket;

  // bind the socket to a port
  // if (socket.bind(54000) != sf::Socket::Done) {
  //     cout<<"Socket binding failed"<<endl;
  // }

  char data[100] = "siema";
  sf::IpAddress recipient = sf::IpAddress::getLocalAddress();
  unsigned short port = 54000;
  if (socket.send(data, 100, recipient, port) != sf::Socket::Done) {
      cout<<"Socket sending failed"<<endl;
  }
  return 0;
}
