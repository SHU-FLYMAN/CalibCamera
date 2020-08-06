#ifndef STEREOCALIBRATION_CHECKBOARD_H
#define STEREOCALIBRATION_CHECKBOARD_H

#include <iostream>
#include <cassert>
#include <vector>
#include <opencv2/opencv.hpp>

#include "Utils.h"
using namespace std;


class CheckBoard {
public:
    int points_per_col;                               // 某列的内角点数量
    int points_per_row;                               // 某行的内焦点数量
    cv::Size patten_size;                             // 棋盘格的尺寸 cv::Size(points_per_row, points_per_col)
    float square_size;                                // 方块格的实际大小(单位毫米)

    vector<cv::String> calib_images;                  // 标定的图片
    cv::Size calib_images_size;                       // 标定图像尺寸大小

    vector<vector<cv::Point3f> > corners_world_all;   // 角点（世界坐标）
    vector<vector<cv::Point2f> > corners_pixel_all;   // 角点（像素坐标）

public:
    CheckBoard(int points_per_col, int points_per_row, float square_size=-1){
        assert(points_per_col > 0); this->points_per_col = points_per_col;
        assert(points_per_row > 0); this->points_per_row = points_per_row;

        this->patten_size = cv::Size(this->points_per_row, this->points_per_col);

        if (square_size != -1){
            assert(square_size > 0);
            this->square_size = square_size;
        }
    };

public:
    // 逻辑主程序
    void detect_corners(const string &calib_folder, bool show=false){
        calib_images.resize(0);
        corners_world_all.resize(0);
        corners_pixel_all.resize(0);

        Utils::list_files(calib_folder, calib_images);

        calib_images_size = cv::imread(calib_images[0]).size();

        cout << "开始检测角点!" << endl;
        cout << "-----------------------" << endl;

        // 增加棋盘格的世界坐标系
        for (int idx = 0; idx < calib_images.size(); ++idx){
            _add_corners_world();
        }

        // 亚像素迭代标准
        cv::TermCriteria criteria = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1);

        vector<int> idxs_delete;  // 角点检测失败的图片
        // 检测棋盘格的角点，并添加相应的像素坐标
        for (int idx = 0; idx < calib_images.size(); ++idx){
            string filename = calib_images[idx];
            cv::Mat img = cv::imread(filename);
            bool found = _add_corners_pixel(img, criteria);

            if (!found){
                idxs_delete.emplace_back(idx);
                cout << "失败:\t" << filename << endl;
            }
            else{
                cout << "成功:\t" << filename << endl;
            }

            if (show){
                cv::drawChessboardCorners(img, patten_size, corners_pixel_all[idx], found);
                vector<string> str_splits;
                Utils::split_string(calib_images[idx], str_splits, "/");
                Utils::show(img, str_splits.back());
            }

        }
        // 删除无效
        for (auto idx : idxs_delete){
            calib_images.erase(calib_images.begin() + idx);
            corners_world_all.erase(corners_world_all.begin() + idx);
        }
    }

    void show_params(){
        cout << "标定板尺寸(points_per_row, points_per_col):\t" << patten_size << endl;
        cout << "棋盘格大小:\t" << square_size << endl;
    }

public:
    void _add_corners_world(){
        if (corners_world_all.empty()){
            vector<cv::Point3f> cornersWorld;

            for (int row = 0; row < points_per_col; ++row) {
                for (int col = 0; col < points_per_row; ++col) {
                    cornersWorld.emplace_back(float(col) * square_size, floor(row) * square_size, 0.f);
                }
            }
            corners_world_all.emplace_back(cornersWorld);
        }
        else
        {
            corners_world_all.emplace_back(corners_world_all[0]);
        }
    }

    bool _add_corners_pixel(const cv::Mat &img, cv::TermCriteria &criteria){
        assert(calib_images_size == img.size());
        vector<cv::Point2f> corners_pixel;
        // 进行角点粗检测
        bool found = cv::findChessboardCorners(img, patten_size, corners_pixel);
        if (found){
            cv::Mat img_gray;
            cv::cvtColor(img, img_gray, CV_BGR2GRAY);
            // 角点亚像素优化
            cv::cornerSubPix(img_gray, corners_pixel, cv::Size(5, 5), cv::Size(-1, -1), criteria);
            corners_pixel_all.emplace_back(corners_pixel);
        }
        return found;
    }
};


#endif //STEREOCALIBRATION_CHECKBOARD_H
