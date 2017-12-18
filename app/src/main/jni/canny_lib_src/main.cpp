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


int main() {

	string dir = "images/"; // 图片的目录

	

	// 测试1，肉眼相似图片间的相似度
	cout << "第一组测试" << endl;
	string arr[] = { 
		"image0.jpg", "image0.jpg",
		"image0.jpg", "image1.jpg",
		"image2.jpg", "image3.jpg",
		"image4.jpg", "image5.jpg",
		"image6.jpg", "image7.jpg",
		"image8.jpg", "image9.jpg",
		"image10.jpg", "image11.jpg",
		"image12.jpg", "image13.jpg",
		"image14.jpg", "image15.jpg",
		"image16.jpg", "image17.jpg",
		"image18.jpg", "image19.jpg",
		"image20.jpg", "image21.jpg",
		"image22.jpg", "image23.jpg",
	}; // 25个测试用例

    // 两两进行测试
	for (int i = 0; i <= 24; i += 2) {
		string path1 = dir + arr[i];
		string path2 = dir + arr[i+1];
		double rate = solution(path1, path2);
	}
	cout << endl;


	// 测试2，肉眼不相似图片间的相似度
	cout << "第二组测试" << endl;
	string arr1[] = {
		"image0.jpg", "image2.jpg",
		"image1.jpg", "image4.jpg",
		"image0.jpg", "image4.jpg",
		"image1.jpg", "image10.jpg",
		"image0.jpg", "image14.jpg",
		"image14.jpg", "image18.jpg",
		"image4.jpg", "image20.jpg",
		"image2.jpg", "image22.jpg",
	}; // 8个测试用例

	for (int i = 0; i <= 7; i += 2) {
		string path1 = dir + arr1[i];
		string path2 = dir + arr1[i + 1];
		double rate = solution(path1, path2);
	}
	cout << endl;


	// 测试3，完全随机测试
	cout << "第三组测试" << endl;
	for (int i = 0; i <= 23; i++) {
		string path1 = dir + arr[i];
		int j = rand() % 23;
		string path2 = dir + arr[j];
		double rate = solution(path1, path2);
	}
	cout << endl;


	//string path1 = dir + "image0.jpg";
	//string path2 = dir + "image1.jpg";
	//double rate = solution(path1, path2);

	
	waitKey(0);
	system("pause"); // 暂停
	return 0;
}


/**
 * 程序的入口
 * 输入：两个图片的路径string类型
 * 输出：匹配度double类型
 */
double solution(string path1, string path2) {

	clock_t start, finish;// 程序计时
	double totaltime;
	start = clock();


	// Step1 提取直线
	Mat image1 = imread(path1, IMREAD_GRAYSCALE);
	Mat image2 = imread(path2, IMREAD_GRAYSCALE);

	// 检查是否有图片
	if (image1.empty()) {
		cout << "Cannot read image file: " <<  path1 << endl;
		return -1;
	}
	if (image2.empty()) {
		cout << "Cannot read image file: " << path2 << endl;
		return -1;
	}

	// 图像大小变化
	double height = (double)image1.rows / image1.cols * image2.cols;
	resize(image1, image1, Size(image2.cols, height), 0, 0, CV_INTER_LINEAR);

	vector<Vec4f> lines_std1 = operation(path1, image1);
	vector<Vec4f> lines_std2 = operation(path2, image2);


	// Step2 搞出轮廓，完全不知道咋整，艹
	// 直线的聚类，延长，求出垂直、水平棱角线

	double rate = match(lines_std1, lines_std2, image1, image2);

	vector<Vec4f>().swap(lines_std1);
	vector<Vec4f>().swap(lines_std2);

	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << path1 << "与" << path2 << "的相似度是" << rate << " 用时" << totaltime << "秒" << endl;
	return rate;
}


/**
 * 图像滤波、边缘检测、直线检测操作
 * 输入：图片路径（用于命名）和图片
 * 输出：返回检测后的直线
 */
