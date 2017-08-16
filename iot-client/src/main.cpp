#pragma warning( disable : 4996)

#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "RequestMessage.h"

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif
#include "TCPClient.h"
#include "EventBase.h"
#include "BlockingSocket.h"
#include <asio.hpp>
using namespace std;
class TestClass
{
private:
	std::string my_string;
	std::vector<int> vec_int;
	std::vector<string> vec_string;
public:
	MSGPACK_DEFINE(my_string, vec_int, vec_string);

	TestClass() {
		my_string = "abcd";
		vec_int.push_back(1);
		vec_int.push_back(2);
		vec_string.push_back("abc123");
		vec_string.push_back("def456");
	}
	~TestClass() {}
};

using namespace asio;

int main(void)
{
	uint8_t buffer[101] = { 0 };

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
	pair<uint8_t *, int32_t> binaryArray;

	RequestMessage requestMessage("token", "192.168.1.1", "192.168.2.1", "GetDeviceList", "void");
	binaryArray = requestMessage.serializeToBytes();

	printf("%d %d\n", (uint32_t)binaryArray.first, binaryArray.second);
	
	EventBase eventbase;
	eventbase.startLoopThread();
	BlockingSocket socket(&eventbase, "192.168.56.1", 8080);

	if (socket.connect() < 0 )
	{
		printf("connect to server failed\r\n");
		eventbase.stop();
		socket.close();
		std::getchar();
		return -1;
	}

	//Sleep(3 * 1000);

	printf("Start reading \r\n");

	socket.write(binaryArray.first, binaryArray.second);
	int32_t readLen = 0;
	readLen = socket.read(buffer, 100, 10000);
	
	printf("Len:%d, %s",readLen, buffer);
	printf("End reading\r\n");

	std::getchar();
	socket.close();
	eventbase.stop();
	return 0;
}