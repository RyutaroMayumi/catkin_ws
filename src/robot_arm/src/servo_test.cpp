#include "ros/ros.h"

#include <pigpiod_if2.h>
#include <std_msgs/Bool.h>


int main(int argc, char** argv)
{
    ros::init(argc, argv, "servo");
    ros::NodeHandle n;

    int pi;
    pi = pigpio_start(NULL, NULL);

    ros::Rate rate(1);
    set_servo_pulsewidth(pi, 26, 1500);
    
    int i = 0;
    while ( ros::ok() ){
        
        if (i == 0) {
            set_servo_pulsewidth(pi, 26, 1400);
            i = 1;
        } else {
            set_servo_pulsewidth(pi, 26, 1600);
            i = 0;
        }
        
        ROS_INFO("%d", i);
        ros::spinOnce();
        rate.sleep();
    }
    
    set_servo_pulsewidth(pi, 26, 0);
    
    return 0;
}
