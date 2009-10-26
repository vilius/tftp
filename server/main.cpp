/**

	TFTP Server
	Atliko: Vilius Paulauskas [3 kursas, IV grupe, MIF PS]
	Uzduotis: TFTP

**/

#include "main.h"
#include "tftp_server.h"
#include "lib/anyoption.h"

using namespace std;

int main(int argc, char **  argv) {

	int port = TFTP_DEFAULT_PORT;
	char* file_dir;

	AnyOption *opt = new AnyOption();

	opt->addUsage("");
    opt->addUsage("Usage: TFTP [-h] [-p port]");
	opt->addUsage("");
	opt->addUsage(" -h Prints this help");
	opt->addUsage(" -p Specifies the server port, if none is specified default 5555 is used");
	opt->addUsage("");

	opt->setFlag('h'); 
    opt->setOption("port", 'p');

	opt->processCommandArgs(argc, argv);

	if (opt->getFlag('h')) {
		opt->printUsage();
	}

	if (opt->getValue('p') != NULL) {
		try {
			port = atoi(opt->getValue('p'));
		} catch (char* str) {
			port = 5555;
		}
		
	}

	delete opt;

	cout << "Starting TFTP server on port " << port << endl;

	#ifdef DEBUG
		TFTPServer server(port, "ftproot/");
	#else
		TFTPServer server(port, "ftproot/");
	#endif
		
	cout << "TFTP Server was shut down" << endl;

	return 1;

}
