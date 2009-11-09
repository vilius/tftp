/**

	TFTP Client
	Atliko: Vilius Paulauskas [3 kursas, IV grupe, MIF PS]
	Uzduotis: TFTP

**/

#include "../server/main.h"
#include "tftp_client.h"
#include "lib/anyoption.h"

using namespace std;

#define COMMAND_PUT 1
#define COMMAND_GET 2

int main(int argc, char ** argv) {

	char* ip = "";
	int	  command = COMMAND_GET;
	char* source = "";
	char* destination = "";
	int	  port;

	AnyOption *opt = new AnyOption();

	opt->addUsage("");
	opt->addUsage("Usage: TFTP -o host [-u] -s source [-d destination] [-p port] [-h]");
	opt->addUsage("");
	opt->addUsage(" -h --help Prints this help");
	opt->addUsage(" -o Specifies the server ip address");
	opt->addUsage(" -u Upload file to server");
	opt->addUsage(" -s Specifies source file name");
	opt->addUsage(" -d Specifies destination file name");
	opt->addUsage(" -p Specifies the port number to be used");
	opt->addUsage("");

	opt->setFlag("help", 'h'); 
	opt->setOption("open", 'o');
	opt->setFlag("upload", 'u');
	opt->setOption("source", 's');
	opt->setOption("destination", 'd');
	opt->setOption("port", 'p');

	opt->processCommandArgs(argc, argv);

	if (opt->getFlag('h')) {
		opt->printUsage();
	}

	if (opt->getFlag('u')) {
		command = COMMAND_PUT;
	}

	if (opt->getValue('o') == NULL) {

		opt->printUsage();
		return 0;

	} else {

		ip = opt->getValue('o');

	}

	if (opt->getValue('p') != NULL) {

		port = atoi(opt->getValue('p'));

	} else {

		port = TFTP_DEFAULT_PORT;
		
	}

	if (opt->getValue('s') != NULL) {

		source = opt->getValue('s');

	} else {

		opt->printUsage();
		return 0;

	}

	if (opt->getValue('d') != NULL) {

		destination = opt->getValue('d');

	} else {

		strcpy(destination, source);

	}

	delete opt;

	cout << "Starting TFTP client\n";

	TFTPClient client(ip, port);

	if (client.connectToServer() != 1) {

		cout << "Error while connecting to server " << ip << endl;

		return 0;
	  
	}

	if (command == COMMAND_GET) {
		
		if (client.getFile(source, destination)) {

			cout << "File downloaded successfully\n";

		} else {

			cout << "Error has occured in file transfer\n";

		}
		
	}

	if (command == COMMAND_PUT) {

		cout << "Trying to send file " << source << " to " << ip << " " << destination << endl;

		if (client.sendFile(source, destination)) {

			cout << "File sent successfully\n";

		} else {

			cout << "Error has occured in file transfer\n";

		}

	}

	client.~TFTPClient();

	cout << "Disconnected from " << ip << endl;

	return 1;

}