vector<Vec4f> operation(string path, Mat image) {
	blur(image, image, Size(3, 3));  // 使用3x3内核来降噪
	// Canny(image, image, 50, 200, 3); // Apply canny edge
	
	myCanny(image, image, 50, 200, 3);
	// imwrite(path + "(50,200)边缘检测结果图.jpg", image);
	// imshow(path+"边缘检测", image);

	// Create and LSD detector with standard or no refinement.
	// LSD_REFINE_NONE，没有改良的方式；
	// LSD_REFINE_STD，标准改良方式，将带弧度的线（拱线）拆成多个可以逼近原线段的直线度；
	// LSD_REFINE_ADV，进一步改良，计算出错误警告数量，减少尺寸进一步精确直线。
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD, 0.99);
	vector<Vec4f> lines_std;

	// Detect the lines
	ls->detect(image, lines_std);


	// Show found lines
	//Mat drawnLines(image);
	//Mat only_lines(image.size(), image.type());
	//ls->drawSegments(drawnLines, lines_std);
	//ls->drawSegments(only_lines, lines_std);
	// imshow(path, drawnLines);
    // imshow(path, only_lines);
	// imwrite(path + "直线提取.jpg", only_lines);

	return lines_std;
}


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
 * 两个直线组中的直线，进行两两配对，或者说匹配
 * 输入：两组直线，包含Line对象的向量，阈值 int
 * 输出：一个n*2的二维向量，n表示匹配的对儿数
 */
vector<vector<Line> > makePair(vector<Line> lineSet1, vector<Line>lineSet2, int th) {
	__gnu_cxx::hash_set<int> set;
	__gnu_cxx::hash_set<int>::iterator pos;
	vector<vector<Line> > pairSet;
	size_t length1 = lineSet1.size();
	size_t length2 = lineSet2.size();

	for (int i = 0; i < length1; i++) {
		Line line1 = lineSet1[i];
		int bestFriendId = -1; // 最佳配对直线的id
		double minDiff = MAX;  // 两条直线最小的差距
		for (int j = 0; j < length2; j++) {
			pos = set.find(j);
			if (pos != set.end()) { // 如果存在
				continue;
			}
			Line line2 = lineSet2[j];
			// 如果直线1,2的斜率相近，长度相近，位置相近，则配对
			// 这里偷个懒，直接用canCluster()函数判断，如果能聚合，也就能配对
			if (canCluster(line1, line2, th * 2)) {
				double diff = lineDiff(line1, line2);
				if (diff < minDiff) {
					minDiff = diff;
					bestFriendId = j;
				}
			}
		}
		// 找到最佳配对的直线后，存储到二维向量中
		if (bestFriendId != -1) {
			set.insert(bestFriendId);
			vector<Line> pair;
			Line bestFriendLine = lineSet2[bestFriendId];
			pair.push_back(line1);
			pair.push_back(bestFriendLine);
			pairSet.push_back(pair);
		}
	}
	vector<Line>().swap(lineSet1); // 回收内存
	vector<Line>().swap(lineSet2);
	return pairSet;
}


/**
 * 计算两组直线的匹配度
 * 输入：两个图像的两组直线 lines1，lines2
 * 输出：匹配度，double类型
 *
 * 算法步骤如下：
 * 1. 计算每组直线的斜率，计算斜率阈值TK、距离阈值TP
 * 2. 根据斜率、距离的差值是否满足阈值，找到最佳匹配直线对
 * 3. 计算每组中的直线与本组中的其他直线之间的夹角
 * 4. 计算夹角矩阵之间的相似度，并把这个相似度，作为直线的匹配度，返回
 * TODO: 通过直线构造三种直线组合，利用直线组合还原高级特征，通过高级特征图匹配
 */
