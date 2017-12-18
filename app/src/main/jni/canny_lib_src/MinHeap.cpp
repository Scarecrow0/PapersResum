#include "MinHeap.h"
#include "Line.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>


using namespace std;
using namespace cv;


MinHeap::MinHeap(int k) {
	maxsize = k;
}

MinHeap::~MinHeap() {
	//minheap.clear();
	vector<Line>().swap(arr);
	//delete (*minheap); //释放空间
}


/**
 * 创建小顶堆
 */
void MinHeap::createMinHeap(vector<Line> lineSet) {
	for (int i = 0; i < maxsize; i++) {
		arr.push_back(lineSet[i]);
	}
}


/**
 * 插入元素
 */
void MinHeap::insert(Line line) {

	if (line.length > getTop().length) {
		arr[0] = line;
		filterDown(0);
	}
}


/**
 * 向下调整
 */
void MinHeap::filterDown(int current) {
	int end = arr.size() - 1;
	int child = current * 2 + 1; //当前节点的左孩子
	Line line = arr[current]; //保存当前节点

	while (child <= end) {
		// 选出两个孩子中的较小孩子
		if (child < end && arr[child + 1].length < arr[child].length)
			child++;
		if (line.length < arr[child].length) break;
		else {
			arr[current] = arr[child];//孩子节点覆盖当前节点
			current = child;
			child = child * 2 + 1;
		}
	}
	arr[current] = line;
}


/**
 * 获取堆顶元素
 */
Line MinHeap::getTop() {
	if (arr.size() != 0)
		return arr[0];
	Line line;
	return line;
}


/** 
 * 获取堆中的全部元素
 */
vector<Line> MinHeap::getHeap() {
	vector<Line> heap;
	for (int i = 0; i < arr.size(); i++)
		heap.push_back(arr[i]);
	return heap;
}
