#include <array>
#include <chrono>
#include <iostream>

#include "params.h"
#include "position.h"

typedef std::array<GeneratedPosition, SENSORS_COUNT> data_storage_t;
static data_storage_t data;

static uint64_t get_timestamp_us()
{
	using namespace std::chrono;

	time_point<system_clock> now = system_clock::now();
	auto duration = now.time_since_epoch();
	return duration_cast<microseconds>(duration).count();
}

static void init_data(data_storage_t &data)
{
	uint64_t now_us = get_timestamp_us();

	for (unsigned i = 0; i < data.size(); ++i) {
		auto &elem = data[i];

		elem.set_sensorid(i);
		elem.set_timestamp_usec(now_us);
		position::init(elem.mutable_position());
	}
}

int main()
{
	init_data(data);
	return 0;
}
