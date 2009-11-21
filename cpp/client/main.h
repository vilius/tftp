#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

#include "tftp_packet.h"

#define DEBUG

#ifdef WIN32

  #include <winsock2.h>

#else

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/time.h>

  #define SOCKET_ERROR -1

#endif

#define TFTP_ERROR_0 "Not defined, see error message (if any)"
#define TFTP_ERROR_1 "File not found"
#define TFTP_ERROR_2 "Access violation"
#define TFTP_ERROR_3 "Disk full or allocation exceeded"
#define TFTP_ERROR_4 "Illegal TFTP operation"
#define TFTP_ERROR_5 "Unknown transfer ID"
#define TFTP_ERROR_6 "File already exists"
#define TFTP_ERROR_7 "No such user"

#define TFTP_SERVER_ROOT "F:/Documents and Settings/Vilius/Desktop/tftp/server/ftproot/"
#define TFTP_DEFAULT_PORT 5555

#endif
