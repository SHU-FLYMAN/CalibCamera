#ifndef STEREOCALIBRATION_CAMERA_H
#define STEREOCALIBRATION_CAMERA_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "CheckBoard.h"
using namespace std;


class Camera {
public:

    cv::Mat cameraMatrix;    // 相机内参： [[fx, γ, u0], [0, fy, v0], [0, 0, 1]]
    cv::Mat distCoeffs;      // 畸变系数： k1, k2, p1, p2, k3
    double calib_pro_error;  // 标定误差

    vector<cv::Mat> _rVecs;  // 旋转矩阵
    vector<cv::Mat> _tVecs;  // 平移矩阵

public:
    // 主程序
    void calib(CheckBoard &checkboard){
        cout << "\n开始标定：..." << endl;
        clock_t start, end;
        start = clock();
        calib_pro_error = cv::calibrateCamera(
                checkboard.corners_world_all,  // 世界坐标系点，vector<vector<cv::Point3f> >
                checkboard.corners_pixel_all,  // 图像坐标系点(对应的)，vector<vector<cv::Point2f> >
                checkboard.calib_images_size,  // 标定图像尺寸大小，cv::Size
                cameraMatrix,      // 相机内参矩阵
                distCoeffs,           // 畸变矩阵
                _rVecs,                  // 旋转向量
                _tVecs                   // 平移矩阵
        );
        end = clock();
        cout << "耗时:\t" << double(end - start) / CLOCKS_PER_SEC << " S" << endl;
        cout << "标定结束!" << endl;

    }

    cv::Mat rectify(cv::Mat &img) const{  // 不能修改任何成员变量的值
        cv::Mat img_rectify = img.clone();
        cv::undistort(img, img_rectify, cameraMatrix, distCoeffs);
        return img_rectify;
    }

    cv::Mat imread(const string &filename) const{
        cv::Mat img = cv::imread(filename);
        return rectify(img);
    }

    void show_params() const{
        cout << "-----------------------" << endl;
        cout << "重投影误差:" << calib_pro_error << endl;

        cout << "-----------------------" << endl;
        cout << "相机内参:\n" << "M = [[fx, γ, u0,] [0, fy, v0], [0, 0, 1]" << endl;
        cout << "=" << cameraMatrix << endl;

        cout << "-----------------------" << endl;
        cout << "畸变系数:\n" << "k1, k2, p1, p2, k3" << endl;
        cout << "=" << distCoeffs << endl;
    }
};

#endif //STEREOCALIBRATION_CAMERA_H
