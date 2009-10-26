/**

	Client

**/

#include "../server/main.h"
#include "tftp_client.h"

using namespace std;

int main(int argc, char ** argv) {

  char* ip = "127.0.0.1";

  cout << "Starting TFTP client\n";

  TFTPClient client(ip);

  if (client.connectToServer() != 1) {

      cout << "Error while connecting to server\n";

      //return 0;
      
  }

  if (client.getFile("readme.txt")) {

	  cout << "File transferred successfully\n";

  }

  client.~TFTPClient();

  //- deskriptoriu rinkiniai 
  fd_set read_set;
  fd_set master_set;
  //int fd_max;

  //- isvalom visus rinkinio deskriptorius
  FD_ZERO(&master_set);
  FD_ZERO(&read_set);

  cout << "Disconnected\n";

  int foo;

  cin >> foo;

  return 1;

}