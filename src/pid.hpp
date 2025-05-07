#ifndef PID_HPP
#define PID_HPP

class PID {
public:
    PID(float kp, float ki, float kd);
    void set_goal(float goal);
    void set_dt(float dt);
    void get_send_power(int id);
    float do_pid(float actual);

private:
    const float kp, ki, kd;
    float dt, goal;
    float prev_error;       // e(k-1)
    float prev_prev_error;  // e(k-2)
    float prev_output;      // u(k-1)
};

inline PID::PID(float _kp, float _ki, float _kd)
    : kp(_kp), ki(_ki), kd(_kd),
      dt(0.01f), goal(0.0f),
      prev_error(0.0f), prev_prev_error(0.0f),
      prev_output(0.0f)
{}

inline void PID::set_goal(float _goal) {
    goal = _goal;
}

inline void PID::set_dt(float _dt) {
    dt = _dt;
}

inline float PID::do_pid(float actual) {
    // 現在の誤差
    float error = goal - actual;

    // 差分形式PID Δu の計算
    float delta_u = kp * (error - prev_error)
                  + ki * error * dt
                  + kd * (error - 2.0f * prev_error + prev_prev_error) / dt;

    // 現在の制御出力
    float output = prev_output + delta_u;

    // 状態更新
    prev_prev_error = prev_error;
    prev_error = error;
    prev_output = output;

    return output;
}

#endif // PID_HPP
