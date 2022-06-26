#include <iostream>
#include <thread>

#include <zmq.hpp>

#include "position.h"
#include "worker.h"

#include "params.h"

static void worker_routine(SensorBroker &broker, size_t start_index)
{
	zmq::context_t context;
	zmq::socket_t socket(context, zmq::socket_type::push);
	socket.connect(ZMQ_SENDER);

	while (true) {
		auto sensor = broker.get_next_sensor(start_index);
		GeneratedPosition &data = sensor.get_data();

		uint64_t old_timestamp_us = data.timestamp_usec();
		uint64_t new_timestamp_us = sensor.get_timestamp_us();
		float time = (new_timestamp_us - old_timestamp_us) / 1000000.;

		position::move(data.mutable_position(), time);
		data.set_timestamp_usec(new_timestamp_us);

		GeneratedPosition pos_copy(data);
		position::apply_noize(data.mutable_position());

		std::string str;
		if (!pos_copy.SerializeToString(&str)) {
			std::cerr << "Can't serialize data" << std::endl;
			continue;
		}

		zmq::message_t msg(str);
		socket.send(msg, zmq::send_flags::none);
	}
}

void start_workers(SensorBroker &broker, size_t count)
{
	size_t sensors_count = broker.size();

	for (size_t i = 0; i < count; ++i) {
		/* spread the threads evenly across the sensors */
		size_t start_index = sensors_count * i / count;
		std::thread worker(worker_routine, std::ref(broker), start_index);
		worker.detach();
	}
}
