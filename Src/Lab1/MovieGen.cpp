#include <iostream> // for standard I/O
#include <string>   // for strings
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>  // Video write
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include<io.h>
using namespace std;
using namespace cv;


#define MAXINPUT 100
#define FRAME 30
#define w 400


VideoWriter outputVideo;

void putTextwithImg(Mat img)
{
	string text = "3180105883 Ziqi Wen";
	Size textsize = getTextSize(text, FONT_HERSHEY_PLAIN, 2, 1, 0);
	Point org((img.cols - textsize.width) / 2, (img.rows - textsize.height) / 8 * 7);
	putText(img, text, org, FONT_HERSHEY_PLAIN, 2.0, Scalar(255, 255, 255));
	outputVideo << img;
}


void MyEllipse(Mat img, double angle)
{
	int thickness = 2;
	int lineType = 8;
	ellipse(img,
		Point(w / 2, w / 2),
		Size(w / 4, w / 16),
		angle,
		0,
		360,
		Scalar(255, 0, 0),
		thickness,
		lineType);
}

void MyFilledCircle(Mat img, Point center)
{
	circle(img,
		center,
		w / 32,
		Scalar(0, 0, 255),
		FILLED,
		LINE_8);
}

void myMovieHead(Size S)
{

	for (int i = 0; i < FRAME * 2; i++)
	{
		Mat atom_image = Mat::zeros(w, w, CV_8UC3);

		MyEllipse(atom_image, 90 + i * 3);
		MyEllipse(atom_image, 0 + i * 3);
		MyEllipse(atom_image, 45 + i * 3);
		MyEllipse(atom_image, -45 + i * 3);
		MyFilledCircle(atom_image, Point(w / 2, w / 2));

		Mat img_in = Mat(S, CV_8UC3);
		resize(atom_image, img_in, S);

		putTextwithImg(img_in);
	}

	cout << "Movie head has been generated!" << endl;

}


void getFiles(string path, vector<string>& files)
{
	long   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}



void generateMovie(string file)
{

	vector<string> fileName;
	getFiles(file, fileName);
	string source, name;
	//get the file in the dir
	for (int i = 0; i < fileName.size(); i++)
	{
		name= fileName[i].c_str();
		if (name.find(".avi") != name.npos)
		{
			source = name;
			break;
		}
	}

	//read the input video
	VideoCapture inputVideo(source);
	if (!inputVideo.isOpened())
	{
		cout << "Could not open the input video: " << source << endl;
		exit(1);
	}

	//get the info in the input video
	int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));
	char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };
	const string NAME = file + "\\output.avi";
	Size S = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));

	//create an output video
	outputVideo.open(NAME, ex, inputVideo.get(CAP_PROP_FPS), S, true);
	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: " << source << endl;
		exit(1);
	}

	//create the head of the movie
	myMovieHead(S);

	//read image
	Mat src, img[MAXINPUT], img_in[MAXINPUT];
	int count = 0;
	for (int i = 0; i < fileName.size(); i++)
	{
		string name = fileName[i].c_str();
		if (name.find(".jpg") != name.npos)
		{
			img[count] = imread(name);

			img_in[count] = Mat(S, CV_8UC3);
			resize(img[count], img_in[count], S);
			count++;
		}
	}


	//show image Gradually
	//first image
	for (int i = 0; i < FRAME/2; i++)
	{
		putTextwithImg(img_in[0]);
		//outputVideo << img_in;
	}

	//cout << "DEBUG:1" << endl;

	for (int i = 0; i < count-1; i++)
	{
		Mat img_Add = Mat(S, CV_8UC3);
		for (int j = 0; j < FRAME; j++)
		{
			double alpha = 1.0 - j * 3.0 / 100.0;
			double beta = (1.0 - alpha);
			addWeighted(img_in[i], alpha, img_in[i+1], beta, 0, img_Add);
			putTextwithImg(img_Add);
		}
		//cout << i << endl;
	}

	//last image
	for (int i = 0; i < FRAME / 2; i++)
	{
		putTextwithImg(img_in[count - 1]);
		//outputVideo << img_in;
	}

	cout << "image writing finish" << endl;

	//write video
	while (1)
	{
		inputVideo >> src;
		if (src.empty()) 
			break;
		putTextwithImg(src);
		
	}
	cout << "Finished writing" << endl;

}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}
	cout << "Program begin" << endl;


	generateMovie(argv[1]);
	//DEBUG:
	//generateMovie("D:\\File_WZQ\\Code_VS\\CV\\CVLab_1\\input");

	cout << "Program end" << endl;
	return 0;
}