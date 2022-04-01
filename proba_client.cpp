#include "Client.cpp"

using namespace std;

int main(int argc, char** argv) {
  sf::IpAddress serverIP = sf::IpAddress(argv[1]);
  unsigned short serverPort = stoi((string)argv[2]);
  Client client = Client(serverIP, serverPort);
  client.connectToServer();
  while(true){
    client.waitForGameState();
    Response response = client.calculateResponse();
    client.sendResponse(response);
  }
  return 0;
}
