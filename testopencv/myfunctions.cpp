#include "myfunctions.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/*1.概述，测试环境*/
void section1_setup() {
	Mat src = imread("D:/dehaze_image/haze0.jpg");
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	namedWindow("test opencv setup", CV_WINDOW_AUTOSIZE);
	imshow("test opencv setup", src);

	waitKey(0);
}


/*2.图像的输入，修改，保存*/
void section2_io() {
	/*
	图像输入，第一个参数为图像路径，第二个参数为图像类型IMREAD_XXX
	默认情况下，以IMREAD_COLOR加载，表示RGB彩色图像，可以通过该参数制定加载为什么类型，如灰度图等
	imread可以加载jpg、png、tif等常见图片格式
	*/
	Mat src = imread("D:/dehaze_image/lena.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// 原图
	imshow("original", src);

	Mat output;
	/*
	cvtColor,convert color,图片颜色类型转换函数，用于不同颜色类型的转换
	三个参数分别为，input，output，和转换参数CV_XXX2XXX
	*/
	cvtColor(src, output, CV_BGR2GRAY);

	// 转换之后的图
	imshow("gray_img", output);

	/*
	保存转换之后的图片
	参数分别为，保存路径，输出图像的array，参数vector
	不指定参数时，会根据保存的后缀名自动适配保存类型
	*/
	imwrite("D:/dehaze_image/lena_gray.jpg", output);
	// pause
	waitKey(0);
}

/*
3.图像掩膜操作，图像的像素级操作
掩膜，即mask操作，模板操作。
此处采用的模板（算子）如下
|0 -1 0 |
|-1 5 -1|
|0 -1 0 |
该算子作用是:提高对比度，中间点的权值高，4邻域为负权值
*/
void section3_1() {
	Mat src = imread("D:/dehaze_image/haze0.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("could't load image..\n");
		return;
	}

	// 显示原图
	namedWindow("original img", CV_WINDOW_AUTOSIZE);
	imshow("original img", src);

	// 做掩膜操作
	// 1.得出行列数：
	// 注意1：这里图像矩阵mat的排列时，每一行的各像素的各通道值是依次排列的
	// 如RGB图像的矩阵某一行是这样的内容:(i0_b,i0_g,i0_r,  i1_b,i1_g,i1_r,  i2_b,i2_g,i2_r)
	// 注意2：模板操作的时候，需要处理边界，此处采用的处理方案是，边界不进行模板操作，故col和row需要减去边界
	// 注意3：cols和rows是从0开始计算的，这个和数组是一样的
	int cols_end = (src.cols - 1)*src.channels();
	int rows_end = src.rows - 1;
	int col_offset = src.channels();

	// 根据输入图像的大小和类型，初始化一个输出图像的矩阵
	Mat out = Mat::zeros(src.size(), src.type());

	int64 t1 = getTickCount();
	for (int row = 1; row < rows_end; row++) {
		// 获取需要处理的三个行向量
		const uchar* currentRow = src.ptr<uchar>(row);
		const uchar* preRow = src.ptr<uchar>(row - 1);
		const uchar* nextRow = src.ptr<uchar>(row + 1);
		uchar * outrow = out.ptr<uchar>(row);
		for (int col = col_offset; col < cols_end; col++) {
			// 掩膜操作，就是卷积操作，对模板的乘加操作
			outrow[col] = saturate_cast<uchar>(5 * currentRow[col] - preRow[col] - nextRow[col] - currentRow[col - col_offset] - currentRow[col + col_offset]);
			// 此处需要判断outrow[col]的值越界吗？超过255，显然是需要的，opencv提供了该函数,将值定在区间[0,255]内
			// outrow[col] = saturate_cast<uchar>(outrow[col]); 
			// 上行代码是错误的，必须在原计算值上saturate处理
			// 原理：outrow的类型根据src.type，故存储值应该是255之内的。若计算值超出，会溢出，
			// 默认的处理溢出的策略是取低8位，这就导致了值异常，而正常的处理是超出255的取255，小于0的取0.
			// 故，outrow[col] = saturate_cast<uchar>(outrow[col])根本是无效的，outrow已经采取过计算机默认的溢出处理
		}
	}
	int64 t2 = getTickCount();
	printf("section3_1 time consume:%.2f\n", (t2 - t1) / getTickFrequency());

	// 输出mask之后图像
	namedWindow("masked image", CV_WINDOW_AUTOSIZE);
	imshow("masked image", out);

	waitKey(0);
}

/*opencv库提供的掩膜操作*/
void section3_2() {
	Mat src = imread("D:/dehaze_image/haze0.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("could't load image..\n");
		return;
	}

	// 显示原图
	namedWindow("original img2", CV_WINDOW_AUTOSIZE);
	imshow("original img2", src);

	// 输出目的
	Mat dst = Mat::zeros(src.size(), src.type());
	// 模板算子
	Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);

	int64 t1 = getTickCount();
	// 库提供的模板卷积操作，第三个参数为深度，在不知道的时候可用-1，表示与输入图像相同的深度
	filter2D(src, dst, src.depth(), kernel);
	int64 t2 = getTickCount();
	printf("section3_2 time consume:%.2f\n", (t2 - t1) / getTickFrequency());

	// 输出mask之后图像
	namedWindow("masked image2", CV_WINDOW_AUTOSIZE);
	imshow("masked image2", dst);

	waitKey(0);
}

