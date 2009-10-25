/**

	Server

**/

#include "main.h"
#include "tftp_server.h"

using namespace std;

int main(int argc, char **  argv) {

	cout << "Starting TFTP server\n";

	TFTPServer server(5555, "F:/Documents and Settings/Vilius/Desktop/tftp/server/ftproot/");

	
 /* // deskriptoriu rinkiniai 
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
                        if (recv_size == 0) 
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
 */

  cout << "TFTP Server was shut down\n";

  return 1;

}
