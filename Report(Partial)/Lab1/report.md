



## 1.实验目的和要求

对输入的一个彩色视频与五张以上照片，用OpenCV实现以下功能或要求：

1. 命令行格式: “xxx.exe 放视频与照片的文件夹路径” ,（例如 MyMakeVideo.exe C:\input ）【假设该文件夹下面只有一个avi视频文件与若干jpg文件】
2. 将输入的视频与照片处理成同样长宽后，合在一起生成一个视频；
3. 这个新视频中，编程生成一个片头，然后按幻灯片形式播放这些输入照片，最后按视频原来速度播放输入的视频；
4. 新视频中要在底部打上含自己学号与姓名等信息的字幕； 
5. 有能力的同学，可以编程实现镜头切换效果；
6. 视频文件无需上载 ，但在实验报告里必须贴图展示输入输出效果



## 2.实验内容和原理

1. 通过`VideoWriter`以及`VideoCapture`来实现视频的读写；
2. 通过`Mat`数据结构来存储图片数据；
3. 通过`text`来实现可显示的文本；



## 3.实验步骤与分析

### 3.1 程序主函数：

读取命令行输入，传入函数`generateMovie()`中生成片头，参数数量不足则报错返回。

```c
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
```



### 3.2 生成视频`generateMovie`

第一步，获取文件夹内所有的文件名，选出".avi"结尾的文件为视频文件并读取；

读取视频通过openCV中的`VideoCapture`类实现，再结合这个类中的相关方法可以获取读取的视频中的各类参数；



第二步，生成输出文件，通过`VideoWriter`类实现视频写出，使用open方法后创建好输出视频，随后，只需要将一帧一帧图片通过`<<`传入即可。



第三步，生成片头，通过函数`myMovieHead()`完成，后文详细介绍；



第四步，读取图片并调整到与视频大小一致，找出先前读取的文件名，找出其中以“.jpg”结尾的文件并读取(`imread()`)，通过`resize()`函数调整至与视频大小相同。



第四步，图片写入，图片之间的镜头切换效果在本程序中通过渐变来实现；

渐变效果实现的原理，是通过`addWeighted()`将两个图片按照一定权重融合，权重随时间线性变化，将连续多帧融合后的图片写入后，就产生了渐变切换的效果(代码如下)：

```c
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
```

两个图片的权重alpha与beta随时间变化，从前者为1后者为0变为后者为1前者为0，从而实现两个图片之间切换。

这里通过自定义函数`putTextwithImg()`来将图像写入视频，主要是为了增加字幕，后文介绍。



第五步，写入视频，不断地将视频中每一帧写入输出视频，直到写完全部帧数即可：

```c
while (1)
{
    inputVideo >> src;
    if (src.empty()) 
        break;
    putTextwithImg(src);

}
```

### 3.3 生成片头`myMovieHead()`

自定义的片头的实现，参照了openCV官方教程中的代码：[OpenCV: Basic Drawing](https://docs.opencv.org/4.3.0/d3/d96/tutorial_basic_geometric_drawing.html)

其主要内容是通过openCV内部函数`ellipse()`绘制多个椭圆，再通过`circle()`函数来绘制一个圆作为中心，绘制在一个图像上，产生一个类似原子的现实效果。

在多帧图片写入的过程中，修改绘制椭圆的参数，实现外围椭圆绕中心圆点旋转的效果：

同时注意将图像大小调整至于视频大小相同；

```c
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
```

### 3.4  添加字幕`putTextwithImg`

通过openCV实现的类`text`可以实现字幕功能，将字幕加入每一帧要写入的图像，写入视频中，形成字幕效果。

```c
void putTextwithImg(Mat img)
{
	string text = "3180105883 Ziqi Wen";
	Size textsize = getTextSize(text, FONT_HERSHEY_PLAIN, 2, 1, 0);
	Point org((img.cols - textsize.width) / 2, (img.rows - textsize.height) / 8 * 7);
	putText(img, text, org, FONT_HERSHEY_PLAIN, 2.0, Scalar(255, 255, 255));
	outputVideo << img;
}
```





