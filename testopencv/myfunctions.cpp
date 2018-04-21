#include "myfunctions.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/*1.���������Ի���*/
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


/*2.ͼ������룬�޸ģ�����*/
void section2_io() {
	/*
	ͼ�����룬��һ������Ϊͼ��·�����ڶ�������Ϊͼ������IMREAD_XXX
	Ĭ������£���IMREAD_COLOR���أ���ʾRGB��ɫͼ�񣬿���ͨ���ò����ƶ�����Ϊʲô���ͣ���Ҷ�ͼ��
	imread���Լ���jpg��png��tif�ȳ���ͼƬ��ʽ
	*/
	Mat src = imread("D:/dehaze_image/lena.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// ԭͼ
	imshow("original", src);

	Mat output;
	/*
	cvtColor,convert color,ͼƬ��ɫ����ת�����������ڲ�ͬ��ɫ���͵�ת��
	���������ֱ�Ϊ��input��output����ת������CV_XXX2XXX
	*/
	cvtColor(src, output, CV_BGR2GRAY);

	// ת��֮���ͼ
	imshow("gray_img", output);

	/*
	����ת��֮���ͼƬ
	�����ֱ�Ϊ������·�������ͼ���array������vector
	��ָ������ʱ������ݱ���ĺ�׺���Զ����䱣������
	*/
	imwrite("D:/dehaze_image/lena_gray.jpg", output);
	// pause
	waitKey(0);
}

/*
3.ͼ����Ĥ������ͼ������ؼ�����
��Ĥ����mask������ģ�������
�˴����õ�ģ�壨���ӣ�����
|0 -1 0 |
|-1 5 -1|
|0 -1 0 |
������������:��߶Աȶȣ��м���Ȩֵ�ߣ�4����Ϊ��Ȩֵ
*/
void section3_1() {
	Mat src = imread("D:/dehaze_image/haze0.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("could't load image..\n");
		return;
	}

	// ��ʾԭͼ
	namedWindow("original img", CV_WINDOW_AUTOSIZE);
	imshow("original img", src);

	// ����Ĥ����
	// 1.�ó���������
	// ע��1������ͼ�����mat������ʱ��ÿһ�еĸ����صĸ�ͨ��ֵ���������е�
	// ��RGBͼ��ľ���ĳһ��������������:(i0_b,i0_g,i0_r,  i1_b,i1_g,i1_r,  i2_b,i2_g,i2_r)
	// ע��2��ģ�������ʱ����Ҫ����߽磬�˴����õĴ������ǣ��߽粻����ģ���������col��row��Ҫ��ȥ�߽�
	// ע��3��cols��rows�Ǵ�0��ʼ����ģ������������һ����
	int rows_end = src.rows - 1;
	int cols_end = (src.cols - 1)*src.channels();
	int col_offset = src.channels();

	// ��������ͼ��Ĵ�С�����ͣ���ʼ��һ�����ͼ��ľ���
	Mat out = Mat::zeros(src.size(), src.type());

	int64 t1 = getTickCount();
	for (int row = 1; row < rows_end; row++) {
		// ��ȡ��Ҫ���������������
		const uchar* currentRow = src.ptr<uchar>(row);
		const uchar* preRow = src.ptr<uchar>(row - 1);
		const uchar* nextRow = src.ptr<uchar>(row + 1);
		uchar * outrow = out.ptr<uchar>(row);
		for (int col = col_offset; col < cols_end; col++) {
			// ��Ĥ���������Ǿ����������ģ��ĳ˼Ӳ���
			outrow[col] = saturate_cast<uchar>(5 * currentRow[col] - preRow[col] - nextRow[col] - currentRow[col - col_offset] - currentRow[col + col_offset]);
			// �˴���Ҫ�ж�outrow[col]��ֵԽ���𣿳���255����Ȼ����Ҫ�ģ�opencv�ṩ�˸ú���,��ֵ��������[0,255]��
			// outrow[col] = saturate_cast<uchar>(outrow[col]); 
			// ���д����Ǵ���ģ�������ԭ����ֵ��saturate����
			// ԭ��outrow�����͸���src.type���ʴ洢ֵӦ����255֮�ڵġ�������ֵ�������������
			// Ĭ�ϵĴ�������Ĳ�����ȡ��8λ����͵�����ֵ�쳣���������Ĵ����ǳ���255��ȡ255��С��0��ȡ0.
			// �ʣ�outrow[col] = saturate_cast<uchar>(outrow[col])��������Ч�ģ�outrow�Ѿ���ȡ�������Ĭ�ϵ��������
		}
	}
	int64 t2 = getTickCount();
	printf("section3_1 time consume:%.2f\n", (t2 - t1) / getTickFrequency());

	// ���mask֮��ͼ��
	namedWindow("masked image", CV_WINDOW_AUTOSIZE);
	imshow("masked image", out);

	waitKey(0);
}

/*opencv���ṩ����Ĥ����*/
void section3_2() {
	Mat src = imread("D:/dehaze_image/haze0.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("could't load image..\n");
		return;
	}

	// ��ʾԭͼ
	namedWindow("original img2", CV_WINDOW_AUTOSIZE);
	imshow("original img2", src);

	// ���Ŀ��
	Mat dst = Mat::zeros(src.size(), src.type());
	// ģ������
	Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);

	int64 t1 = getTickCount();
	// ���ṩ��ģ��������������������Ϊ��ȣ��ڲ�֪����ʱ�����-1����ʾ������ͼ����ͬ�����
	filter2D(src, dst, src.depth(), kernel);
	int64 t2 = getTickCount();
	printf("section3_2 time consume:%.2f\n", (t2 - t1) / getTickFrequency());

	// ���mask֮��ͼ��
	namedWindow("masked image2", CV_WINDOW_AUTOSIZE);
	imshow("masked image2", dst);

	waitKey(0);
}

