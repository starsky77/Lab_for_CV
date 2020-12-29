#include "eigenFace_test.h"
//#define DEBUG
//���в���֮ǰ���뽫Ԥ�����ȡ��ѵ������Ƭ�ƶ���model�ļ�����

#ifdef DEBUG

int main()
{
	FaceLibrary facelibrary;
	string modelFile = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/model";
	string RecogFile = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/RecgFace";
	//facenum����ʶ������ٸ����ƵĶ���
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


