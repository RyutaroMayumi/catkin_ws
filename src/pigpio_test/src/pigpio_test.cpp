#include "ros/ros.h"

#include <pigpiod_if2.h>
#include <std_msgs/Bool.h>


class gpio{
public:
    //construct
    gpio();

private:
    int pi;
    void cb_LED(const std_msgs::Bool::ConstPtr &data);
    ros::NodeHandle nh;
    ros::Subscriber sub_led;
};

gpio::gpio()
{
    pi = pigpio_start(NULL,NULL);
    sub_led = nh.subscribe("/led", 5, &gpio::cb_LED, this);
}

void gpio::cb_LED(const std_msgs::Bool::ConstPtr &data)
{
    if (data->data == 1) {
        gpio_write(pi,26,1);
    } else {
        gpio_write(pi,26,0);
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "gpio");
    gpio gpio;
    ros::spin();
    
    return 0;
}

