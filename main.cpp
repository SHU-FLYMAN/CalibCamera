#include "Srcs/CheckBoard.h"
#include "Srcs/Camera.h"
#include <opencv2/opencv.hpp>


void test(CheckBoard &checkboard,
          Camera &camera,
          const string &file){
    cv::Mat img_original = cv::imread(file);
    cv::Mat img_rectify_1 = camera.rectify(img_original);
    cv::Mat img_rectify_2= camera.imread(file);

    Utils::show(img_original, "original");
    Utils::show(img_rectify_1, "img_rectify_1");
    Utils::show(img_rectify_2, "img_rectify_2");

    Utils::show(img_rectify_1 - img_original, "img_rectify_1 - img_original");
    Utils::show(img_rectify_2 - img_original, "img_rectify_2 - img_original");
    Utils::show(img_rectify_2 - img_rectify_1, "img_rectify_2 - img_rectify_1");
}


int main() {
    CheckBoard checkboard( 4, 5, 25.f);
    checkboard.detect_corners("/home/flyman/Desktop/CalibCamera/data", true);
    checkboard.show_params();

    Camera camera;
    camera.calib(checkboard);
    camera.show_params();

    test(checkboard, camera, "/home/flyman/Desktop/CalibCamera/data/left0000.jpg");
    return 0;
}