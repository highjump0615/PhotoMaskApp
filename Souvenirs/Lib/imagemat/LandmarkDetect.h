// #include "Comon.h"
// #include "faceapi.h"
// #include "structures.h"
#include "MainProc.h"
#ifndef _LANDMARKDETECT_
#define _LANDMARKDETECT_
#define CNT_USING_LANDMARK 14


typedef struct HNLAttributes_N_{
public:
	int			off_0;
	BYTE*		pbyPoseImg;
	BYTE*		pbyIlluminationImg;
	SRect		faceRect;
	float		dblYaw;
	float		dblRoll;
	float		dblPitch;
	WORD		leftEyeX;
	WORD		leftEyeY;
	signed char		off_84;
	WORD		rightEyeX;
	signed char		off_8C;
	WORD		rightEyeY;
	BYTE*		off_C8;
	BYTE*		off_CC;
	int*		off_D0;
	int*		off_D4;
}FACE_ATTRIBUTE, *LPFACE_ATTRIBUTE;

typedef struct {
	SPoint	leftEye;
	double		leftProperty;
	SPoint	rightEye;
	double		rightProperty;
} EYES_POINT;

typedef struct _ACTIVE_POINT{
	short index;
	short x;
	short y;
	BYTE active;
}ACTIVE_POINT, *LPACTIVE_POINT;

typedef struct {
	float x;
	float y;
} POINT_FLOAT;
typedef struct _RCPR_38_4{
	int nCnt_0;
	float* pflt_4;
	short* psh_8;
	short* psh_C;
	float* pflt_10;
}RCPR_38_4, *LPRCPR_38_4;

typedef struct _RCPR_38{
	int nCntStu_4;
	LPRCPR_38_4 pstu_4;
	int n_8;
	int n_C;
	short* psh_10;
	short* psh_14;
	float* pflt_18;
}RCPR_38, *LPRCPR_38;

typedef struct _RCPRMODEL{
	int nCntLandmark_24;
	int n_28;
	float flt_2C;
	float* pflt_30;
	int nCntStu_38;
	LPRCPR_38 pstu_38;//The array count is 10.
}RCPRMODEL, *LPRCPRMODEL;


