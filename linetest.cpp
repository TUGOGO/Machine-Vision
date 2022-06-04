#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include<time.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

int max_delta = 10;
int interval = 20;

void edgedect(Mat &original_img,Mat &gray_img,Rect& roi, vector<Point>& plist,char flag) {
	Mat roi_img;
	GaussianBlur(gray_img(roi), roi_img, Size(3, 3), 1);
	uint8_t* pixel;
	pixel = roi_img.ptr();
	plist.clear();
	switch (flag) {
	case 'y':
		for (int i = interval; i < roi.height; i += interval) {
			line(original_img, Point(roi.x, roi.y + i), Point(roi.x + roi.width, roi.y + i), Scalar(0, 0, 255));
			int max = -1, temp = -1;
			for (int j = 2, k = i * roi.width + 2; j < roi.width - 2; ++j, ++k) {
				int cha = abs((int)(pixel[k + 2]) - (int)(pixel[k - 2]));
				if (cha > max_delta && cha > temp) {
					max = j;
					temp = cha;
				}
			}
			if (max > 0) {
				plist.push_back(Point(roi.x + max, roi.y + i));
				circle(original_img, plist.back(), 1, Scalar(0, 255, 255), 2);
			}
		}
		break;
	case 'x':
		for (int i = interval; i < roi.width; i += interval) {
			line(original_img, Point(roi.x+i, roi.y), Point(roi.x + i, roi.y + roi.height), Scalar(0, 0, 255));
			int set = 0, temp = 0;
			for (int j = 2, k1 = i ,k2 = 4*roi.width + i; j < roi.height - 2; ++j, k1+=roi.width, k2+=roi.width) {
				int cha = abs((int)(pixel[k2]) - (int)(pixel[k1]));
				if (cha > max_delta && cha > temp) {
					set = j;
					temp = cha;
				}
			}
			if (set > 0) {
				plist.push_back(Point(roi.x + i, roi.y + set));
				circle(original_img, plist.back(), 1, Scalar(0, 255, 255), 2);
			}
		}
		break;
	default:break;
	}
}

void least_sqaure(float &k,float &b, vector<Point>& plist, char flag) {
	int xa = 0, ya = 0, xy = 0, xx = 0, yy = 0, n = 0;
	for (uint8_t i = 1; i < plist.size(); i++) {
		float kt = pow(plist[i].x - plist[i - 1].x, 2)  + pow(plist[i].y - plist[i - 1].y, 2);
		if ( sqrt(kt) < interval + 0.1) {
			xa += plist[i].x;
			ya += plist[i].y;
			xy += plist[i].x * plist[i].y;
			xx += plist[i].x * plist[i].x;
			yy += plist[i].y * plist[i].y;
			n++;
		}
	}
	if (n != 0) {
		xa /= n;
		ya /= n;
	}
	else {
		cout << "error!" << endl;
	}
	if (flag == 'y') {
		k = (xy - n * xa * ya) / (xx - n * xa ^ 2);
		b = ya - k * xa;
	}
	else if (flag == 'x') {
		k = (xy - n * xa * ya) / (yy - n * ya ^ 2);
		b = xa - k * ya;
	}
}

int main() {
	Mat gray;
	Mat img = imread("./MtoolImage/00005746 - A261.842.jpg");
	Rect roix(600, 480, 680, 200), roiy(600, 480, 200, 540);
	vector<Point>plistx, plisty;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	edgedect(img, gray, roiy, plisty, 'y');
	edgedect(img, gray, roix, plistx, 'x');

	int numx = plistx.size();
	int numy = plisty.size();
	float k1, b1;
	least_sqaure(k1, b1, plistx, 'y');
	line(img, Point(600, 600 * k1 + b1), Point(1300, 1300 * k1 + b1), Scalar(255, 255, 0), 2, 8, 0);
	
	float k2, b2;
	least_sqaure(k2, b2, plisty, 'x');
	line(img, Point(480 * k2 + b2, 480), Point(1300 * k2 + b2, 1300), Scalar(255, 255, 0), 2, 8, 0);
	int x, y;
	y = (k1 * b2 + b1) / (1 - k1 * k2);
	x = k2 * y + b2;
	putText(img, "(" + to_string(x) + "," + to_string(y) + ")", Point(x, y -50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
	imshow("img", img);
	waitKey(10000);
	return 0;
}