/*Mat的使用*/
void section4_mat()
{
	Mat src = imread("D:/dehaze_image/haze1.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// Mat dst = Mat(src.size(), src.type()); // 创建一个空的Mat对象
	// dst = Scalar(255, 0, 0); // 赋初值，scalar三个参数分别为BGR

	// Mat dst = src.clone();   // Clone方法完全复制一个Mat对象
	
	Mat dst;
	// src.copyTo(dst);
	cvtColor(src, dst, CV_BGR2GRAY);

	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("output", dst);

	// 采用构造： Mat(rols,cols,type,&scaler)
	Mat m(3, 3, CV_8UC3, Scalar(0, 255, 0));
	cout << "m = " << endl;
	cout << m << endl;

	// 另一种，create方式
	Mat mat;
	mat.create(src.size(), src.type());
	mat = Scalar(0, 255, 0);
	imshow("mat", mat);
	
	// 定义模板、kernel,小的二维数组
	Mat k = (Mat_<int>(3, 3) << 0, 0, 0, 1, 1, 1, 0, 0, 0);
	cout << k << endl;

	// 单位矩阵eye，零矩阵Mat::zeros(rows,cols,type)
	Mat e = Mat::eye(3, 3, CV_8UC1);
	cout << e << endl;


	cout << "src channel:" << src.channels() << ",size:" << src.size() << endl;
	cout << "dst channel:" << dst.channels() << ",size:" << dst.size() << endl;
	waitKey(0);
}

/*5.像素操作，单通道*/
void section5_pixel_1c()
{
	Mat src, gray_src;
	src = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// mat.at<type>(row_index,col_index)方式访问单通道像素
	// 由于是单通道，所以需要先转换为灰度图
	cvtColor(src, gray_src, CV_BGR2GRAY);
	// 输出原灰度图
	imshow("original gray", gray_src);
	for(int i = 0; i < gray_src.rows; i++) {
		for (int j = 0; j < gray_src.cols; j++) {
			uchar value = gray_src.at<uchar>(i, j);
			// mat.at函数可以原地赋值操作
			gray_src.at<uchar>(i, j) = 255 - value; // 反色操作
		}
	}
	// 反色之后的灰度图
	// opencv默认在没有namedwindow时候，自动创建显示窗口，当然它会先去寻找同名的窗口（参数一）
	imshow("processed gray", gray_src);

	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	waitKey(0);
}

/*5.像素操作,多通道*/
void section5_pixel_mc()
{
	Mat src, dst;
	src = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	dst.create(src.size(), src.type());
	dst = Scalar(0, 0, 255);

	// 输出原图
	imshow("original", src);

	// mat.at<数组类型>(row_index,col_index)方式访问多通道像素
	int channels = src.channels();
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			// typedef Vec<uchar, 3> Vec3b; 
			// 注意左边的类型应该是引用类型&，才能原地操作
			// 这里代码并不能通用各种通道，因为Vec3b指定了通道数为3
			Vec3b & values = src.at<Vec3b>(i, j);
			Vec3b & dst_values = dst.at<Vec3b>(i, j);
			// 操作每个通道
			for (int c = 0; c < channels; c++) {
				// 反色操作,同样可以原地操作,看mat.at返回的&引用，故可以原地操作
				dst_values[c] = 255 - values[c];
			}
		}
	}
	imshow("processed", dst);

	// opencv内置的同样效果的函数,bitwise_not,内部通过位操作实现
	Mat cvdst;
	bitwise_not(src, cvdst);
	imshow("cv processed", cvdst);

	waitKey(0);
}

/*6.图像融合*/
void section6_blend()
{
	Mat src1, src2, dst;
	src1 = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	src2 = imread("D:/dehaze_image/hazefree2.jpg", IMREAD_COLOR);
	imshow("src1", src1);
	imshow("src2", src2);

	// opencv内置 权重混合函数
	if (src1.size == src2.size && src1.type() == src2.type()) {
		//addWeighted(src1, 0.5, src2, 0.5, 1, dst);
		//imshow("blend", dst);
	}

	// 像素实现,采用指针访问
	if (src1.size == src2.size && src1.type() == src2.type()) {
		dst = Mat::zeros(src1.size(), src1.type());
		for (int i = 0; i < src1.rows; i++) {
			uchar * row_src1_ptr = src1.ptr<uchar>(i);
			uchar * row_src2_ptr = src2.ptr<uchar>(i);
			uchar * row_dst_ptr = dst.ptr<uchar>(i);
			for (int j = 0; j < src1.cols*src1.channels(); j++)
			{
				row_dst_ptr[j] = row_src1_ptr[j] * 0.5 + row_src2_ptr[j] * 0.5;
			}
		}
		imshow("blend", dst);
	}
	else {
		// 大小类型不同，需要转换
	}
	waitKey(0);
}
