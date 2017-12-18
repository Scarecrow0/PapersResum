/*************************************************

Copyright:bupt
Author: lijialin 1040591521@qq.com
Date:2017-10-14
Description:写这段代码的时候，只有上帝和我知道它是干嘛的
            现在只有上帝知道

**************************************************/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include <hash_set>
#include "Line.h"
#include "MinHeap.h"
#include "canny.h"
#include "main_canny_lib.h"

#define MAX 1.7976931348623158e+308

using namespace cv;
using namespace std;


/***************************** 函数声明部分 start ***********************************/


/***************************** 函数声明部分 end *************************************/




/**
 * 对向量中的每一条直线构造Line对象
 * 返回一个向量集合，集合里的元素是Line对象
 * 输入：包含Vec4f类型直线的向量
 * 输出：包含Line对象的向量
 */
vector<Line> createLine(vector<Vec4f> lines) {
	vector<Line> LineSet;
	size_t len = lines.size();
	for (int i = 0; i < len; i++) {
		Line *line = new Line(lines[i]);
		LineSet.push_back(*line);
	}
	vector<Vec4f>().swap(lines);
	return LineSet;
}


/**
 * 过滤短小直线
 * TODO 过滤孤立的直线
 * 输入：包含Line对象的向量
 * 输出：清除短小直线后的Line对象向量
 */
vector<Line> cleanShort(vector<Line> lines) {
	vector<Line> *result = new vector<Line>();
	size_t length = lines.size();
	if (length == 0) return lines;

	// 计算average长度
	double sum = 0;
	for (int i = 0; i < length; i++) {
		Line line = lines[i];
		sum += line.length;
	}
	double avg = sum / length;

	// 过滤短小的直线
	for (int i = 0; i < length; i++) {
		Line line = lines[i];
		if (line.length >= avg) {
			(*result).push_back(line);
		}		
	}
	vector<Line>().swap(lines);
	return *result;
}


/**
 * 判断两条直线是否具备聚合条件
 * 判断规则：斜率相近，直线间距相近，则可以聚合
 * 输入：两条直线（Line），和阈值
 * 输出：bool型，true表示两条直线可以聚合，false表示不能聚合
 */
bool canCluster(Line l1, Line l2, int th) {
	// double th = (l1.length + l2.length) / 2.0;
	return abs(l1.k - l2.k) <= 0.3 &&  // 斜率差的绝对值小于0.3
		((l1.k > 0 && l2.k > 0) || (l1.k < 0 && l2.k < 0)) &&  // 斜率同号
		distanceBetweenLine(l1, l2) < th; // 距离较近
}


/**
 * 计算两个点的距离
 * 输入：两个点（Point类型的对象）
 * 输出：两个点之间的距离，double类型
 */
double pointDistance(Point p1, Point p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}


/**
 * 原方法：利用点到直线距离，估算两条直线的距离
 * 改进方法：直线间中点的距离
 * 输入：两条直线（Line对象）
 * 输出：直线之间的距离， double类型
 */
double distanceBetweenLine(Line l1, Line l2) {
	/*Point mid = l1.mid;
	double A = l2.k, B = -1, C = -(l2.k * l2.start.x - l2.start.y);
	return abs(A * mid.x + B * mid.y + C) / sqrt(A * A + B * B);*/
	return pointDistance(l1.mid, l2.mid);
}


/**
 * 判断两个点是否相近
 * x，y间距均小于阈值
 * 输入：两个点（Point对象），阈值double类型
 * 输出：bool类型，true表示两个点离得很近，false表示离得远
 */
bool isPointNear(Point p1, Point p2, double th){
	return (abs(p1.x - p2.x) < th && abs(p1.y - p2.y) < th);
}


/**
 * 判断直线首尾是否相接，并返回相连的类型
 * 输入：两条直线（Line对象），阈值 int
 * 输出：一个int型，表示的类型如下：
 *    0：不相连
 *    1：l1的end   和 l2的start   相连
 *    2：l1的end   和 l2的end     相连
 *    3：l1的start 和 l2的start   相连
 *    4：l1的start 和 l2的end     相连
*/
int isConnect(Line l1, Line l2, int th) {
	double len = max(l1.length, l2.length);
	int status = 0;
	if (isPointNear(l1.end, l2.start, th) && !isPointNear(l1.start, l2.end, len)) {
		status = 1;
	}else if (isPointNear(l1.end, l2.end, th) && !isPointNear(l1.start, l2.start, len)) {
		status = 2;
	}else if (isPointNear(l1.start, l2.start, th) && !isPointNear(l1.end, l2.end, len)) {
		status = 3;
	} else if (isPointNear(l1.start, l2.end, th) && !isPointNear(l1.end, l2.start, len)) {
		status = 4;
	}
	return status;
}


/**
 * 产生首尾相连的长直线
 * 输入：两条直线（Line对象），类型 int
 * 输出：一条连接后的直线（Line对象）
 */
