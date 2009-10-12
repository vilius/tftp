#include "../server/main.h"
#include "tftp_client.cpp"

using namespace std;

int main(int argc, char **  argv) {

  char ip[15] = "192.168.1.0";

  TFTPClient client(ip);

  //- socket'o endpoint'u strukturos
  struct sockaddr_in client_address;

  int connection;

  //- deskriptoriu rinkiniai 
  fd_set read_set;
  fd_set master_set;
  int fd_max;

  //- socketu deskriptoriai
  int socket_descriptor;

  //- isvalom visus rinkinio deskriptorius
  FD_ZERO(&master_set);
  FD_ZERO(&read_set);

  socklen_t client_address_length;

  cout << "Starting TFTP client\n";

  socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);
 
  if (socket_descriptor == -1) {
    
    cout << "Failed to create a socket\n";

    return -1;

  }

  cout << "Socket created\n";

  client_address.sin_family = AF_INET;
  client_address.sin_port = PORT;
  client_address.sin_addr.s_addr = inet_addr("127.0.0.1");

#ifdef WIN32
  memset(client_address.sin_zero, 0, sizeof(client_address.sinzero); //- suvienodinam SOCKADDR_IN ir SOCKADDR strukturu dydzius
#endif

  connection = connect(socket_descriptor, (const struct sockaddr *)&client_address, sizeof(client_address));

  if (connection != 0) {
      cout << "Unable to connect to an address\n";
      close(socket_descriptor);
      return -1;
  }

  int number = 1;
  int send_size;

  while (1) {

    send_size = send(socket_descriptor, (void *)&number, sizeof(number), 0);

    if (send_size == -1) {
        cout << "Error on send function\n";
        close(socket_descriptor);
    }

    break;

  }

  close(socket_descriptor);

  cout << "TFTP Server was shut down\n";

  return 1;

}