/*Mat��ʹ��*/
void section4_mat()
{
	Mat src = imread("D:/dehaze_image/haze1.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// Mat dst = Mat(src.size(), src.type()); // ����һ���յ�Mat����
	// dst = Scalar(255, 0, 0); // ����ֵ��scalar���������ֱ�ΪBGR

	// Mat dst = src.clone();   // Clone������ȫ����һ��Mat����
	
	Mat dst;
	// src.copyTo(dst);
	cvtColor(src, dst, CV_BGR2GRAY);

	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("output", dst);

	// ���ù��죺 Mat(rols,cols,type,&scaler)
	Mat m(3, 3, CV_8UC3, Scalar(0, 255, 0));
	cout << "m = " << endl;
	cout << m << endl;

	// ��һ�֣�create��ʽ
	Mat mat;
	mat.create(src.size(), src.type());
	mat = Scalar(0, 255, 0);
	imshow("mat", mat);
	
	// ����ģ�塢kernel,С�Ķ�ά����
	Mat k = (Mat_<int>(3, 3) << 0, 0, 0, 1, 1, 1, 0, 0, 0);
	cout << k << endl;

	// ��λ����eye�������Mat::zeros(rows,cols,type)
	Mat e = Mat::eye(3, 3, CV_8UC1);
	cout << e << endl;


	cout << "src channel:" << src.channels() << ",size:" << src.size() << endl;
	cout << "dst channel:" << dst.channels() << ",size:" << dst.size() << endl;
	waitKey(0);
}

/*5.���ز�������ͨ��*/
void section5_pixel_1c()
{
	Mat src, gray_src;
	src = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// mat.at<type>(row_index,col_index)��ʽ���ʵ�ͨ������
	// �����ǵ�ͨ����������Ҫ��ת��Ϊ�Ҷ�ͼ
	cvtColor(src, gray_src, CV_BGR2GRAY);
	// ���ԭ�Ҷ�ͼ
	imshow("original gray", gray_src);
	for(int i = 0; i < gray_src.rows; i++) {
		for (int j = 0; j < gray_src.cols; j++) {
			uchar value = gray_src.at<uchar>(i, j);
			// mat.at��������ԭ�ظ�ֵ����
			gray_src.at<uchar>(i, j) = 255 - value; // ��ɫ����
		}
	}
	// ��ɫ֮��ĻҶ�ͼ
	// opencvĬ����û��namedwindowʱ���Զ�������ʾ���ڣ���Ȼ������ȥѰ��ͬ���Ĵ��ڣ�����һ��
	imshow("processed gray", gray_src);

	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	waitKey(0);
}

/*5.���ز���,��ͨ��*/
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

	// ���ԭͼ
	imshow("original", src);

	// mat.at<��������>(row_index,col_index)��ʽ���ʶ�ͨ������
	int channels = src.channels();
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			// typedef Vec<uchar, 3> Vec3b; 
			// ע����ߵ�����Ӧ������������&������ԭ�ز���
			// ������벢����ͨ�ø���ͨ������ΪVec3bָ����ͨ����Ϊ3
			Vec3b & values = src.at<Vec3b>(i, j);
			Vec3b & dst_values = dst.at<Vec3b>(i, j);
			// ����ÿ��ͨ��
			for (int c = 0; c < channels; c++) {
				// ��ɫ����,ͬ������ԭ�ز���,��mat.at���ص�&���ã��ʿ���ԭ�ز���
				dst_values[c] = 255 - values[c];
			}
		}
	}
	imshow("processed", dst);

	// opencv���õ�ͬ��Ч���ĺ���,bitwise_not,�ڲ�ͨ��λ����ʵ��
	Mat cvdst;
	bitwise_not(src, cvdst);
	imshow("cv processed", cvdst);

	waitKey(0);
}

