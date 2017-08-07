#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

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
	TestClass testClass;

	// serialize it into simple buffer.
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, testClass);

	// deserialize it.
	msgpack::object_handle oh =
		msgpack::unpack(sbuf.data(), sbuf.size());

	// print the deserialized object.
	msgpack::object obj = oh.get();
	std::cout << obj << std::endl; 

	// convert it into statically typed object.
	TestClass rClass;
	obj.convert(rClass);

	return 0;
}