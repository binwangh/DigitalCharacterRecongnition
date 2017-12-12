#ifndef SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API
#define SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API _declspec(dllexport) 
#endif

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include "..\include\classification-c.h"
#include "time.h"
#include <opencv2\opencv.hpp>
#include "..\include\digitalcharacterrecongnition.h"
#include "windows.h"

using namespace std;
using namespace cv;

#define		PROBABILITYCOUT		0	// ���ַ��������

// ��̬���ӿ⣬��ֱ�ӵ��ã�����������ʱ����Ҫ��DLLһ�𷢲�������
// ������libgcc_s_dw2-1.dll��libopenblas.dll
#pragma comment(lib, "..\\lib\\classification_dll.lib")

// ��̬���ӿ⣬��ֱ�ӵ��ã�����������ʱ��ʲô������Ҫ��������
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_core249.lib")
#pragma comment( lib, "IlmImf.lib" )  
#pragma comment( lib, "libjpegd.lib" )  
#pragma comment( lib, "libjasper.lib" )  
#pragma comment( lib, "libjpeg.lib" )  
#pragma comment( lib, "libpng.lib" )  
#pragma comment( lib, "libtiff.lib" )  
#pragma comment( lib, "zlib.lib" )  
// win32������Ҫ���vfw32.lib��comctl32.lib�������������ļ�
#pragma comment( lib, "vfw32.lib" )  
#pragma comment( lib, "comctl32.lib" )  

// ȫ�ֱ���
Classifier *g_classifier = NULL;
float g_scale_raw = 0.00390625;
char* g_mean_file = NULL;
int g_num_means = 3;
vector<string> g_labelMap;
bool _loadDeepLearningModelFlag = false;

// ��Բ�ͬ����Ŀ��������
float g_means[3] = { 189.4260, 196.4261, 188.5993 };
int g_time_step = 46;
int g_blank_label = 0;
int g_alphabet_size = 35;
const int _OutputWidth = 368;
const int _OutputHeight = 48;
string g_label = "_ABCDEFGHJKLMNPQRSTUVWXYZ0123456789";

