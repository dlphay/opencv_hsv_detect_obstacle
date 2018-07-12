
/*  ���ܳ������ϰ���ļ�� */

// opencv������ͷ�ļ�
#include<opencv2/opencv.hpp>
// opencv ��CV�ռ�����
using namespace cv;


// ������
void main() 
{
	//ѡ�����ģʽ��  0ͼ�� 1����ͷ 2��Ƶ
	int MOD_to_test = 0;

	//���ƿ�Ӻͺ���Χǽ
	int MODE_dect = 0; // 0ƿ��  1��ǽ

	// ����VideoCapture����
     VideoCapture cap;
	 int camera_flag = 0; //����ͷ�򿪱�־λ  ��1 û��0


	 if (MOD_to_test == 1)
	 {
		 cap.open(0); //������ͷ�ӿ�
		 if (!cap.isOpened())//�����Ƶ�����������򷵻�
			 return;
		 else
		 {
			 camera_flag = 1; // �򿪳ɹ�
			 printf("��Ƶ��������ͷ�򿪳ɹ�");
		 }
	 }
	 if (MOD_to_test == 2)
	 {
		 cap.open("E://2.avi"); //����Ƶ�ӿ�
		 if (!cap.isOpened())//�����Ƶ�����������򷵻�
			 return;
		 else
		 {
			 camera_flag = 1; // �򿪳ɹ�
			 printf("��Ƶ��������ͷ�򿪳ɹ�");
		 }
	 }


	 // ͼ��ӿ�

	 Mat input_test; // ��������Ĳ���ͼƬ
	 if(MOD_to_test == 0)  input_test = imread("test.jpg", CV_LOAD_IMAGE_UNCHANGED);

	 // ����ͷ���Գ���
     Mat frame;
	 if (camera_flag)
	 {
		 while (1)
		 {
			 cap >> frame;//�ȼ���cap.read(frame);
			 if (frame.empty())//���ĳ֡Ϊ�����˳�ѭ��
				 break;
			 input_test = frame;
			 Mat insert_test;  // �õ�С�ֱ��ʵ�ͼ��
			 Mat HSV_test;  // �õ�HSV�ռ��ͼ��
			 Mat HSV_Thresholded_test;  // �õ�HSV�ռ��ͼ��
										// ˫���Բ�ֵ
			 resize(input_test, insert_test, Size(), 0.8, 0.8); // 0.4  0.4
																//RGB תΪ HSV  
			 cvtColor(insert_test, HSV_test, COLOR_BGR2HSV);

			 //��ɫ�ص�HSV��Χ  
			 int iLowH = 10 / 2;
			 int iHighH = 100 / 2;
			 int iLowS = 0 * 255 / 100;
			 int iHighS = 70 * 255 / 100;
			 int iLowV = 40 * 255 / 100;
			 int iHighV = 110 * 255 / 100;
			 // HSVͨ����ɫ�ص���ֵ���
			 inRange(HSV_test, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), HSV_Thresholded_test);

			 //������ (ȥ��һЩ���)  �����ֵ����ͼƬ���Ų�����Ȼ�ܶ࣬���������size  
			 Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
			 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_OPEN, element);

			 //�ղ��� (����һЩ��ͨ��)    
			 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_CLOSE, element);

			 //��ֵ��λȡ�� ����ԭ����  255-bin(x,y)
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

			 //ȥ����ֵͼ�����°벿�ֵ�������
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
			 Mat Sobel_test;  // �õ�HSV�ռ��ͼ��
			 Mat Sobel_dest_test;  // �õ�HSV�ռ��ͼ��
			 Mat Roberts_dest_test;  // �õ�HSV�ռ��ͼ��
			 cvtColor(insert_test, Sobel_test, CV_RGB2GRAY);
			 int col = Sobel_test.cols, row = Sobel_test.rows;
			 Sobel_dest_test.create(row, col, CV_8UC1);
			 Roberts_dest_test.create(row, col, CV_8UC1);
			 //Sobel���
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
			 //Roberts���
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





			 imshow("˫���Բ�ֵ", insert_test);
			 imshow("HSV", HSV_test);
			 imshow("HSV��ֵ", HSV_Thresholded_test);
			 //imshow("SOBEL", Sobel_dest_test);
			 //imshow("ROBERTS", Roberts_dest_test);




			 //imshow("video", frame);
			 waitKey(20);//ÿ֡��ʱ20����
		 }
		 cap.release();//�ͷ���Դ
	 }

	 // ͼ����Գ��� ֻ���ƿ����Ϊ�ϰ���
	 if (MODE_dect == 0)
	 {
		 Mat insert_test;  // �õ�С�ֱ��ʵ�ͼ��
		 Mat HSV_test;  // �õ�HSV�ռ��ͼ��
		 Mat HSV_Thresholded_test;  // �õ�HSV�ռ��ͼ��
		 // ˫���Բ�ֵ
		 resize(input_test, insert_test, Size(), 0.3, 0.3); // 0.4  0.4
		 //RGB תΪ HSV  
		 cvtColor(insert_test, HSV_test, COLOR_BGR2HSV);

		 //��ɫ�ص�HSV��Χ   
		 int iLowH = 30 / 2;
		 int iHighH = 130 / 2;
		 int iLowS = 25 * 255 / 100;
		 int iHighS = 70 * 255 / 100;
		 int iLowV = 20 * 255 / 100;
		 int iHighV = 120 * 255 / 100;
		 // HSVͨ����ɫ�ص���ֵ���
		 inRange(HSV_test, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), HSV_Thresholded_test);

		 //������ (ȥ��һЩ���)  �����ֵ����ͼƬ���Ų�����Ȼ�ܶ࣬���������size  
		 Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_OPEN, element);

		 //�ղ��� (����һЩ��ͨ��)    
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_CLOSE, element);

		 int temp[9];
		 Mat Sobel_test;  // �õ�HSV�ռ��ͼ��
		 Mat Sobel_dest_test;  // �õ�HSV�ռ��ͼ��
		 Mat Roberts_dest_test;  // �õ�HSV�ռ��ͼ��
		 cvtColor(insert_test, Sobel_test, CV_RGB2GRAY);
		 int col = Sobel_test.cols, row = Sobel_test.rows;
		 Sobel_dest_test.create(row, col, CV_8UC1);
		 Roberts_dest_test.create(row, col, CV_8UC1);
		 //Sobel���
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
		 //Roberts���
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





		 imshow("˫���Բ�ֵ", insert_test);
		 imshow("HSV", HSV_test);
		 imshow("HSV��ֵ", HSV_Thresholded_test);
		 //imshow("SOBEL", Sobel_dest_test);
		 //imshow("ROBERTS", Roberts_dest_test);
		 waitKey(0);
	 }

	 // ͼ����Գ��� ������ǽ��Ϊ�ϰ���
	 if (MODE_dect == 1)
	 {
		 Mat insert_test;  // �õ�С�ֱ��ʵ�ͼ��
		 Mat HSV_test;  // �õ�HSV�ռ��ͼ��
		 Mat HSV_Thresholded_test;  // �õ�HSV�ռ��ͼ��
									// ˫���Բ�ֵ
		 resize(input_test, insert_test, Size(), 0.3, 0.3); // 0.4  0.4
															//RGB תΪ HSV  
		 cvtColor(insert_test, HSV_test, COLOR_BGR2HSV);

		 //��ɫ�ص�HSV��Χ  
		 int iLowH = 10 / 2;
		 int iHighH = 100 / 2;
		 int iLowS = 0 * 255 / 100;
		 int iHighS = 70 * 255 / 100;
		 int iLowV = 40 * 255 / 100;
		 int iHighV = 150 * 255 / 100;
		 // HSVͨ����ɫ�ص���ֵ���
		 inRange(HSV_test, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), HSV_Thresholded_test);

		 //������ (ȥ��һЩ���)  �����ֵ����ͼƬ���Ų�����Ȼ�ܶ࣬���������size  
		 Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_OPEN, element);

		 //�ղ��� (����һЩ��ͨ��)    
		 morphologyEx(HSV_Thresholded_test, HSV_Thresholded_test, MORPH_CLOSE, element);

		 //��ֵ��λȡ�� ����ԭ����  255-bin(x,y)
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

		 //ȥ����ֵͼ�����°벿�ֵ�������
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
		 Mat Sobel_test;  // �õ�HSV�ռ��ͼ��
		 Mat Sobel_dest_test;  // �õ�HSV�ռ��ͼ��
		 Mat Roberts_dest_test;  // �õ�HSV�ռ��ͼ��
		 cvtColor(insert_test, Sobel_test, CV_RGB2GRAY);
		 int col = Sobel_test.cols, row = Sobel_test.rows;
		 Sobel_dest_test.create(row, col, CV_8UC1);
		 Roberts_dest_test.create(row, col, CV_8UC1);
		 //Sobel���
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
		 //Roberts���
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

		 imshow("˫���Բ�ֵ", insert_test);
		 imshow("HSV", HSV_test);
		 imshow("HSV��ֵ", HSV_Thresholded_test);
		 //imshow("SOBEL", Sobel_dest_test);
		 //imshow("ROBERTS", Roberts_dest_test);
		 waitKey(0);
	 }
}