#ifndef GUARD_8C2EF127_0C4E_4CB9_95A6_09580FB373ED
#define GUARD_8C2EF127_0C4E_4CB9_95A6_09580FB373ED

#include <chrono>
#include <cstdint>

static const float HUMAN_MAX_SPEED = 12.5; /* meters per second */

static const float MIN_X = 0.;
static const float MIN_Y = 0.;
static const float MIN_Z = 0.;
static const float MAX_X = 100.;
static const float MAX_Y = 100.;
static const float MAX_Z = 3.;

static const float NOISE_XY = 0.3;

static const unsigned SENSORS_COUNT = 10;
static const auto GENERATION_PERIOD = std::chrono::seconds(1);
static const unsigned WORKERS_COUNT = 2;

#endif
