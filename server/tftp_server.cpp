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

	//- ijungiam nesiblokuojanti rezima
	unsigned long non_blockig_mode = 1;
	ioctlsocket(server_socket, FIONBIO, &non_blockig_mode);

	cout << "Server started, waiting for connections" << endl;

	while (true) {

		acceptClients();
		receiveFromClients();
		sendToClients();

	}

}

/**
*  Klientu prijungimas
**/
void TFTPServer::acceptClients() {

	for (int i = 0; i < TFTP_SERVER_MAX_CLIENTS; i++) {

		if (clients[i].connected == TFTP_SERVER_CLIENT_NOT_CONNECTED) {

			//- su visais laisvais clientais, bandom ka nors prijungti
			acceptClient(&clients[i]);

		}

		if (clients[i].connected == TFTP_SERVER_CLIENT_CONNECTED) {
			//- turim prisijungusi klienta, darom jam veiksmus

			if (clients[i].request == TFTP_SERVER_REQUEST_UNDEFINED) {

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

						char* filename = (char*)calloc(TFTP_PACKET_MAX_SIZE, sizeof(char));
						strcpy_s(filename, TFTP_PACKET_MAX_SIZE, server_ftproot);

						clients[i].last_packet.getString(2, (filename + strlen(filename)), clients[i].last_packet.getSize());

						clients[i].file_rrq = new ifstream(filename, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);

						if (!clients[i].file_rrq->is_open() || !clients[i].file_rrq->good()) {

							sendError(&clients[i], 1);
							disconnectClient(&clients[i]);

						} else {

							//- prasomas failas rastas
							clients[i].file_rrq->seekg(0, std::ios_base::beg);
							ifstream::pos_type begin_pos = clients[i].file_rrq->tellg();
							clients[i].file_rrq->seekg(0, ios_base::end);
							int file_size = (int)(clients[i].file_rrq->tellg() - begin_pos);

							clients[i].file_rrq->seekg(0, ios_base::beg);

							clientStatus(&clients[i], "File was found, starting GET transfer");
							
							if (sendPacketData(&clients[i])) {

								//- jei viskas ok, nesiunciam, kol negavom ACK paketo

								clients[i].acknowledged = TFPT_SERVER_CLIENT_ACK_WAITING;

							}

						}

					} else if (clients[i].last_packet.isWRQ()) {
						
						clients[i].request = TFTP_SERVER_REQUEST_WRITE;

						//- patikrinam ar egzistuoja toks failas
						//- jei taip sukuriam handler`i ir pasiunciam pirma paketa

						char* filename = (char*)calloc(TFTP_PACKET_MAX_SIZE, sizeof(char));
						strcpy_s(filename, TFTP_PACKET_MAX_SIZE, server_ftproot);

						clients[i].last_packet.getString(2, (filename + strlen(filename)), clients[i].last_packet.getSize());

						clients[i].file_rrq = new ifstream(filename, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);

						if (clients[i].file_rrq->is_open() || clients[i].file_rrq->good()) {
							
							clientStatus(&clients[i], "PUT failed. File already exists on server");
							sendError(&clients[i], 6);
							disconnectClient(&clients[i]);

						} else {

							delete clients[i].file_rrq;

							clients[i].file_wrq = new ofstream(filename, std::ios_base::binary);

							clientStatus(&clients[i], "Starting PUT transfer");
							
							TFTP_Packet* packet_ack = new TFTP_Packet();

							// Since the positive response to a write request is an acknowledgment packet, in this special case the
							// block number will be zero.
							packet_ack->createACK(0); //- pirmo wrq ack packeto bloko nr = 0

							if (sendPacket(packet_ack, &clients[i])) {

								clientStatus(&clients[i], "Acknowledgement sent");

							} else {

								clientStatus(&clients[i], "Error in sending acknowledgement");

							}

							delete packet_ack;

						}

					} else {

						//- neatpazintas paketas, diskonektinam

						clientStatus(&clients[i], "Client sent unexpected packet, so we will disconnect him");
						sendError(&clients[i], 4);
						disconnectClient(&clients[i]);

					}

				}

			}

		}

	}

}

/**
*  Pabandom prijungti klienta
**/
bool TFTPServer::acceptClient(ServerClient* client) {

	int sockaddr_length = sizeof(sockaddr);

	client->client_socket = accept(server_socket,(sockaddr *)&client->address, &sockaddr_length);

	if ((client->client_socket != 0) && (client->client_socket != SOCKET_ERROR)) {
		//- turime prisijungima!

		client->connected = TFTP_SERVER_CLIENT_CONNECTED;

		FD_ZERO(&client->set);
		FD_SET(client->client_socket, &(client->set));

		client->ip = inet_ntoa(client->address.sin_addr);

		clientStatus(client, "Connected");

		return true;

	}

	return false;

}

