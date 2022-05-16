#include "Client.cpp"

using namespace std;

int main(int argc, char **argv) {
  sf::IpAddress serverIP = sf::IpAddress(argv[1]);
  unsigned short serverPort = stoi((string)argv[2]);
  int num = stoi((string)argv[3]);
  Version version = (Version)stoi((string)argv[4]);
  Client client = Client(serverIP, serverPort, version, num = num);
  client.run();
  return 0;
}
