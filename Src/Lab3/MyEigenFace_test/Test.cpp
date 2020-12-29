#include "eigenFace_test.h"
//#define DEBUG
//进行测试之前，请将预处理获取的训练集照片移动到model文件夹内

#ifdef DEBUG

int main()
{
	FaceLibrary facelibrary;
	string modelFile = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/model";
	string RecogFile = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/RecgFace";
	//facenum决定识别出多少个相似的对象
	int facenum = 3;

	facelibrary.loadModel(modelFile);
	facelibrary.loadRecFace(RecogFile);
	facelibrary.recFace(facenum);
}

#endif // DEBUG

#ifndef DEBUG

int main(int argc, char* argv[]) 
{

	FaceLibrary facelibrary;

	if (argc != 4) {
		printf("Parameter Invalid\n");
		return -1;
	}

	int facenum = atoi(argv[3]);

	facelibrary.loadModel(argv[2]);
	facelibrary.loadRecFace(argv[1]);
	facelibrary.recFace(facenum);
}

#endif // !DEBUG


