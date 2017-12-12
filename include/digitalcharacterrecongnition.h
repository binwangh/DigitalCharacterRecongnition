#pragma once

#ifndef SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API
#define SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API
#endif

#include <windows.h>

/**
// !使用例子：
	loadDeepLearningModel("deploy.prototxt", "model.caffemodel");
	for (int i = 0; i < 10; i++)
	{
		char *result = (char*)malloc(50 * sizeof(char));
		double *probability = (double*)malloc(50 * sizeof(double));
		int *x_coordinate = (int*)malloc(50 * sizeof(int));
		int num = digitalCharacterRecongnition("038_豫A888AQ.jpg", result, probability, x_coordinate);
		printf("%s\n", result);
		for (int m = 0; m < num; m++)
		{
			cout << "第" << m + 1 << "个字符的概率: " << probability[m] << endl;
		}
		free(x_coordinate);
		free(probability);
		free(result);
	}
	releaseDeepLearningModel();
**/

#ifdef __cplusplus
extern "C" {
#endif 

	/**
	* @name   loadDeepLearningModel
	* @brief  初始化
	* @return //   0	成功
			  //  -1    失败
	**/
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int loadDeepLearningModel(const char* prototxtFile, const char* modelFile);

	/**
	* @name   digitalCharacterRecongnition/digitalCharacterRecongnitionBYHandle
	* @brief  数字和字母混合识别
	* @param[in]	图像路径/句柄参数
	* @param[out]	识别结果
	* @param[out]	概率
	* @param[out]	字符的X轴坐标
	* @return //  num   字符个数
			  //  -1    失败
	**/
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int digitalCharacterRecongnition(const char*szImgPath, char *result, double *probability, int *x_coordinate);
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int digitalCharacterRecongnitionBYHandle(HANDLE hDib, char *result, double *probability, int *x_coordinate);

	/**
	* @name   releaseDeepLearningModel
	* @brief  释放
	* @return //  0		成功
			  //  -1    失败
	**/
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int releaseDeepLearningModel();

#ifdef __cplusplus
}
#endif 