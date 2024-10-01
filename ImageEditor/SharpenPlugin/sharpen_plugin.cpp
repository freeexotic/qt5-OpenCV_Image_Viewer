#include <sharpen_plugin.hpp>

QString Sharpen_plugin::name(){
    return "Sharpen";
}

void Sharpen_plugin::edit(const cv::Mat &input, cv::Mat &output){
    int intensity = 2;
    cv::Mat smoothed;
    cv::GaussianBlur(input, smoothed, cv::Size(9,9),0);
    output = input +(input-smoothed)*intensity;
}
