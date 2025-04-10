#include "pid.hpp"

void PID::set_goal(double buf_goal)
{
    goal = buf_goal;
}

double PID::get_goal()
{
    return goal;
}

void PID::set_dt(double time)
{
    dt = time;
}

double PID::do_pid(double actual)
{
    float error = goal - actual;
    float porp = error - pre_error;
    float deriv = porp - pre_porp;
    float du = kp * porp + ki * error * dt + kd * deriv;
    output = du + output;
    pre_error = error;
    pre_porp = porp;
    

    return output;
}