#ifndef PID_HPP
#define PID_HPP


class PID
{
    public:
    PID(double kp, double ki, double kd);
    
    double do_pid(double actual);
    void set_goal(double goal);
    void set_dt(double time);
    double get_goal();
    
    private:
    const double kp;
    const double ki;
    const double kd;
    double dt;
    double goal;
    double pre_error;
    double pre_porp;
    double output;
    double pre_output;
};

inline PID::PID(double kp, double ki, double kd) : kp(kp), ki(ki), kd(kd)
{
    pre_error = 0.0;
    pre_porp = 0.0;
    pre_output = 0.0;
    output = 0.0;
}

extern PID catapult_pid; 
extern PID cone_pid;

#endif