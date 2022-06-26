#include <memory>

#include "broker.h"
#include "params.h"
#include "worker.h"

int main()
{
	// TODO: Take count from ENV variables
	std::shared_ptr<SensorBroker> broker = std::make_shared<SensorBroker>(SENSORS_COUNT, GENERATION_PERIOD);
	start_workers(broker, WORKERS_COUNT);
	return broker->loop();
}