HANDLE SunCreateHandle(DWORD dwWidth/*=800*/, DWORD dwHeight/*=600*/, int whichType/*=1*/)
{
	//strcpy(gszLastError,"");
	LPBITMAPINFOHEADER biHdr;
	int iLength;
	HANDLE hDIB;

	LONG nWidthBytes;
	BYTE *lpDst;

	long wBpp;
	DWORD biClrUsed;
	switch (whichType)
	{
	case 1:
		wBpp = 24;
		biClrUsed = 0;
		break;
	case 2:
		wBpp = 8;
		biClrUsed = 256;
		break;
	case 3:
		wBpp = 4;
		biClrUsed = 16;
		break;
	case 4:
		wBpp = 1;
		biClrUsed = 2;
		break;
	case 5:
		wBpp = 8;
		biClrUsed = 256;
		break;
	default:
		wBpp = 0;
		biClrUsed = 0;
		//strcpy(gszLastError,"error C1002: not support this image format");
		return NULL;
	}

	nWidthBytes = (wBpp*dwWidth + 31) / 32 * 4;

	iLength = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*biClrUsed + nWidthBytes * dwHeight;

	hDIB = ::GlobalAlloc(GHND, iLength);
	if (!hDIB)
	{
		return NULL;
	}

	biHdr = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
	biHdr->biSize = sizeof(BITMAPINFOHEADER);
	biHdr->biWidth = dwWidth;  	// fill in width from parameter  
	biHdr->biHeight = dwHeight;       // fill in height from parameter
	biHdr->biPlanes = 1;              // must be 1
	biHdr->biBitCount = (unsigned short)wBpp;    // from parameter 
	biHdr->biCompression = BI_RGB;
	biHdr->biSizeImage = nWidthBytes * dwHeight;
	biHdr->biXPelsPerMeter = 0;
	biHdr->biYPelsPerMeter = 0;
	if (whichType == 1)
		biHdr->biClrUsed = 0;
	else
		biHdr->biClrUsed = 1 << wBpp;
	biHdr->biClrImportant = 0;

	//switch(Color)
	switch (whichType)
	{
	case 1:  //���ɫ����ɫ��
		break;

	case 2:  //256�ֱ�׼��ɫ��
	{
				 const BYTE pal256[1024] = { 0, 0, 0, 0, 0, 0, 128, 0, 0, 128, 0, 0, 0, 128, 128, 0, 128, 0, 0, 0, 128, 0, 128, 0, 128, 128, 0, 0, 192, 192, 192, 0,
					 192, 220, 192, 0, 240, 202, 166, 0, 212, 240, 255, 0, 177, 226, 255, 0, 142, 212, 255, 0, 107, 198, 255, 0,
					 72, 184, 255, 0, 37, 170, 255, 0, 0, 170, 255, 0, 0, 146, 220, 0, 0, 122, 185, 0, 0, 98, 150, 0, 0, 74, 115, 0, 0,
					 50, 80, 0, 212, 227, 255, 0, 177, 199, 255, 0, 142, 171, 255, 0, 107, 143, 255, 0, 72, 115, 255, 0, 37, 87, 255, 0, 0,
					 85, 255, 0, 0, 73, 220, 0, 0, 61, 185, 0, 0, 49, 150, 0, 0, 37, 115, 0, 0, 25, 80, 0, 212, 212, 255, 0, 177, 177, 255, 0,
					 142, 142, 255, 0, 107, 107, 255, 0, 72, 72, 255, 0, 37, 37, 255, 0, 0, 0, 254, 0, 0, 0, 220, 0, 0, 0, 185, 0, 0, 0, 150, 0,
					 0, 0, 115, 0, 0, 0, 80, 0, 227, 212, 255, 0, 199, 177, 255, 0, 171, 142, 255, 0, 143, 107, 255, 0, 115, 72, 255, 0,
					 87, 37, 255, 0, 85, 0, 255, 0, 73, 0, 220, 0, 61, 0, 185, 0, 49, 0, 150, 0, 37, 0, 115, 0, 25, 0, 80, 0, 240, 212, 255, 0,
					 226, 177, 255, 0, 212, 142, 255, 0, 198, 107, 255, 0, 184, 72, 255, 0, 170, 37, 255, 0, 170, 0, 255, 0, 146, 0, 220, 0,
					 122, 0, 185, 0, 98, 0, 150, 0, 74, 0, 115, 0, 50, 0, 80, 0, 255, 212, 255, 0, 255, 177, 255, 0, 255, 142, 255, 0, 255, 107, 255, 0,
					 255, 72, 255, 0, 255, 37, 255, 0, 254, 0, 254, 0, 220, 0, 220, 0, 185, 0, 185, 0, 150, 0, 150, 0, 115, 0, 115, 0, 80, 0, 80, 0,
					 255, 212, 240, 0, 255, 177, 226, 0, 255, 142, 212, 0, 255, 107, 198, 0, 255, 72, 184, 0, 255, 37, 170, 0, 255, 0, 170, 0,
					 220, 0, 146, 0, 185, 0, 122, 0, 150, 0, 98, 0, 115, 0, 74, 0, 80, 0, 50, 0, 255, 212, 227, 0, 255, 177, 199, 0, 255, 142, 171, 0,
					 255, 107, 143, 0, 255, 72, 115, 0, 255, 37, 87, 0, 255, 0, 85, 0, 220, 0, 73, 0, 185, 0, 61, 0, 150, 0, 49, 0, 115, 0, 37, 0,
					 80, 0, 25, 0, 255, 212, 212, 0, 255, 177, 177, 0, 255, 142, 142, 0, 255, 107, 107, 0, 255, 72, 72, 0, 255, 37, 37, 0, 254, 0,
					 0, 0, 220, 0, 0, 0, 185, 0, 0, 0, 150, 0, 0, 0, 115, 0, 0, 0, 80, 0, 0, 0, 255, 227, 212, 0, 255, 199, 177, 0, 255, 171, 142, 0,
					 255, 143, 107, 0, 255, 115, 72, 0, 255, 87, 37, 0, 255, 85, 0, 0, 220, 73, 0, 0, 185, 61, 0, 0, 150, 49, 0, 0, 115, 37, 0,
					 0, 80, 25, 0, 0, 255, 240, 212, 0, 255, 226, 177, 0, 255, 212, 142, 0, 255, 198, 107, 0, 255, 184, 72, 0, 255, 170, 37, 0,
					 255, 170, 0, 0, 220, 146, 0, 0, 185, 122, 0, 0, 150, 98, 0, 0, 115, 74, 0, 0, 80, 50, 0, 0, 255, 255, 212, 0, 255, 255, 177, 0,
					 255, 255, 142, 0, 255, 255, 107, 0, 255, 255, 72, 0, 255, 255, 37, 0, 254, 254, 0, 0, 220, 220, 0, 0, 185, 185, 0, 0, 150, 150, 0,
					 0, 115, 115, 0, 0, 80, 80, 0, 0, 240, 255, 212, 0, 226, 255, 177, 0, 212, 255, 142, 0, 198, 255, 107, 0, 184, 255, 72, 0,
					 170, 255, 37, 0, 170, 255, 0, 0, 146, 220, 0, 0, 122, 185, 0, 0, 98, 150, 0, 0, 74, 115, 0, 0, 50, 80, 0, 0, 227, 255, 212, 0,
					 199, 255, 177, 0, 171, 255, 142, 0, 143, 255, 107, 0, 115, 255, 72, 0, 87, 255, 37, 0, 85, 255, 0, 0, 73, 220, 0, 0, 61, 185, 0,
					 0, 49, 150, 0, 0, 37, 115, 0, 0, 25, 80, 0, 0, 212, 255, 212, 0, 177, 255, 177, 0, 142, 255, 142, 0, 107, 255, 107, 0, 72, 255, 72, 0,
					 37, 255, 37, 0, 0, 254, 0, 0, 0, 220, 0, 0, 0, 185, 0, 0, 0, 150, 0, 0, 0, 115, 0, 0, 0, 80, 0, 0, 212, 255, 227, 0, 177, 255, 199, 0,
					 142, 255, 171, 0, 107, 255, 143, 0, 72, 255, 115, 0, 37, 255, 87, 0, 0, 255, 85, 0, 0, 220, 73, 0, 0, 185, 61, 0, 0, 150, 49, 0, 0,
					 115, 37, 0, 0, 80, 25, 0, 212, 255, 240, 0, 177, 255, 226, 0, 142, 255, 212, 0, 107, 255, 198, 0, 72, 255, 184, 0, 37, 255, 170, 0,
					 0, 255, 170, 0, 0, 220, 146, 0, 0, 185, 122, 0, 0, 150, 98, 0, 0, 115, 74, 0, 0, 80, 50, 0, 212, 255, 255, 0, 177, 255, 255, 0,
					 142, 255, 255, 0, 107, 255, 255, 0, 72, 255, 255, 0, 37, 255, 255, 0, 0, 254, 254, 0, 0, 220, 220, 0, 0, 185, 185, 0, 0,
					 150, 150, 0, 0, 115, 115, 0, 0, 80, 80, 0, 242, 242, 242, 0, 230, 230, 230, 0, 218, 218, 218, 0, 206, 206, 206, 0, 194, 194, 194, 0,
					 182, 182, 182, 0, 170, 170, 170, 0, 158, 158, 158, 0, 146, 146, 146, 0, 134, 134, 134, 0, 122, 122, 122, 0, 110, 110, 110, 0,
					 98, 98, 98, 0, 86, 86, 86, 0, 74, 74, 74, 0, 62, 62, 62, 0, 50, 50, 50, 0, 38, 38, 38, 0, 26, 26, 26, 0, 14, 14, 14, 0, 240, 251, 255, 0,
					 164, 160, 160, 0, 128, 128, 128, 0, 0, 0, 255, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 255, 255, 0 };
				 lpDst = (BYTE *)biHdr + sizeof(BITMAPINFOHEADER);
				 memcpy(lpDst, pal256, 1024);
				 break;
	}

	case 3: //16�ֱ�׼��ɫ��
	{
				const BYTE pal16[64] = { 0, 0, 0, 0, 0, 0, 128, 0, 0, 128, 0, 0, 0, 128, 128, 0, 128, 0, 0, 0, 128, 0, 128, 0, 128, 128, 0, 0, 192, 192, 192, 0,
					128, 128, 128, 0, 0, 0, 255, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 255, 255, 0 };
				lpDst = (BYTE *)biHdr + sizeof(BITMAPINFOHEADER);
				//memcpy(GetPalette(),pal16,64);
				memcpy(lpDst, pal16, 64);
				break;
	}

	case 4:	//�ڰ׶�ֵͼ���ɫ�帳ֵ:
	{
				lpDst = (BYTE *)biHdr + sizeof(BITMAPINFOHEADER);
				*lpDst = 0;
				lpDst++;
				*lpDst = 0;
				lpDst++;
				*lpDst = 0;
				lpDst++;
				*lpDst = 0;
				lpDst++;

				*lpDst = 255;
				lpDst++;
				*lpDst = 255;
				lpDst++;
				*lpDst = 255;
				lpDst++;
				*lpDst = 0;
				break;
	}

	case 5: // 256���Ҷȼ���ɫ�帳ֵ:
	{
				lpDst = (BYTE *)biHdr + sizeof(BITMAPINFOHEADER);
				for (int i = 0; i < 256; i++)
				{
					lpDst[i * 4] = i;
					lpDst[i * 4 + 1] = i;
					lpDst[i * 4 + 2] = i;
					lpDst[i * 4 + 3] = 0;
				}
				break;
	}
	default:
	{
			   GlobalUnlock(hDIB);
			   //strcpy(gszLastError,"error C1002: not support this image format");
			   return NULL;
	}
	}
	GlobalUnlock(hDIB);
	return hDIB;
}

