#include "eigenFace_train.h"


PreTreat::PreTreat()
{
	ROI.x = 0;
	ROI.y = 0;
	ROI.height = 80;
	ROI.width = 60;
}

PreTreat::~PreTreat() {}


//NumImg表示文件内图片数量
void PreTreat::PretreatMent(string Dirin, string Dirout, int NumImg, bool isTrain)
{
	string Prefix = "BioID_";
	char img_src[200];
	char eye_src[200];
	int increment;
	if (isTrain)
	{
		increment = 11;
	}
	else
	{
		increment = 7;
	}
	for (int i = 0; i < NumImg; i += increment)
	{
		sprintf(img_src, "%s/%s%04d.pgm", Dirin.data(), Prefix.data(), i);
		sprintf(eye_src, "%s/%s%04d.eye", Dirin.data(), Prefix.data(), i);
		Mat img = imread(img_src, IMREAD_GRAYSCALE);

		//读取眼睛位置并转换为字符串
		ifstream in(eye_src);
		ostringstream buf;
		char ch;
		while (buf && in.get(ch))
			buf.put(ch);
		string PosEye = buf.str();


		Eye pos_eye;
		//用正则表达式确定眼睛位置,对结构体赋值
		regex ip_reg("(.*)\t(.*)\t(.*)\t(.*)\n(.*)\t(.*)\t(.*)\t(.*)\n");
		smatch matchResult;
		if (regex_match(PosEye, matchResult, ip_reg))
		{
			string s = matchResult[5].str();
			pos_eye.LX = atoi(s.c_str());
			s = matchResult[6].str();
			pos_eye.LY = atoi(s.c_str());
			s = matchResult[7].str();
			pos_eye.RX = atoi(s.c_str());
			s = matchResult[8].str();
			pos_eye.RY = atoi(s.c_str());
		}
		else
		{
			cout << "Not match!" << endl;
			return;
		}


		//按照眼间距放缩图片
		pos_eye.distance = sqrt(pow(pos_eye.LX - pos_eye.RX, 2) + pow(pos_eye.LY - pos_eye.RY, 2));
		double ROIdistance = ROI.width * 1.0 / 3;
		double ratio = ROIdistance / pos_eye.distance;
		Mat img_resize;
		resize(img, img_resize, Size(img.cols * ratio, img.rows * ratio), 0, 0, CV_INTER_LINEAR);

		//重新定位ROI,注意这里的右眼是人的右眼，不是右边的眼
		ROI.x = pos_eye.RX * ratio - ROI.width * 1.0 / 3;
		if (ROI.x <= 0)
		{
			ROI.x = 1;
		}
		//Q:Y坐标的正方向是？这里假设为向下
		ROI.y = pos_eye.RY * ratio - ROI.height * 1.0 / 3;
		if (ROI.y <= 0)
		{
			ROI.y = 1;
		}

		//裁剪ROI
		if (ROI.x + ROI.width > img_resize.cols || ROI.y + ROI.height > img_resize.rows)
		{
			printf("CORP ERROR:img:%d\n", i);
			continue;
		}
		Mat img_roi = img_resize(ROI);
		char img_dst[100];
		sprintf(img_dst, "%s/%s%04d.pgm", Dirout.data(), Prefix.data(), i);

		equalizeHist(img_roi, img_roi);

		//DEBUG
		/*cvNamedWindow("after_corp", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("before_corp", CV_WINDOW_AUTOSIZE);
		imshow("before_corp", img_resize);
		imshow("after_corp", img_roi);
		Point lefteye = Point(ROI.x + ROI.width / 3, ROI.y + ROI.height / 3);
		Point righteye = Point(ROI.x + ROI.width * 2 / 3, ROI.y + ROI.height / 3);
		rectangle(img_resize, ROI, Scalar(0, 0, 255));
		line(img_resize, lefteye - Point(5, 0), lefteye + Point(5, 0), Scalar(0, 0, 255));
		line(img_resize, lefteye - Point(0, 5), lefteye + Point(0, 5), Scalar(0, 0, 255));
		line(img_resize, righteye - Point(5, 0), righteye + Point(5, 0), Scalar(0, 0, 255));
		line(img_resize, righteye - Point(0, 5), righteye + Point(0, 5), Scalar(0, 0, 255));
		imshow("before_corp", img_resize);
		cvWaitKey(0);
		cvDestroyWindow("after_corp");
		cvDestroyWindow("before_corp");*/

		imwrite(img_dst, img_roi);
	}
}

