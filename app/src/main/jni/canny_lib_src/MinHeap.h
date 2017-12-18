#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "Line.h"

using namespace cv;
using namespace std;

/**
 * 小顶堆, 父节点的值小于孩子的值
 * 假设需要我们在一堆海量数据中找出排名前k的数据；
 * 最好的方法是用最小堆排序，直接用前k个数据建立一个小顶堆，然后遍历剩余的数，
 * ①如果此数 < 堆顶元素, 说明：比k个数中最小的数还要小，直接跳过此数，遍历下一个数；
 * ②如果此数 > 堆顶的数，则将此数和堆顶的数交换，然后从堆顶向下调整堆，使其重新满足小顶堆。
 */

class MinHeap
{
private:
	int maxsize; // 堆的大小
	void filterDown(int begin); // 向下调整堆
	vector<Line> arr;

public:
	MinHeap(int k);
	~MinHeap();

	void createMinHeap(vector<Line> lineSet); // 创建堆
	void insert(Line val); // 插入元素
	Line getTop();         // 获取堆顶元素
	//static double getLength(Line line); // 计算长度
	vector<Line> getHeap(); // 获取堆中的全部元素
};