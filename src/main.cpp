#include "broker.h"
#include "params.h"
#include "worker.h"

int main()
{
	// TODO: Take count from ENV variables
	SensorBroker broker(SENSORS_COUNT, GENERATION_PERIOD);
	start_workers(broker, WORKERS_COUNT);
	return broker.loop();
}