void PreTreat::Interface(int mode)
{
	string Dirin = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/BioID-FaceDatabase-V1.2";

	
	string Dirout;
	//用于预处理训练集
	if (mode == 0)
	{
		Dirout = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/FaceDataBase_pretreat";
		PretreatMent(Dirin, Dirout, 1521, true);
	}
	//用于预处理测试集
	else if(mode==1)
	{
		Dirout = "D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/RecgFace";
		PretreatMent(Dirin, Dirout, 1521, false);
	}
	

}



FaceLibrary::FaceLibrary(){}
FaceLibrary::~FaceLibrary(){}

void FaceLibrary::loadFace(string input)
{
	DIR* dir = NULL;
	dir = opendir(input.data());
	dirent* ent = NULL;

	if (dir == NULL)
		return;

	while ((ent = readdir(dir)) != NULL)
	{
		char* suffix = new char[100];
		char* name = new char[100];
		sscanf(ent->d_name, "%*[^.].%s", suffix);
		sscanf(ent->d_name, "%[^.]%*s", name);
		if (strcmp(suffix, "pgm") == 0)
		{
			Face* f = new Face();
			f->name = name;
			char* img_src = new char[100];
			sprintf(img_src, "%s/%s", input.data(), ent->d_name);
			IplImage* img = cvLoadImage(img_src, CV_LOAD_IMAGE_GRAYSCALE);
			CvMat* mat = cvCreateMat(img->height, img->width, CV_64FC1);
			cvConvert(img, mat);
			f->data = mat;
			face.push_back(f);
		}
	}
	row = face[0]->data->rows;
	col = face[0]->data->cols;
	number = face.size();
}

void FaceLibrary::getMeanFace(string input)
{
	printf("number:%d, row:%d, col:%d\n", number, row, col);
	//CV_[bite](U|S|F)C[channels]  F : float
	image_points[0] = cvCreateMat(number, row * col, CV_64FC1);

	for (int num = 0; num < number; num++)
	{
		for (int i = 0; i < row; i++) 
			for (int j = 0; j < col; j++)
			{
				//imagepoint中的每一行储存一个图片的所有像素点位置
				//为了作为计算协方差的输入
				CV_MAT_ELEM(*image_points[0], double, num, i * col + j) = CV_MAT_ELEM(*face[num]->data, double, i, j);
			}
	}
	covarCvMat = cvCreateMat(row*col, row*col, CV_64FC1);
	meanCvMat = cvCreateMat(1, row * col, CV_64FC1);
	cvCalcCovarMatrix((const CvArr**)image_points, number, covarCvMat, meanCvMat, CV_COVAR_NORMAL | CV_COVAR_ROWS);

	CvMat* mean = cvCreateMat(row, col, CV_64FC1);
	for(int i=0;i<row;i++)
		for (int j = 0; j < col; j++)
		{
			CV_MAT_ELEM(*mean, double, i, j) = CV_MAT_ELEM(*meanCvMat, double, 0, i * col + j);
		}

	cvNamedWindow("meanFace");


	//CvMat->IplImage->Mat
	CvSize s;
	s.width = mean->cols;
	s.height = mean->rows;

	IplImage* mm = cvCreateImage(s, 8, 1);
	cvConvert(mean, mm);

	Mat src;
	src = cvarrToMat(mm);

	
	imshow("meanFace", src);
	//储存平均脸图片
	imwrite(input + "/mean.jpg", src);
	//暂定
	//imwrite("D:/File_WZQ/Code_VS/CV_hw3/myEigenFace/Data/mean.jpg", src);
	cvWaitKey(0);
	cvDestroyWindow("meanFace");

	cvReleaseImage(&mm);
}

