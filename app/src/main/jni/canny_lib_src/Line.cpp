#include "Line.h"
#include<opencv2/opencv.hpp>
#include<math.h>

#define PI 3.1415926


using namespace cv;

Line::Line(Vec4f line) {

	// 计算起点，终点和中点
	start = Point(line[0], line[1]);
	end = Point(line[2], line[3]);
	mid = getMidPoint();
	
	length = getLength(start, end); // 计算长度和相对长度
	k = getK(); // 计算直线的斜率
	theta = getTheta(); //计算直线与水平方向的夹角
}

Line::Line(void) {
}

Line::~Line(void){
}

/**
 * 计算直线的长度
 */
double Line::getLength() {
	return getLength(start, end);
}

double Line::getLength(Point start, Point end) {
	double x = start.x - end.x;
	double y = start.y - end.y;
	return sqrt(pow(x, 2) + pow(y, 2));
}

/**
 * 计算直线的中点
 */
Point Line::getMidPoint() {
	double mid_x = (start.x + end.x) / 2;
	double mid_y = (start.y + end.y) / 2;
	Point *mid = new Point(mid_x, mid_y);
	return *mid;
}

/**
 * 计算斜率
 */
double Line::getK() {
	double k = 100000;
	if (abs(end.x - start.x) > 1e-6) {
		k = (double)(start.y - end.y) / (start.x - end.x);
	}
	return k;
}


/**
 * 计算theta
 */
double Line::getTheta() {
	return atan(k) * 180 / PI;
}