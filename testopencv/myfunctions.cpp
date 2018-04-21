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
	int rows_end = src.rows - 1;
	int cols_end = (src.cols - 1)*src.channels();
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
	src1 = imread("D:/workspace/image/haze2.jpg", IMREAD_COLOR);
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
				row_dst_ptr[j] = static_cast<uchar>(row_src1_ptr[j] * 0.5 + row_src2_ptr[j] * 0.5);
			}
		}
		imshow("blend", dst);
	}
	else {
		// 大小类型不同，需要转换
	}
	waitKey(0);
}

/*7.图像亮度与对比度*/
void section7()
{
	Mat src, dst;
	src = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	imshow("src1", src);

	dst = Mat::zeros(src.size(), src.type());
	float a = 1.5;
	float b = 0;
	for (int i = 0; i < src.rows; i++) {
		// 这里用指针ptr操作，也可以直接用at函数操作
		uchar * row_src_ptr = src.ptr<uchar>(i);
		uchar * row_dst_ptr = dst.ptr<uchar>(i);
		for (int j = 0; j < src.cols*src.channels(); j++)
		{
			row_dst_ptr[j] = saturate_cast<uchar>(row_src_ptr[j] * a + b);
		}
	}
	imshow("processed", dst);

	waitKey(0);
}