void FaceLibrary::getEigen(string input)
{
	eigenValue = cvCreateMat(row * col, 1, CV_64FC1);
	eigenVector = cvCreateMat(row * col, row * col, CV_64FC1);
	printf("ready to compute eigen\n");
	cvEigenVV(covarCvMat, eigenVector, eigenValue);
	printf("finish computing eigen\n");

	CvSize s;
	s.width = col * 5;
	s.height = row * 2;
	IplImage* dstImg = cvCreateImage(s, 8, 1);

	cvZero(dstImg);

	for (int t1 = 0; t1 < 2; t1++)
	{
		for (int t2 = 0; t2 < 5; t2++)
		{
			CvMat* img_mat = cvCreateMat(row, col, CV_64FC1);
			for (int i = 0; i < row; i++)
				for (int j = 0; j < col; j++)
					CV_MAT_ELEM(*img_mat, double, i, j) = CV_MAT_ELEM(*eigenVector, double, t1 * 5 + t2, i * col + j);
			cvNormalize(img_mat, img_mat, 255.0, 0.0, CV_MINMAX);
			CvSize s;
			s.height = row;
			s.width = col;
			IplImage* img = cvCreateImage(s, 8, 1);
			cvConvert(img_mat, img);
			cvSetImageROI(dstImg, cvRect(t2 * col, t1 * row, col, row));
			cvCopy(img, dstImg);
			cvResetImageROI(dstImg);
		}
	}

	Mat src;
	src = cvarrToMat(dstImg);

	cvNamedWindow("eigenFace");
	cvShowImage("eigenFace", dstImg);
	//储存特征脸
	imwrite(input + "eigenFace.jpg", src);
	//cvSaveImage("../Resource/dstimg.jpg", dstImg);
	cvWaitKey(-1);
	cvDestroyWindow("eigenFace");
}



void FaceLibrary::saveModel(string input, int energyNum)
{
	A = cvCreateMat(row * col, energyNum, CV_64FC1);
	//Save image of eigenFace
	for (int num = 0; num < energyNum; num++)
	{
		CvMat* eigenFace = cvCreateMat(row, col, CV_64FC1);
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				CV_MAT_ELEM(*eigenFace, double, i, j) = CV_MAT_ELEM(*eigenVector, double, num, i * col + j);
			}
		}
		cvNormalize(eigenFace, eigenFace, 255.0, 0.0, CV_MINMAX);
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				CV_MAT_ELEM(*A, double, i * col + j, num) = CV_MAT_ELEM(*eigenFace, double, i, j);
			}
		}

		Mat src;
		src = cvarrToMat(eigenFace);
		char* img_src = new char[100];
		sprintf(img_src, "%s/eigenFace%d.pgm", input.data(), num);
		imwrite(img_src, src);
	}

	CvMat* At = cvCreateMat(energyNum, row * col, CV_64FC1);
	cvTranspose(A, At);
	char* src = new char[100];
	sprintf(src, "%s/A.xml", input.data());
	cvSave(src, A);
	cout << "finish saving eigen" << endl;

	CvMat* X = cvCreateMat(row * col, number, CV_64FC1);
	cvTranspose(image_points[0], X);
	eigen_image = cvCreateMat(energyNum, number, CV_64FC1);
	cvMatMul(At, X, eigen_image);
	sprintf(src, "%s/EigenImage.xml", input.data());
	cvSave(src, eigen_image);

	for (int n = 0; n < number; n++) {
		CvMat* yi = cvCreateMat(energyNum, 1, CV_64FC1);
		for (int i = 0; i < energyNum; i++)
			CV_MAT_ELEM(*yi, double, i, 0) = CV_MAT_ELEM(*eigen_image, double, i, n);
		char* img_src = new char[100];
		sprintf(img_src, "%s/face/%s.xml", input.data(), face[n]->name);
		cvSave(img_src, yi);
	}
	cout << "finish eigen face" << endl;

}

void FaceLibrary::loadModel(string modelFile)
{
	//load matrix A
	char* A_src = new char[100];
	sprintf(A_src, "%s/A.xml", modelFile.data());
	A = (CvMat*)cvLoad(A_src);


	//load face
	DIR* dir = NULL;
	char* face_src = new char[100];
	sprintf(face_src, "%s/face", modelFile.data());

	dir = opendir(face_src);

	dirent* ent = NULL;
	if (dir == NULL)
		return;

	while ((ent = readdir(dir)) != NULL)
	{
		char* suffix = new char[100];
		char* name = new char[100];
		sscanf(ent->d_name, "%*[^.].%s", suffix);
		sscanf(ent->d_name, "%[^.]%*s", name);
		if (strcmp(suffix, "xml") == 0)
		{
			Face* f = new Face();
			f->name = name;
			char* model_src = new char[100];
			sprintf(model_src, "%s/face/%s", modelFile.data(), ent->d_name);
			CvMat* model = (CvMat*)cvLoad(model_src);
			f->eigen = model;

			//读取对应的图像数据
			char* img_src = new char[100];

			sprintf(img_src, "%s/face/%s%s", modelFile.data(), name, ".pgm");
			IplImage* img = cvLoadImage(img_src, CV_LOAD_IMAGE_GRAYSCALE);
			CvMat* mat = cvCreateMat(img->height, img->width, CV_64FC1);
			cvConvert(img, mat);
			f->data = mat;
			face.push_back(f);
		}
	}

	number = face.size();
	eigen_number = face[0]->eigen->rows;
}

