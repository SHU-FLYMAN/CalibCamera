#ifndef STEREOCALIBRATION_UTILS_H
#define STEREOCALIBRATION_UTILS_H

#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <opencv2/opencv.hpp>

using namespace std;


class Utils {
public:
    static void split_string(const string &str, vector<string> &str_splits, const string &seg){
        boost::split(str_splits, str, boost::is_any_of(seg));
    }

    static void list_files(const string &folder, vector<cv::String> &files_list){
        cv::glob(folder, files_list);
        assert(!files_list.empty());
    }
    static void show(const cv::Mat &img, const string &win_name) {
        cv::imshow(win_name, img);
        cv::waitKey(0);
        cv::destroyWindow(win_name);
    }
};


#endif //STEREOCALIBRATION_UTILS_H