/*6.ͼ���ں�*/
void section6_blend()
{
	Mat src1, src2, dst;
	src1 = imread("D:/workspace/image/haze2.jpg", IMREAD_COLOR);
	src2 = imread("D:/dehaze_image/hazefree2.jpg", IMREAD_COLOR);
	imshow("src1", src1);
	imshow("src2", src2);

	// opencv���� Ȩ�ػ�Ϻ���
	if (src1.size == src2.size && src1.type() == src2.type()) {
		//addWeighted(src1, 0.5, src2, 0.5, 1, dst);
		//imshow("blend", dst);
	}

	// ����ʵ��,����ָ�����
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
		// ��С���Ͳ�ͬ����Ҫת��
	}
	waitKey(0);
}

/*7.ͼ��������Աȶ�*/
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
		// ������ָ��ptr������Ҳ����ֱ����at��������
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


/*8.����ͼ�������*/
void section8()
{
	Mat src;
	src = imread("D:/workspace/image/haze1.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// ����
	Point p1(30, 50);
	Point p2;
	p2.x = 300;
	p2.y = 400;
	Scalar col_red(0, 0, 255);
	line(src, p1, p2, col_red, 2, LINE_AA); // line_aa,�����

	// rectangle
	Rect rect = Rect(p1, p2); // �������Խǵ㣬�������ϵ�+������
	Scalar col_blue(255, 0);
	rectangle(src, rect, col_blue, 3, LINE_8);

	// ellipse,�������RotateRect��3��point�Ĺ�����Ҫע��������������ܹ��ɾ��Ρ�
	// ����û����LINE_AA�����Գ��־��
	ellipse(src, RotatedRect( Point(10,20), Point(10,300),Point(200,300)), Scalar(0, 255), 3,LINE_8);
	// ������ͼ�壬���ģ������ᣬ��б�Ƕȣ���ʼ�Ƕȣ���ֹ�Ƕȣ����ڻ���������ɫ
	ellipse(src, Point(200,200),Size(200,150),45,0,180, Scalar(50, 200, 200), 2, LINE_AA);

	// circle
	circle(src, Point(300, 300), 80, col_red, 3, LINE_AA);
	
	// �����
	Point pts[1][5] = {Point(0,0),Point(200,120),Point(120,180),Point(180,60),Point(60,50)};
	const Point * ppts[] = { pts[0] }; // pointָ������飬��Ȼ�������Ԫ��ֻ��1��
	const int ptCount = 5;
	fillPoly(src, ppts, &ptCount, 1 ,col_blue, LINE_8 ,0);
	// ����������������ĵ��ĵڶ��������е�ֹ֡���
	polylines(src, ppts,&ptCount,1, true, col_red, 2, LINE_AA);

	// ����ı���
	putText(src, String("Hello World!"), Point(400, 100), CV_FONT_BLACK, 1.0, col_blue, 2, LINE_AA);

	// SHOW
	namedWindow("draw", CV_WINDOW_AUTOSIZE);
	imshow("draw", src);
	waitKey(0);

	// �������Ч��
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

/*9.ͼ��ģ��*/
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

	// blur�����δ���ģ��
	blur(src, dst, Size(15, 15), Point(-1, -1));
	imshow("output-15*15", dst);
	// x��ģ��
	blur(src, dst, Size(15, 1), Point(-1, -1));
	imshow("output-15*1", dst);
	// y��ģ��
	blur(src, dst, Size(1, 15), Point(-1, -1));
	imshow("output-1*15", dst);

	// ��˹ģ��
	GaussianBlur(src, dst, Size(15, 15), 1);
	imshow("gaussianBulr", dst);
	
	waitKey(0);
}

/*��ֵ�˲�����˹˫���˲�*/
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

	// ��ֵ�˲����Խ�������Ч����
	medianBlur(src, dst, 3);
	imshow("output-median", dst);
	waitKey(0);

	// ˫���˲��͸�˹�˲��ĶԱ�
	Mat src2, dst2;
	src2 = imread("D:/workspace/image/woman.jpg", IMREAD_COLOR);
	imshow("input", src2);
	// ��˹ģ��
	GaussianBlur(src2, dst2, Size(15, 15), 1);
	imshow("output-gaussian", dst2);
	// ˫���˲���������Ե����������ƽ��
	bilateralFilter(src2, dst2, 15, 100, 5);
	imshow("bilateral", dst2);
	// ˫���˲�֮����϶Աȶ���ǿ
	Mat kernel = (Mat_<int>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	filter2D(dst2, dst2, -1, kernel, Point(-1, -1));
	imshow("bilateral-with-edgEnhance", dst2);

	waitKey(0);
}

/*10.��ʴ������*/
void section10()
{
	// �ֲ�����������TrackBarCallBack����
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
	// ���������������Ĭ��Ϊ0��ָ������
	// �ص�������ָ�룬����ԭ�͹涨��������Ϊ(int,void*),��һ��ΪtrackBar�ĵ�ǰֵ���ڶ���Ϊ�û����ݵ�ַ��ע�����������Ǻ����ĵ�ַ
	// �û����ݵ�ָ�룬void*���ͣ�����ȫ�ֱ�����ʹ�ã�������������ݵ��ص���TrackBarCallBack������
	createTrackbar("Element Size:", "output", &element_size, max_ele_size, myCallBack , &src);
	// ����һ�θú�����������ͨ�������ã�����ͼƬ��ʾ������֮�������ʾ�е�TrackBar��ѭ���ٴ����ûص�����
	myCallBack(element_size, &src);
	// ���waitKey�ŵ�callBack֮�⣬����������
	waitKey(0);
}
void myCallBack(int element_size, void* src_) {
	int e_size = 2 * element_size + 1;
	Mat dst;
	Mat * src = static_cast<Mat*>(src_);
	// ��̬ѧ��������Ҫ�ȴ����ṹԪ��,����������˵��
	Mat struct_elmt = getStructuringElement(MorphShapes::MORPH_RECT, Size(e_size, e_size), Point(-1, -1));
	// ��ʴ
	//erode(*src, dst, struct_elmt,Point(-1,-1));
	// ����
	dilate(*src, dst, struct_elmt, Point(-1, -1));
	// show
	imshow("output", dst);
}

/*10.1ͼ����̬ѧ���������ղ���*/
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
	// ��
	morphologyEx(src, dst, CV_MOP_OPEN, kernel);
	imshow("output", dst);
	waitKey(0);

	// ��
	morphologyEx(src, dst, CV_MOP_CLOSE, kernel);
	imshow("output", dst);
	waitKey(0);

	// ��̬ѧ�ݶ�
	morphologyEx(src, dst, CV_MOP_GRADIENT, kernel);
	imshow("output", dst);
	waitKey(0);

	// ��ñ
	morphologyEx(src, dst, CV_MOP_TOPHAT, kernel);
	imshow("output", dst);
	waitKey(0);

	// ��ñ
	morphologyEx(src, dst, CV_MOP_BLACKHAT, kernel);
	imshow("output", dst);
	waitKey(0);
}


