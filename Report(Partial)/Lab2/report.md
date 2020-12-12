



# 实验报告

## 1.实验目的和要求

调用**CvBox2D** **cvFitEllipse2**( const CvArr* points )实现椭圆拟合



## 2.实验内容和原理

1. 通过`createTrackbar`手动调整二值图像阈值，从而将原输入转为二值图像；
2. `findContours`从输入的二值图像中获取轮廓；
3. `fitEllipse`来拟合椭圆；
4. `ellipse`绘制拟合出的椭圆,`drawContours`绘制提起的轮廓；



## 3.实验步骤与分析

1. 命令行读取参数，获取图像文件(默认文件为ellipses.jpg)：

   ```c++
   const char* filename = argc >= 2 ? argv[1] : "ellipses.jpg";
   image = imread(samples::findFile(filename), 0);
   if (image.empty())
   {
       cout << "Couldn't open image " << filename << "\n";
       return 0;
   }
   ```

2. 创建滑动条，控制二值图像阈值

   ```c++
   namedWindow("result", WINDOW_NORMAL);
   createTrackbar("Threshold", "result", &sliderPos, 255, processImage);
   processImage(0, 0);
   ```

3. 将输入转换为二值图像，并提取其轮廓：

   ```c++
   RotatedRect box;
   vector<vector<Point>>contours;
   Mat bimage = image >= sliderPos;
   findContours(bimage, contours, RETR_LIST, CHAIN_APPROX_NOE);
   ```

4. 抛弃提取的轮廓中过小的轮廓（组成轮廓的点太小）；

   对剩下的轮廓依次进行椭圆拟合；

   抛弃拟合的椭圆中比例过于巨大（离心率过大）的部分；

   绘制提取的轮廓，以及拟合的椭圆；

   显示二值图像，拟合图像，以及原图像；

   ```c++
   Mat fit = Mat::zeros(bimage.size(), CV_8UC3);
   for (size_t i = 0; i < contours.size(); i++)
   {
       size_t count = contours[i].size();
       //过小的轮廓抛弃
       if (count < 6)
           continue;
       Mat pointsf;
       Mat(contours[i]).convertTo(pointsf, CV_32F);
   
       RotatedRect box = fitEllipse(pointsf);
       //不合比例的轮廓被抛弃
       if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
           continue;
   
       //绘制捕捉的轮廓，用白色表示
       drawContours(fit, contours, (int)i, Scalar::all(255), 1, 8);
   
       //绘制拟合后的效果
       ellipse(fit, box, Scalar(0, 0, 255), 1, LINE_AA);
       Point2f vtx[4];
       box.points(vtx);
       //绘制box
       for (int j = 0; j < 4; j++)
           line(fit, vtx[j], vtx[(j + 1) % 4], Scalar(0, 255, 0), 1, LINE_AA);
   }
   
   imshow("source", image);
   imshow("bimage", bimage);
   imshow("result", fit);
   ```

   

## 4.实验结果

以下图像从左到右依次是：二值图像，拟合的椭圆（黄色）以及提取的轮廓（白色），原图像

测试图像1：

<img src=".\img\1.png" alt="1" style="zoom:50%;" />

测试图像1，改变二值图像阈值：

<img src=".\img\2.png" alt="1" style="zoom:50%;" />

测试图像2：

<img src=".\img\3.png" alt="1" style="zoom:50%;" />



## 5.心得体会

拟合椭圆之前需要对图像进行一些预处理，一些函数的使用方式和参数在这里值得说明；

### 5.1图像的二值化

首先是图像的二值化，原本在图像处理课程中需要一定代码才能实现的二值化，在openCV只需一个简便的运算即可实现：

```c++
Mat bimage = image >= sliderPos;
```

image中大于`sliderPos`的像素值为255，小于的为1，即可快速根据阈值二值化；

### 5.3 使用findContours的原因

这里之所以要进行二值化，是因为``findContours``的输入必须是二值图像。

``findContours``的目的在于从输入的二值图像中，该函数返回一个`vector<vector<Point>>`变量，通过`vector<Point>`来表示一个轮廓，而该函数返回值是多个轮廓的集合。我们再通过对多个提取到的轮廓进行椭圆拟合，就可以得到我们需要的结果。

这里需要注意的是，轮廓和上课所讲的边缘有一定区别，椭圆拟合显然无法直接将边缘检测的结果作为输入——因为我们需要对图像中每一个近似于椭圆的图形进行单独的拟合，但是边缘检测返回的结果是所有被判断为边缘的像素的集合，这样混杂的输入无法直接传递给椭圆拟合函数中，因此，必然需要经过轮廓提取，将这些不同图形的轮廓区分出来，对每一个图形的轮廓进行椭圆拟合。

