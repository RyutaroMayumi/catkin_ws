#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"
#include "opencv2/highgui/highgui.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <boost/circular_buffer.hpp>


// 定数
static std::string ros_namespace = "slideshow";
static std::string default_path = "/home/user/images";
static double default_duration = 1.00;

/**
 * @class SlideShow
 * @brief フォルダ内の画像を切り替えながら配信する
 */
class SlideShow {
private:
    std::vector<std::string> img_paths_;
    boost::circular_buffer<cv::Mat> images_;
    boost::circular_buffer<cv::Mat>::iterator img_iterator_;
    ros::NodeHandle nh_;
    ros::Timer timer_;
    ros::Publisher pub_;
    // ImageTransport を使う場合（実行時エラーになるため保留）
    // image_transport::ImageTransport img_transporter_;
    // image_transport::Publisher pub_;

public:

    /**
     * @brief コンストラクタ
     */
    SlideShow()
    : img_paths_(),
      images_(),
      img_iterator_()
    //   img_transporter_(nh_)
    {
        // ノードの引数を受け取る
        std::string path;
        double duration;
        if (!nh_.getParam(ros_namespace + "/path", path)) {
            path = default_path;
        }
        if (!nh_.getParam(ros_namespace + "/duration", duration)) {
            duration = default_duration;
        }

        // 指定されたパスにある画像を読み込む
        set_images(path);

        // パブリッシャの定義
        // timer_ = nh_.createTimer(ros::Duration(duration), timer_callback);   // これだとコンパイルエラー
        timer_ = nh_.createTimer(ros::Duration(duration), &SlideShow::timer_callback, this);
        pub_ = nh_.advertise<sensor_msgs::Image>("/image_raw", 10);
        // ImageTransport を使う場合
        // pub_ = img_transporter_.advertise("/image_raw", 10);

        ROS_INFO("SlideShow instance created");
    }

    /**
     * @brief 画像を読み込む
     * @param path 画像または画像を含むフォルダへのパス
     */
    void set_images(std::string path)
    {
        // ファイル名の読み込み
        DIR* dp;
        dirent* entry;
        dp = opendir(path.c_str());
        if (dp != NULL) {
            // printf("this is a directory\n");
            do {
                entry = readdir(dp);
                if (entry != NULL && entry->d_type == DT_REG) {
                    img_paths_.push_back(path + "/" + std::string(entry->d_name));
                }
            } while (entry != NULL);
        } else {
            // printf("this is a file\n");
            img_paths_.push_back(path);
        }

        // 循環バッファのサイズを変更
        images_.set_capacity(img_paths_.size());

        // 画像の読み込み
        for (const auto& f : img_paths_) {
            cv::Mat img;
            struct stat st;
            if (0 == stat(f.c_str(), &st)) {
                img = cv::imread(f, cv::IMREAD_COLOR);
            }

            if (!img.empty()) {
                printf("imread: %s is loaded\n", f.c_str());
                images_.push_back(img);
            } else {
                printf("imread: %s is not an image file\n", f.c_str());
            }
        }

        // 先頭の画像をセット
        if (!images_.empty()) {
            printf("%ld files are loaded\n", images_.size());
            img_iterator_ = images_.begin();
        }
    }

    /**
     * @brief タイマーによるコールバック関数
     */
    void timer_callback(const ros::TimerEvent&)
    {
        // 画像データを配信する
        cv_bridge::CvImage cv_img;
        cv_img.header = std_msgs::Header();
        cv_img.header.stamp = ros::Time::now();
        cv_img.encoding = "bgr8";
        cv_img.image = *img_iterator_;
        pub_.publish(cv_img.toImageMsg());
        // ImageTransport を使う場合
        // sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", *img_iterator_).toImageMsg();
        // pub_.publish(msg);

        // 次の画像に遷移
        if (1 < images_.size()) {
            // circular_buffer のサイズが１の場合、++で領域外にアクセスしてしまう
            images_.rotate(++img_iterator_);
        }
        //boost::next(img_iterator_);
    }

};


/**
 * @brief slideshow のメイン関数
 * @param argc 引数リストの数
 * @param argv 引数リスト
 */
int main(int argc, char** argv)
{
    ros::init(argc, argv, ros_namespace);
    SlideShow slideShow;
    ros::spin();

    return 0;
}
