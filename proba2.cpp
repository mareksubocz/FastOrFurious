#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <dirent.h>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  sf::UdpSocket socket;
  sf::SocketSelector selector;
  selector.add(socket);
  socket.setBlocking(true);
  if (socket.bind(54000) != sf::Socket::Done) {
    cout << "Socket binding failed" << endl;
  }
  sf::IpAddress sender;
  unsigned short port;
  char data[100];
  std::size_t received;
  while(true){
    while (socket.receive(data, 100, received, sender, port) != sf::Socket::Done) {
      cout<<'.';
    }
    cout<<(string)data<<endl;
  }
  return 0;
}
