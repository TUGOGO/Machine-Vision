#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
using namespace cv;
using namespace std;
#define x_direction 1
#define y_direction 2
#define max_pixel 10
void findedge(Mat& original, Mat& gray, vector<Point>& points, Rect& roi, int step, int
	direction)
{
	if (direction == y_direction) {
		Mat img_roi;
		GaussianBlur(gray(roi), img_roi, Size(3, 3), 1);
		uint8_t* pixel;
		pixel = img_roi.ptr();
		points.clear();
		for (int j = step; j < roi.height; j += step) {
			line(original, Point(roi.x, roi.y + j), Point(roi.x + roi.width, roi.y + j), Scalar(0, 0, 255));
			int max = -1, temp = -1;
			for (int i = 2, k = j * roi.width + 2; i < roi.width - 2; ++i, ++k) {
				int cha = abs((int)(pixel[k + 2]) - (int)(pixel[k - 2]));
				if (cha > max_pixel && cha > temp) {
					max = i;
					temp = cha;
				}
			}
			if (max > 0) {
				points.push_back(Point(roi.x + max, roi.y + j));
				circle(original, points.back(), 2, Scalar(0, 255, 255), 2);
			}
		}
	}
	if (direction == x_direction) {
		Mat img_roi;
		GaussianBlur(gray(roi), img_roi, Size(3, 3), 1);
		uint8_t* pixel;
		pixel = img_roi.ptr();
		points.clear();
		for (int i = step; i < roi.width; i += step) {
			line(original, Point(roi.x + i, roi.y), Point(roi.x + i, roi.y + roi.height), Scalar(0, 0, 255));
			int max = -1, temp = -1;
			for (int j = 2, k1 = i, k2 = 4 * roi.width + i; j < roi.height -
				2; ++j, k1 += roi.width, k2 += roi.width) {
				int cha = abs((int)(pixel[k2]) - (int)(pixel[k1]));
				//cout<<cha<<" ";
				if (cha > max_pixel && cha > temp) {
					max = j;
					temp = cha;
				}
			}
			if (max > 0) {
				points.push_back(Point(roi.x + i, roi.y + max));
				circle(original, points.back(), 2, Scalar(0, 255, 255), 2);
			}
		}
	}
}
int main()
{
	Mat imggray;
	Mat img = imread("C:/Users/86157/Desktop/OpenCVevi/MtoolImage/00005744 - A261.642.jpg");
	Rect roi1(600, 480, 680, 200), roi2(600, 480, 200, 540);
	vector<Point> point1, point2;
	cvtColor(img, imggray, COLOR_BGR2GRAY);
	findedge(img, imggray, point2, roi2, 20, y_direction);
	findedge(img, imggray, point1, roi1, 20, x_direction);
	int numx = point1.size();
	int numy = point2.size();

	//cout<<point1.size()<<endl<<point2.size();
	int xa = 0, ya = 0, xy = 0, xx = 0;
	float k, b;
	for (int i = 10; i < 20; i++) {
		xa += point1[i].x;
		ya += point1[i].y;
		xy += point1[i].x * point1[i].y;
		xx += point1[i].x * point1[i].x;
	}
	xa /= 10;
	ya /= 10;
	k = (xy - 10 * xa * ya) / (xx - 10 * xa ^ 2);
	b = ya - k * xa;
	//cout<<k<<" "<<b;
	line(img, Point(600, 600 * k + b), Point(1300, 1300 * k + b), Scalar(255, 0, 0), 3, 8, 0);
	int xa2 = 0, ya2 = 0, xy2 = 0, yy2 = 0;
	float k2, b2;
	for (int i = 10; i < 20; i++) {
		xa2 += point2[i].x;
		ya2 += point2[i].y;
		xy2 += point2[i].x * point2[i].y;
		yy2 += point2[i].y * point2[i].y;
	}
	xa2 /= 10;
	ya2 /= 10;
	k2 = (xy2 - 10 * xa2 * ya2) / (yy2 - 10 * ya2 ^ 2);
	b2 = xa2 - k2 * ya2;
	//cout<<k<<" "<<b;
	line(img, Point(500 * k2 + b2, 500), Point(1300 * k2 + b2, 1300), Scalar(255, 0, 0), 3, 8, 0);
	int x_x, y_y;
	y_y = (k * b2 + b) / (1 - k * k2);
	x_x = k2 * y_y + b2;
	putText(img, "(" + to_string(x_x) + "," + to_string(y_y) + ")", Point(x_x, y_y -
		50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
	imshow("img", img);
	waitKey(100000);
	return 0;

}