### 5.4以Canny边缘检测的结果作为findContours的输入

实际上，轮廓检测函数的输入常常是边缘检测的输出，效果会较为良好（在上述程序中通过将二值化的图像直接作为输入，在特定的阈值下的效果也比较可观）。

因此本人修改一下初始的代码，不直接将二值图像作为轮廓提取的输入，而是先进行Canny边缘检测，再进行轮廓提取，在函数`processImage(int, void*)`开头加入如下代码，完成Canny边缘检测，并将边缘检测的结果作为轮廓提取的输入：

```c++
blur(src_gray, detected_edges, Size(3, 3));
Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * iratio, kernel_size);
canny_image = Scalar::all(0);
image.copyTo(canny_image, detected_edges);
imshow("Canny", canny_image);
```

最终效果如下：

其中左图为拟合后的结果，有图是边缘检测的结果：

<img src=".\img\4.png" alt="1" style="zoom:50%;" />

此部分的完整代码可见"myFitEllipse_canny.cpp"

### 5.5 findContours细节说明

```c++
void cv::findContours	(	InputArray 	image,
							OutputArrayOfArrays 	contours,
							int 	mode,
							int 	method,
							Point 	offset = Point() 
						)		
```

其中第一个参数是二值图像的输入，第二个参数是findContours的输出，是`vector<vector<Point>>`型变量；

这里稍微说明一下第三个参数与第四个参数：

第三个参数`mode`定义了轮廓的检索模式，这将影响到该函数具体返回什么样的轮廓：

它具有如下四种取值：`CV_RETR_EXTERNAL`,`CV_RETR_LIST `,`CV_RETR_CCOMP`,`CV_RETR_TREE`;

1. CV_RETR_EXTERNAL只检测最外围轮廓，包含在外围轮廓内的内围轮廓被忽略;
2. CV_RETR_LIST  检测所有的轮廓，包括内围、外围轮廓，但是检测到的轮廓不建立等级关系，彼此之间独立，没有等级关系，这就意味着这个检索模式下不存在父轮廓或内嵌轮廓;
3. CV_RETR_CCOMP  检测所有的轮廓，但所有轮廓只建立两个等级关系，外围为顶层，若外围内的内围轮廓还包含了其他的轮廓信息，则内围内的所有轮廓均归属于顶层;
4. CV_RETR_TREE， 检测所有轮廓，所有轮廓建立一个等级树结构。外层轮廓包含内层轮廓，内层轮廓还可以继续包含内嵌轮廓。

程序中我们选择第二种取值，因为我们只需要简单返回所有轮廓即可，不需要等级关系；

第四个参数代表轮廓的近似方式，这里只说明`CV_CHAIN_APPROX_NONE`,`CV_CHAIN_APPROX_SIMPLE`这两种取值：前者将保存物体边界上所有连续的轮廓点到contours向量内，而后者仅保存轮廓的拐点信息。

### 5.5 fitEllipse

该函数输入只需要对应的图像数据，在程序中我们先得到了关于每一个轮廓的`vector<Point>`信息，再将其转化为`Mat`格式即可得到所需要的输入。

fitEllipse返回的是`RotatedRect`类型，表示一个矩形，我们可以通过这个矩形的数据来调用`ellipse`绘制出对应的椭圆，调用方式有两种：

```c++
ellipse(cimage, box, Scalar(0,0,255), 1, LINE_AA);
ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0,255,255), 1, LINE_AA);
```

值得一提的是，OpenCV的椭圆拟合函数还有其他的两种：`fitEllipseAMS()`与`fitEllipseDirect()`,是基于不同数学原理的椭圆拟合，具体的内容可以详见官方文档https://docs.opencv.org/4.3.0/d3/dc0/group__imgproc__shape.html#gadf1ad6a0b82947fa1fe3c3d497f260e0

### 5.6 心得

实验内容相对基础，代码本身并不复杂。OpenCV官方有给出很多的Sample代码，是一个了解OpenCV各函数如何使用的很好的学习资料，可以先通过这些sample代码入手，观察函数调用的方式，参数取值方式以及其他可能的取值情况，来进一步理解OpenCV的使用。其次，虽然本次实验的代码内容较为简单，但归供与OpenCV将复杂的数学原理封装在简单的函数中，在OpenCV的官方文档以及教程中，往往有对这些数学原理的进一步说明，可以借此学习，即使无法彻底理解整个数学过程，但是对原理有一定了解，能够更好地明白很多OpenCV函数中参数的意义以及不同取值的影响。