#include "main.h"
#include "../client/tftp_packet.h"
#include "tftp_server.h"
#include "utils.h"

using namespace std;

TFTPServer::TFTPServer(int port, char* ftproot) {

	server_port = port;
	server_socket = -1; //- reikalingas destruktoriui
	server_ftproot = ftproot;

	#ifdef WIN32

		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 2);

		if (WSAStartup(wVersionRequested, &wsaData) != 0) {
			throw new ETFTPSocketInitialize;
		}

	#endif

	server_socket = socket(PF_INET, SOCK_STREAM, 0);

	if (server_socket == -1) {
    
		throw new ETFTPSocketCreate;

	}

	DEBUGMSG("Server socket created successfully");

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY; //- pribindinam nezinodami serverio interfeiso, prie visu egzistuojanciu interfeisu

	//#ifdef WIN32
	//	memset(server_address.sin_zero, 0, sizeof(server_address.sinzero); //- suvienodinam SOCKADDR_IN ir SOCKADDR strukturu dydzius
	//#endif

	listener = bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address));

	if (listener != 0) {
		
		throw new ETFTPSocketBind;

	}

	DEBUGMSG("Socket binded to address");

	listener = listen(server_socket, SOMAXCONN); //- is SOMAXCONN portable?

	if (listener != 0) {
	
		throw new ETFTPSocketListen;

	}

	DEBUGMSG("Listening for connections...");

	//- ijungiam nesiblokuojanti rezima
	unsigned long non_blockig_mode = 1;
	ioctlsocket(server_socket, FIONBIO, &non_blockig_mode);

	while (true) {

		acceptClients();

	}

/*	int acceptSocket;

	while (1) {

		acceptSocket = SOCKET_ERROR;

		while (acceptSocket == SOCKET_ERROR) {

			acceptSocket = accept(server_socket, NULL, NULL);

		}

		client_socket.push_back(acceptSocket);

		cout << "Client connected\n";

		TFTP_Packet received_packet;

		if (waitForPacket(&received_packet, client_socket[client_socket.size() - 1], 1000)) {

			received_packet.dumpData();
			
			if (received_packet.isRRQ()) {

				cout << "Request for file received\n";

				//The WRQ and DATA packets are acknowledged by ACK or ERROR packets, 
				//while RRQ and ACK packets are acknowledged by DATA or ERROR packets.
				
				char* filename = (char*)calloc(TFTP_PACKET_MAX_SIZE, sizeof(char));
				strcpy(filename, (char*)TFTP_SERVER_ROOT);

				received_packet.getString(2, (filename + strlen(filename)), received_packet.getSize());

				ifstream file_rrq;
				file_rrq.open(filename, std::ios_base::binary | std::ios_base::in);

				if (!file_rrq.is_open() || !file_rrq.good()) {

					disconnectOnError(client_socket[client_socket.size()-1], 1);

				} else {

					//- prasomas failas rastas
					file_rrq.seekg(0, std::ios_base::beg);
					ifstream::pos_type begin_pos = file_rrq.tellg();
					file_rrq.seekg(0, ios_base::end);
					int file_size = (int)(file_rrq.tellg() - begin_pos);

					file_rrq.seekg(0, ios_base::beg);

					cout << "Starting transfer of file '" << filename << "' [" << file_size << "]" << endl;

					int total_blocks_required = (int)(file_size / TFTP_PACKET_DATA_SIZE);
					
					char memblock[TFTP_PACKET_DATA_SIZE];

					TFTP_Packet data_packet;

					for (int i = 0; i <= total_blocks_required; i++) {

						file_rrq.read(memblock, TFTP_PACKET_DATA_SIZE);
cout << memblock;
						data_packet.clear();

						data_packet.createData(i, (char*)memblock);

					}

				}

				file_rrq.close();


			}

		}
		
		break;
	
	}
	*/

}

void TFTPServer::acceptClients() {

	for (int i = 0; i < TFTP_SERVER_MAX_CLIENTS; i++) {

		if (clients[i].connected == TFTP_SERVER_CLIENT_NOT_CONNECTED) {

			//- su visais laisvais clientais, bandom ka nors prijungti
			acceptClient(&clients[i]);

		}

		if (clients[i].connected == TFTP_SERVER_CLIENT_CONNECTED) {
			//- turim prisijungusi klienta, darom jam veiksmus

			switch (clients[i].request) {

				case TFTP_SERVER_REQUEST_UNDEFINED:
					//- turim nauja prisijungima, darom "naujoko" veiksmus
					//- identifikuojam komanda
					//- sukuriam file handlerius

					if (receivePacket(&clients[i])) {

						//- gavom paketa is cliento
						//- 1 Any transfer begins with a request to read or write a file.
						//- nustatom koks tai requestas

						if (clients[i].last_packet.isRRQ()) {

							clients[i].request = TFTP_SERVER_REQUEST_READ;

							//- patikrinam ar egzistuoja toks failas
							//- jei taip sukuriam handler`i ir pasiunciam pirma paketa

							cout << "jeba";

						} else if (clients[i].last_packet.isRRQ()) {

							clients[i].request = TFTP_SERVER_REQUEST_WRITE;

						} else {

							//- neatpazintas paketas, diskonektinam

							sendError(&clients[i], 4);
							disconnectClient(&clients[i]);

						}

					}

				break;

				case TFTP_SERVER_REQUEST_READ:

					DEBUGMSG("TURIM READ");

				break;

				case TFTP_SERVER_REQUEST_WRITE:

					DEBUGMSG("TURIM WRITE");

				break;

				default:

					DEBUGMSG("FATAL ERROR");

				break;

			}

		}

	}

}

