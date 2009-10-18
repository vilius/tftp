#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <exception>

#define DEBUG

#ifdef WIN32

  #include <winsock2.h>

#else

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netinet/in.h>

#endif

#define PORT 5555

#endif
