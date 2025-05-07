// #include "pid.hpp"
// #include "mbed.h"
// void PID::set_goal(float buf_goal)
// {
//     goal = buf_goal;
// }

// float PID::get_goal()
// {
//     return goal;
// }

// void PID::set_dt(float time)
// {
//     dt = time;
// }

// float PID::do_pid(float actual)
// {
//     float error = goal - actual;
//     static Timer timer;
//     static bool timer_started = false;
//     if (!timer_started)
//     {
//         timer.start();
//         timer_started = true;
//     }
//     _integral += error * dt;
//     const float integral_max = 1000.0f; // Adjust as needed
//     const float integral_min = -1000.0f; // Adjust as needed
//     if (_integral > integral_max) _integral = integral_max;
//     if (_integral < integral_min) _integral = integral_min;

//     float p_tream = kp * error;
//     float i_tream = ki * _integral;
//     float d_tream = kd * (error - pre_error) / dt;
//     float rate = (p_tream + i_tream + d_tream);
//     pre_error = error;

//     return rate;
// }