bool TFTPServer::acceptClient(ServerClient* client) {

	int sockaddr_length = sizeof(sockaddr);

	client->client_socket = accept(server_socket,(sockaddr *)&client->address, &sockaddr_length);

	if ((client->client_socket != 0) && (client->client_socket != SOCKET_ERROR)) {
		//- turime prisijungima!

		client->connected = TFTP_SERVER_CLIENT_CONNECTED;

		FD_ZERO(&client->set);
		FD_SET(client->client_socket, &(client->set));

		client->ip = inet_ntoa(client->address.sin_addr);

		clientStatus(client, "Connected!");

		return true;

	}

	return false;

}

bool TFTPServer::receivePacket(ServerClient* client) {

	if (FD_ISSET(client->client_socket, &client->set)) {

		client->temp = recv(client->client_socket, (char*)client->last_packet.getData(), TFTP_PACKET_MAX_SIZE, 0);

		if (client->temp == 0) {
			
			//- transfer ended?
			DEBUGMSG("SHALL WE DISCONNET?");

			return false;

		} else if (client->temp < 0) {

			//DEBUGMSG("DO WE HAVE AN AERROR");
			return false;

		}

		return true;

	}
 
	return false;

}

/* bloguojantis variantas */
bool TFTPServer::waitForPacket(TFTP_Packet* packet, int current_client_socket, int timeout_ms) {

	packet->clear();

	FD_SET fd_reader; // soketu masyvo struktura
	timeval connection_timer; // laiko struktura perduodama select()

	connection_timer.tv_sec = timeout_ms / 1000; // s
	connection_timer.tv_usec = 0; // neveikia o.0 timeout_ms; // ms 

	FD_ZERO(&fd_reader);

	// laukiam, kol bus ka nuskaityti
	FD_SET(current_client_socket, &fd_reader);

	int select_ready = select(0, &fd_reader, NULL, &fd_reader, &connection_timer);

	if (select_ready == -1) {

		DEBUGMSG("Error in select()");
		return false;

	} else if (select_ready == 0) {

		DEBUGMSG("Timeout");
		return false;

	}

	//- turim sekminga event`a

	int receive_status;

	receive_status = recv(current_client_socket, (char*)packet->getData(), TFTP_PACKET_MAX_SIZE, 0);

	if (receive_status == 0) {
		cout << "Connection was closed by client\n";
		return false;
    }

	if (receive_status == SOCKET_ERROR)	{
		DEBUGMSG("recv() error in waitForPackage()");
		return false;
	}

	//- receive_status - gautu duomenu dydis

	packet->setSize(receive_status);

	return true;

}

bool TFTPServer::waitForPacketACK(int packet_number, int timeout_ms) {

	return true;

}

bool TFTPServer::sendPacket(TFTP_Packet* packet, int client_socket) {

	return send(client_socket, (char*)packet->getData(), packet->getSize(), 0);

}

bool TFTPServer::disconnectClient(ServerClient* client) {

	if (client == NULL) return false;

	//- uznulinam viska

	client->last_packet.clear();
	client->ip = "";
	client->connected = TFTP_SERVER_CLIENT_NOT_CONNECTED;
	client->block = 0;
	client->request = TFTP_SERVER_REQUEST_UNDEFINED;
	client->temp = 0;

	closesocket(client->client_socket);

	return true;

}

bool TFTPServer::sendError(ServerClient* client, int error_no, char* custom_message) {

	TFTP_Packet error_packet;

	error_packet.createError(error_no, custom_message);

	return (sendPacket(&error_packet, client->client_socket) > 0);

}

void TFTPServer::clientStatus(ServerClient* client, char* message) {

	cout << client->ip << ": " << message;

}


TFTPServer::~TFTPServer() {

    if (server_socket != -1) {

        #ifdef WIN32

            closesocket(server_socket);
            WSACleanup();
            
        #else

            close(server_socket);

        #endif

    }

}