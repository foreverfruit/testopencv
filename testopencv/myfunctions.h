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


/*1.���������Ի���*/
void section1_setup();
/*2.ͼ������룬�޸ģ�����*/
void section2_io();
/*3.ͼ����Ĥ������ͼ������ؼ�����*/
void section3_1();
/*opencv���ṩ����Ĥ����*/
void section3_2();
/*4.mat�����ʹ��*/
void section4_mat();
/*5.���ز���,��ͨ��*/
void section5_pixel_1c();
/*5.���ز���,��ͨ��*/
void section5_pixel_mc();
/*6.ͼ���ں�*/
void section6_blend();
/*7.ͼ��������Աȶ�*/
void section7();
/*8.����ͼ�κ�����*/
void section8();
/*9.ͼ��ģ��*/
void section9();
void section9_1();
/*10.��ʴ������*/
void section10();
/*��̬ѧ������*/
void section10_1();
/*��̬ѧӦ�ã�ֱ����ȡ*/
void section10_2();
/*��̬ѧӦ�ã���֤��ȥ������*/
void section10_3();
/*11.ͼ�������*/
void section11();
/*12.��ֵ��*/
void section12();
/*13.�Զ��������˲�*/
void section13();
/*14.�����Ե����*/
void section14();
/*15.Sobel����*/
void section15();
/*16.Laplace*/
void section16();
/*17.Canny�㷨*/
void canny();
/*18.Hough�任-ֱ�߼��*/
void hough();
/*19.Hough���Բ��*/
void hough_circle();

#endif
