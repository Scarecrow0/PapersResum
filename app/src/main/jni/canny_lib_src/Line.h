#pragma once
#include<opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class Line
{
public:
	Vec4f line;    // 直线本身
	Point start;   // 直线的起点
	Point end;     // 直线的终点
	Point mid;     // 直线的中点
	double length; // 直线的长度
	double k;      // 直线的斜率
	double theta;  // 直线与水平方向的夹角


	Line(Vec4f line);
	Line(void);
	~Line(void);

	double getLength(); // 计算长度
	double getLength(Point start, Point end); // 计算长度的重载函数
	Point getMidPoint(); // 计算中点
	double getK(); // 计算斜率
	double getTheta(); // 计算theta
};