// ͼ��ת���ɾ��
HANDLE Mat2Dib(Mat img)
{
	if (img.empty())
	{
		return NULL;
	}
	Mat tempImg;

	if (img.channels() == 3)
	{
		tempImg = img.clone();

	}
	else if (img.channels() == 1)
	{
		cvtColor(img, tempImg, CV_GRAY2BGR);
	}

	int nWidthStep = tempImg.step1();
	int nHeight = tempImg.rows;
	int nWidth = tempImg.cols;
	int nDibWidthStep = (((24 * nWidth) + 31) / 32) * 4;

	HANDLE hDib = (HANDLE)SunCreateHandle(nWidth, nHeight, 1);
	LPBITMAPINFOHEADER  lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDib);

	LPBYTE lpDibData = (LPBYTE)lpbi + sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(lpbi->biClrUsed);

	LPBYTE lpImgData = (LPBYTE)tempImg.data;

	//unsigned char* lpImgData = tempImg.data;
	//unsigned char* lpDibData = (unsigned char*)lpbi + sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(lpbi->biClrUsed);

	for (int y = 0; y<nHeight; y++) //ע��IplImage�����ϣ���HDIB(����)������ԭ�㲻ͬ
	{
		memcpy(lpDibData + (nHeight - 1 - y)*nDibWidthStep, lpImgData + y*nWidthStep, nWidthStep);
	}

	//memcpy(lpDibData, lpImgData, lpbi->biSizeImage);

	::GlobalUnlock(hDib);
	return hDib;
}

