#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include <iostream>
#include <fstream>


static const std::string ROS_NAMESPACE = "webcam_detect";
static const std::string MODEL_FILENAME = "/home/pi/Workspace/catkin_ws/src/webcam_detect/resource/detect.tflite";
static const std::string LABEL_FILENAME = "/home/pi/Workspace/catkin_ws/src/webcam_detect/resource/labelmap.txt";
static const int IMAGE_WIDTH = 640;
static const int IMAGE_HEIGHT = 480;
static const float MIN_CONF_THRESHOLD = 0.5;

class Detector {
private:
    int model_batch_;
    int model_height_;
    int model_width_;
    int model_channels_;
    int model_type_;
    int image_width_;
    int image_height_;

    // for tflite
    std::unique_ptr<tflite::FlatBufferModel> model_;
    tflite::ops::builtin::BuiltinOpResolver resolver_;
    std::unique_ptr<tflite::Interpreter> interpreter_;
    std::vector<std::string> labels_;
    // for ros
    ros::NodeHandle nh_;
    // ros::Subscriber sub_;
    // ros::Publisher pub_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber sub_;
    image_transport::Publisher pub_;

    void callback(const sensor_msgs::ImageConstPtr& msg)
    {
        // cv::Mat への変換
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e) {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }

        // リサイズ
        cv::Mat resized(model_height_, model_width_, cv_ptr->image.type());
        cv::resize(cv_ptr->image, resized, resized.size(), cv::INTER_CUBIC);
        // BGR -> RGB
        cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);

        // 入力テンソルにデータをセットする
        uint8_t* input = interpreter_->typed_input_tensor<uint8_t>(0);
        memcpy(input, resized.reshape(0, 1).data, sizeof(uint8_t) * 1 * model_height_ * model_width_ * model_channels_);

        // 認識を実行する
        TfLiteStatus status = interpreter_->Invoke();

        // 認識結果を取得する
        if (status == kTfLiteOk)
        {
            float32_t* boxes = interpreter_->typed_output_tensor<float32_t>(0);
            float32_t* classes = interpreter_->typed_output_tensor<float32_t>(1);
            float32_t* scores = interpreter_->typed_output_tensor<float32_t>(2);
            float32_t* num = interpreter_->typed_output_tensor<float32_t>(3);
            int number_of_objects = int(num[0]);

            for (int i = 0; i < number_of_objects; ++i)
            {
                if (MIN_CONF_THRESHOLD < scores[i] && scores[i] <= 1.0)
                {
                    // Draw box
                    int ymin = std::max(1, int(boxes[i*4 + 0] * image_height_));
                    int xmin = std::max(1, int(boxes[i*4 + 1] * image_width_));
                    int ymax = std::min(int(boxes[i*4 + 2] * image_height_), image_height_);
                    int xmax = std::min(int(boxes[i*4 + 3] * image_width_), image_width_);
                    cv::rectangle(cv_ptr->image, cv::Point(xmin,ymin), cv::Point(xmax,ymax), cv::Scalar(255,0,0), 2);

                    // Draw label
                    int font_face = cv::FONT_HERSHEY_SIMPLEX;
                    double font_scale = 0.7;
                    int thickness = 2;
                    int baseline = 0;
                    std::string object_name = labels_[int(classes[i])];
                    std::string label = object_name + ": " + std::to_string(int(scores[i]*100)) + "%";
                    cv::Size label_size = cv::getTextSize(label, font_face, font_scale, thickness, &baseline);
                    baseline += thickness;
                    int label_ymin = std::max(ymin, label_size.height + 10);
                    cv::rectangle(cv_ptr->image, cv::Point(xmin, label_ymin - label_size.height - 10), cv::Point(xmin + label_size.width, label_ymin + baseline - 10), cv::Scalar::all(255), cv::FILLED);
                    cv::putText(cv_ptr->image, label, cv::Point(xmin, label_ymin - 7), font_face, font_scale, cv::Scalar::all(0), thickness);
                }
            }
        }

        // 検知結果の表示
        // cv::imshow("window", cv_ptr->image);
        // cv::waitKey(3);

        // 検知結果の配信
        pub_.publish(cv_ptr->toImageMsg());
    }

public:
    Detector()
    : it_(nh_)
    {
        // ノードの引数を受け取る
        std::string model_path, label_path;
        if (!nh_.getParam(ROS_NAMESPACE + "/model_path", model_path)) {
            model_path = MODEL_FILENAME;
        }
        if (!nh_.getParam(ROS_NAMESPACE + "/label_path", label_path)) {
            label_path = LABEL_FILENAME;
        }
        if (!nh_.getParam(ROS_NAMESPACE + "/image_width", image_width_)) {
            image_width_ = IMAGE_WIDTH;
        }
        if (!nh_.getParam(ROS_NAMESPACE + "/image_height", image_height_)) {
            image_height_ = IMAGE_HEIGHT;
        }

        // モデルのロード
        std::cout << "loading model file : " << model_path << std::endl;
        model_ = tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
        if (model_ != nullptr) {
            // インタープリターのビルド
            tflite::InterpreterBuilder(*model_, resolver_)(&interpreter_);
        }
        if (interpreter_ != nullptr) {
            // バッファの割り当て
            interpreter_->AllocateTensors();
        }

        // ラベルの読み込み
        std::cout << "loading label file : " << label_path << std::endl;
        std::ifstream file(label_path.c_str());
        if (file) {
            std::string line;
            while (std::getline(file, line))
            {
                labels_.push_back(line);
            }
            labels_.erase(labels_.begin());
        }

        // モデルの入力パラメータを取得する
        int tensor_index;
        tensor_index = interpreter_->inputs()[0];
        TfLiteIntArray* in_dims = interpreter_->tensor(tensor_index)->dims;
        model_batch_ = in_dims->data[0];
        model_height_ = in_dims->data[1];
        model_width_ = in_dims->data[2];
        model_channels_ = in_dims->data[3];
        model_type_ = interpreter_->tensor(tensor_index)->type;
        std::cout << "input : (batch, height, width, channels, type) = ("
                << std::to_string(model_batch_)     << ", "
                << std::to_string(model_height_)    << ", "
                << std::to_string(model_width_)     << ", "
                << std::to_string(model_channels_)  << ", "
                << std::to_string(model_type_)      << ")"
                << std::endl;

        // ROSの設定
        // sub_ = nh_.subscribe("usb_cam/image_raw", 1, &Detector::callback, this);
        // pub_ = nh_.advertise<sensor_msgs::Image>("image_rects", 1);
        sub_ = it_.subscribe("/usb_cam/image_raw", 1, &Detector::callback, this);
        pub_ = it_.advertise("/image_rects", 1);
    }

};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "webcam_detect");
    Detector detector;
    ros::spin();

    return 0;
}
