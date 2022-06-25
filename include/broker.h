#ifndef GUARD_6F569C4D_2C43_4409_839E_17F465F56A1E
#define GUARD_6F569C4D_2C43_4409_839E_17F465F56A1E

#include <condition_variable>
#include <mutex>
#include <vector>

#include "position.pb.h"

class SensorBroker {
private:
	enum class SensorState {
		UNPROCESSED,
		IN_PROCESS,
		PROCESSED
	};

	struct SensorData {
		GeneratedPosition data;
		std::atomic<SensorState> state;
		std::mutex mutex;
	};

public:
	SensorBroker(size_t sensors_count, const std::chrono::microseconds &period);

	SensorBroker() = delete;
	SensorBroker(const SensorBroker&) = delete;
	SensorBroker(const SensorBroker&&) = delete;
	SensorBroker& operator=(const SensorBroker&) = delete;
	SensorBroker& operator=(const SensorBroker&&) = delete;

	class LockedSensor;
	LockedSensor get_next_sensor(size_t index);
	int loop();
	size_t size() const
	{
		return sensors.size();
	}

private:
	std::vector<SensorData> sensors;
	std::atomic_size_t unprocessed_sensors_count;
	std::atomic_size_t processed_sensors_count;
	uint64_t timestamp_us;
	std::chrono::microseconds generation_period;
	std::mutex mutex;
	std::condition_variable cond_var;
};

class SensorBroker::LockedSensor {
public:
	GeneratedPosition &get_data() const
	{
		return data.data;
	}

	uint64_t timestamp() const
	{
		return timestamp_us;
	}

	LockedSensor(const LockedSensor &&);

	LockedSensor(const LockedSensor &) = delete;
	LockedSensor operator=(const LockedSensor &) = delete;
	LockedSensor operator=(const LockedSensor &&) = delete;

	~LockedSensor()
	{
		data.state.store(SensorState::PROCESSED);
		++processed_count;
		data.mutex.unlock();
	}
private:
	friend SensorBroker;

	LockedSensor(SensorData &sensor_data, uint64_t ts_us, std::atomic_size_t& count)
	: data(sensor_data)
	, timestamp_us(ts_us)
	, processed_count(count)
	{
		data.state.store(SensorState::IN_PROCESS);
	}

	SensorData &data;
	uint64_t timestamp_us;
	std::atomic_size_t &processed_count;
};

#endif
