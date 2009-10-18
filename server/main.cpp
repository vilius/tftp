#include "./main.h"

using namespace std;

int main(int argc, char **  argv) {

  // deskriptoriu rinkiniai 
  fd_set read_set;
  fd_set master_set;
  int fd_max;

  //- socketu deskriptoriai
  int server_socket_descriptor;
  int client_socket_descriptor;

  //- isvalom visus rinkinio deskriptorius
  FD_ZERO(&master_set);
  FD_ZERO(&read_set);

  socklen_t client_address_length;

  cout << "Starting TFTP server\n";

  if (server_socket_descriptor == -1) {
    
    cout << "Failed to create a socket\n";

    return -1;

  }

  cout << "Socket created\n";

  server_address.sin_family = AF_INET;
  server_address.sin_port = PORT;
  server_address.sin_addr.s_addr = INADDR_ANY; //- pribindinam nezinodami serverio interfeiso, prie visu egzistuojanciu interfeisu

#ifdef WIN32
  memset(server_address.sin_zero, 0, sizeof(server_address.sinzero); //- suvienodinam SOCKADDR_IN ir SOCKADDR strukturu dydzius
#endif

  listener = bind(server_socket_descriptor, (const struct sockaddr *)&server_address, sizeof(server_address));

  if (listener != 0) {
      cout << "Unable to bind an address\n";
      close(server_socket_descriptor);
      return -1;
  }

  listener = listen(server_socket_descriptor, 10);

  if (listener != 0) {
      cout << "Unable to start listening for incoming connectinos\n";
      close(server_socket_descriptor);
      return -1;
  }

  cout << "Listening started on port " << PORT << "\n";

  FD_SET(server_socket_descriptor, &master_set);
  fd_max = server_socket_descriptor + 1;

  for (;;) {

      read_set = master_set;

      if (select(fd_max, &read_set, NULL, NULL, NULL) == -1) {
        cout << "Descriptor selection error\n";
        close(server_socket_descriptor);
        return -1;
      }

      for (int i = 0; i < fd_max; i++) {

          if (FD_ISSET(i, &read_set)) {

              if (i == server_socket_descriptor) {

                  if ( (client_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr *)&client_address, &client_address_length)) == -1) {
                 
                    cout << "Accept failed\n";
                  
                  } else {

                    FD_SET(client_socket_descriptor, &master_set);
                    if (client_socket_descriptor > fd_max) {
                        fd_max = client_socket_descriptor + 1;
                    }

                    cout << "Client pool appended\n";

                  }

              } else {
				  
				  //- turim duomenu
				  
				  int recv_size;
				  char buf[255];
				  
				  if ((recv_size = recv(i, buf, sizeof(buf),0)) <= 0)
                    {
                        if (recv_size == 0) /* connection closed print notification */
                        {
                            cout << "Connection closed/n";
                        }
                        else
                            cout << "error\n";
							
                        close(i);
                        FD_CLR(i, &master_set);

                    }
                    else {
                        
						cout << "DATA: " << buf << "\n";
						
					}

				  
			  }

          }

      }

  }

  close(server_socket_descriptor);

  cout << "TFTP Server was shut down\n";

  return 1;

}
