#include "ros/ros.h"
#include "sensor_msgs/JointState.h"

#include <cmath>
#include <vector>
#include <map>
#include <pigpiod_if2.h>

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#define MIN_PULSE_WIDTH  500
#define MID_PULSE_WIDTH 1500
#define MAX_PULSE_WIDTH 2500


static const std::map<std::string, unsigned char> DEFINED_GPIO = { 
    { "joint0", 17 }, 
    { "joint1", 27 }, 
    { "joint2", 22 }, 
    { "joint3",  5 }, 
    { "joint4",  6 }, 
    { "left_gripper_joint", 16} 
};

static const double DEFAULT_ANGLE_DEG = 0.0;

static inline double deg2rad(double deg)
{
    return deg / 360.0 * 2.0 * M_PI;
}

static inline double rad2deg(double rad)
{
    return rad / 2.0 / M_PI * 360.0;
}

static inline uint rad2pw(double rad)
{
    // pw_per_rad = (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / M_PI
    // rad_shift = rad * pw_per_rad
    // pw = rad_shift + pw_middle
    return (uint)(rad * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / M_PI + MID_PULSE_WIDTH);
}

class MotorDriver {
private:
    std::map<std::string, unsigned char> gpios_;
    int pi_;
    ros::NodeHandle nh_;
    ros::Subscriber sub_;

    void callback(const sensor_msgs::JointState& msg)
    {
        // print_joint_states(msg.name, msg.position);

        for (int i = 0; i < msg.name.size(); ++i) {
            if (gpios_.count(msg.name[i])) {
                set_joint(msg.name[i], msg.position[i]);
            }
        }
    }

    void init_joints(double rad)
    {
        for (const auto& pair : gpios_) {
            set_servo_pulsewidth(pi_, pair.second, rad2pw(rad));
        }
    }

    void set_joint(std::string joint_name, double rad)
    {
        set_servo_pulsewidth(pi_, gpios_[joint_name], rad2pw(rad));
    }

    void print_joint_states(std::vector<std::string> joint_names, std::vector<double> joint_position)
    {
        for (int i = 0; i < joint_names.size(); ++i) {
            uint pulse_width = rad2pw(joint_position[i]);
            unsigned char port_no = gpios_[joint_names[i]];
            printf("%s : %lf/%d -> %d\n", joint_names[i].c_str(), joint_position[i], pulse_width, port_no);
        }
    }

public:
    MotorDriver()
    : gpios_{},
      pi_(0)
    {
        sub_ = nh_.subscribe("joint_states", 10, &MotorDriver::callback, this);
        gpios_ = DEFINED_GPIO;
        pi_ = pigpio_start(NULL, NULL);

        init_joints(deg2rad(DEFAULT_ANGLE_DEG));
    }
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "motor_driver");
    MotorDriver driver;
    ros::spin();
    
    return 0;
}