/*10.2.��̬ѧӦ�á�����ȡֱ��*/
void section10_2()
{
	Mat src;
	src = imread("D:/workspace/image/lines.png", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	// ԭͼ̫������Ϊ�˹۲췽�㣬������
	pyrDown(src, src, Size(src.cols/2,src.rows/2));
	imshow("input", src);

	// �Ҷ�ͼ
	Mat gray;
	cvtColor(src,gray,CV_BGR2GRAY);
	imshow("gray", gray);

	// ��ֵͼ
	Mat binary;
	// ������adaptiveThreshould����������ȡ�����������������Ķ�ֵͼ�����ղ���threshold����
	//adaptiveThreshold(gray, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -1);
	// ���ڱ����ǰ�ɫ�������ֶ�������ֵΪ250
	threshold(gray, binary, 250, 255, THRESH_OTSU);
	imshow("binary", binary);

	// ������̬ѧ�����ṹԪ�أ���ȡ���ߣ�����һ�����߽ṹԪ��
	Mat dst;
	// �����shape��sizeȷ���˽ṹԪ�ص����ӣ�����size�ĳ��ȣ��ṹԪ�غ��򳤶ȣ���Ӱ�쵽��ĸ�еĺ����Ƿ���ɾ�
	// �����ҵ�size.widthȡ�úܴ󣬷���������ĸA�еú��߲���
	Mat kernel = getStructuringElement(MORPH_RECT, Size(51, 1));
	//dilate(binary, dst, kernel);
	//erode(dst, dst, kernel);
	// ��ʵ�ϣ����������������Ǳղ���
	morphologyEx(binary, dst, CV_MOP_CLOSE, kernel);
	// Ϊ�˽�����ã���ƽ��һ��
	blur(dst, dst, Size(3, 3));
	imshow("result", dst);

	waitKey(0);
}

/*10.3.��̬ѧӦ�ã���֤��ȥ������*/
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

	// �Ҷ�ͼ
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	imshow("gray", gray);

	// ��ֵͼ
	Mat binary;
	// ���ڱ����ǰ�ɫ�������ֶ�������ֵΪ250
	threshold(gray, binary, 250, 255, THRESH_OTSU);
	imshow("binary", binary);

	// ������̬ѧ�����ṹԪ��
	Mat dst;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(binary, dst, CV_MOP_CLOSE, kernel);
	// Ϊ�˽�����ã���ƽ��һ��
	blur(dst, dst, Size(3, 3));
	imshow("result", dst);

	waitKey(0);
}

