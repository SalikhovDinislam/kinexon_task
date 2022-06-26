#include <iostream>

#include <zmq.hpp>

#include "params.h"
#include "position.pb.h"

int main()
{
	zmq::context_t context;
    zmq::socket_t socket(context, zmq::socket_type::pull);
    socket.bind(ZMQ_RECEIVER);
    
	while (true) {
		zmq::message_t data;
		socket.recv(data, zmq::recv_flags::none);

		GeneratedPosition position;
		position.ParseFromString(data.to_string());

		std::cout << position.DebugString() << std::endl;
	}

	return 0;
}
