#ifndef TFTPCLIENT
#define TFTPCLIENT

#include "tftp_packet.h"

#define TFTP_CLIENT_SERVER_TIMEOUT 2000

#define TFTP_CLIENT_ERROR_TIMEOUT 0
#define TFTP_CLIENT_ERROR_SELECT 1
#define TFTP_CLIENT_ERROR_CONNECTION_CLOSED 2
#define TFTP_CLIENT_ERROR_RECEIVE 3
#define TFTP_CLIENT_ERROR_NO_ERROR 4
#define TFTP_CLIENT_ERROR_PACKET_UNEXPECTED 5

class TFTPClient {

	private:

		char* server_ip;
		int	server_port;

		//- kliento socketo descriptorius
		int socket_descriptor;

		//- socket'o endpoint'u strukturos
		struct sockaddr_in client_address;
		int connection;

		TFTP_Packet received_packet;

	protected:

		int sendBuffer(char *);
		int sendPacket(TFTP_Packet* packet);

	public:

		TFTPClient(char* ip, int port);
		~TFTPClient();

		int connectToServer();
		bool getFile(char* filename, char* destination);
		bool sendFile(char* filename, char* destination);

		int waitForPacket(TFTP_Packet* packet, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);
		bool waitForPacketACK(int packet_number, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);
		int waitForPacketData(int packet_number, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);

		void errorReceived(TFTP_Packet* packet);

};

class ETFTPSocketCreate: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to create a socket";
  }
};

class ETFTPSocketInitialize: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to find socket library";
  }
};

void DEBUGMSG(char*);

#endif
