#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>
#include <pigpiod_if2.h>


class gpio {
private:
    int pi_;
    ros::NodeHandle nh_;
    ros::Timer timer_;
    ros::Publisher pub_;

    void timer_callback(const ros::TimerEvent&)
    {
        int voltage = gpio_read(pi_, 26);
        printf("%d\n", voltage);

        std_msgs::String msg;
        std::stringstream ss;
        ss << "voltage : " << std::to_string(0.5);
        msg.data = ss.str();
        pub_.publish(msg);
    }

public:
    gpio()
    {
        pi_ = pigpio_start(NULL, NULL);
        timer_ = nh_.createTimer(ros::Duration(0.01), &gpio::timer_callback, this);
        pub_ = nh_.advertise<std_msgs::String>("/voltage", 10);
    }

};


int main(int argc, char** argv)
{
    ros::init(argc, argv, "gpio_read");
    gpio gpio;
    ros::spin();
    
    return 0;
}
