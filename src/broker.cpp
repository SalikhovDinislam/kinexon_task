#include <chrono>
#include <thread>

#include "broker.h"
#include "position.h"

static uint64_t time_point_to_us(const SensorBroker::time_point_t &time_point)
{
	using namespace std::chrono;

	auto duration = time_point.time_since_epoch();
	return duration_cast<microseconds>(duration).count();
}

static SensorBroker::time_point_t now()
{
	return SensorBroker::clock_t::now();
}

SensorBroker::SensorBroker(size_t sensors_count, const std::chrono::microseconds &period)
: sensors(sensors_count)
, unprocessed_sensors_count(sensors_count)
, processed_sensors_count(0)
, current_ts(now())
, generation_period(period)
{
	uint64_t timestamp_us = time_point_to_us(current_ts);

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
	while (true) {
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
				uint64_t timestamp = time_point_to_us(current_ts);
				return LockedSensor(elem, timestamp, processed_sensors_count);
			}
		}

		std::unique_lock<std::mutex> lock(mutex);
		auto &sensors_count = unprocessed_sensors_count;
		cond_var.wait(lock, [&sensors_count]{return sensors_count > 0;});
	}
}

int SensorBroker::loop()
{
	while (true) {
		time_point_t next_ts = current_ts + generation_period;
		std::this_thread::sleep_until(next_ts);

		// TODO: what should be the right way to handle it?
		if (processed_sensors_count != sensors.size()) {
			std::cerr << processed_sensors_count << " of " << sensors.size()
				<< " sensors are processed in time" << std::endl;

			return 1;
		}

		current_ts = next_ts;
		for (auto &sensor: sensors) {
			sensor.state.store(SensorState::UNPROCESSED);
		}

		processed_sensors_count.store(0);
		unprocessed_sensors_count.store(sensors.size());
		cond_var.notify_all();
	}

}
