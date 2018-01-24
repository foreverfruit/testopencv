#pragma once

#ifndef MYFUNCTIONS__
#define MYFUNCTIONS__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

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
#endif