/*8.绘制图像和文字*/
void section8()
{
	Mat src;
	src = imread("D:/workspace/image/haze1.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// 画线
	Point p1(30, 50);
	Point p2;
	p2.x = 300;
	p2.y = 400;
	Scalar col_red(0, 0, 255);
	line(src, p1, p2, col_red, 2, LINE_AA); // line_aa,反锯齿

	// rectangle
	Rect rect = Rect(p1, p2); // 常用两对角点，或者左上点+长宽构造
	Scalar col_blue(255, 0);
	rectangle(src, rect, col_blue, 3, LINE_8);

	// ellipse,采用这个RotateRect的3个point的构造需要注意三个顶点必须能构成矩形。
	// 这里没有用LINE_AA，明显出现锯齿
	ellipse(src, RotatedRect( Point(10,20), Point(10,300),Point(200,300)), Scalar(0, 255), 3,LINE_8);
	// 参数：图板，中心，长短轴，倾斜角度，开始角度，中止角度（用于画弧），颜色
	ellipse(src, Point(200,200),Size(200,150),45,0,180, Scalar(50, 200, 200), 2, LINE_AA);

	// circle
	circle(src, Point(300, 300), 80, col_red, 3, LINE_AA);
	
	// 多边形
	Point pts[1][5] = {Point(0,0),Point(200,120),Point(120,180),Point(180,60),Point(60,50)};
	const Point * ppts[] = { pts[0] }; // point指针的数组，显然这个数组元素只有1个
	const int ptCount = 5;
	fillPoly(src, ppts, &ptCount, 1 ,col_blue, LINE_8 ,0);
	// 多边形轮廓，这里文档的第二个参数有点怪怪……
	polylines(src, ppts,&ptCount,1, true, col_red, 2, LINE_AA);

	// 输出文本框
	putText(src, String("Hello World!"), Point(400, 100), CV_FONT_BLACK, 1.0, col_blue, 2, LINE_AA);

	// SHOW
	namedWindow("draw", CV_WINDOW_AUTOSIZE);
	imshow("draw", src);
	waitKey(0);

	// 随机画线效果
	RNG randn = RNG(12345);
	Mat m = Mat::zeros(src.size(), src.type());
	Point p3, p4;
	namedWindow("randomLines", CV_WINDOW_AUTOSIZE);
	while (1) {
		p3.x = randn.uniform(0, m.cols);
		p4.x = randn.uniform(0, m.cols);
		p3.y = randn.uniform(0, m.rows);
		p4.y = randn.uniform(0, m.rows);
		Scalar color(randn.uniform(0, 255), randn.uniform(0, 255), randn.uniform(0,255));
		line(m, p3, p4, color, randn.uniform(0, 3), LINE_AA);
		imshow("randomLines", m);
		if (waitKey(50) > 0) {
			break;
		}
	}
}

/*9.图像模糊*/
void section9()
{
	Mat src,dst;
	src = imread("D:/workspace/image/lena.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// blur，矩形窗口模糊
	blur(src, dst, Size(15, 15), Point(-1, -1));
	imshow("output-15*15", dst);
	// x轴模糊
	blur(src, dst, Size(15, 1), Point(-1, -1));
	imshow("output-15*1", dst);
	// y轴模糊
	blur(src, dst, Size(1, 15), Point(-1, -1));
	imshow("output-1*15", dst);

	// 高斯模糊
	GaussianBlur(src, dst, Size(15, 15), 1);
	imshow("gaussianBulr", dst);
	
	waitKey(0);
}

/*中值滤波、高斯双边滤波*/
void section9_1()
{
	Mat src, dst;
	src = imread("D:/workspace/image/lena_noise.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// 中值滤波，对椒盐噪声效果好
	medianBlur(src, dst, 3);
	imshow("output-median", dst);
	waitKey(0);

	// 双边滤波和高斯滤波的对比
	Mat src2, dst2;
	src2 = imread("D:/workspace/image/woman.jpg", IMREAD_COLOR);
	imshow("input", src2);
	// 高斯模糊
	GaussianBlur(src2, dst2, Size(15, 15), 1);
	imshow("output-gaussian", dst2);
	// 双边滤波，保留边缘，其他部分平滑
	bilateralFilter(src2, dst2, 15, 100, 5);
	imshow("bilateral", dst2);
	// 双边滤波之后配合对比度增强
	Mat kernel = (Mat_<int>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	filter2D(dst2, dst2, -1, kernel, Point(-1, -1));
	imshow("bilateral-with-edgEnhance", dst2);

	waitKey(0);
}

/*10.腐蚀与膨胀*/
void section10()
{
	// 局部函数声明：TrackBarCallBack函数
	void myCallBack(int ,void*);
	Mat src;
	src = imread("D:/workspace/image/lena.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	int element_size = 0;
	int max_ele_size = 21;
	// 最后两个参数都是默认为0的指针类型
	// 回调函数的指针，函数原型规定参数必须为(int,void*),第一个为trackBar的当前值，第二个为用户数据地址。注：函数名就是函数的地址
	// 用户数据的指针，void*类型，避免全局变量的使用，用这个参数传递到回调的TrackBarCallBack函数中
	createTrackbar("Element Size:", "output", &element_size, max_ele_size, myCallBack , &src);
	// 调用一次该函数（用作普通函数调用），让图片显示出来，之后操作显示中的TrackBar会循环再触发该回调函数
	myCallBack(element_size, &src);
	// 这个waitKey放到callBack之外，否则会出问题
	waitKey(0);
}
void myCallBack(int element_size, void* src_) {
	int e_size = 2 * element_size + 1;
	Mat dst;
	Mat * src = static_cast<Mat*>(src_);
	// 形态学操作，需要先创建结构元素,参数看函数说明
	Mat struct_elmt = getStructuringElement(MorphShapes::MORPH_RECT, Size(e_size, e_size), Point(-1, -1));
	// 腐蚀
	//erode(*src, dst, struct_elmt,Point(-1,-1));
	// 膨胀
	dilate(*src, dst, struct_elmt, Point(-1, -1));
	// show
	imshow("output", dst);
}

/*10.1图像形态学操作：开闭操作*/
void section10_1() {
	Mat src,dst;
	src = imread("D:/workspace/image/morphology.png", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	int s = 15;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(s, s));
	// 开
	morphologyEx(src, dst, CV_MOP_OPEN, kernel);
	imshow("output", dst);
	waitKey(0);

	// 闭
	morphologyEx(src, dst, CV_MOP_CLOSE, kernel);
	imshow("output", dst);
	waitKey(0);

	// 形态学梯度
	morphologyEx(src, dst, CV_MOP_GRADIENT, kernel);
	imshow("output", dst);
	waitKey(0);

	// 顶帽
	morphologyEx(src, dst, CV_MOP_TOPHAT, kernel);
	imshow("output", dst);
	waitKey(0);

	// 黑帽
	morphologyEx(src, dst, CV_MOP_BLACKHAT, kernel);
	imshow("output", dst);
	waitKey(0);
}


/*10.2.形态学应用——提取直线*/
void section10_2()
{
	Mat src;
	src = imread("D:/workspace/image/lines.png", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	// 原图太大，这里为了观察方便，降采样
	pyrDown(src, src, Size(src.cols/2,src.rows/2));
	imshow("input", src);

	// 灰度图
	Mat gray;
	cvtColor(src,gray,CV_BGR2GRAY);
	imshow("gray", gray);

	// 二值图
	Mat binary;
	// 这里用adaptiveThreshould方法总是提取到轮廓，而不是填充的二值图，最终采用threshold方法
	//adaptiveThreshold(gray, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -1);
	// 由于背景是白色，这里手动设置阈值为250
	threshold(gray, binary, 250, 255, THRESH_OTSU);
	imshow("binary", binary);

	// 构造形态学操作结构元素，提取横线，构造一个横线结构元素
	Mat dst;
	// 这个和shape和size确定了结构元素的样子，这里size的长度（结构元素横向长度）会影响到字母中的横线是否处理干净
	// 这里我的size.width取得很大，否则会残留字母A中得横线部分
	Mat kernel = getStructuringElement(MORPH_RECT, Size(51, 1));
	//dilate(binary, dst, kernel);
	//erode(dst, dst, kernel);
	// 事实上，以上两个操作就是闭操作
	morphologyEx(binary, dst, CV_MOP_CLOSE, kernel);
	// 为了结果更好，再平滑一下
	blur(dst, dst, Size(3, 3));
	imshow("result", dst);

	waitKey(0);
}

/*10.3.形态学应用：验证码去干扰线*/
void section10_3()
{
	Mat src;
	src = imread("D:/workspace/image/checkcode.png", IMREAD_COLOR);
	pyrDown(src, src, Size(src.cols/2, src.rows/2));
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// 灰度图
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	imshow("gray", gray);

	// 二值图
	Mat binary;
	// 由于背景是白色，这里手动设置阈值为250
	threshold(gray, binary, 250, 255, THRESH_OTSU);
	imshow("binary", binary);

	// 构造形态学操作结构元素
	Mat dst;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(binary, dst, CV_MOP_CLOSE, kernel);
	// 为了结果更好，再平滑一下
	blur(dst, dst, Size(3, 3));
	imshow("result", dst);

	waitKey(0);
}

/*11.图像金字塔*/
void section11()
{
	Mat src;
	src = imread("D:/workspace/image/lena.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// 上采样
	Mat upImg;
	// 注意，这里的Size参数是(宽,高)，对应的就是(cols,rows)，而不是一般矩阵计算的（rows，cols）
	pyrUp(src, upImg, Size(src.cols * 2, src.rows * 2));
	imshow("上采样", upImg);

	// 降采样
	Mat downImg;
	// 默认就是宽高+1/2，应对奇偶问题
	pyrDown(src, downImg, Size( (src.cols+1) / 2, (src.rows+1) / 2));
	imshow("降采样", downImg);

	// 高斯不同，彩色
	Mat d1, d2, dst;
	GaussianBlur(src, d1, Size(5, 5), 0, 0);
	GaussianBlur(d1, d2, Size(5, 5), 0, 0);
	// 两图相减
	subtract(d1, d2, dst, Mat());
	normalize(dst, dst, 255, 0, NORM_MINMAX);
	imshow("DOG-Color", dst);

	// 高斯不同，灰度
	Mat gray,d3,d4,dst2;
	cvtColor(src, gray, CV_BGR2GRAY);
	// if both sigmas are zeros, they are computed from ksize.width and ksize.height,根据模板大小自动计算sigma
	GaussianBlur(gray, d3, Size(5, 5), 0, 0);
	GaussianBlur(d3, d4, Size(5, 5), 0, 0);
	// 两图相减
	subtract(d3, d4, dst2, Mat());
	// 通常高斯不同（差值）很小，需要进行归一化，下列公式以最大最小值归一化公式，实际进行的是强度的线性拉伸操作
	normalize(dst2, dst2, 255, 0, NormTypes::NORM_MINMAX);
	imshow("DOG-Gray", dst2);

	waitKey(0);
}

//--------------------------------------------
// 构建一个用于callback传递数据的结构体
struct UserData {
	Mat * gray;
	int * thresh_value;
	int *thresh_max;
	int *op_value;
	int *op_max;
	// C++ 结构体可以有函数，如初始化函数
	UserData(Mat *gray, int * thresh_value, int *thresh_max, int *op_value, int *op_max)
	{
		this->gray = gray;
		this->thresh_value = thresh_value;
		this->thresh_max = thresh_max;
		this->op_value = op_value;
		this->op_max = op_max;
	}
};
/*12.阈值化*/
void section12()
{
	Mat src,gray;
	src = imread("D:/workspace/image/woman.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	cvtColor(src, gray, CV_BGR2GRAY);

	int thresh_value = 127; // 自定义阈值初始值
	int thresh_max = 255; // max threshold
	int op_value = 0; // 对应THRESH_BINARY，二值化
	int op_max = 4;
	char title[] = "threshold window"; // title基本就是当常量用，就不传参了
	UserData userdata(&gray, &thresh_value, &thresh_max, &op_value, &op_max);

	// 声明两个Trackbar的回调函数，分别控制阈值及操作类型
	void CallBack_Thresh(int, void*);
	void CallBack_Operate(int, void*);

	// 设置TrackBar
	namedWindow(title, CV_WINDOW_AUTOSIZE);
	createTrackbar("threshold_value:", title, &thresh_value, thresh_max, CallBack_Thresh, static_cast<void*>(&userdata));
	createTrackbar("threshold_op:", title, &op_value, op_max, CallBack_Operate, static_cast<void*>(&userdata));

	// 主动调用一下，让图显示出来，以触发后续的回调
	CallBack_Thresh(thresh_value, (void*)(&userdata));

	waitKey(0);
}
// section12，阈值化中Trackbar，用于threshold调节
void CallBack_Thresh(int threshold_value, void * data) {
	// 传进来来的数据类型还原
	UserData * userdata = (UserData*)(data);
	// 每次在trackbar中调整了value，回去调整主调处的value。
	// 这一步我猜不用做，因为createTrackbar中value参数是int*类型，估计内部做了这个操作了。
	*(userdata->thresh_value) = threshold_value; 

	Mat dst;
	threshold( *(userdata->gray), dst, *(userdata->thresh_value), *(userdata->thresh_max), *(userdata->op_value));
	imshow("threshold window", dst);
}
// threshold operate的trackbar回调函数
void CallBack_Operate(int op_value, void * data) {
	UserData * userdata = (UserData*)(data);
	Mat dst;
	threshold(*(userdata->gray), dst, *(userdata->thresh_value), *(userdata->thresh_max), *(userdata->op_value));
	imshow("threshold window", dst);
}
//----------------------------------------------------------

/*13.自定义线性滤波*/
void section13()
{
	Mat src, gray;
	src = imread("D:/workspace/image/lena.jpg", IMREAD_COLOR);
	pyrDown(src, src);
	cvtColor(src, src, CV_BGR2GRAY);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);


	// Robert算子
	Mat robx, roby;
	// robert x和y 方向算子
	Mat k_rob_x = (Mat_<int>(2, 2) << 1, 0, 0, -1);
	Mat k_rob_y = (Mat_<int>(2, 2) << 0, 1, -1, 0);
	filter2D(src, robx, -1, k_rob_x);
	filter2D(src, roby, -1, k_rob_y);
	imshow("robert x", robx);
	imshow("robert y", roby);
	imshow("robert x+y", robx + roby);


/*
	// Sobel算子
	Mat sobx, soby;
	Mat k_sob_x = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat k_sob_y = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	filter2D(src, sobx, -1, k_sob_x);
	filter2D(src, soby, -1, k_sob_y);
	imshow("sobel x", sobx);
	imshow("sobel y", soby);
	imshow("sobel x+y", sobx + soby);

	// Laplace算子
	Mat lap;
	Mat k_lap = (Mat_<int>(3, 3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
	filter2D(src, lap, -1, k_lap);
	imshow("laplace", lap);
*/

	waitKey(0);
}


/*14.卷积边缘处理*/
void section14() {
	Mat src,dst;
	src = imread("D:/workspace/image/lena.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	char c;
	int bordertype = BORDER_DEFAULT;
	int bordersize = static_cast<int>(src.cols*0.1);
	RNG rng(12345);
	while (1) {
		c = waitKey(500);
		if (c == 27){ // ESC pressed to quit
			break;
		}
		if (c == 'c') {
			bordertype = BORDER_CONSTANT;
		}
		else if (c == 'r') {
			bordertype = BORDER_REFLECT;
		}
		else if (c == 'd') {
			bordertype = BORDER_DEFAULT;
		}
		else if (c == 'a') {
			bordertype = BORDER_WRAP;
		}
		// 为图像添加边缘
		Scalar color(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255)); // color 仅在 BORDER_CONSTANT下作用
		copyMakeBorder(src, dst,bordersize, bordersize, bordersize, bordersize,bordertype,color);
		imshow("border demo", dst);
	}
}


/*15.Sobel算子*/
void section15() {
	Mat src, dst;
	src = imread("D:/workspace/image/woman.jpg", IMREAD_COLOR);
	pyrDown(src, src);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// blur
	GaussianBlur(src, src, Size(3, 3), 0, 0);
	// gray
	cvtColor(src, src, CV_BGR2GRAY);
	imshow("blured_gray", src);

	// 1.若不convertScaleAbs，显示图像处问题，因为sobel操作之后会有负值。
	// 2.Sobel函数得第三个类型为输出数据类型，一定要范围大于等于输入图像。但若等于输入图像(-1),可能发生数据截断
	// 3.Sobel函数得第4、5个参数为卷积模板选择，x还是y方向，不能同时选中（1），结果很差，不知道内部源码如何处理得.
	Mat sobelx, sobely,sobelxy;
	Sobel(src, sobelx, CV_16S, 1, 0);
	Sobel(src, sobely, CV_16S, 0, 1);
	convertScaleAbs(sobelx, sobelx);
	convertScaleAbs(sobely, sobely);
	imshow("x", sobelx);
	imshow("y", sobely);

	// 存在溢出
	sobelxy = sobelx + sobely;
	imshow("xy+", sobelxy);
	
	addWeighted(sobelx, 0.5, sobely, 0.5, 0, sobelxy);
	imshow("xy_add", sobelxy);

	waitKey(0);
}


/*16.Laplace*/
void section16() {
	Mat src,gray,lap;
	src = imread("D:/workspace/image/lena.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	// 显示原图
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// gray
	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(3, 3),0);

	Laplacian(gray, lap, CV_16S, 3);
	convertScaleAbs(lap, lap);
	//threshold(lap, lap, 0, 255, THRESH_OTSU | THRESH_BINARY);
	imshow("Laplace", lap);

	waitKey(0);
}

/*17.Canny算法*/
void canny()
{
	Mat src, gray, can;
	src = imread("d:/workspace/ComputerVision/Assignment2/image/building.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	// 显示原图
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(3, 3), 0);

	// Canny中的双阈值，这里设置低阈值为ThreshLow通过TrackBar调节。高阈值通过阈值比为2：1求得（通常为2：1或3：1）
	int threshLow = 30; // default = 30
	Mat * userdata[] = { &can, &gray }; // 用来传递callback中使用到的用户数据
	namedWindow("Canny", CV_WINDOW_AUTOSIZE);
	void CallBack_Canny(int, void*);	// 函数声明
	createTrackbar("ThreshLow", "Canny", &threshLow, 255, CallBack_Canny, userdata);
	// 普通调用函数显示图像
	CallBack_Canny(threshLow, userdata);
	
	waitKey(0);
}
// canny trackbar callback.（注：这是一个普通函数，可用户调用。同时又是一个回调函数，可通过trackbar自动触发）
void CallBack_Canny(int threshLow, void* userdataArr) {
	// 这里语法问题，调试了很久。关键：userdataArr是mat*的二维数组。简单说就是[]和*算是一对等价的记号。数组[]还原的时候不知道长度，所以用*，这样就变成了Mat**
	Mat ** userdata = (Mat**)userdataArr;
	Mat * can = *(userdata + 0); // 二维数组第一行（还是一个数组），实际只有一个元素
	Mat * gray = *(userdata + 1);// 同理，第二行，内部也只有一个元素，所以可以对它直接取值操作就是gray
	// 后两个参数分别是
	// Sobel的windowsize，取默认的3，窗口为3模板进行梯度计算
	// 最后一个参数，取默认的false，意为求梯度时用近似计算（范数为1的公式），而true为标准的2范数的公式
	Canny(*gray, *can, threshLow, threshLow * 2, 3, false);
	// 显然这里*gray可以用gray[0]，*can可以用can[0]
	imshow("Canny", *can);
}

/*18.Hough变换-直线检测*/
void hough() {
	Mat src, gray, can, hou;
	src = imread("D:/workspace/image/hough_test.png", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}

	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(3, 3), 0);
	int threshLow = 70;
	Canny(gray, can, threshLow, threshLow * 2, 3, false);

	Mat result = Mat::zeros(src.size(), src.type());
	vector<Vec4i> lines;
	HoughLinesP(can, lines, 1, CV_PI / 180, 80, 30, 8);
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(result, Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 1, 8);
	}

	imshow("gray", gray);
	imshow("can", can);
	imshow("result", result);

	waitKey(0);
}

/*19.Hough检测圆弧*/
void hough_circle() {
	Mat src, gray, hou;
	src = imread("D:/workspace/image/hough_circle_test.png", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}

	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(3, 3), 0);

	Mat result = src.clone();
	// find circle
	vector<Vec3f> circles;
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 10, 100, 30, 5, 50);
	for (int i = 0; i < circles.size(); i++) {
		Vec3f cir =  circles[i];
		circle(result, Point(cir[0], cir[1]), cir[2], RED, 2, LINE_AA);
		circle(result, Point(cir[0], cir[1]), 2, RED, 1);
	}

	imshow("input", src);
	imshow("gray", gray);
	imshow("result", result);

	waitKey(0);
}