/*11.ͼ�������*/
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

	// �ϲ���
	Mat upImg;
	// ע�⣬�����Size������(��,��)����Ӧ�ľ���(cols,rows)��������һ��������ģ�rows��cols��
	pyrUp(src, upImg, Size(src.cols * 2, src.rows * 2));
	imshow("�ϲ���", upImg);

	// ������
	Mat downImg;
	// Ĭ�Ͼ��ǿ��+1/2��Ӧ����ż����
	pyrDown(src, downImg, Size( (src.cols+1) / 2, (src.rows+1) / 2));
	imshow("������", downImg);

	// ��˹��ͬ����ɫ
	Mat d1, d2, dst;
	GaussianBlur(src, d1, Size(5, 5), 0, 0);
	GaussianBlur(d1, d2, Size(5, 5), 0, 0);
	// ��ͼ���
	subtract(d1, d2, dst, Mat());
	normalize(dst, dst, 255, 0, NORM_MINMAX);
	imshow("DOG-Color", dst);

	// ��˹��ͬ���Ҷ�
	Mat gray,d3,d4,dst2;
	cvtColor(src, gray, CV_BGR2GRAY);
	// if both sigmas are zeros, they are computed from ksize.width and ksize.height,����ģ���С�Զ�����sigma
	GaussianBlur(gray, d3, Size(5, 5), 0, 0);
	GaussianBlur(d3, d4, Size(5, 5), 0, 0);
	// ��ͼ���
	subtract(d3, d4, dst2, Mat());
	// ͨ����˹��ͬ����ֵ����С����Ҫ���й�һ�������й�ʽ�������Сֵ��һ����ʽ��ʵ�ʽ��е���ǿ�ȵ������������
	normalize(dst2, dst2, 255, 0, NormTypes::NORM_MINMAX);
	imshow("DOG-Gray", dst2);

	waitKey(0);
}

