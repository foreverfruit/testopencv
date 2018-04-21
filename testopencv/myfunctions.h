#pragma once

#ifndef MYFUNCTIONS__
#define MYFUNCTIONS__

#include <opencv2/opencv.hpp>
#include <opencv.hpp>  
#include <imgproc.hpp>

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

#define RED Scalar(0,0,255) 
#define GREEN Scalar(0,255,0)
#define BLUE Scalar(255,0,0)


/*1.概述，测试环境*/
void section1_setup();
/*2.图像的输入，修改，保存*/
void section2_io();
/*3.图像掩膜操作，图像的像素级操作*/
void section3_1();
/*opencv库提供的掩膜操作*/
void section3_2();
/*4.mat对象的使用*/
void section4_mat();
/*5.像素操作,单通道*/
void section5_pixel_1c();
/*5.像素操作,多通道*/
void section5_pixel_mc();
/*6.图像融合*/
void section6_blend();
/*7.图像亮度与对比度*/
void section7();
/*8.绘制图形和文字*/
void section8();
/*9.图像模糊*/
void section9();
void section9_1();
/*10.腐蚀与膨胀*/
void section10();
/*形态学：开闭*/
void section10_1();
/*形态学应用：直线提取*/
void section10_2();
/*形态学应用：验证码去干扰线*/
void section10_3();
/*11.图像金字塔*/
void section11();
/*12.阈值化*/
void section12();
/*13.自定义线性滤波*/
void section13();
/*14.卷积边缘处理*/
void section14();
/*15.Sobel算子*/
void section15();
/*16.Laplace*/
void section16();
/*17.Canny算法*/
void canny();
/*18.Hough变换-直线检测*/
void hough();
/*19.Hough检测圆弧*/
void hough_circle();

#endif
