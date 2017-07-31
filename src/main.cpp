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
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include "AuthServer.h"
#include "pthread.h"

#include <event2/event-config.h>

void *func(void *arg)
{
	printf("this is a new thread\r\n");
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t threadId;
	pthread_create(&threadId, NULL, func, NULL);
	pthread_create(&threadId, NULL, func, NULL);
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
