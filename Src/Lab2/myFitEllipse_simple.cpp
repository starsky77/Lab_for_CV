#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int sliderPos = 70;
Mat image;
Scalar fitEllipseColor = Scalar(255, 255, 0);

void processImage(int, void*);

int main(int argc, char** argv)
{
    const char* filename = argc >= 2 ? argv[1] : "ellipses.jpg";
    image = imread(samples::findFile(filename), 0);
    if (image.empty())
    {
        cout << "Couldn't open image " << filename << "\n";
        return 0;
    }
    namedWindow("result", WINDOW_NORMAL);

    createTrackbar("Threshold", "result", &sliderPos, 255, processImage);
    processImage(0, 0);

    // Wait for a key stroke; the same function arranges events processing
    waitKey();
    return 0;
}

void processImage(int, void*)
{
    RotatedRect box;
    //contours定义为“vector<vector<Point>> contours”，是一个双重向量（向量内每个元素保存了一组由连续的Point构成的点的集合的向量），
    //每一组点集就是一个轮廓，有多少轮廓，contours就有多少元素；
    vector<vector<Point>>contours;
    //vector<vector<Point>>contours;
    Mat bimage = image >= sliderPos;
    //RETR_LIST:检测所有轮廓
    //CHAIN_APPROX_NONE:保存物体边界上所有连续的轮廓点到contours向量内
    findContours(bimage, contours, RETR_LIST, CHAIN_APPROX_NONE);

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
        //过于细小的轮廓被抛弃
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
}



