#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat image;
Scalar fitEllipseColor = Scalar(255, 255, 0);

Mat src_gray;
Mat canny_image, detected_edges;
int lowThreshold = 0;
const int max_lowThreshold = 100;
const int iratio = 3;
const int kernel_size = 3;

void processImage(int, void*);


int main(int argc, char** argv)
{
    const char* filename = argc >= 2 ? argv[1] : "ellipses.jpg";
    image = imread(samples::findFile(filename), IMREAD_COLOR);
    if (image.empty())
    {
        cout << "Couldn't open image " << filename << "\n";
        return 0;
    }

    canny_image.create(image.size(), image.type());
    cvtColor(image, src_gray, COLOR_BGR2GRAY);


    namedWindow("Canny", WINDOW_AUTOSIZE);
    createTrackbar("Min Threshold:", "Canny", &lowThreshold, max_lowThreshold, processImage);

    processImage(0,0);

    // Wait for a key stroke; the same function arranges events processing
    waitKey();
    return 0;
}


void processImage(int, void*)
{
    //ͨ��Canny��Ե����ȡ��ֵͼ��
    blur(src_gray, detected_edges, Size(3, 3));
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * iratio, kernel_size);
    canny_image = Scalar::all(0);
    image.copyTo(canny_image, detected_edges);
    imshow("Canny", canny_image);


    RotatedRect box;
    //contours����Ϊ��vector<vector<Point>> contours������һ��˫��������������ÿ��Ԫ�ر�����һ����������Point���ɵĵ�ļ��ϵ���������
    //ÿһ��㼯����һ���������ж���������contours���ж���Ԫ�أ�
    vector<vector<Point>>contours;
    //vector<vector<Point>>contours;
    Mat bimage = detected_edges;
    //RETR_LIST:�����������
    //CHAIN_APPROX_NONE:��������߽������������������㵽contours������
    findContours(bimage, contours, RETR_LIST, CHAIN_APPROX_NONE);

    Mat fit = Mat::zeros(bimage.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++)
    {
        size_t count = contours[i].size();
        //��С����������
        if (count < 6)
            continue;
        Mat pointsf;
        Mat(contours[i]).convertTo(pointsf, CV_32F);

        RotatedRect box = fitEllipse(pointsf);
        //����ϸС������������
        if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
            continue;

        //���Ʋ�׽���������ð�ɫ��ʾ
        drawContours(fit, contours, (int)i, Scalar::all(255), 1, 8);

        //������Ϻ��Ч��
        ellipse(fit, box, Scalar(0, 0, 255), 1, LINE_AA);
        Point2f vtx[4];
        box.points(vtx);
        //����box
        //for (int j = 0; j < 4; j++)
            //line(fit, vtx[j], vtx[(j + 1) % 4], Scalar(0, 255, 0), 1, LINE_AA);
    }

    imshow("result", fit);
}



