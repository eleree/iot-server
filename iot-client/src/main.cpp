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


int main(void)
{
	char buffer[101] = { 0 };

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
	pair<uint8_t *, int32_t> binaryArray;

	RequestMessage requestMessage("token", "192.168.1.1", "192.168.2.1", "GetDeviceList", "void");
	binaryArray = requestMessage.serializeToBytes();

	printf("%d %d\n", (uint32_t)binaryArray.first, binaryArray.second);
	
	TCPClient client("192.168.56.1", 8080, 0);
	client.connect("192.168.56.1", 8080,5000);
	printf("Write Data\r\n");
	client.write(binaryArray.first, binaryArray.second, 5000);
	printf("Read Data\r\n");
	time_t nowtime;
	nowtime = time(NULL); //获取当前时间  
	cout << nowtime << endl;
	client.read((uint8_t*)buffer, 100, 10000);
	nowtime = time(NULL); //获取当前时间  
	cout << nowtime << endl;
	cout << buffer << endl;
	client.write(binaryArray.first, binaryArray.second, 5000);
	return 0;


	// build socket
	int port = 8080;
	struct sockaddr_in my_address;
	memset(&my_address, 0, sizeof(my_address));
	my_address.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.56.1", &my_address.sin_addr);
	my_address.sin_port = htons(port);

	// build event base
	struct event_base* base = event_base_new();

	// set TCP_NODELAY to let data arrive at the server side quickly
	evutil_socket_t fd;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	struct bufferevent* conn = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	int enable = 1;
	//if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable)) < 0)
	//	printf("ERROR: TCP_NODELAY SETTING ERROR!\n");
	//bufferevent_setcb(conn, NULL, NULL, NULL, NULL); // For client, we don't need callback function
	bufferevent_enable(conn, EV_WRITE|EV_READ);
	if (bufferevent_socket_connect(conn, (struct sockaddr*)&my_address, sizeof(my_address)) == 0)
		printf("connect success\n");

	// start to send data
	bufferevent_write(conn, binaryArray.first, binaryArray.second);
	
	printf("Sleep\r\n");	
	Sleep(10000);


	// check the output evbuffer
	struct evbuffer* output = bufferevent_get_output(conn);
	int len = 0;
	len = evbuffer_get_length(output);
	printf("output buffer has %d bytes left\n", len);

	struct timeval ten_sec;

	ten_sec.tv_sec = 10;
	ten_sec.tv_usec = 0;

	event_base_loopexit(base, &ten_sec);
	event_base_dispatch(base);

	int32_t readLen = 0;
	readLen = bufferevent_read(conn, buffer, 100);
	printf("readlen %d\r\n", readLen);

	bufferevent_free(conn);
	event_base_free(base);

	free(binaryArray.first);

	return 0;
}