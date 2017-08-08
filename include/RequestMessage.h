#pragma once

#include <stdint.h>
#include <string>
#include <msgpack.hpp>

using namespace std;
class RequestMessage
{
public:
	string _token;
	string _source;
	string _target;	
	string _method;
	string _params;

	MSGPACK_DEFINE(_token, _source, _target, _method, _params);

	RequestMessage(string token, string source, string target, string method, string params):
		_token(token),_source(source),_target(target),_method(method),_params(params)
	{}
	~RequestMessage() {}

	int32_t parseFromBytes(uint8_t * data, int size)
	{

		return 0;
	}
	/*
	int32_t serializeToBytes(uint8_t * data)
	{
		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, *this);
		// deserialize it.
		msgpack::object_handle oh =
			msgpack::unpack(sbuf.data(), sbuf.size());

		for (uint8_t i = 0; i < sbuf.size(); i++)
		{
			printf("%02x ", (uint8_t)sbuf.data()[i]);
		}
		// print the deserialized object.
		msgpack::object obj = oh.get();
		std::cout << obj << std::endl;
		return 0;
	}*/

	pair<uint8_t *, int32_t> serializeToBytes(void)
	{
		pair<uint8_t *, int32_t> binaryArray;
		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, *this);

		
		binaryArray.first = (uint8_t *)calloc(sbuf.size() + 3, 1);
		binaryArray.second = sbuf.size()+3;

		memcpy(binaryArray.first + 3, sbuf.data(), sbuf.size());

		for (uint8_t i = 0; i < binaryArray.second; i++)
		{
			printf("%02x ", (uint8_t)binaryArray.first[i]);
		}

		return binaryArray;
	}
};


/* 
type - Normal
method - Login
params - username:xxxx, password=xxxx
*/
/*
-->
type = Tunnel;
source = "User0001"
target = "Device0001"
method = "GetDeivceCapability"
params = "";

<--
type = 0x11;
source = "Device0001"
target = "User0001"
restult = "{\" On \",\" Off \"}"

*/