// ���ת����ͼ��
Mat Dib2Mat(HANDLE hDib)
{
	Mat dst(Size(100, 50), CV_8UC1);

	if (hDib == NULL)
	{
		return dst;
	}

	LPBITMAPINFOHEADER  lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDib);

	if (lpbi == NULL)
	{
		return dst;
	}

	int nWidth = lpbi->biWidth;
	int nHeight = lpbi->biHeight;
	int nChannels = lpbi->biBitCount / 8;
	int nDepth = 8;

	//cout<<nWidth<<","<<nHeight<<endl;
	if (nWidth > 10000 || nHeight > 10000)
	{
		return dst;
	}

	dst.create(Size(nWidth, nHeight), CV_MAKE_TYPE(nDepth, nChannels));

	unsigned char* pDstData = (unsigned char*)dst.data;
	unsigned char* pDibData = (unsigned char*)lpbi + sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(lpbi->biClrUsed);

	int nWidthStep = dst.step1();
	int nDibWidthStep = (((24 * nWidth) + 31) / 32) * 4;

	for (int y = 0; y<nHeight; y++)
	{
		memcpy(pDstData + y*nWidthStep, pDibData + (nHeight - 1 - y)*nDibWidthStep,
			nWidthStep);
	}

	::GlobalUnlock(hDib);
	return dst;
}

/**
* @name  getAllImgsFilePath
* @brief ��ȡָ��·���µ������ļ�ȫ·��
**/
int getAllImgsFilePath(char *szDir, vector<string>& testImgFiles)
{
	WIN32_FIND_DATA ffd;

	HANDLE hFind = FindFirstFile((szDir), &ffd);
	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		} else {
			string searchImgDirWithType = szDir;							//����Ŀ¼���������ļ����͵�			
			int npos = searchImgDirWithType.find_last_of("\\");			//
			string searchImgDir = searchImgDirWithType.substr(0, npos + 1);	//��Ŀ¼	//substr ����pos��ʼ��n���ַ���ɵ��ַ���
			string imgWholePath = searchImgDir + ffd.cFileName;
			testImgFiles.push_back(imgWholePath);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return 0;
}


Mat getHistImg(const Mat& hist)
{
	double maxVal = 0;
	double minVal = 0;

	//�ҵ�ֱ��ͼ�е����ֵ����Сֵ
	minMaxLoc(hist, &minVal, &maxVal, 0, 0);
	int histSize = hist.rows;
	Mat histImg(histSize, histSize, CV_8U, Scalar(255));
	// ��������ֵΪͼ��߶ȵ�90%
	int hpt = static_cast<int>(0.9*histSize);

	for (int h = 0; h<histSize; h++)
	{
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		cv::line(histImg, Point(h, histSize), Point(h, histSize - intensity), Scalar::all(0));
	}

	return histImg;
}

