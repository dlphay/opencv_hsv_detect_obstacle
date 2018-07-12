
/*  智能车对于障碍物的检测 */

// opencv包含的头文件
#include<opencv2/opencv.hpp>
// opencv 的CV空间域名
using namespace cv;


// 主函数
void main() 
{
	//选择测试模式：  0图像 1摄像头 2视频
	int MOD_to_test = 0;

	//检测瓶子和后面围墙
	int MODE_dect = 0; // 0瓶子  1带墙

	// 创建VideoCapture对象
     VideoCapture cap;
	 int camera_flag = 0; //摄像头打开标志位  打开1 没打开0


	 if (MOD_to_test == 1)
	 {
		 cap.open(0); //打开摄像头接口
		 if (!cap.isOpened())//如果视频不能正常打开则返回
			 return;
		 else
		 {
			 camera_flag = 1; // 打开成功
			 printf("视频或者摄像头打开成功");
		 }
	 }
	 if (MOD_to_test == 2)
	 {
		 cap.open("E://2.avi"); //打开视频接口
		 if (!cap.isOpened())//如果视频不能正常打开则返回
			 return;
		 else
		 {
			 camera_flag = 1; // 打开成功
			 printf("视频或者摄像头打开成功");
		 }
	 }


	 // 图像接口

	 Mat input_test; // 本地输入的测试图片
	 if(MOD_to_test == 0)  input_test = imread("test.jpg", CV_LOAD_IMAGE_UNCHANGED);

	 // 摄像头测试程序
     Mat frame;
	 if (camera_flag)
	 {
		 while (1)
		 {
			 cap >> frame;//等价于cap.read(frame);
			 if (frame.empty())//如果某帧为空则退出循环
				 break;
			 input_test = frame;
			 Mat insert_test;  // 得到小分辨率的图像
			 Mat HSV_test;  // 得到HSV空间的图像
			 Mat HSV_Thresholded_test;  // 得到HSV空间的图像
										// 双线性插值
			 resize(input_test, insert_test, Size(), 0.8, 0.8); // 0.4  0.4
																//RGB 转为 HSV  
			 cvtColor(insert_test, HSV_test, COLOR_BGR2HSV);

			 //颜色矩的HSV范围  
			 int iLowH = 10 / 2;
			 int iHighH = 100 / 2;
			 int iLowS = 0 * 255 / 100;
			 int iHighS = 70 * 255 / 100;
			 int iLowV = 40 * 255 / 100;
			 int iHighV = 110 * 255 / 100;
			 // HSV通道颜色矩的阈值检测
			 inRange(HSV_test, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), HSV_Thresholded_test);

			 //开操作 (去除一些噪点)  如果二值化后图片干扰部分依然很多，增大下面的size  
			 Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
			 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_OPEN, element);

			 //闭操作 (连接一些连通域)    
			 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_CLOSE, element);

			 //二值化位取反 计算原理是  255-bin(x,y)
			 Mat newBImg(HSV_Thresholded_test.rows, HSV_Thresholded_test.cols, HSV_Thresholded_test.type());
			 uchar* newBImgData = newBImg.data;
			 uchar* binaryData = HSV_Thresholded_test.data;
			 int step = HSV_Thresholded_test.step / sizeof(uchar);
			 for (int i = 0; i<HSV_Thresholded_test.rows; i++)
			 {
				 for (int j = 0; j<HSV_Thresholded_test.cols; j++)
				 {
					 newBImgData[i*step + j] = 255 - binaryData[i*step + j];
				 }
			 }
			 HSV_Thresholded_test = newBImg.clone();

			 //去除二值图像中下半部分的噪声点
			 //uchar* binaryData_noise = HSV_Thresholded_test.data;
			 //for (int i = HSV_Thresholded_test.rows / 2.2; i<HSV_Thresholded_test.rows; i++)
			 //{
				// for (int j = 0; j<HSV_Thresholded_test.cols; j++)
				// {
				//	 if (binaryData_noise[i*step + j] == 255)
				//		 binaryData_noise[i*step + j] = 0;
				// }
			 //}


			 int temp[9];
			 Mat Sobel_test;  // 得到HSV空间的图像
			 Mat Sobel_dest_test;  // 得到HSV空间的图像
			 Mat Roberts_dest_test;  // 得到HSV空间的图像
			 cvtColor(insert_test, Sobel_test, CV_RGB2GRAY);
			 int col = Sobel_test.cols, row = Sobel_test.rows;
			 Sobel_dest_test.create(row, col, CV_8UC1);
			 Roberts_dest_test.create(row, col, CV_8UC1);
			 //Sobel检测
			 for (int i = 1; i < col - 1; i++)
			 {
				 for (int j = 1; j < row - 1; j++)
				 {
					 temp[0] = Sobel_test.at<uchar>(j - 1, i - 1);
					 temp[1] = Sobel_test.at<uchar>(j - 1, i);
					 temp[2] = Sobel_test.at<uchar>(j - 1, i + 1);
					 temp[3] = Sobel_test.at<uchar>(j, i - 1);
					 temp[4] = Sobel_test.at<uchar>(j, i);
					 temp[5] = Sobel_test.at<uchar>(j, i + 1);
					 temp[6] = Sobel_test.at<uchar>(j + 1, i - 1);
					 temp[7] = Sobel_test.at<uchar>(j + 1, i);
					 temp[8] = Sobel_test.at<uchar>(j + 1, i + 1);

					 Sobel_dest_test.at<uchar>(j, i) = (int)sqrt((temp[6] + 2 * temp[7] + temp[8] - temp[1] - 2 * temp[2] - temp[3])*
						 (temp[6] + 2 * temp[7] + temp[8] - temp[1] - 2 * temp[2] - temp[3])
						 + (temp[2] + 2 * temp[5] + temp[8] - temp[0] - 2 * temp[3] - temp[6])*
						 (temp[2] + 2 * temp[5] + temp[8] - temp[0] - 2 * temp[3] - temp[6]));
				 }
			 }
			 //Roberts检测
			 for (int i = 1; i < col - 1; i++)
			 {
				 for (int j = 1; j < row - 1; j++)
				 {
					 temp[0] = Sobel_test.at<uchar>(j, i);
					 temp[1] = Sobel_test.at<uchar>(j, i + 1);
					 temp[2] = Sobel_test.at<uchar>(j + 1, i);
					 temp[3] = Sobel_test.at<uchar>(j + 1, i + 1);
					 Roberts_dest_test.at<uchar>(j, i) = (int)sqrt((temp[0] - temp[3])*(temp[0] - temp[3])
						 + (temp[1] - temp[2])*(temp[1] - temp[2]));
				 }
			 }





			 imshow("双线性差值", insert_test);
			 imshow("HSV", HSV_test);
			 imshow("HSV阈值", HSV_Thresholded_test);
			 //imshow("SOBEL", Sobel_dest_test);
			 //imshow("ROBERTS", Roberts_dest_test);




			 //imshow("video", frame);
			 waitKey(20);//每帧延时20毫秒
		 }
		 cap.release();//释放资源
	 }

	 // 图像测试程序 只检测瓶子作为障碍物
	 if (MODE_dect == 0)
	 {
		 Mat insert_test;  // 得到小分辨率的图像
		 Mat HSV_test;  // 得到HSV空间的图像
		 Mat HSV_Thresholded_test;  // 得到HSV空间的图像
		 // 双线性插值
		 resize(input_test, insert_test, Size(), 0.3, 0.3); // 0.4  0.4
		 //RGB 转为 HSV  
		 cvtColor(insert_test, HSV_test, COLOR_BGR2HSV);

		 //颜色矩的HSV范围   
		 int iLowH = 30 / 2;
		 int iHighH = 130 / 2;
		 int iLowS = 25 * 255 / 100;
		 int iHighS = 70 * 255 / 100;
		 int iLowV = 20 * 255 / 100;
		 int iHighV = 120 * 255 / 100;
		 // HSV通道颜色矩的阈值检测
		 inRange(HSV_test, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), HSV_Thresholded_test);

		 //开操作 (去除一些噪点)  如果二值化后图片干扰部分依然很多，增大下面的size  
		 Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_OPEN, element);

		 //闭操作 (连接一些连通域)    
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_CLOSE, element);

		 int temp[9];
		 Mat Sobel_test;  // 得到HSV空间的图像
		 Mat Sobel_dest_test;  // 得到HSV空间的图像
		 Mat Roberts_dest_test;  // 得到HSV空间的图像
		 cvtColor(insert_test, Sobel_test, CV_RGB2GRAY);
		 int col = Sobel_test.cols, row = Sobel_test.rows;
		 Sobel_dest_test.create(row, col, CV_8UC1);
		 Roberts_dest_test.create(row, col, CV_8UC1);
		 //Sobel检测
		 for (int i = 1; i < col - 1; i++)
		 {
			 for (int j = 1; j < row - 1; j++)
			 {
				 temp[0] = Sobel_test.at<uchar>(j - 1, i - 1);
				 temp[1] = Sobel_test.at<uchar>(j - 1, i);
				 temp[2] = Sobel_test.at<uchar>(j - 1, i + 1);
				 temp[3] = Sobel_test.at<uchar>(j, i - 1);
				 temp[4] = Sobel_test.at<uchar>(j, i);
				 temp[5] = Sobel_test.at<uchar>(j, i + 1);
				 temp[6] = Sobel_test.at<uchar>(j + 1, i - 1);
				 temp[7] = Sobel_test.at<uchar>(j + 1, i);
				 temp[8] = Sobel_test.at<uchar>(j + 1, i + 1);

				 Sobel_dest_test.at<uchar>(j, i) = (int)sqrt((temp[6] + 2 * temp[7] + temp[8] - temp[1] - 2 * temp[2] - temp[3])*
					 (temp[6] + 2 * temp[7] + temp[8] - temp[1] - 2 * temp[2] - temp[3])
					 + (temp[2] + 2 * temp[5] + temp[8] - temp[0] - 2 * temp[3] - temp[6])*
					 (temp[2] + 2 * temp[5] + temp[8] - temp[0] - 2 * temp[3] - temp[6]));
			 }
		 }
		 //Roberts检测
		 for (int i = 1; i < col - 1; i++)
		 {
			 for (int j = 1; j < row - 1; j++)
			 {
				 temp[0] = Sobel_test.at<uchar>(j, i);
				 temp[1] = Sobel_test.at<uchar>(j, i + 1);
				 temp[2] = Sobel_test.at<uchar>(j + 1, i);
				 temp[3] = Sobel_test.at<uchar>(j + 1, i + 1);
				 Roberts_dest_test.at<uchar>(j, i) = (int)sqrt((temp[0] - temp[3])*(temp[0] - temp[3])
					 + (temp[1] - temp[2])*(temp[1] - temp[2]));
			 }
		 }





		 imshow("双线性差值", insert_test);
		 imshow("HSV", HSV_test);
		 imshow("HSV阈值", HSV_Thresholded_test);
		 //imshow("SOBEL", Sobel_dest_test);
		 //imshow("ROBERTS", Roberts_dest_test);
		 waitKey(0);
	 }

	 // 图像测试程序 检测后面墙作为障碍物
	 if (MODE_dect == 1)
	 {
		 Mat insert_test;  // 得到小分辨率的图像
		 Mat HSV_test;  // 得到HSV空间的图像
		 Mat HSV_Thresholded_test;  // 得到HSV空间的图像
									// 双线性插值
		 resize(input_test, insert_test, Size(), 0.3, 0.3); // 0.4  0.4
															//RGB 转为 HSV  
		 cvtColor(insert_test, HSV_test, COLOR_BGR2HSV);

		 //颜色矩的HSV范围  
		 int iLowH = 10 / 2;
		 int iHighH = 100 / 2;
		 int iLowS = 0 * 255 / 100;
		 int iHighS = 70 * 255 / 100;
		 int iLowV = 40 * 255 / 100;
		 int iHighV = 150 * 255 / 100;
		 // HSV通道颜色矩的阈值检测
		 inRange(HSV_test, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), HSV_Thresholded_test);

		 //开操作 (去除一些噪点)  如果二值化后图片干扰部分依然很多，增大下面的size  
		 Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_OPEN, element);

		 //闭操作 (连接一些连通域)    
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_CLOSE, element);

		 //二值化位取反 计算原理是  255-bin(x,y)
		 Mat newBImg(HSV_Thresholded_test.rows, HSV_Thresholded_test.cols, HSV_Thresholded_test.type());
		 uchar* newBImgData = newBImg.data;
		 uchar* binaryData = HSV_Thresholded_test.data;
		 int step = HSV_Thresholded_test.step / sizeof(uchar);
		 for (int i = 0; i<HSV_Thresholded_test.rows; i++)
		 {
			 for (int j = 0; j<HSV_Thresholded_test.cols; j++)
			 {
				 newBImgData[i*step + j] = 255 - binaryData[i*step + j];
			 }
		 }
		 HSV_Thresholded_test = newBImg.clone();

		 //去除二值图像中下半部分的噪声点
		 uchar* binaryData_noise = HSV_Thresholded_test.data;
		 for (int i = HSV_Thresholded_test.rows/2.2; i<HSV_Thresholded_test.rows; i++)
		 {
			 for (int j = 0; j<HSV_Thresholded_test.cols; j++)
			 {
				 if(binaryData_noise[i*step + j] == 255)
					 binaryData_noise[i*step + j] = 0;
			 }
		 }

		 int temp[9];
		 Mat Sobel_test;  // 得到HSV空间的图像
		 Mat Sobel_dest_test;  // 得到HSV空间的图像
		 Mat Roberts_dest_test;  // 得到HSV空间的图像
		 cvtColor(insert_test, Sobel_test, CV_RGB2GRAY);
		 int col = Sobel_test.cols, row = Sobel_test.rows;
		 Sobel_dest_test.create(row, col, CV_8UC1);
		 Roberts_dest_test.create(row, col, CV_8UC1);
		 //Sobel检测
		 for (int i = 1; i < col - 1; i++)
		 {
			 for (int j = 1; j < row - 1; j++)
			 {
				 temp[0] = Sobel_test.at<uchar>(j - 1, i - 1);
				 temp[1] = Sobel_test.at<uchar>(j - 1, i);
				 temp[2] = Sobel_test.at<uchar>(j - 1, i + 1);
				 temp[3] = Sobel_test.at<uchar>(j, i - 1);
				 temp[4] = Sobel_test.at<uchar>(j, i);
				 temp[5] = Sobel_test.at<uchar>(j, i + 1);
				 temp[6] = Sobel_test.at<uchar>(j + 1, i - 1);
				 temp[7] = Sobel_test.at<uchar>(j + 1, i);
				 temp[8] = Sobel_test.at<uchar>(j + 1, i + 1);

				 Sobel_dest_test.at<uchar>(j, i) = (int)sqrt((temp[6] + 2 * temp[7] + temp[8] - temp[1] - 2 * temp[2] - temp[3])*
					 (temp[6] + 2 * temp[7] + temp[8] - temp[1] - 2 * temp[2] - temp[3])
					 + (temp[2] + 2 * temp[5] + temp[8] - temp[0] - 2 * temp[3] - temp[6])*
					 (temp[2] + 2 * temp[5] + temp[8] - temp[0] - 2 * temp[3] - temp[6]));
			 }
		 }
		 //Roberts检测
		 for (int i = 1; i < col - 1; i++)
		 {
			 for (int j = 1; j < row - 1; j++)
			 {
				 temp[0] = Sobel_test.at<uchar>(j, i);
				 temp[1] = Sobel_test.at<uchar>(j, i + 1);
				 temp[2] = Sobel_test.at<uchar>(j + 1, i);
				 temp[3] = Sobel_test.at<uchar>(j + 1, i + 1);
				 Roberts_dest_test.at<uchar>(j, i) = (int)sqrt((temp[0] - temp[3])*(temp[0] - temp[3])
					 + (temp[1] - temp[2])*(temp[1] - temp[2]));
			 }
		 }

		 imshow("双线性差值", insert_test);
		 imshow("HSV", HSV_test);
		 imshow("HSV阈值", HSV_Thresholded_test);
		 //imshow("SOBEL", Sobel_dest_test);
		 //imshow("ROBERTS", Roberts_dest_test);
		 waitKey(0);
	 }
}