int InitRCPRModel(char* path);
int ReleaseRCPRModel();
int LandMarkExtract(BYTE* pImage, int imgWidth, int imgHeight, SRect faceRect, SFaceStru* pFaceStru);
///////////////////////////////////////////////////////////////////////////////////////////
// double GetRotaionAngle(FPoint LEye, FPoint REye);
// 
// int GetLeftRightvalue(int dis1, int dis2);
// int GetLeftRightvalue(int ang);
// 
// void Select_BestFaces(EnrollItem *pFeatureArray, int nRegCnt, int &nChooseCnt, int* pSelIndex);
// void Select_BestPoses(EnrollItem *pFeatureArray, int* pSelIndex);
// ////////////////////////////////////////////////////////////////
// 
// int FaceAPI_DetectFaceForCamera(BYTE* i_pImage, int i_nWidth, int i_nHeight, int i_nBitCnt, SRect& i_ROIRect, SFaceInfo* o_pFaceInfo);
// int FaceAPI_DetectFaceForCameraMultiPerson(BYTE* i_pImage, int i_nWidth, int i_nHeight, int i_nBitCnt, SRect& i_ROIRect, SFaceInfo* o_pFaceInfo);
// int FaceAPI_DetectFaceForImage(BYTE* i_pImage, int i_nWidth, int i_nHeight, int i_nBitCnt, SFaceInfo* o_pFaceInfo);
// int FaceAPI_DetectLandMark(BYTE* i_pImage, int i_nWidth, int i_nHeight, SFaceInfo* io_pFaceInfo);
// int FaceAPI_NormalizeFace(BYTE* i_pImage, int i_nWidth, int i_nHeight, SFaceStru* o_pFaceStru, BYTE* o_pNormalizeFace, int i_nNorWidth, int i_nNorHeight, SPoint* o_pFeatPoint);
// int FaceAPI_ExtractFromNormalFace(BYTE* i_pNormalizeFace, SPoint* i_pFeatNum, EncodeGaborFeature* o_lpFeature);
// int	FaceAPI_ExtractFeatureForImage(BYTE* i_pImage, int i_nWidth, int i_nHeight, int i_nBitCnt, EncodeGaborFeature* o_pFeatures,SFaceInfo* o_pFaceInfo);
// int FaceAPI_ExtractDeepFeature(BYTE* i_pImage, int i_nWidth, int i_nHeight, SFaceStru* o_pFaceStru, EncodeGaborFeature* o_lpFeature);
// int FaceAPI_LivenessDetect(BYTE* i_pImage, int i_nWidth, int i_nHeight, SFaceInfo* i_pFaceInfo);
// 
// int FaceAPI_GetRegisterNum(TCHAR* i_pRegDataPath);
// int FaceAPI_ReadAllRegisterData(TCHAR *i_strRegDataPath, EnrollItem *o_pRegItem);
// int FaceAPI_AddOneRegisterData(TCHAR *i_pRegDataPath, EnrollItem *i_pRegItem);
// 
// int	FaceAPI_MatchOneToNForCamera(EncodeGaborFeature *i_pEnGaborFeat, EnrollItem *i_pRegItem, int i_nRegItems, float fThreshold, int *o_nRegID, float *o_fMatchSim);
// int	FaceAPI_MatchOneToNForImage(BYTE *i_pFeature, EnrollItem* i_pRegItem, int i_nRegItems, int *o_nRegID, float *o_fMatchSim);
// float FaceAPI_MatchOneToOne(BYTE* i_RegFeature, BYTE* i_MatchFeature);
// 
// int FaceAPI_EnrollPreprocForCamera(
// 	BYTE*			i_pImage,
// 	int				i_nWidth,
// 	int				i_nHeight,
// 	int				i_nBitCnt,
// 	SRect&		i_ROIRect,
// 	SFaceInfo*		o_pFaceInfo,
// 	BYTE			*o_pGrayImage[MAX_ENROLL_CAND_POSE_NUM],
// 	BYTE			o_pNormalImage[MAX_ENROLL_CAND_POSE_NUM][NORMAL_FACE_WIDTH*NORMAL_FACE_HEIGHT],
// 	int*			io_nFrmCount,	
// 	int*			io_nRegCount,
// 	SPoint		o_FeaturePoint[MAX_ENROLL_CAND_POSE_NUM][GRID_NUM - 2]
// );
// 
// 
// int	FaceAPI_EnrollProcForCamera( 
// 	BYTE			*i_pGrayImage[MAX_ENROLL_CAND_POSE_NUM],
// 	int				i_nWidth,
// 	int				i_nHeight,
// 	BYTE			i_pNormalImage[MAX_ENROLL_CAND_POSE_NUM][NORMAL_FACE_WIDTH*NORMAL_FACE_HEIGHT],
// 	SFaceInfo		i_pFaceInfo[MAX_ENROLL_CAND_POSE_NUM],
// 	SPoint		i_FeaturePoint[MAX_ENROLL_CAND_POSE_NUM][GRID_NUM - 2],
// 	int				i_nRegCnt,
// 	int				o_pBestIndex[MAX_ENROLL_MULTI_NUM],
// 	EnrollItem*		o_lpEnrollFeature
// 	);
// 
// int FaceAPI_MatchPreprocForCamera(
// 	BYTE*			i_pImage,
// 	int				i_nWidth,
// 	int				i_nHeight,
// 	int				i_nBitCnt,
// 	SRect&		i_ROIRect,
// 	SFaceInfo*		o_pFaceInfo,
// 	BYTE			o_pNormalImage[NORMAL_FACE_WIDTH*NORMAL_FACE_HEIGHT],
// 	SPoint		o_FeaturePoint[GRID_NUM - 2]
// );
// 
// int FaceAPI_MatchPreprocForCameraMutliPerson(
// 	BYTE*			i_pImage,
// 	int				i_nWidth,
// 	int				i_nHeight,
// 	int				i_nBitCnt,
// 	SRect&		i_ROIRect,
// 	SFaceInfo*		o_pFaceInfo,
// 	BYTE*			o_pGrayImage
// 	);
// 
// int	FaceAPI_MatchProcForCamera( 
// 	BYTE*			i_pGrayImage,
// 	SFaceStru*		i_pFaceStru,
// 	int				i_nWidth,
// 	int				i_nHeight,
// 	EncodeGaborFeature* o_pFeatures
// 	);

#endif
