# 单目相机标定方法从理论到实践

> 帮助文档：
>
> 1. [单目相机成像原理](https://github.com/SHU-FLYMAN/CalibCamera/blob/master/docs/01%20%E7%90%86%E8%AE%BA%E9%83%A8%E5%88%86%EF%BC%9A%E5%8D%95%E7%9B%AE%E6%88%90%E5%83%8F%E8%BF%87%E7%A8%8B.md)
> 2. [程序帮助文档](https://github.com/SHU-FLYMAN/CalibCamera/blob/master/docs/02%20%E5%AE%9E%E8%B7%B5%E9%83%A8%E5%88%86%EF%BC%9A%E5%BC%A0%E6%AD%A3%E5%8F%8B%E6%A0%87%E5%AE%9A%E6%B3%95%20-%20OpenCV%E5%AE%9E%E7%8E%B0.md)

单目相机的标定是所有三维重建算法的基础，所以，大致明白单目相机的成像原理，以及基于OpenCV的标定流程，对于我们后序深入地学习三维重建算法至关重要。 

> 尽管通过这个教程，你不能明白张正友标定法里面具体的细节，但我仍然觉得这个具有重要意义。因为这大概就是做研究的常态，对于某个方法，除非我们要在它的这个方向上做深入研究，否则这些算法中的细节，一来你没有时间去详细了解它，其次，这些细节对你来说其实是“无关紧要”的。
>
> 我的意思是，比如你做的是双目结构光三维重建融合算法的创新，那么，对于相机标定中详细的算法细节，你就没有必要去了解其中算法非常细节的东西，因为这会浪费你去了解融合算法细节的时间。

相机标定的具体原理比较复杂，但好在OpenCV很多过程都帮我们封装好了，基于张正友标定发的单目相机标定具体步骤如下：

1. 准备标定图片（不同位置、角度、姿态下拍摄，至少需要3张，以10~20张为宜）
2. 对每一张标定图片，提取角点信息，并进行亚像素角点优化（绘制相应图像）
3. 调用张正友标定方法，对相机进标定，也就是计算内参和畸变系数
4. 对标定结果进行评价（重投影误差）
5. 利用标定结果（内参、畸变系数），对任何输入的图像进行矫正

相信在明白了理论之后，阅读我们的代码应该非常容易：尽量命名做到望文知义、封装做到了面对对象、增加了额外注释。



## 使用方法

1. [OpenCV安装与配置](https://www.cnblogs.com/huang-y-x/p/11635126.html)（尽量不需要选择特别新的OpenCV版本，很难编译通过）
2. [Clion安装与配置](https://www.cnblogs.com/FLYMANJB/p/13339001.html)（Ubuntu是相类似的）
3. 修改相应的配置路径
   - `CMakeList.txt`，主要是OpenCV包的路径
   - `main.cpp`，里面标定图片文件夹路径，以及测试图片路径

之后点击运行即可，当然你也可以用Visual studio来运行，因为是Cmake来管理环境变量的，所以迁移较为方便。代码已经在Windows、Ubuntu18.04下测试，能够运行。

<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200806162930.png" width="宽度" height="高度" alt="图片名称" align=center>



<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200806163002.png" width="宽度" height="高度" alt="图片名称" align=center>

<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200806163018.png" width="宽度" height="高度" alt="图片名称" align=center>



## 基础知识（简要回顾）

简要的原理回顾如下：相机模型如下，主要包含四个坐标系：

<img src="https://img2018.cnblogs.com/blog/1516317/201902/1516317-20190219150222035-99963244.png" alt="img" style="zoom: 67%;" />

四个坐标系的转换关系如下：

<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200604162350.png" width="宽度" height="高度" alt="图片名称" align=center style="zoom:67%;" >

而我们主要关心的是从世界坐标系到像素坐标系的变换，它们的公式如下：

<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200806162243.png" width="宽度" height="高度" alt="图片名称" align=center>

而相机成像过程中存在着径向畸变和切向畸变，合并后：

<img src="https://flyman-cjb.oss-cn-hangzhou.aliyuncs.com/picgos/20200806162329.png" width="宽度" height="高度" alt="图片名称" align=center>

为了之后三维重建算法的精确性，我们必须对这些畸变进行矫正。我们可以通过张正友标定法得到相机的**畸变系数**，除此之外还有它的**内参**。获得了这两个参数，我们就能对相机的整个成像过程建立较为准确的映射关系（至少是单向的）。

## 感谢

标定用的图片搜集自互联网，感谢原作者！邮箱：fly_cjb@163.com，有问题欢迎提问！