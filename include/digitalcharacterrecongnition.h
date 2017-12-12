#pragma once

#ifndef SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API
#define SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API
#endif

#include <windows.h>

/**
// !ʹ�����ӣ�
	loadDeepLearningModel("deploy.prototxt", "model.caffemodel");
	for (int i = 0; i < 10; i++)
	{
		char *result = (char*)malloc(50 * sizeof(char));
		double *probability = (double*)malloc(50 * sizeof(double));
		int *x_coordinate = (int*)malloc(50 * sizeof(int));
		int num = digitalCharacterRecongnition("038_ԥA888AQ.jpg", result, probability, x_coordinate);
		printf("%s\n", result);
		for (int m = 0; m < num; m++)
		{
			cout << "��" << m + 1 << "���ַ��ĸ���: " << probability[m] << endl;
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
	* @brief  ��ʼ��
	* @return //   0	�ɹ�
			  //  -1    ʧ��
	**/
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int loadDeepLearningModel(const char* prototxtFile, const char* modelFile);

	/**
	* @name   digitalCharacterRecongnition/digitalCharacterRecongnitionBYHandle
	* @brief  ���ֺ���ĸ���ʶ��
	* @param[in]	ͼ��·��/�������
	* @param[out]	ʶ����
	* @param[out]	����
	* @param[out]	�ַ���X������
	* @return //  num   �ַ�����
			  //  -1    ʧ��
	**/
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int digitalCharacterRecongnition(const char*szImgPath, char *result, double *probability, int *x_coordinate);
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int digitalCharacterRecongnitionBYHandle(HANDLE hDib, char *result, double *probability, int *x_coordinate);

	/**
	* @name   releaseDeepLearningModel
	* @brief  �ͷ�
	* @return //  0		�ɹ�
			  //  -1    ʧ��
	**/
	SUNYARD_DIGIATAL_CHARACTER_RECONGNITION_API int releaseDeepLearningModel();

#ifdef __cplusplus
}
#endif 