//@2017-09-18 ���ػҶ�ͼ��ֱ��ͼ���ֵ�� isfitToBinImgByThresh(CameraPro��Ŀ)�����ı� 
int getMostGrayValue(Mat& grayImg, double _RatioThresh = 0.8)
{
	if (grayImg.empty() || (grayImg.channels() != 1))
		return -1;

	//ͳ��ֱ��ͼ��Ϣ
#if 1
	Mat  hist;
	int narrays = 1;		//Number of source arrays
	int channels[] = { 0 }; // List of the dims channels used to compute the histogram	
	int dims = 1;			// �C Histogram dimensionality
	int bins = 256;
	int histSizes[] = { bins };  //Array of histogram sizes in each dimension.
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	calcHist(&grayImg, narrays, channels, Mat(), hist, dims, histSizes, &histRange, uniform, accumulate);

	//Mat histShowImg = getHistImg( hist);
	//imwrite("histShowImg.jpg",histShowImg);

#if _ShowImage
	Mat histShowImg = getHistImg(hist);
	imshow("histShowImg", histShowImg);
#endif
#endif

	//����ͳ�Ƶ�ֱ��ͼ��Ϣ �ж��ǲ����ʺ���ֱ��ͼ��ֵ��ֵ��
#if 1

	double maxVal;  //ֱ��ͼ�����ֵ  	 
	Point maxIdx(0, 0);//ֱ��ͼ���ֵ��������(�Ҷ�ֵ)
	minMaxLoc(hist, NULL, &maxVal, NULL, &maxIdx); //����ֱ��ͼ���ֵ  
	//cout<<"ֱ��ͼ maxVal: "<<maxVal<<endl;
	//cout<<"ratio 1 : "<<maxVal/(grayImg.rows*grayImg.cols)<<endl;
	//cout<<"ֱ��ͼ maxIdx: "<<maxIdx.x<<" "<<maxIdx.y<<endl;

	//ֱ��ͼ�����ֵ
	int mostGrayValue = maxIdx.y;
	//cout << "mostGrayValue : " << mostGrayValue << endl;
	//1.1) ֱ��ͼ�����ֵҪС��128����������ɫ�ģ�Ŀǰ����Ժڲ���Ϊ��
	//if (mostGrayValue>=128 || mostGrayValue<0)
	if (mostGrayValue>255 || mostGrayValue<0)
		return -1;

	//֮ǰ��ͳ�� ֱ��ͼ���ֵ����ռ��
#if 0

	//1.2) ֱ��ͼ�����ֵ����6���Ҷȷ�Χ��������Ŀ�� �������������75%��
	//int colorOffset = 6;	//ƫ��N������֮�ڶ����ɫ,Ӱ��
	int colorOffset = 10;	//ƫ��N������֮�ڶ����ɫ,Ӱ��
	int threshHighValue = mostGrayValue + colorOffset;
	int threshLowValue = mostGrayValue - colorOffset;

	if (threshHighValue>255)
		threshHighValue = 255;	//����255��255
	if (threshLowValue <0)
		threshLowValue = 0;		//С��0��0	 	 	 

	int histSize = hist.rows;
	float totalPixelsNum = 0.0;
	for (int h = threshLowValue; h <= threshHighValue; h++)
	{
		totalPixelsNum += hist.at<float>(h);
		//cout<<h<<" hist.at<float>(h)�� "<<hist.at<float>(h)<<endl;
	}

	cout << "totalPixelsNum ratio 1 : " << totalPixelsNum / (grayImg.rows*grayImg.cols) << endl;

	//1.2) ֱ��ͼ�����ֵ����6���Ҷȷ�Χ��������Ŀ�� �������������75%��
	double totalRatio = totalPixelsNum / (grayImg.rows*grayImg.cols);
	//@2017-7-18 22:03 ��ֱ��ͼ����6�����ض�95%��Ŷ�����ü��а���������ֵ��ߵ�0.8 �� 
	//@2017-7-19 08:18 ����75%�ɡ���ʵ��û��ô����ġ���
	//@2017-7-19 08:49 ����80%�ɣ���Ȼ��Щ�ڲ������ĸ���λ��־�Ŀ��ܱ���������
	//const double _RatioThresh = 0.3;
	if (totalRatio>_RatioThresh)
		return mostGrayValue;  //ֱ�ӷ���ֱ��ͼ�����ֵ���ڶ�ֵ����
	else
		return -1;

#endif

#endif

	return mostGrayValue;
}