//--------------------------------------------
// ����һ������callback�������ݵĽṹ��
struct UserData {
	Mat * gray;
	int * thresh_value;
	int *thresh_max;
	int *op_value;
	int *op_max;
	// C++ �ṹ������к��������ʼ������
	UserData(Mat *gray, int * thresh_value, int *thresh_max, int *op_value, int *op_max)
	{
		this->gray = gray;
		this->thresh_value = thresh_value;
		this->thresh_max = thresh_max;
		this->op_value = op_value;
		this->op_max = op_max;
	}
};
/*12.��ֵ��*/
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

	int thresh_value = 127; // �Զ�����ֵ��ʼֵ
	int thresh_max = 255; // max threshold
	int op_value = 0; // ��ӦTHRESH_BINARY����ֵ��
	int op_max = 4;
	char title[] = "threshold window"; // title�������ǵ������ã��Ͳ�������
	UserData userdata(&gray, &thresh_value, &thresh_max, &op_value, &op_max);

	// ��������Trackbar�Ļص��������ֱ������ֵ����������
	void CallBack_Thresh(int, void*);
	void CallBack_Operate(int, void*);

	// ����TrackBar
	namedWindow(title, CV_WINDOW_AUTOSIZE);
	createTrackbar("threshold_value:", title, &thresh_value, thresh_max, CallBack_Thresh, static_cast<void*>(&userdata));
	createTrackbar("threshold_op:", title, &op_value, op_max, CallBack_Operate, static_cast<void*>(&userdata));

	// ��������һ�£���ͼ��ʾ�������Դ��������Ļص�
	CallBack_Thresh(thresh_value, (void*)(&userdata));

	waitKey(0);
}
// section12����ֵ����Trackbar������threshold����
void CallBack_Thresh(int threshold_value, void * data) {
	// �����������������ͻ�ԭ
	UserData * userdata = (UserData*)(data);
	// ÿ����trackbar�е�����value����ȥ������������value��
	// ��һ���Ҳ²���������ΪcreateTrackbar��value������int*���ͣ������ڲ�������������ˡ�
	*(userdata->thresh_value) = threshold_value; 

	Mat dst;
	threshold( *(userdata->gray), dst, *(userdata->thresh_value), *(userdata->thresh_max), *(userdata->op_value));
	imshow("threshold window", dst);
}
// threshold operate��trackbar�ص�����
void CallBack_Operate(int op_value, void * data) {
	UserData * userdata = (UserData*)(data);
	Mat dst;
	threshold(*(userdata->gray), dst, *(userdata->thresh_value), *(userdata->thresh_max), *(userdata->op_value));
	imshow("threshold window", dst);
}
//----------------------------------------------------------

/*13.�Զ��������˲�*/
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


	// Robert����
	Mat robx, roby;
	// robert x��y ��������
	Mat k_rob_x = (Mat_<int>(2, 2) << 1, 0, 0, -1);
	Mat k_rob_y = (Mat_<int>(2, 2) << 0, 1, -1, 0);
	filter2D(src, robx, -1, k_rob_x);
	filter2D(src, roby, -1, k_rob_y);
	imshow("robert x", robx);
	imshow("robert y", roby);
	imshow("robert x+y", robx + roby);


