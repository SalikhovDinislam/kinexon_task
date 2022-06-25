#include "broker.h"

int main()
{
	SensorBroker broker(SENSORS_COUNT, GENERATION_PERIOD);

	return broker.loop();
}
