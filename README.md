[TOC]

---

# Opencv 入门

---

## 一、环境搭建

Window下环境的搭建分为以下几个步骤：（opencv3.4+vs2017）

- 官网下载opencv压缩包
- 解压之后找到压缩目录（我的是：D:\opencv\），设置环境变量：D:\opencv\build\x64\vc15\bin
- 打开vs新建空白项目，找到：视图-属性管理器
- Debug|x64下的Microsoft.Cpp.x64.user右键选择属性
- 配置属性“VC++目录”，包含目录，添加3个目录，分别为
  - D:\opencv\build\include
  - D:\opencv\build\include\opencv
  - D:\opencv\build\include\opencv2
- 同样是“VC++目录”下，库目录，添加1个目录：D:\opencv\build\x64\vc15\lib
- 链接器，输入，附加依赖项，添加：opencv_world340d.lib
- 确定

测试代码

```C++
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char** argv) {
    Mat src = imread("D:/dehaze_image/haze0.jpg");
    if (src.empty()) {
        printf("could't load image..\n");
        return -1;
    }
    namedWindow("test opencv setup", CV_WINDOW_AUTOSIZE);
    imshow("test opencv setup", src);

    waitKey(0);
    return 0;
}
// 解决方案要选择，Debug，x64
```

---

## 二、加载、修改、保存图片

加载：` Mat imread( const String& filename, int flags = IMREAD_COLOR )`

转换颜色类型：`void cvtColor( InputArray src, OutputArray dst, int code, int dstCn = 0 )`

保存图片：`bool imwrite( const String& filename, InputArray img,const std::vector<int>& params = std::vector<int>())`

```C++
/*2.图像的输入，修改，保存*/
void section2_io() {
    /*
    图像输入，第一个参数为图像路径，第二个参数为图像类型IMREAD_XXX
    默认情况下，以IMREAD_COLOR加载，表示RGB彩色图像，可以通过该参数制定加载为什么类型，如灰度图等
    imread可以加载jpg、png、tif等常见图片格式
    */
    Mat src = imread("D:/dehaze_image/lena.jpg",IMREAD_COLOR);
    if (src.empty()) {
        printf("could't load image..\n");
        return ;
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
```

---

## 三、矩阵的掩膜(mask)操作

0、什么是掩膜操作？（这里涉及部分数字图像处理的原理的自我理解——可能存在问题）

掩膜操作即图像的模板卷积操作，实际就是乘加的操作。卷积定理有：函数卷积的傅里叶变换是函数傅里叶变换的乘积。即，一个域中的卷积相当于另一个域中的乘积，例如时域中的卷积就对应于频域中的乘积，卷积定理表达如下：（代码并没有用到这个，但是卷积和傅里叶具有天然联系，需要了解）

$$F(g(x)*f(x)) = F(g(x))*F(f(x))$$

$$时域卷积=频域相乘$$

**为什么模板操作具有例如平滑、去噪等功能？**

- 在时域来讲，模板操作是一种权值分配的计算，不同算子体现了不同的权重思想，相应得取得了不同的效果。
- 在频域来讲，模板卷积，实际相当于频域的滤波（用一个滤波信号，遍历于每一个图块相乘），过滤图块中的特定频率，而图像的频域分析已知，高频部分对应图像的纹理和细节，低频部分对应图像的大体轮廓，故不同的滤波对高频和低频的处理不同，产生不同的效果。



1、像素操作

`uchar * rowVecter = Mat.ptr<uchar>(rowIndex);//此处uchar为泛型类型的实类型`

Mat提供了获取一个行像素的方法ptr<类型>(行号)。



2、矩阵模型

Mat表示图像的像素矩阵，行表示每一行像素，但列比较特殊，列是每一个像素的各通道值依次排列。

即，col_count  == mat.cols * mat.channels()。即，假如图像为3通道，则一个像素占据一行的3个列的空间。在像素处理的时候，需要谨记该存储模型。



3、默认的标准化函数（防溢出）

opencv提供了防止数据溢出的函数：saturate_cast< T >(int value)，将value规定在图像范围之内，如[0,255]，采用的是越界则取边界值的方法（大于255则取255，小于0则取0）。



4、opencv库里提供的mask操作，filter2D

- `Mat mat = Mat::zeros(size,type) // 根据大小和类型构建一个图像的像素矩阵，初始化为0`
- `Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);// 构建一个特定的模板算子`
- `filter2D(src, dst, src.depth(), kernel);//模板操作，第三个参数可用-1，表示深度和输入图像一致`



5、计时器：计算运行时间

两个函数：`int64 t = getTickCount()//获取时间`，`(t2-t1)/getTickFrequency();//将tickcount转换为具体的时间，单位为秒，因为getTickFrequency的物理意义为每秒有多少个Tick`。



6、代码样例

```c++
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
    int rows_end = src.rows-1;
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
        for (int col = col_offset; col < cols_end; col ++) {
            // 掩膜操作，就是卷积操作，对模板的乘加操作
            outrow[col] = saturate_cast<uchar>(5*currentRow[col]-preRow[col]-nextRow[col]-currentRow[col-col_offset]-currentRow[col+col_offset]);
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
```

```C++
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
```

---

## 四、Mat对象

- Mat对象和IplImage对象：IplImage是最初的C风格的数据结构，开发者管理内存，容易造成内存泄漏。Mat是后续版本Opencv提供的图像数据结构，自动进行内存管理，面向对象的数据结构。包含头部与数据两个部分。之后的使用都推荐使用Mat对象。
- Mat常用方法：
    - Mat a; Mat b(a); // 此时拷贝构造，只复制Mat的头和指针部分内容，不会复制数据部分。
    - a.clone() or a.copyTo() // 完全复制
      ![img](http://ba9e1f0f.wiz03.com/share/resources/8799e511-9544-4ea7-99ec-ea19452cbdfc/index_files/b31bde9a-cf8f-461e-9808-a64e3ca2b2b4.jpg)

- 输出图像的内存是自动分配的
- 使用opencv的c++接口，不需要考虑内存分配问题
- 复制操作和拷贝构造函数只会复制头部分信息
- clone和copyTo两个函数实现数据完全复制 
- 定义数组：
    - 高维数组（很少用到）：
    - 定义小数组（模板，kernel）：Mat kernel = (Mat_<type>(rows,cols) << value1,value2....)
    - 零矩阵：Mat::zeros()
    - 单位矩阵：Mat::eye()
```c++
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

    // Mat dst = src.clone();  // Clone方法完全复制一个Mat对象
    
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

```
---
## 五、图像操作
- 读写图像：imread、imwrite
- 读写像素 ： mat.at、mat.ptr指针操作
```c++
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

```
---
## 六、图像混合
- 理论-线性混合：$g(x) = (1-a)f_1(x) + af_2(x)$
- API：addWeighted
- notice：两个图必须大小相同，类型相同
```c++
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
```
---
## 七、












