//adaptiveResizeEndToEndChineseOCRImgs �������޸ģ������ߵĵط�ģ����
//@2017-11-01 RGB��ɫ�ռ�
int adaptiveResizeEndToEndChineseOCRImgsWithBlurBorder(Mat &img, Mat &newImage)
{
	char result[100] = { '\0' };

	bool findBugImg = false;

	int wLargerThanH_Num = 0;	//��߱ȹ������Ŀ

	//����3ͨ��
	if (img.empty() && (img.channels() != 3))
		return -1;

	//��ȡԭͼ��RGBÿ��ͨ����ֱ��ͼ�����ֵ
#if 1
	vector<Mat> matNchsRGB;
	split(img, matNchsRGB);
	Mat imgBluechannel = matNchsRGB[0].clone();
	Mat imgGreenchannel = matNchsRGB[1].clone();
	Mat imgRedchannel = matNchsRGB[2].clone();

	int blueMostGrayValue = getMostGrayValue(imgBluechannel);
	int greenMostGrayValue = getMostGrayValue(imgGreenchannel);
	int redMostGrayValue = getMostGrayValue(imgRedchannel);

	Scalar tempVal = mean(img);
	int blueMeanValue = (int)(tempVal.val[0] + 0.5);
	int greenMeanValue = (int)(tempVal.val[0] + 0.5);
	int redMeanValue = (int)(tempVal.val[0] + 0.5);


	//ĳ��ԭ��getMostGrayValue����-1 �Ͳ�������ȥ��
	if ((blueMostGrayValue == -1) || (greenMostGrayValue == -1) || (redMostGrayValue == -1))
		return -1; 

#endif

		//Mat adaptiveResizeImg;
		Mat tmpResizeImg;
		Mat adaptiveResizeImg;
		///�������ɵĶ���440*56��ͼ��
		////const int _OutputWidth = 440;
		////const int _OutputHeight = 56;
		////@2017-10-11 whb�ļ�ʻ֤��ʻ֤�ǵ�ַ�ֶε�ѵ������Ҫ368*48
		//const int _OutputWidth = 368;
		//const int _OutputHeight = 48;
		double _OutputWvsHratio = (double)_OutputWidth / double(_OutputHeight);

		//����Ӧ���Ź���
#if 1
		//��������Ƚ��ٵ�
		// ��߱ȱ�_OutputWvsHratio����˵��ͼ��̫����Ŷ��������䱳��ɫʹ�ﵽ��߱�440/56��(����ɫ:ֱ��ͼ�����ֵ��Ϊ����ɫ)
		if ((double)(img.cols) / (double)(img.rows) > _OutputWvsHratio)
		{
			//��ȱ��ֲ���
			int width = _OutputWidth;
			int height = (int)(img.rows * width / img.cols);
			resize(img, tmpResizeImg, Size(width, height), 0, 0, INTER_CUBIC);
			
			// ���
			Mat adaptiveResizeImgTmp(_OutputHeight, _OutputWidth, CV_8UC3, Scalar(blueMostGrayValue, greenMostGrayValue, redMostGrayValue));

			int addHeightOffset = (_OutputHeight - height) / 2;
			Rect cutRoi(0, addHeightOffset, width, height);
			Mat RoiImg = adaptiveResizeImgTmp(cutRoi);
			tmpResizeImg.copyTo(RoiImg);

			adaptiveResizeImg = adaptiveResizeImgTmp.clone();
			wLargerThanH_Num++;
		}
		else	// ͼ��̫���ˣ��������ұ�Ե ����������
		{

			//@2017-09-18 ��߱�С��440/56��������䱳��ɫʹ�ﵽ��߱�440/56��(����ɫ:ֱ��ͼ�����ֵ��Ϊ����ɫ)
#if 1
			//�߿��̫��������Ӧ�߶ȣ�����������
			int height = _OutputHeight;
			int width = (int)(img.cols * height / img.rows);
			resize(img, tmpResizeImg, Size(width, height), 0, 0, INTER_CUBIC);
			
			Mat adaptiveResizeImgTmp(_OutputHeight, _OutputWidth, CV_8UC3, Scalar(blueMostGrayValue, greenMostGrayValue, redMostGrayValue));

			int addOffset = (_OutputWidth - width) / 2;
			Rect cutRoi(addOffset, 0, width, height);
			Mat RoiImg = adaptiveResizeImgTmp(cutRoi);
			tmpResizeImg.copyTo(RoiImg);
			adaptiveResizeImg = adaptiveResizeImgTmp.clone();

			//������߽�ģ������
#if 1
			int blurRaduis = 3; //ģ���߽����ҷ�Χ blurKernelSize7 ���3����
			int blurRange = 2 * blurRaduis;
			int startX = addOffset - blurRaduis;
			if (startX < 0)
			{
				startX = 0;
				blurRange = blurRaduis;
			}

			Rect ROIBorderLeft(startX, 0, blurRange, _OutputHeight); //�ͱ߽總��ģ��
			//Rect ROIBorderLeft(0,0,addOffset+blurRaduis,_OutputHeight);
			Mat ROIImgBorderLeft = adaptiveResizeImg(ROIBorderLeft);
			int blurKernelSize = 5;
			GaussianBlur(ROIImgBorderLeft, ROIImgBorderLeft, Size(blurKernelSize, blurKernelSize), 1.0);

#endif

			//�����ұ߽�ģ������

#if 1
			blurRange = 2 * blurRaduis;
			int borderRightStartX = _OutputWidth - addOffset - blurRaduis;

			//����ͼ���ұ߽��˴�����
			if ((borderRightStartX + blurRange)> _OutputWidth)
				blurRange = _OutputWidth - borderRightStartX - 1;

			Rect ROIBorderRight(borderRightStartX, 0, blurRange, _OutputHeight); //�ͱ߽總��ģ��
			Mat ROIImgBorderRight = adaptiveResizeImg(ROIBorderRight);
			GaussianBlur(ROIImgBorderRight, ROIImgBorderRight, Size(blurKernelSize, blurKernelSize), 1.0);

#endif

#endif
		}

#endif

	if (!adaptiveResizeImg.empty())
	{
		//һ��ʼԭͼ�Ѿ����ȹ�һ�����˾�����
		normalize(adaptiveResizeImg, newImage, 255, 0, NORM_MINMAX);
	}

	return 0;
}


