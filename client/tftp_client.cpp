#include "../server/main.h"

#include "tftp_packet.h"
#include "tftp_client.h"

using namespace std;

TFTPClient::TFTPClient(char* ip) {

	TFTP_Packet packet;

	server_ip = ip;

	//- standartines reiksmes

	socket_descriptor = -1;

	//- wsa

	#ifdef WIN32

		/* edited */

		WSADATA wsaData;

		WORD wVersionRequested = MAKEWORD(2, 2);

		int err = WSAStartup(wVersionRequested, &wsaData);

		if (err != 0) {

			throw new ETFTPSocketInitialize;
		    
		}

	#endif

}

int TFTPClient::connectToServer() {

    socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);

    if (socket_descriptor == -1) {

        throw new ETFTPSocketCreate;

    }

    DEBUGMSG("Socket created");

    client_address.sin_family = AF_INET;
	client_address.sin_port = htons(5555);	//- taip pat turi buti ir serveryje!
    client_address.sin_addr.s_addr = inet_addr(this->server_ip);

    #ifdef WIN32
        //memset(client_address.sin_zero, 0, sizeof(client_address.sinzero);
        //- suvienodinam SOCKADDR_IN ir SOCKADDR strukturu dydzius
    #endif

    connection = connect(socket_descriptor, (const struct sockaddr *)&client_address, sizeof(client_address));

    if (connection != 0) {

        cout << "Unable to connect to an address\n";
        return -1;
        
    }

    DEBUGMSG("Successfully connected");

    return 1;

}

int TFTPClient::sendBuffer(char *buffer) {

    return send(socket_descriptor, buffer, (int)strlen(buffer), 0);

}

int TFTPClient::sendPacket(TFTP_Packet* packet) {

	return send(socket_descriptor, (char*)packet->getData(), packet->getSize(), 0);

}

unsigned char* TFTPClient::getFile(char* filename) {

	TFTP_Packet packet;

	packet.createRRQ(filename);

	//packet.dumpData();

	sendPacket(&packet);

	int last_packet_no = 1;

	while (true) {

		if (!waitForPacketData(last_packet_no, TFTP_CLIENT_SERVER_TIMEOUT)) {
			
			received_packet.dumpData();
			break;

		}

		last_packet_no++;

		received_packet.dumpData();

		cout << received_packet.getNumber();

	}

	return packet.getData(0);

}

bool TFTPClient::waitForPacket(TFTP_Packet* packet, int timeout_ms) {

	packet->clear();

	FD_SET fd_reader;		  // soketu masyvo struktura
	timeval connection_timer; // laiko struktura perduodama select()

	connection_timer.tv_sec = timeout_ms / 1000; // s
	connection_timer.tv_usec = 0; // neveikia o.0 timeout_ms; // ms 

	FD_ZERO(&fd_reader);

	// laukiam, kol bus ka nuskaityti
	FD_SET(socket_descriptor, &fd_reader);

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

	receive_status = recv(socket_descriptor, (char*)packet->getData(), TFTP_PACKET_MAX_SIZE, 0);

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

bool TFTPClient::waitForPacketACK(int packet_number, int timeout_ms) {

	TFTP_Packet received_packet;

	if (waitForPacket(&received_packet, 1000)) {

		if (received_packet.isError()) {

			errorReceived(&received_packet);

		}

		if (received_packet.isData()) {

			return true;

		}

	}

	return true;

}

bool TFTPClient::waitForPacketData(int packet_number, int timeout_ms) {

	if (waitForPacket(&received_packet, timeout_ms)) {

		if (received_packet.isError()) {

			errorReceived(&received_packet);

			return false;

		}

		if (received_packet.isData()) {
			
			return true;

		}

	}

	return false; //- timeout occured

}

void TFTPClient::errorReceived(TFTP_Packet* packet) {

	int error_code = packet->getWord(2);

	cout << "Error! Error code: " << error_code << endl;
	cout << "Error message: ";
	
	switch (error_code) {

		case 1: cout << TFTP_ERROR_1; break;
		case 2: cout << TFTP_ERROR_2; break;
		case 3: cout << TFTP_ERROR_3; break;
		case 4: cout << TFTP_ERROR_4; break;
		case 5: cout << TFTP_ERROR_5; break;
		case 6: cout << TFTP_ERROR_6; break;
		case 7: cout << TFTP_ERROR_7; break;
		case 0: 
		default: cout << TFTP_ERROR_0; break;

	}

	cout << endl;

	TFTPClient::~TFTPClient();

}

TFTPClient::~TFTPClient() {

    if (socket_descriptor != -1) {

        #ifdef WIN32

            closesocket(socket_descriptor);
            WSACleanup();
            
        #else

            close(socket_descriptor);

        #endif

    }

}

void DEBUGMSG(char *msg) {
    #ifdef DEBUG
	std::cout << msg << "\n";
    #endif
}