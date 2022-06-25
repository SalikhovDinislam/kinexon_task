#include <chrono>
#include <thread>

#include "broker.h"
#include "position.h"

static uint64_t get_timestamp_us()
{
	using namespace std::chrono;

	time_point<system_clock> now = system_clock::now();
	auto duration = now.time_since_epoch();
	return duration_cast<microseconds>(duration).count();
}

SensorBroker::SensorBroker(size_t sensors_count, const std::chrono::microseconds &period)
: sensors(sensors_count)
, unprocessed_sensors_count(sensors_count)
, processed_sensors_count(0)
, generation_period(period)
{
	timestamp_us = get_timestamp_us();

	for (size_t i = 0; i < sensors.size(); ++i) {
		sensors[i].state.store(SensorState::UNPROCESSED);

		auto &elem = sensors[i].data;
		elem.set_sensorid(i);
		elem.set_timestamp_usec(timestamp_us);
		position::init(elem.mutable_position());
	}
}

SensorBroker::LockedSensor SensorBroker::get_next_sensor(size_t &index)
{
	while (unprocessed_sensors_count > 0) {
		index = (index + 1) % sensors.size();
		auto &elem = sensors[index];

		/*
		We check sensor's state the second time to make sure that
		it wasn't processed before we acquire the mutex
		*/
		if (elem.state.load() == SensorState::UNPROCESSED
		 && elem.mutex.try_lock()
		 && elem.state.load() == SensorState::UNPROCESSED) {
			--unprocessed_sensors_count;
			return LockedSensor(elem, timestamp_us, processed_sensors_count);
		}
	}

	{
		std::unique_lock<std::mutex> lock(mutex);
		auto &sensors_count = unprocessed_sensors_count;
		if (sensors_count > 0) {
			return get_next_sensor(index);
		}
		cond_var.wait(lock, [&sensors_count]{return sensors_count > 1;});
	}

	return get_next_sensor(index);
}

int SensorBroker::loop()
{
	/* TODO: iteration takes a bit longer than generation_period */
	uint64_t now_us = get_timestamp_us();
	auto elapsed = std::chrono::microseconds(now_us - timestamp_us);
	if (elapsed < generation_period) {
		auto duration = generation_period - elapsed;
		std::this_thread::sleep_for(duration);
	}

	// TODO: what should be the right way to handle it?
	if (processed_sensors_count != sensors.size()) {
		std::cerr << processed_sensors_count << " of " << sensors.size()
			<< " sensors are processed in time" << std::endl;

		return 1;
	}

	timestamp_us = get_timestamp_us();
	for (auto &sensor: sensors) {
		sensor.state.store(SensorState::UNPROCESSED);
	}

	{
		processed_sensors_count.store(0);

		std::unique_lock<std::mutex> lock(mutex);
		unprocessed_sensors_count.store(sensors.size());
		cond_var.notify_all();
	}

	return loop();
}
