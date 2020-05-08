#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "mcp3008Spi.h"

#include <sstream>
#include <cmath>


static const std::string ROS_NAMESPACE = "range_sensor";
static const std::string DEFAULT_DEVICE_NAME = "/dev/spidev0.0";
static const uint DEFAULT_SPI_MODE = SPI_MODE_0;
static const uint DEFAULT_SPI_SPEED = 500000;   // [Hz]
static const uint DEFAULT_SPI_BPW = 8;          // [bit]
static const double DEFAULT_DURATION = 1.00;    // [sec]
static const double DEFAULT_REF_VOLTS = 5.0;    // [V]
// static mcp3008Spi a2d(
//     DEFAULT_DEVICE_NAME, 
//     DEFAULT_SPI_MODE, 
//     DEFAULT_SPI_SPEED, 
//     DEFAULT_SPI_BPW
// );

static inline float convert_volts(int data, float vref)
{
    return (data * vref) / float(1023);
}

static inline float convert_dist(float volts)
{
    return 1.0/std::pow(0.266, -1.0/0.975) * std::pow(volts, -1.0/0.975);
}


class Range_sensor {
private:
    // mcp3008Spi a2d_; // 通常のメンバとして定義すると実行時エラーで停止する
    mcp3008Spi* a2d_;
    double vref_;
    ros::NodeHandle nh_;
    ros::Timer timer_;
    ros::Publisher pub_;

    void timer_callback(const ros::TimerEvent&)
    {
        int a2dVal = 0;
        int a2dChannel = 0;
        unsigned char data[3];

        data[0] = 1;  //  first byte transmitted -> start bit
        data[1] = 0b10000000 |( ((a2dChannel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
        data[2] = 0; // third byte transmitted....don't care

        // a2d.spiWriteRead(data, sizeof(data));   // 実行時エラーで停止する
        a2d_->spiWriteRead(data, sizeof(data));

        a2dVal = 0;
        a2dVal = (data[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
        a2dVal |=  (data[2] & 0xff);
        // std::cout << "The Result is: " << a2dVal << std::endl;

        // 測距データを配信する
        float dist = convert_dist(convert_volts(a2dVal, vref_));
        std_msgs::Float32 msg;
        msg.data = dist;
        pub_.publish(msg);
    }

public:
    Range_sensor()
    : a2d_(nullptr), 
      vref_(0.0)
    {
        // ノードの引数を受け取る
        std::string device_name;
        double duration;
        if (!nh_.getParam(ROS_NAMESPACE + "/device_name", device_name)) {
            device_name = DEFAULT_DEVICE_NAME;
        }
        if (!nh_.getParam(ROS_NAMESPACE + "/duration", duration)) {
            duration = DEFAULT_DURATION;
        }
        if (!nh_.getParam(ROS_NAMESPACE + "/ref_volts", vref_)) {
            vref_ = DEFAULT_REF_VOLTS;
        }

        // SPI デバイスの設定
        // a2d = mcp3008Spi(device_name, DEFAULT_SPI_MODE, DEFAULT_SPI_SPEED, DEFAULT_SPI_BPW);  // メンバとして定義すると実行時エラーで停止する
        // 宣言時に初期化しないとメンバ関数の実行時にエラーが発生するため、ポインタアクセスを利用する
        a2d_ = new mcp3008Spi(device_name, DEFAULT_SPI_MODE, DEFAULT_SPI_SPEED, DEFAULT_SPI_BPW);  // メンバとして定義すると実行時エラーで停止する

        // パブリッシャの定義
        timer_ = nh_.createTimer(ros::Duration(duration), &Range_sensor::timer_callback, this);
        pub_ = nh_.advertise<std_msgs::Float32>("/range", 10);

        ROS_INFO("Range_sensor instance created");
    }

    ~Range_sensor()
    {
        delete(a2d_);
    }

};


int main(int argc, char** argv)
{
    ros::init(argc, argv, ROS_NAMESPACE);
    Range_sensor range_sensor;
    ros::spin();

    return 0;
}