vector<char> readFile(const char* file){
	vector<char> data;
	FILE* f = fopen(file, "rb");
	if (!f) return data;

	int len = 0;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (len > 0){
		data.resize(len);
		fread(&data[0], 1, len, f);
	}
	fclose(f);
	return data;
}

int readFileNorm(const char* file, vector<char> &data) {
	
	cv::Mat src = imread(file);

	if (src.empty()) {
		printf("�ļ�������ô�� %s\n", file);
		return -1;
	}

	cv::Mat img;
	// �߽�Ԥ����
	adaptiveResizeEndToEndChineseOCRImgsWithBlurBorder(src, img);

	// ת��Ϊcaffe�ڲ�ʹ�õ����ݸ�ʽ
	vector<uchar> udata;
	cv::imencode(".jpg", img, udata);
	for (size_t i = 0; i < udata.size(); i++)
	{
		char c = (char)udata[i];
		data.push_back(c);
	}

	return 0;
}

int readFileNorm(Mat &src, vector<char> &data) {
	cv::Mat img;
	// �߽�Ԥ����
	adaptiveResizeEndToEndChineseOCRImgsWithBlurBorder(src, img);

	// ת��Ϊcaffe�ڲ�ʹ�õ����ݸ�ʽ
	vector<uchar> udata;
	cv::imencode(".jpg", img, udata);
	for (size_t i = 0; i < udata.size(); i++)
	{
		char c = (char)udata[i];
		data.push_back(c);
	}

	return 0;
}

vector<string> loadCodeMap(const char* file){
	ifstream infile(file);
	string line;
	vector<string> out;
	//while (std::getline(infile, line)){
	//	out.push_back(line);
	//}
	while (infile >> line){
		out.push_back(line);
	}
	return out;
}

string getLabel(const vector<string>& labelMap, int index) {
	if (index < 0 || index >= labelMap.size())
		return "*";

	return labelMap[index];
}

void main()
{
	//loadDeepLearningModel("deploy.prototxt", "model.caffemodel");
	//loadDeepLearningModel("Plate_Mean_MultiFonts_LSTMV2_DataV5_V2_deploy.prototxt", "Plate_Mean_MultiFonts_LSTMV2_DataV5_V2_iter_170000.caffemodel");
	loadDeepLearningModel("..\\test\\model\\DigitalCharacter_Mean_MultiFonts_LSTMV2_DataV5_V1_deploy.prototxt", "..\\test\\model\\DigitalCharacter_Mean_MultiFonts_LSTMV2_DataV5_V1_iter_705000.caffemodel");
	clock_t start_time = clock();
	clock_t end_time = clock();
	double sumCostTime = 0.0;
	for (int i = 0; i < 100; i++)
	{
		start_time = clock();

		char *result = (char*)malloc(50 * sizeof(char));
		double *probability = (double*)malloc(50 * sizeof(double)); 
		int *x_coordinate = (int*)malloc(50 * sizeof(int));
		//int num = digitalCharacterRecongnition("..\\test\\Img\\kk 001 - ����.jpg", result, probability, x_coordinate);
		int num = digitalCharacterRecongnitionBYHandle("..\\test\\Img\\kk 001 - ����.jpg", result, probability, x_coordinate);
		printf("%s\n", result);
		for (int m = 0; m < num; m++)
		{
			cout << "��" << m + 1 << "���ַ��ĸ���: " << probability[m] << endl;
		}
		free(x_coordinate);
		free(probability);
		free(result);

		end_time = clock();
		sumCostTime += (double)(end_time - start_time);
	}
	std::cout << "ƽ��ÿ������ʱ�䣺 " << sumCostTime / (100) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
	releaseDeepLearningModel();

	return;
}

