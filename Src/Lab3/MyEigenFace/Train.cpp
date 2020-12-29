#include "eigenFace_train.h"
//#define DEBUG

#ifndef DEBUG
int main(int argc, char* argv[]) {

	if (argc != 4) {
		printf("Parameters Invalid");
		return -1;
	}

	int energy = atoi(argv[1]);
	char* folder = argv[2];
	int mode = atoi(argv[3]);

	FaceLibrary library;
	PreTreat preTreat;


	//预处理训练集
	if (mode == 0)
	{
		preTreat.Interface(0);
	}
	//预处理测试集
	else if (mode == 1)
	{
		preTreat.Interface(1);
	}
	//训练
	else if (mode == 2)
	{
		char* load_src = new char[100];
		sprintf(load_src, "%s/FaceDataBase_pretreat", folder);
		char* model_src = new char[100];
		sprintf(model_src, "%s/model", folder);
		library.loadFace(load_src);
		library.getMeanFace(folder);
		library.getEigen(folder);
		library.saveModel(model_src, energy);
	}
	//全部进行
	else if (mode == 3)
	{
		preTreat.Interface(0);
		preTreat.Interface(1);

		char* load_src = new char[100];
		sprintf(load_src, "%s/FaceDataBase_pretreat", folder);
		char* model_src = new char[100];
		sprintf(model_src, "%s/model", folder);
		library.loadFace(load_src);
		library.getMeanFace(folder);
		library.getEigen(folder);
		library.saveModel(model_src, energy);
	}
	else
	{
		cout << "Invaid mode!" << endl;
	}

	return 0;
}

#endif // !DEBUG


#ifdef DEBUG
int main(int argc, char* argv[]) {

	
	int energy = 10;
	const char* folder = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data";
	int mode = 2;
	FaceLibrary library;
	PreTreat preTreat;


	//预处理训练集
	if (mode == 0) 
	{
		preTreat.Interface(0);
	}
	//预处理测试集
	else if (mode == 1)
	{
		preTreat.Interface(1);
	}
	//训练
	else if (mode == 2)
	{
		char* load_src = new char[100];
		sprintf(load_src, "%s/FaceDataBase_pretreat", folder);
		char* model_src = new char[100];
		sprintf(model_src, "%s/model", folder);
		library.loadFace(load_src);
		library.getMeanFace(folder);
		library.getEigen(folder);
		library.saveModel(model_src, energy);
	}
	//全部进行
	else if (mode == 3)
	{
		preTreat.Interface(0);
		preTreat.Interface(1);

		char* load_src = new char[100];
		sprintf(load_src, "%s/FaceDataBase_pretreat", folder);
		char* model_src = new char[100];
		sprintf(model_src, "%s/model", folder);
		library.loadFace(load_src);
		library.getMeanFace(folder);
		library.getEigen(folder);
		library.saveModel(model_src, energy);
	}
	else
	{
		cout << "Invaid mode!" << endl;
	}

	return 0;
}

#endif // DEBUG