Line createConnectLine(Line l1, Line l2, int type) {	
	Line l = Line();
	if (type == 1) {
		l.start = l1.start;
		l.end = l2.end;
	}else if (type == 2) {
		l.start = l1.start;
		l.end = l2.start;
	}else if (type == 3) {
		l.start = l1.end;
		l.end = l2.end;
	}else if (type == 4) {
		l.start = l1.end;
		l.end = l2.start;
	}
	l.length = l.getLength();
	l.mid = l.getMidPoint();	
	return l;
}


/**
 * 直线连接，如果两个直线首尾能相接，则连接成一条直线
 * 输入：包含Line对象的向量，阈值，矩阵Mat（作用就是测试的时候画图用）
 * 输出：包含Line对象的向量
 */
vector<Line> connectLines(vector<Line> lines, int th, Mat dst) {
	vector<Line> *result = new vector<Line>();
	size_t length = lines.size();

	for (int i = 0; i < length; i++) {
		Line line1 = lines[i];
		bool useless = true;
		for (int j = 0; j < length; j++) {
			Line line2 = lines[j];
			if (canCluster(line1, line2, th)) { // 如果具备聚合条件
				int type = isConnect(line1, line2, th); // 计算类型
                if (type != 0) {  // 如果是连接型
					useless = false;
					Line tmp = createConnectLine(line1, line2, type);
					(*result).push_back(tmp);
					//line(dst, tmp.start, tmp.end, Scalar(0, 0, 255), 3, CV_AA);
					break;
				}
			}
		}
		if (useless) {
			(*result).push_back(line1);
		}
	}
	vector<Line>().swap(lines);
	return *result;
}


/**
 * 直线聚合函数,聚合的原则：
 * 如果两个直线的起点和终点相似，则保留那条长直线
 * 由于直线数量不多，采用暴力求解的方法，时间复杂度O(n2)
 * 输入：包含Line对象的向量，阈值int，矩阵Mat（作用就是测试的时候画图用）
 * 输出：包含Line对象的向量
 */
vector<Line> clusterLines(vector<Line> lines, int th, Mat dst) {
	vector<Line> *result = new vector<Line>();
	size_t length = lines.size();
	__gnu_cxx::hash_set<int> set;
	__gnu_cxx::hash_set<int>::iterator pos;

	for (int i = 0; i < length; i++) {
		pos = set.find(i);
		if (pos != set.end()) { // 如果存在
			continue;
		}
		Line line1 = lines[i];
		bool useless = true;
		for (int j = i; j < length; j++) {
			pos = set.find(j);
			if (pos != set.end()) { // 如果存在
				continue;
			}
			Line line2 = lines[j];
			if (canCluster(line1, line2, th)) { // 如果具备聚合条件
				set.insert(i);
				set.insert(j);
				useless = false;
				if (line1.length >= line2.length) {
					(*result).push_back(line1);
					//line(dst, line1.start, line1.end, Scalar(0, 255, 0), 2, CV_AA);
				}else {
					(*result).push_back(line2);
					//line(dst, line2.start, line2.end, Scalar(0, 255, 0), 2, CV_AA);
				}
				break;
			}
		}
	    if (useless) {
			(*result).push_back(line1);
		}
	}
	vector<Line>().swap(lines);
	return *result;
}


/**
 * 按长度取得topK的直线（这个函数暂时用不到）
 * 输入：包含Line对象的向量，TopK的K
 * 输出：包含Line对象的向量，表示前K条直线
 */
vector<Line> getTopK(vector<Line> lines, int K) {

	// MinHeap heap(K);
	MinHeap* heap = new MinHeap(K);

	// 创建大顶堆
	(*heap).createMinHeap(lines);
	for (int i = K + 1; i< lines.size(); i++) {
		(*heap).insert(lines[i]);
	}
	// (*heap).print();
	lines = (*heap).getHeap();
	delete heap;
	return lines;
}


/**
 * 计算两条直线的差距，算法如下：
 *  - 计算长度差距
 *  - 计算斜率的差距
 * 返回：长度差距 * 斜率差距
 * 输入：两条直线（Line对象）
 * 输出：直线之间的差距，double类型
 */
double lineDiff(Line line1, Line line2) {
	return abs(line1.length - line2.length) * abs(line1.k - line2.k);
}


/**
 * 在图像中画出直线，仅测试时用
 * 输入：包含Line对象的向量，表示直线组；Mat矩阵，颜色Scalar类型，窗口名字
 * 输出：void
 */
void drawLine(vector<Line> lineSet, Mat image, Scalar color, string name) {
	size_t len = lineSet.size();
	for (int i = 0; i < len; i++) {
		Line l = lineSet[i];
		line(image, l.start, l.end, color, 1, CV_AA);
	}
	imshow(name, image);
}


/**
 * 计算两条直线夹角
 * 输入：斜率k1和k2，double类型
 * 输出：反正切角度，double类型
 */
double getAngle(double k1, double k2) {
	return atan(abs(k2 - k1) / (1 + k1 * k2));
}

