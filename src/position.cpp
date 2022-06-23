#include <cmath>
#include <random>
#include <thread>

#include "params.h"
#include "position.h"

namespace position
{
static float get_random(float min, float max)
{
	using namespace std;

	/* add time to get seed unique for the run */
	static const thread_local unsigned seed = hash<thread::id>()(this_thread::get_id());
	static thread_local mt19937 generator(seed);

	uniform_real_distribution<float> dist(min, max);

	return dist(generator);
}

void init(Data3d *position)
{
	position->set_x(get_random(MIN_X, MAX_X));
	position->set_y(get_random(MIN_Y, MAX_Y));
	position->set_z(get_random(MIN_Z, MAX_Z));
}

static float move(float pos, float delta, float min, float max)
{
	pos += delta;

	if (pos < min) {
		return min;
	}

	if (pos > max) {
		return max;
	}

	return pos;
}

void move(Data3d *position, float time)
{
	float angle = get_random(0., 2 * 3.1416);
	float distance = get_random(0., time * HUMAN_MAX_SPEED);
	float delta_x = std::cos(angle) * distance;
	float delta_y = std::sin(angle) * distance;

	float new_x = move(position->x(), delta_x, MIN_X, MAX_X);
	float new_y = move(position->y(), delta_y, MIN_Y, MAX_Y);
	float new_z = get_random(MIN_Z, MAX_Z);

	position->set_x(new_x);
	position->set_y(new_y);
	position->set_z(new_z);
}

void apply_noize(Data3d *position)
{
	position->set_x(position->x() + get_random(-NOISE_XY, NOISE_XY));
	position->set_y(position->y() + get_random(-NOISE_XY, NOISE_XY));
}

} /* namespace position */
