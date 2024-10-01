#include<cartoonplugin.hpp>

QString CartoonPlugins::name(){
    return "Cartoon Image";
}

void CartoonPlugins::edit(const cv::Mat &input, cv::Mat &output){
    int num_down = 2;
    int num_bilateral = 7;
    cv::Mat copy1, copy2;

    copy1 = input.clone();

    for(int i =0 ; i!=num_down; i++){
        cv::pyrDown(copy1, copy2);
        copy1 = copy2.clone();
    }

    for(int i = 0; i!=num_bilateral; i++){
        cv::bilateralFilter(copy1, copy2, 9,9,7);
        copy1 = copy2.clone();
    }

    for(int i =0 ; i!=num_down; i++){
        cv::pyrUp(copy1, copy2);
        copy1 = copy2.clone();
    }


    if (input.cols != copy1.cols || input.rows != copy1.rows){
        cv::Rect rect(0,0,input.cols, input.rows);
        copy1(rect).copyTo(copy2);
        copy1 = copy2;
    }

    cv::Mat image_gray, image_edge;

    int line_size = 9;
    int blur_value = 2;

    cv::cvtColor(input, image_gray, cv::COLOR_RGB2GRAY);
    cv::medianBlur(image_gray, image_gray, 5);
    cv::adaptiveThreshold(image_gray, image_gray,255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, line_size, blur_value);

    cv::cvtColor(image_gray, image_edge, cv::COLOR_GRAY2RGB);

    output = copy1 & image_edge;

}