double match(vector<Vec4f> lines1, vector<Vec4f> lines2, InputArray m1, InputArray m2) {

	// Step1 创建直线
	vector<Line> lineSet1 = createLine(lines1);
	vector<Line> lineSet2 = createLine(lines2);
	
	vector<Vec4f>().swap(lines1); // 回收内存
	vector<Vec4f>().swap(lines2);

	int threshold = 8; // 阈值【5-10】
	Mat dst1(m1.getMat().rows, m1.getMat().cols, CV_8UC3, Scalar(255,255,255));
	Mat dst2(m1.getMat().rows, m1.getMat().cols, CV_8UC3, Scalar(255,255,255));

	// Step2 删除较短的直线 (可选)
	lineSet1 = cleanShort(lineSet1);
	lineSet2 = cleanShort(lineSet2);

	// Step3 先进行直线的连接，然后聚合直线
    lineSet1 = connectLines(lineSet1, threshold, dst1); // 连接
	lineSet2 = connectLines(lineSet2, threshold, dst2);

	lineSet1 = clusterLines(lineSet1, threshold, dst1); // 聚合
	lineSet2 = clusterLines(lineSet2, threshold, dst2);
	
	size_t length1 = lineSet1.size();
	size_t length2 = lineSet2.size();

	// 画出聚合后的图像，便于分析
	// line(dst1, Point(0, 0), Point(10, 0), Scalar(255, 0, 0), 3, CV_AA); // 测试阈值
	//drawLine(lineSet1, dst1, Scalar(0,0,0), "连接、聚合后的图像1");
	//drawLine(lineSet2, dst2, Scalar(0,0,0), "连接、聚合后的图像2");


	// Step4. 从第一张图中选择一条直线，然后遍历第二张图，找到最佳的配对直线
	vector<vector<Line> > pairSet = makePair(lineSet1, lineSet2, threshold);
	size_t pairLen = pairSet.size(); // 有多少对直线
	
	// 画出配对后的图像，便于分析
	//Mat dst3(m1.getMat().rows, m1.getMat().cols, CV_8UC3, Scalar(255,255,255));
	//Mat dst4(m2.getMat().rows, m2.getMat().cols, CV_8UC3, Scalar(255,255,255));	
	//for (int i = 0; i < pairLen; i++) {
	//	int b = rand() % 255; //产生三个随机数
	//	int g = rand() % 255;
	//	int r = rand() % 255;
	//	vector<Line> v = pairSet[i];
	//	line(dst3, v[0].start, v[0].end, Scalar(b, g, r), 2, CV_AA);
	//	line(dst4, v[1].start, v[1].end, Scalar(b, g, r), 2, CV_AA);
	//}
	//imshow("配对后的图像1", dst3);
	//imshow("配对后的图像2", dst4);


	// Step 5. 计算直线与其他直线的夹角，构造夹角矩阵
	vector<vector<double> > angleList1, angleList2;
	for (int i = 0; i < pairLen; i++) {
		vector<Line> v1 = pairSet[i];
		vector<double> angle1, angle2;

		for (int j = i + 1; j < pairLen; j++) {
			vector<Line> v2 = pairSet[j];
			 angle1.push_back(getAngle(v1[0].k, v2[0].k));
			 angle2.push_back(getAngle(v1[1].k, v2[1].k));
		}
		angleList1.push_back(angle1);
		angleList2.push_back(angle2);

		vector<double>().swap(angle1); // 回收内存
		vector<double>().swap(angle2);
	}

	// 然后计算夹角矩阵的相似度
	double rate = calculateCorr2(angleList1, angleList2);

	if (length1 != length2) {
		rate *= (double) min(length1, length2) / max(length1, length2); 
	}
	return rate;
}


/**
 * 计算两条直线夹角
 * 输入：斜率k1和k2，double类型
 * 输出：反正切角度，double类型
 */
double getAngle(double k1, double k2) {
	return atan(abs(k2 - k1) / (1 + k1 * k2));
}


/**
 * 计算两个矩阵的相似度
 * matlab中的corr2()函数，好麻烦
 * 输入：二维向量
 * 输出：二维向量的均值
 */
double calculateMean(vector<vector<double> > m) {
	double sum = 0.0;
	int num = 0;
	size_t rows = m.size();
	for (int i = 0; i < rows; i++) {
		size_t cols = m[i].size();
		for (int j = 0; j < cols; j++) {
			sum += m[i][j];
			num++;
		}
	}
	return sum / num;
}


/**
 * corr2函数，计算两个矩阵的相关系数
 * 计算结果被归一化在[0,1]区间内，数值越大说明相似度越高
 * 输入：两个二维向量
 * 输出：它们的相关系数，double类型
 */
double calculateCorr2(vector<vector<double> > m1, vector<vector<double> > m2) {

	double mean1 = calculateMean(m1);
	double mean2 = calculateMean(m2);

	// 增加一条判断，如果平均值相似，则认为是两张完全相同的图像
	if (abs(mean1 - mean2) <= 1e-6 ) { return 1.0; }

	//计算分子
	double numerator = 0;
	size_t len = m1.size();
	for (size_t i = 0; i < len; i++) {
		size_t len1 = m1[i].size();
		for (size_t j = 0; j < len1; j++) {
			numerator += (m1[i][j] - mean1) * (m2[i][j] - mean2);
		}
		for (size_t j = len1; j <= len1; j++) {
			numerator += mean1 * mean2;
		}
	}


	//计算分母 sqrt(pow(x,2) + pow(y,2));
	double d1 = 0;
	double d2 = 0;
	for (size_t i = 0; i < len; i++) {
		size_t len1 = m1[i].size();
		for (size_t j = 0; j < len1; j++) {
			d1 += pow((m1[i][j] - mean1), 2);
			d2 += pow((m2[i][j] - mean2), 2);
		}
		for (size_t j = len1; j <= len; j++) {
			d1 += pow(mean1, 2);
			d2 += pow(mean2, 2);
		}
	}
	double denominator = sqrt(d1) * sqrt(d2);

	if (numerator == 0) return 0.0;
	return numerator / denominator;

}

