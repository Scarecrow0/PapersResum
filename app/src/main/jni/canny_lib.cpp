//
// Created by root on 17-11-1.
// 这是写canny jni 部分代码的地方

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>
#include <jni.h>

//来自调用函数的jni类
#include "OpenCvCanny_OpenCVCannyLib.h"

//cpp部分各种函数的头文件
#include "canny_lib.h"
#include "canny_lib_src/Line.h"
#include "canny_lib_src/MinHeap.h"
#include "canny_lib_src/main_canny_lib.h"

using namespace cv;
using namespace std;

// 记得各种java的int之类的类型都得转成jint什么的
JNIEXPORT void JNICALL Java_OpenCvCanny_OpenCVCannyLib_cannyLauncher
        (JNIEnv *env, jclass obj, jintArray img1_param, jint w1, jint h1, jobject callback){

    CallbackInterface callbackInterface(callback,env);

    //回调类的获得
    //这里在Cpp部分对JNI回调的接口进行了下封装 方便使用

    jint *img1_jintarr;

    img1_jintarr = env->GetIntArrayElements(img1_param, NULL);



    Mat img(h1, w1, CV_8UC4, (unsigned char *) img1_jintarr); // 初始化一个矩阵（图像）4通道的图像
    Mat img_orgin(img);
    cvtColor(img, img, COLOR_BGR2GRAY); // 转为灰度图

    callbackInterface.setImageSize(img.cols, img.rows);
    EdgeDetected(img, callbackInterface);

//   提取直线
    Mat segmentResult_img = SegmentExtract(img);

//  画出结果
    callbackInterface.onSegmentExtracted(segmentResult_img);

//  垃圾回收
    env->ReleaseIntArrayElements(img1_param, img1_jintarr, 0);
}

void EdgeDetected(Mat img1,CallbackInterface& callbackInterface){
//todo 把各种canny 函数加进去

    GaussianBlur(img1, img1, Size(3, 3), 0, 0); // 高斯滤波
    Canny(img1, img1, 50, 150, 3); // 边缘检测
 /*
    blur(img1, img1, Size(3, 3));
    myCanny(img1, img1, 50, 200, 3);
    blur(img2, img2, Size(3, 3));
    myCanny(img2, img2, 50, 200, 3);
  */
    //通过传入的回调类 进行对app部分的回调
    callbackInterface.onEdgeDetected(img1);
}

Mat SegmentExtract(Mat& img){

    // Create and LSD detector with standard or no refinement.
    // LSD_REFINE_NONE，没有改良的方式；
    // LSD_REFINE_STD，标准改良方式，将带弧度的线（拱线）拆成多个可以逼近原线段的直线度；
    // LSD_REFINE_ADV，进一步改良，计算出错误警告数量，减少尺寸进一步精确直线。
    Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_NONE, 0.99);
    vector<Vec4f> lines_std;

    // Detect the lines
    ls->detect(img, lines_std);


    vector<Line> line_set = createLine(lines_std);
    int threshold = 15; // 阈值【5-10】
    Mat img_sgmt_res(img);
    Mat dst(img.rows, img.cols, CV_8UC3, Scalar(255,255,255));




   // for(int j = 0; j < 2;j++) {


        for (int i = 0; i < 10; i++) {
            // Step2 删除较短的直线 (可选)
            //   line_set = cleanShort(line_set);

            // Step3 先进行直线的连接，然后聚合直线
            line_set = connectLines(line_set, threshold + i*2, dst); // 连接
            line_set = clusterLines(line_set, threshold + i*3, dst); // 聚合

            //   line_set = clusterLines(line_set, threshold, dst); // 聚合
        }

        line_set = cleanShort(line_set);

//        line_set = clusterLines(line_set, threshold, dst); // 聚合
  //  }

//   画出提取出直线后的图
    for (int i = 0; i < line_set.size(); i++) {
       	int b = rand() % 255; //产生三个随机数
       	int g = rand() % 255;
       	int r = rand() % 255;
       	line(img_sgmt_res, line_set[i].start, line_set[i].end, Scalar(b, g, r), 2, CV_AA);
    }
    return img_sgmt_res;
}


CallbackInterface::CallbackInterface(jobject& callback,JNIEnv* env) {
    this -> env = env;
    this->callback_obj = callback;
    this->callback_class = env->GetObjectClass(callback);
    //回调类的获得
    this->methodID_methodonEdgeDetected =
            env->GetMethodID(callback_class, "onEdgeDetectComplete", "([III)V");
    this->methodID_onSegmentExtracted =
            env->GetMethodID(callback_class, "onLineSegmentExtractionComplete", "([III)V");

}

void CallbackInterface::onEdgeDetected(Mat& img1) {
    CallbackMethodSelect(img1, ON_EDGE_DETECTED);
}

void CallbackInterface::onSegmentExtracted(Mat& img1) {
    CallbackMethodSelect(img1, ON_SEGMENT_EXTRACTED);
}


void CallbackInterface::setImageSize(int w, int h) {
    this->w = w;
    this->h = h;
}

void CallbackInterface::CallbackMethodSelect(Mat &img1, int action, double rate) {
    jintArray img1_arr,img2_arr ;
    switch (action){
        case ON_EDGE_DETECTED:
            img1_arr = bitmapArrayFactoryGrayScale(env,img1,w,h);
            env->CallVoidMethod(this->callback_obj,this->methodID_methodonEdgeDetected,img1_arr,w,h);
            break;
        case ON_SEGMENT_EXTRACTED:
            img1_arr = bitmapArrayFactoryGrayScale(env,img1,w,h);
            env->CallVoidMethod(callback_obj,methodID_onSegmentExtracted,img1_arr,w,h);
            break;
        default:
            break;
    }
}


/*
    安卓的 ARGB 8888：
        一个32bit的unsigned
        4个8 分别对应 透明度 红色 绿色 蓝色
        其中需要注意的是透明度是反过来的  即值越大越不透明 FFH就是完全不透明
        对一个unsigned int 使用移位 相加等位操作即可构建需要的像素了

    在OpenCV里 mat通过 uchar* data = image.ptr<uchar>(i)获得的像素
    如果是个彩色的 实际上是将一个像素的RGB挨着放在一起 称为通道
    取出进行操作时 要三个三个取

*/

jintArray bitmapArrayFactoryGrayScale(JNIEnv *env, Mat image, int w,int h){
    int* outImage = new int[w * h];
    int n = 0;
    for(int i = 0; i < h; i++) {
        uchar* data = image.ptr<uchar>(i);
        for(int j = 0; j < w; j++) {
            if(data[j] > 0) {
                outImage[n++] =  0xFF000000;
            }else {
                outImage[n++] =  0xFFFFFFFF;
            }
        }
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, outImage);
    return result;
}

jintArray bitmapArrayFactoryColor(JNIEnv *env, cv::Mat image, int w,int h){
    int* outImage = new int[w * h];
    int n = 0;
    for(int i = 0; i < h; i++) {
        uchar* data = image.ptr<uchar>(i);
        for(int j = 0; j < w; j++) {
            int pix_pointer = j * 3;
            unsigned int alpha = 0xFF000000;
            unsigned int red = data[pix_pointer];
            red = red << 16;
            unsigned int green = data[pix_pointer + 1];
            green = green << 8;
            unsigned int blue = data[pix_pointer + 2];
            outImage[n++] = alpha + red + green + blue;

        }
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, outImage);
    return result;
}