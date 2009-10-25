#ifndef TFTPCLIENT
#define TFTPCLIENT

#include "tftp_packet.h"

#define TFTP_CLIENT_SERVER_TIMEOUT 1000

class TFTPClient {

	private:

		char* server_ip;

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

		TFTPClient(char* ip);
		~TFTPClient();

		int connectToServer();
		unsigned char* getFile(char* filename);

		bool waitForPacket(TFTP_Packet* packet, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);
		bool waitForPacketACK(int packet_number, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);
		bool waitForPacketData(int packet_number, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);

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
