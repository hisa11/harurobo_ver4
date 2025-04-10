#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP
#include "mbed.h"
#include "pid.hpp"

void updateCatapultState(bool R2, bool catapult_limit, int catapult_revolutions);

#endif