/*
	// Sobel����
	Mat sobx, soby;
	Mat k_sob_x = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat k_sob_y = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	filter2D(src, sobx, -1, k_sob_x);
	filter2D(src, soby, -1, k_sob_y);
	imshow("sobel x", sobx);
	imshow("sobel y", soby);
	imshow("sobel x+y", sobx + soby);

	// Laplace����
	Mat lap;
	Mat k_lap = (Mat_<int>(3, 3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
	filter2D(src, lap, -1, k_lap);
	imshow("laplace", lap);
*/

	waitKey(0);
}


/*14.�����Ե����*/
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
		// Ϊͼ����ӱ�Ե
		Scalar color(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255)); // color ���� BORDER_CONSTANT������
		copyMakeBorder(src, dst,bordersize, bordersize, bordersize, bordersize,bordertype,color);
		imshow("border demo", dst);
	}
}


/*15.Sobel����*/
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

	// 1.����convertScaleAbs����ʾͼ�����⣬��Ϊsobel����֮����и�ֵ��
	// 2.Sobel�����õ���������Ϊ����������ͣ�һ��Ҫ��Χ���ڵ�������ͼ�񡣵�����������ͼ��(-1),���ܷ������ݽض�
	// 3.Sobel�����õ�4��5������Ϊ���ģ��ѡ��x����y���򣬲���ͬʱѡ�У�1��������ܲ��֪���ڲ�Դ����δ����.
	Mat sobelx, sobely,sobelxy;
	Sobel(src, sobelx, CV_16S, 1, 0);
	Sobel(src, sobely, CV_16S, 0, 1);
	convertScaleAbs(sobelx, sobelx);
	convertScaleAbs(sobely, sobely);
	imshow("x", sobelx);
	imshow("y", sobely);

	// �������
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
	// ��ʾԭͼ
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

/*17.Canny�㷨*/
void canny()
{
	Mat src, gray, can;
	src = imread("d:/workspace/ComputerVision/Assignment2/image/building.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("load error!");
		return;
	}
	// ��ʾԭͼ
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(3, 3), 0);

	// Canny�е�˫��ֵ���������õ���ֵΪThreshLowͨ��TrackBar���ڡ�����ֵͨ����ֵ��Ϊ2��1��ã�ͨ��Ϊ2��1��3��1��
	int threshLow = 30; // default = 30
	Mat * userdata[] = { &can, &gray }; // ��������callback��ʹ�õ����û�����
	namedWindow("Canny", CV_WINDOW_AUTOSIZE);
	void CallBack_Canny(int, void*);	// ��������
	createTrackbar("ThreshLow", "Canny", &threshLow, 255, CallBack_Canny, userdata);
	// ��ͨ���ú�����ʾͼ��
	CallBack_Canny(threshLow, userdata);
	
	waitKey(0);
}
// canny trackbar callback.��ע������һ����ͨ���������û����á�ͬʱ����һ���ص���������ͨ��trackbar�Զ�������
void CallBack_Canny(int threshLow, void* userdataArr) {
	// �����﷨���⣬�����˺ܾá��ؼ���userdataArr��mat*�Ķ�ά���顣��˵����[]��*����һ�Եȼ۵ļǺš�����[]��ԭ��ʱ��֪�����ȣ�������*�������ͱ����Mat**
	Mat ** userdata = (Mat**)userdataArr;
	Mat * can = *(userdata + 0); // ��ά�����һ�У�����һ�����飩��ʵ��ֻ��һ��Ԫ��
	Mat * gray = *(userdata + 1);// ͬ���ڶ��У��ڲ�Ҳֻ��һ��Ԫ�أ����Կ��Զ���ֱ��ȡֵ��������gray
	// �����������ֱ���
	// Sobel��windowsize��ȡĬ�ϵ�3������Ϊ3ģ������ݶȼ���
	// ���һ��������ȡĬ�ϵ�false����Ϊ���ݶ�ʱ�ý��Ƽ��㣨����Ϊ1�Ĺ�ʽ������trueΪ��׼��2�����Ĺ�ʽ
	Canny(*gray, *can, threshLow, threshLow * 2, 3, false);
	// ��Ȼ����*gray������gray[0]��*can������can[0]
	imshow("Canny", *can);
}

/*18.Hough�任-ֱ�߼��*/
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

/*19.Hough���Բ��*/
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