void FaceLibrary::loadRecFace(string input)
{
	DIR* dir = NULL;
	dir = opendir(input.data());

	dirent* ent = NULL;

	if (dir == NULL) return;

	while ((ent = readdir(dir)) != NULL) {
		char* suffix = new char[100];
		char* name = new char[100];
		sscanf(ent->d_name, "%*[^.].%s", suffix);
		sscanf(ent->d_name, "%[^.]%*s", name);
		if (strcmp(suffix, "pgm") == 0) {
			Face* f = new Face();
			f->name = name;
			char* img_src = new char[100];
			sprintf(img_src, "%s/%s", input.data(), ent->d_name);
			IplImage* d = cvLoadImage(img_src, CV_LOAD_IMAGE_GRAYSCALE);
			CvMat* mat = cvCreateMat(d->height, d->width, CV_64FC1);
			cvConvert(d, mat);
			f->data = mat;
			rec_face.push_back(f);
		}
	}
	row = rec_face[0]->data->rows;
	col = rec_face[0]->data->cols;
}


void FaceLibrary::recFace(int recn)
{
	//recn表示选取多少个相似的人脸（按照距离由小到大排序）
	CvMat* At = cvCreateMat(eigen_number, row * col, CV_64FC1);
	cvTranspose(A, At);

	for (int t = 0; t < rec_face.size(); t++)
	{
		//cvNamedWindow(rec_face[t]->name);
		CvMat* rec = cvCreateMat(row * col, 1, CV_64FC1);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				CV_MAT_ELEM(*rec, double, i * col + j, 0) = CV_MAT_ELEM(*rec_face[t]->data, double, i, j);
		rec_face[t]->eigen = cvCreateMat(eigen_number, 1, CV_64FC1);
		cvMatMul(At, rec, rec_face[t]->eigen);


		//比较欧氏距离，选出距离最近的recn个人脸
		double min[200];
		int min_index[200];

		for (int i = 0; i < recn; i++)
		{
			min[i] = cvNorm(face[i]->eigen, rec_face[t]->eigen, CV_L2);
			min_index[i] = i;
		}

		for (int i = recn; i < face.size(); i++) {
			double dis = cvNorm(face[i]->eigen, rec_face[t]->eigen, CV_L2);
			if (dis < min[recn - 1]) {
				for (int j = 0; j < recn; j++) {
					if (dis < min[j]) {
						for (int k = recn - 1; k > j; k--)
						{
							min[k] = min[k - 1];
							min_index[k] = min_index[k - 1];
						}
						min[j] = dis;
						min_index[j] = i;
						j = recn;
					}
				}
			}
		}

		cout << "Test people: " << rec_face[t]->name << endl;
		for (int i = 0; i < recn; i++)
			cout << "This might be " << face[min_index[i]]->name << "; distance is " << min[i] << endl;



		//输出图片（需要放大）
		cvNamedWindow("Test people", CV_WINDOW_AUTOSIZE);
		CvSize s;
		s.width = rec_face[t]->data->cols;
		s.height = rec_face[t]->data->rows;
		IplImage* img_rec = cvCreateImage(s, 8, 1);
		cvConvert(rec_face[t]->data, img_rec);
		Mat mat_rec;
		mat_rec = cvarrToMat(img_rec);
		resize(mat_rec, mat_rec, Size(mat_rec.cols * 2, mat_rec.rows * 2), 0, 0, CV_INTER_LINEAR);
		imshow("Test people", mat_rec);


		cvNamedWindow("Test result", CV_WINDOW_AUTOSIZE);
		s.width = face[min_index[0]]->data->cols;
		s.height = face[min_index[0]]->data->rows;
		IplImage* img_result = cvCreateImage(s, 8, 1);
		cvConvert(face[min_index[0]]->data, img_result);
		Mat mat_result;
		mat_result = cvarrToMat(img_result);
		resize(mat_result, mat_result, Size(mat_result.cols * 2, mat_result.rows * 2), 0, 0, CV_INTER_LINEAR);
		imshow("Test result", mat_result);

		cvWaitKey(-1);
		cvDestroyWindow("Test people");
		cvDestroyWindow("Test result");
	}
}



