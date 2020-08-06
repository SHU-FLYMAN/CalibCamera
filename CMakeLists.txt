cmake_minimum_required(VERSION 3.16)
project(StereoCalibration)

set(CMAKE_CXX_STANDARD 11)
# 01 导入OpenCV包
find_package(OpenCV REQUIRED)
# 02 包含头文件搜索路径
include_directories(${OpenCV_INCLUDE_DIRS})
# 03 添加可执行文件
add_executable(
        StereoCalibration main.cpp  # 主程序
        Srcs/Camera.h               # 相机对象
        Srcs/CheckBoard.h           # 标定板对象
        Srcs/Utils.h                # 工具类
        )
# 04 链接相应的库
target_link_libraries(StereoCalibration ${OpenCV_LIBS})