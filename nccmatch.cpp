// HW_meanfilter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include<time.h>

using namespace cv;
using namespace std;

struct match
{
	Mat img;
	int x=0;
	int y=0;
	float nc=0;
	float tim=0;
}ma1;

match nccmat(Mat img, Mat temp);

int main()
{
	
	Mat temp = imread("C:/Users/86157/Desktop/OpenCVevi/image7/model.jpg");
	cvtColor(temp, temp, CV_BGR2GRAY);
	for (unsigned int i = 1; i < 20; i++)
	{
		string name = format("C:/Users/86157/Desktop/OpenCVevi/image7/%d.jpg", i);
		Mat image = imread(name);
		if (image.empty())      // please, *always check* resource-loading.
		{
			cerr << "whaa " << name << " can't be loaded!" << endl;
			continue;
		}
		cvtColor(image, image, CV_BGR2GRAY);
		double startTime1 = clock();//1计时开始
		ma1 = nccmat(image, temp);
		double endTime1 = clock();//1计时结束
		int curx = ma1.x;
		int cury = ma1.y;
		//imshow("原图", image);
		//imshow("模板", temp);
		//imshow("结果", ma1.img);
		// 输出1运行时间; 
		std::cout << "Image " << i << endl;
		std::cout << "The position is: " << ma1.x << "," << ma1.y << endl;
		std::cout << "The NCC is: " << ma1.nc << endl;
		std::cout << "The run time of meanfilter is: " << (double)(endTime1 - startTime1) / CLOCKS_PER_SEC << "s" << std::endl << endl;
	}
	waitKey(1000000);
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

match nccmat(Mat img, Mat temp)
{
	float mt = 0;
	float st = 0;
	float matr = 0;
	float matc = 0;
	Mat result;
	match rt;

	double startTime1 = clock();//1计时开始

	GaussianBlur(img, img, Size(9, 9), 1);
	GaussianBlur(temp, temp, Size(9, 9), 1);
	Mat small_Image;
	Mat small_Temp;
	img.copyTo(small_Image);
	temp.copyTo(small_Temp);

	
	Size dsizei = Size(img.rows / 2, img.cols / 2);
	resize(img, small_Image, dsizei, 0, 0, INTER_AREA);
	Size dsizet = Size(temp.rows / 2, temp.cols / 2);
	resize(temp, small_Temp, dsizet, 0, 0, INTER_AREA);

	GaussianBlur(small_Image, small_Image, Size(9, 9), 1);
	GaussianBlur(small_Temp, small_Temp, Size(9, 9), 1);
	dsizei = Size(small_Image.rows / 2, small_Image.cols / 2);
	resize(small_Image, small_Image, dsizei, 0, 0, INTER_AREA);
	dsizet = Size(small_Temp.rows / 2, small_Temp.cols / 2);
	resize(small_Temp, small_Temp, dsizet, 0, 0, INTER_AREA);
	
	for (int rm = 0; rm < small_Temp.rows; rm++)
	{
		for (int cm = 0; cm < small_Temp.cols; cm++)
		{
			mt += small_Temp.at<uchar>(rm, cm);
		}
	}
	mt = mt / (small_Temp.rows * small_Temp.cols);
	for (int rm = 0; rm < small_Temp.rows; rm++)
	{
		for (int cm = 0; cm < small_Temp.cols; cm++)
		{
			st += pow(small_Temp.at<uchar>(rm, cm) - mt, 2);
		}
	}
	for (int r = 0; r < (small_Image.rows - small_Temp.rows); r++)
	{
		for (int c = 0; c < (small_Image.cols - small_Temp.cols); c++)
		{
			float mf = 0;
			float sf = 0;
			float ncc = 0;
			for (int rm = r; rm < r + small_Temp.rows; rm++)
			{
				for (int cm = c; cm < c + small_Temp.cols; cm++)
				{
					mf += small_Image.at<uchar>(rm, cm);
				}
			}
			mf = mf / (small_Temp.rows * small_Temp.cols);
			for (int rm = r; rm < r + small_Temp.rows; rm++)
			{
				for (int cm = c; cm < c + small_Temp.cols; cm++)
				{
					sf += pow(small_Image.at<uchar>(rm, cm) - mf, 2);
				}
			}
			for (int rm = r; rm < r + small_Temp.rows; rm++)
			{
				for (int cm = c; cm < c + small_Temp.cols; cm++)
				{
					ncc += (small_Temp.at<uchar>(rm - r, cm - c) - mt) * (small_Image.at<uchar>(rm, cm) - mf);
				}
			}
			ncc = pow(ncc / sqrt(st * sf), 2);
			
			if (ncc > 0.6 && ncc <= 1 && ncc > rt.nc)
			{
				matr = r;
				matc = c;
			}
		}
	}
	int startr = matr*img.rows /small_Image.rows - matr * img.rows / (small_Image.rows * 16);
	int endr = matr * img.rows / small_Image.rows + matr * img.rows / (small_Image.rows * 16);

	if (endr > img.rows - temp.rows)
	{
		endr = img.rows - temp.rows;
	}
	int startc = matc * img.cols / small_Image.cols - matc * img.cols / (small_Image.cols * 16);
	int endc = matc * img.cols / small_Image.cols + matc * img.cols / (small_Image.cols * 16);
	if (endc > img.cols - temp.cols)
	{
		endc = img.cols - temp.cols;
	}
	mt = 0;
	st = 0;
	matr = 0;
	matc = 0;
	for (int rm = 0; rm < temp.rows; rm++)
	{
		for (int cm = 0; cm < temp.cols; cm++)
		{
			mt += temp.at<uchar>(rm, cm);
		}
	}
	mt = mt / (temp.rows * temp.cols);
	for (int rm = 0; rm < temp.rows; rm++)
	{
		for (int cm = 0; cm < temp.cols; cm++)
		{
			st += pow(temp.at<uchar>(rm, cm) - mt, 2);
		}
	}
	for (int r = startr; r < endr; r++)
	{
		for (int c = startc; c < endc; c++)
		{
			float mf = 0;
			float sf = 0;
			float ncc = 0;
			for (int rm = r; rm < r + temp.rows; rm++)
			{
				for (int cm = c; cm < c + temp.cols; cm++)
				{
					mf += img.at<uchar>(rm, cm);
				}
			}
			mf = mf / (temp.rows * temp.cols);
			for (int rm = r; rm < r + temp.rows; rm++)
			{
				for (int cm = c; cm < c + temp.cols; cm++)
				{
					sf += pow(img.at<uchar>(rm, cm) - mf, 2);
				}
			}
			for (int rm = r; rm < r + temp.rows; rm++)
			{
				for (int cm = c; cm < c + temp.cols; cm++)
				{
					ncc += (temp.at<uchar>(rm - r, cm - c) - mt) * (img.at<uchar>(rm, cm) - mf);
				}
			}
			ncc = pow(ncc / sqrt(st * sf), 2);
			if (ncc > 0.6 && ncc <= 1 && ncc > rt.nc)
			{
				matr = r;
				matc = c;
				rt.nc = ncc;
			}
		}
	}
	double endTime1 = clock();//1计时结束
	cvtColor(img,result,CV_GRAY2RGB);
	Rect rect(matc, matr, temp.cols, temp.rows);
	cv::rectangle(result, rect, Scalar(255, 0, 0), 1, LINE_8, 0);
	rt.img = result;
	rt.x = matr;
	rt.y = matc;
	rt.tim = endTime1 - startTime1;
	return rt;
}