//
// Created by root on 17-11-4.
//

#ifndef JNIPROJECT_MAIN_CANNY_LIB_H
#define JNIPROJECT_MAIN_CANNY_LIB_H
#include "Line.h"
#include "MinHeap.h"
#include "canny.h"
using namespace std;
using namespace cv;



vector<Line> createLine(vector<Vec4f> lines);    // 构造直线
bool canCluster(Line l1, Line l2, int th);       // 判断能否聚合或连接
bool isPointNear(Point p1, Point p2, double th); // 判断两个点是否接近
double distanceBetweenLine(Line l1, Line l2);    // 估计两条直线间的距离
int isConnect(Line l1, Line l2, int th);         // 返回连接的类型

vector<Line> createLine(vector<Vec4f> lines);    // 对向量中的每一条直线构造Line对象
vector<Line> cleanShort(vector<Line> lines);     //清除短小直线
vector<Line> connectLines(vector<Line> lines, int th, Mat dst); // 连接直线
vector<Line> clusterLines(vector<Line> lines, int th, Mat dst); // 聚合直线
vector<Line> getTopK(vector<Line> lines, int K);         // 取得topK的直线
double lineDiff(vector<Line> line1, vector<Line> line2); // 计算两条直线的差距
void drawLine(vector<Line> lineSet, Mat image, Scalar color, string name); // 在图像中画出直线
double pointDistance(Point p1, Point p2);       // 计算两个点的距离


double getAngle(double k1, double k2);          // 计算两条直线夹角


#endif //JNIPROJECT_MAIN_CANNY_LIB_H