/**
*  Siunciamu paketu gavimas
**/
void TFTPServer::receiveFromClients() {

	char memblock[TFTP_PACKET_DATA_SIZE];

	for (int i = 0; i < TFTP_SERVER_MAX_CLIENTS; i++) {

		if (clients[i].connected == TFTP_SERVER_CLIENT_CONNECTED) {

			if (clients[i].request == TFTP_SERVER_REQUEST_READ) {

				if (TFPT_SERVER_CLIENT_ACK_WAITING == clients[i].acknowledged) {

					if (receivePacket(&clients[i])) {

						if (clients[i].last_packet.isACK()) {

							//- atejo ack, tikrinam ar jis ack teisinga

							if (clients[i].block == clients[i].last_packet.getNumber()) {

								//- ack teisingas, keiciam kliento statusa i ack_ok

								clients[i].acknowledged = TFPT_SERVER_CLIENT_ACK_OK;

								if (clients[i].disconnect_on_ack) {

									clientStatus(&clients[i], "Transfer finished, disconnected!");
									disconnectClient(&clients[i]);
									
								}

							} else {

								//- darome persiuntima paskutinio paketo
								clientStatus(&clients[i], "Packet was not received, try to resend");
								
								sendPacket(&(clients[i].last_sent_packet), &clients[i]);

							}

						} else {

							clientStatus(&clients[i], "Sent an unexpected packet");

							sendError(&clients[i], 4, "Unexpected packet arrived, you have been disconnected");
							disconnectClient(&clients[i]);

						}

					}

				}

			}

			if (clients[i].request == TFTP_SERVER_REQUEST_WRITE) {

				if (receivePacket(&clients[i])) {

					if (clients[i].last_packet.isData()) {

						clients[i].block++;

						if (clients[i].block == clients[i].last_packet.getNumber()) {

							clients[i].last_packet.copyData(4, memblock, (clients[i].last_packet.getSize() - 4));

							clients[i].file_wrq->write(memblock, (clients[i].last_packet.getSize() - 4));

							TFTP_Packet* packet_ack = new TFTP_Packet();

							packet_ack->createACK(clients[i].block);

							if (sendPacket(packet_ack, &clients[i])) {

								clientStatus(&clients[i], "Acknowledgement sent");

							} else {

								clientStatus(&clients[i], "Error in sending acknowledgement");

							}

							delete packet_ack;

							if (clients[i].last_packet.getSize() < TFTP_PACKET_DATA_SIZE + 4) {

								clientStatus(&clients[i], "Client successfully transfered a file");
								clientStatus(&clients[i], "Disconnected");
								disconnectClient(&clients[i]);

							}

						} else {

							//- darome persiuntima paskutinio paketo
							clientStatus(&clients[i], "Packet order mismatched");
							
						}

					} else {

						clientStatus(&clients[i], "Sent an unexpected packet");

						sendError(&clients[i], 4, "Unexpected packet arrived, you have been disconnected");
						disconnectClient(&clients[i]);

					}

				}

			}

		}

	}

}

bool TFTPServer::receivePacket(ServerClient* client) {

	if (FD_ISSET(client->client_socket, &client->set)) {

		client->temp = recv(client->client_socket, (char*)client->last_packet.getData(), TFTP_PACKET_MAX_SIZE, 0);

		if (client->temp == 0) {
			
			//- transfer ended
			clientStatus(client, "Client closed connection");
			disconnectClient(client);
			
			return false;

		} else if (client->temp < 0) {

			if (WSAGetLastError() != 10035) {//- tada tikrai ne normali klaida, nonblocking rezime
				
				clientStatus(client, "Error in receiving packet");

			}

			return false;

		}

		client->last_packet.setSize(client->temp);

		clientStatus(client, "Packet received");

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

void TFTPServer::sendToClients() {

	for (int i = 0; i < TFTP_SERVER_MAX_CLIENTS; i++) {

		if (clients[i].connected == TFTP_SERVER_CLIENT_CONNECTED) {

			if (clients[i].request == TFTP_SERVER_REQUEST_READ) {

				if (clients[i].acknowledged == TFPT_SERVER_CLIENT_ACK_OK) {
					//- turim klienta, jam galime siusti ka nors

					if (sendPacketData(&clients[i])) {

						clients[i].acknowledged = TFPT_SERVER_CLIENT_ACK_WAITING;

					}

				}

			}

		}

	}

}

bool TFTPServer::sendPacket(TFTP_Packet* packet, ServerClient* client) {

	if (client->connected == TFTP_SERVER_CLIENT_NOT_CONNECTED) {
		DEBUGMSG("Tried to send to not connected client");
		return false;
	}

	return send(client->client_socket, (char*)packet->getData(), packet->getSize(), 0);

}

bool TFTPServer::sendPacketData(ServerClient* client) {

	char memblock[TFTP_PACKET_DATA_SIZE];

	client->file_rrq->read(memblock, TFTP_PACKET_DATA_SIZE);

	if (client->file_rrq->eof()) {
		client->disconnect_on_ack = true;
	}

	client->block++;

	cout << client->ip << ": Sending data packet no. " << client->block << endl;

	client->last_sent_packet.createData(client->block, (char*)memblock, client->file_rrq->gcount());
	client->last_sent_packet.dumpData();

	return sendPacket(&(client->last_sent_packet), client);

}

bool TFTPServer::disconnectClient(ServerClient* client) {

	if (client == NULL) return false;

	//- uznulinam viska

	client->last_packet.clear();
	client->last_sent_packet.clear();
	client->ip = "";
	client->connected = TFTP_SERVER_CLIENT_NOT_CONNECTED;
	client->block = 0;
	client->request = TFTP_SERVER_REQUEST_UNDEFINED;
	client->temp = 0;
	client->disconnect_on_ack = false;
	
/*
	if (client->file_rrq != NULL) {

		if (client->file_rrq->is_open()) {
			client->file_rrq->close();
		}

		delete client->file_rrq;

	}

	if (client->file_wrq != NULL) {

		if (client->file_wrq->is_open()) {
			client->file_wrq->close();
		}

		delete client->file_wrq;

	}*/

	closesocket(client->client_socket);

	return true;

}

bool TFTPServer::sendError(ServerClient* client, int error_no, char* custom_message) {

	TFTP_Packet error_packet;

	error_packet.createError(error_no, custom_message);

	return (sendPacket(&error_packet, client) > 0);

}

void TFTPServer::clientStatus(ServerClient* client, char* message) {

	cout << client->ip << ": " << message << endl;

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