#pragma once
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include "dirent.h"
#include <sys/types.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include<algorithm>


#pragma warning( disable : 4996 )  //for sprintf;

using namespace cv;
using namespace std;

struct Face
{
	char* name;
	CvMat* data;
	CvMat* eigen;
};

typedef struct Eye
{
	int LX;
	int LY;
	int RX;
	int RY;
	double distance;
}Eye;

class FaceLibrary
{
public:
	FaceLibrary();
	~FaceLibrary();
	void loadFace(string input);
	void getMeanFace(string input);
	void getEigen(string input);
	void saveModel(string input, int energyNum);
	//used for test program
	void loadModel(string modelFile);
	void loadRecFace(string input);
	void recFace(int recn);


private:
	vector<Face*>face;
	vector<Face*> rec_face;
	CvMat* image_points[1];
	//Mat_<double>meanMat;
	//Mat_<double>covMat;
	//Mat_<double>sample[100];
	CvMat* covarCvMat;
	CvMat* meanCvMat;
	CvMat* eigenValue;
	CvMat* eigenVector;
	CvMat* A;
	CvMat* eigen_image;

	int row;
	int col;
	int number;
	int eigen_number;
};



class PreTreat
{
public:
	PreTreat();
	~PreTreat();
	void PretreatMent(string Dirin, string Dirout, int NumImg, bool isTrain);
	void Interface(int mode);
private:
	Rect ROI;


};


