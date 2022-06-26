#ifndef GUARD_CBE30D31_9FC5_4656_A18E_2202843CE42F
#define GUARD_CBE30D31_9FC5_4656_A18E_2202843CE42F

#include <memory>

#include "broker.h"

void start_workers(std::shared_ptr<SensorBroker> &broker, size_t count);

#endif
