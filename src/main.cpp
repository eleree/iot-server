/*
This example program provides a trivial server program that listens for TCP
connections on port 9995.  When they arrive, it writes a short message to
each client connection, and closes each connection once it is flushed.

Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/
#pragma warning( disable : 4996)

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include "pthread.h"

#include "AuthServer.h"
#include "SQLConnection.h"


#include <event2/event-config.h>

using namespace std;
int main(int argc, char **argv)
{
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	printf("starting iot server...\n");
	AuthServer authServer;
	authServer.start();

	for (;;)
		Sleep(1);

}