int RecongnitionCore(vector<char> &data, char *result, double *probability, int *x_coordinate)
{
	forward(g_classifier, &data[0], data.size());

	BlobData* premuted_fc = getBlobData(g_classifier, "premuted_fc");

	float* ptr = premuted_fc->list;
	int prev_label = g_blank_label;
	string result_, result_raw;

	int char_num = 0;
	for (int i = 0; i < g_time_step; ++i){
		float* lin = ptr + i * g_alphabet_size;
		int predict_label = std::max_element(lin, lin + g_alphabet_size) - lin;

		if (PROBABILITYCOUT) {	// �Ƿ���е����ַ��ĸ������
			cout << i + 1 << " ";
			for (int indx = 0; indx < g_alphabet_size; indx++) {
				if (*(lin + indx) > 0.01) {
					cout << indx << " : " << *(lin + indx) << "    ";
				}
			}
			cout << endl;
			float value = lin[predict_label];
		}

		if (predict_label != g_blank_label && predict_label != prev_label){
			result_ = result_ + getLabel(g_labelMap, predict_label);
			probability[char_num] = lin[predict_label];
			x_coordinate[char_num] = i * 8 + 4;
			char_num++;
		}

		result_raw = result_raw + getLabel(g_labelMap, predict_label);
		prev_label = predict_label;
	}

	releaseBlobData(premuted_fc);

	int g;
	for (g = 0; g < result_.length(); g++) {
		result[g] = result_[g];
	}
	result[g] = '\0';

	return char_num;
}

SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int digitalCharacterRecongnition(const char*szImgPath, char *result, double *probability, int *x_coordinate)
{
	vector<char> data;
	int label = readFileNorm(szImgPath, data);

	if (label != 0) {
		printf("�ļ�������ô?");
		return -1;
	}

	int char_num = 0;
	char_num = RecongnitionCore(data, result, probability, x_coordinate);

	// ��֤x�������ȷ��
#if 0
	// δ��������ڼ���x�����ʱ�򣬵�һ����ƫ������о���ģ�ͳ������������ָ��ģ������ȷ��
	Mat imgTemp;
	adaptiveResizeEndToEndChineseOCRImgsWithBlurBorder(szImgPath, imgTemp);
	for (size_t i = 0; i < char_num; i++)
	{
		line(imgTemp, Point(x_coordinate[i], 0), Point(x_coordinate[i], imgTemp.cols), Scalar(0, 0, 0), 2);
	}
	imshow("111", imgTemp);
	waitKey(0);

	////��������Ƚ��ٵ�
	//// ��߱ȱ�_OutputWvsHratio����˵��ͼ��̫����Ŷ��������䱳��ɫʹ�ﵽ��߱�440/56��(����ɫ:ֱ��ͼ�����ֵ��Ϊ����ɫ)
	//if ((double)(img.cols) / (double)(img.rows) > _OutputWvsHratio)
	//{
	//	//��ȱ��ֲ���
	//}
	//else	// ͼ��̫���ˣ��������ұ�Ե ����������
	//{

	//	//@2017-09-18 ��߱�С��440/56��������䱳��ɫʹ�ﵽ��߱�440/56��(����ɫ:ֱ��ͼ�����ֵ��Ϊ����ɫ)

	//}

#endif

	return char_num;
}

SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int digitalCharacterRecongnitionBYHandle(HANDLE hDib, char *result, double *probability, int *x_coordinate)
{
	Mat src;

	// ���Ծ������
	if (1)
	{
		Mat img = imread("..\\test\\Img\\kk 001 - ����.jpg");
		HANDLE handle = Mat2Dib(img);
		src = Dib2Mat(handle);
	}
	else{
		src = Dib2Mat(hDib);
	}

	vector<char> data;
	int label = readFileNorm(src, data);

	if (label != 0) {
		printf("�ļ�������ô?");
		return -1;
	}

	int char_num = 0;
	char_num = RecongnitionCore(data, result, probability, x_coordinate);

	return char_num;
}

SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int loadDeepLearningModel(const char* prototxtFile, const char* modelFile)
{
	if (_loadDeepLearningModelFlag)
		return 0;

	//��ֹcaffe�����Ϣ
	disableErrorOutput();

	g_classifier = createClassifier(prototxtFile,
		modelFile,
		g_scale_raw,
		0,
		g_num_means,
		g_means);

	// ��ȡ��ǩ������ӵ�g_labelMap��
	for (int m = 0; m < g_label.length(); m++) {
		string tmp;
		
		if (g_label[m] < 0) {
			tmp = "��";
			tmp[0] = g_label[m];
			tmp[1] = g_label[m + 1];
			m++;
		} else {
			tmp = g_label[m];
		}

		g_labelMap.push_back(tmp);
	}

	// ��ʾ
	//for (int i = 0; i < g_labelMap.size(); i++)
	//	cout << g_labelMap[i] << endl;

	int retValue = -1;
	if (g_classifier != NULL) {
		retValue = 0;
		_loadDeepLearningModelFlag = true;
	}
	else {
		retValue = -1;
		_loadDeepLearningModelFlag = false;
	}

	return retValue;
}

SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int releaseDeepLearningModel()
{
	int retValue = -1;

	if (_loadDeepLearningModelFlag) {
		_loadDeepLearningModelFlag = false;
		releaseClassifier(g_classifier);
		retValue = 0;
	}
	else {
		retValue = -1;
	}

	return retValue;
}