[toc]

# 01 基础知识（回顾）

相机模型如下（如有不明白，请查看之前的推送），主要包含四个坐标系：

<img src="https://img2018.cnblogs.com/blog/1516317/201902/1516317-20190219150222035-99963244.png" alt="img" style="zoom: 67%;" />

四个坐标系的转换关系如下：

<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200604162350.png" width="宽度" height="高度" alt="图片名称" align=center style="zoom:67%;" >

而我们主要关心的是从世界坐标系到像素坐标系的变换，它们的公式如下：
$$
{Z_c}\underbrace {\left[ {\begin{array}{*{20}{c}}
  u \\ 
  v \\ 
  1 
\end{array}} \right]}_{像素坐标系} = \underbrace {\left[ {\begin{array}{*{20}{c}}
  {{f_x}}&0&{{u_0}} \\ 
  0&{{f_y}}&{{v_0}} \\ 
  0&0&1 
\end{array}} \right]}_{M1：内参}\underbrace {\left[ {\begin{array}{*{20}{c}}
  {{R_{3 \times 3}}}&{{T_{3 \times 1}}} 
\end{array}} \right]}_{M2：外参}\underbrace {\left[ {\begin{array}{*{20}{c}}
  {{X_W}} \\ 
  {{Y_W}} \\ 
  {{Z_W}} \\ 
  1 
\end{array}} \right]}_{世界坐标系}
$$
相机成像过程中存在着径向畸变和切向畸变，合并后：
$$
\left[ {\begin{array}{*{20}{c}}
{x'}\\
{y'}
\end{array}} \right] = \underbrace {\left( {1 + {k_1}{r^2} + {k_2}{r^4} + {k_3}{r^6}} \right)\left[ {\begin{array}{*{20}{c}}
x\\
y
\end{array}} \right]}_{径向畸变} + \underbrace {\left[ {\begin{array}{*{20}{c}}
{2{p_1}xy + {p_2}\left( {{r^2} + 2{x^2}} \right)}\\
{2{p_2}xy + p1\left( {{r^2} + 2{y^2}} \right)}
\end{array}} \right]}_{切向畸变}
$$
为了之后三维重建算法的精确性，我们必须对这些畸变进行矫正。我们可以通过张正友标定法得到相机的**畸变系数**，除此之外还有它的**内参**。获得了这两个参数，我们就能对相机的整个成像过程建立较为准确的映射关系（至少是单向的），尽管具体的矫正实现比较复杂，但好在OpenCV很多过程都帮我们封装好了。



# 02 OpenCV实践

下面就开始实践，我们来实现一个基于OpenCV的实践，具体的步骤如下：

1. 准备标定图片（不同位置、角度、姿态下拍摄，至少需要3张，以10~20张为宜）
2. 对每一张标定图片，提取角点信息，并进行亚像素角点优化（绘制相应图像）
3. 调用张正友标定方法，对相机进标定，也就是计算内参和畸变系数
4. 对标定结果进行评价（重投影误差）
5. 利用标定结果（内参、畸变系数），对任何输入的图像进行矫正

更多的请直接看代码，相信在明白了理论之后，阅读我们的代码应该非常容易（尽量命名做到望文知义、封装做到了面对对象）。关键的代码如下：

## 1）准备标定图片

用于标定的图片如下，我们需要知道它的包括三个信息，某行/列的内角点数量，以及每个棋盘格的大小：

| <img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200806150017.png" width="宽度" height="高度" alt="图片名称" align=center style="zoom: 50%;" > |
| :----------------------------------------------------------: |
|                      **图1 标定板图像**                      |

在我们的代码中，即以下函数：

```c++
// 读取所有标定图片到calib_images <vector> 中 
Utils::list_files(calib_folder, calib_images);
```

## 2）检测角点，并亚像素优化

```c++
// 粗检测 
bool found = cv::findChessboardCorners(img, patten_size, corners_pixel);
// 迭代标准
cv::TermCriteria criteria = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1);
// 迭代优化
 cv::cornerSubPix(img_gray, corners_pixel, cv::Size(5, 5), cv::Size(-1, -1), criteria);
```

## 3）调用张正友标定法

获取相机内参矩阵、畸变矩阵：

```c++
calib_pro_error = cv::calibrateCamera(
    checkboard.corners_world_all,  // 世界坐标系点，vector<vector<cv::Point3f> >
    checkboard.corners_pixel_all,  // 图像坐标系点(对应的)，vector<vector<cv::Point2f> >
    checkboard.calib_images_size,  // 标定图像尺寸大小
    cameraMatrix,      									// 相机内参矩阵
    distCoeffs,           						 // 畸变矩阵
    _rVecs,                  				 // 旋转向量
    _tVecs                   				 // 平移矩阵
);
```

## 4）纠正图像

利用得到的内参矩阵、畸变系数对图像进行矫正：

```c++
cv::Mat img_rectify = img.clone();
cv::undistort(img, img_rectify, cameraMatrix, distCoeffs);
```



# 03 注意事项

拍摄图片的有以下注意事项：

1. 用于标定的标定板制作精度以一定要高，否则影响精度；
2. 棋盘格与相机之间的夹角要小于45°，否则误差较大；
3. 应使视场大小与标定板大小相当，或者稍微大一点，并且保证成像清晰、曝光适当；
4. 标定板所处离相机的距离，应与平时相机的工作距离相近；

