using namespace std;
#include <vector>
#include "MainProc.h"


#define MAX_FACE_PASS_CNT					3	//If the number of detected faces as of one person is less than this value abandon that.
#define CNT_DETECT_STEP						3
#define MAX_IMG_WIDTH						1600
#define MAX_IMG_HEIGHT						1600
#define MAX_ORGINIMG_WIDTH					5000
#define MAX_ORGINIMG_HEIGHT					5000
#define MAX_IMG_SIZE						(MAX_IMG_WIDTH * MAX_IMG_HEIGHT)
#define FIRST_SHRINK_RATE					1.25f
#define BORDER_WIDTH						8
#define BORDER_HEIGHT						8
#define SCAN_WIDTH							28
#define SCAN_HEIGHT							28
#define LIMIT_OF_SHRINK_RATE				83333.33f
#define SHRINK_COEFFICIENT					1.1892100572586060000f
#define WEIGHT_COEFFICIENT					0.8
#define FIRST_THRESHOLD						50.0
#define INTEGRAL_AVG_COEFF					0.00127551017794758f
#define STANDARD_CX							600//1600

#define mac_flt_50							0.3
#define mac_flt_58							0.349999994039536
#define mac_flt_5C							0.6

#define STEP_CNT0_0	10
#define STEP_CNT0_1	3
#define STEP_CNT0_2	2

#define STEP_CNT1_0	16
#define STEP_CNT1_1	6
#define STEP_CNT1_2	4

typedef struct _stu_detected_info{//0x38
	float fltX;
	float fltY;
	float fltScale;
	float flt_20;
	float flt_24;
}STU_DETECTED_INFO, *LPSTU_DETECTED_INFO;

typedef struct _stu_pattern0{//size=0x20
	int n_0;
	char byPosX;
	char byPosY;
	char byCx;
	char byCy;
	char nType;
	char by_9;
	WORD wp;
	signed char pby_C[16];
	int n_1C;
}STU_PATTERN0, *LPSTU_PATTERN0;

typedef struct _stu_filter0{
	float fltFilterCoeff;
	float fltFilterLimit;
	WORD* pwCntStepPattern;
	float* pfltStepThreshold;
	int nStepCnt;
	LPSTU_PATTERN0 pstuPattern;
}STU_FILTER0, *LPSTU_FILTER0;

typedef struct _stu_pattern2{//0x34
	int n_0;
	char byX;
	char byY;
	char byWidth;
	char byHeight;
	char by_8;
	char by_9;
	char by_A;
	char by_B;
	float flt_C;
	float flt_10;
	unsigned char puby_14[16];
	signed char pby_24[16];
}STU_PATTERN2, *LPSTU_PATTERN2;

typedef struct _stu_filter2{//
	float flt_10;
	int nPatternSize;
	WORD* pwCntStepPattern;
	float* pfltFloorThreshold;
	float* pfltCeilThreshold;
	float* pflt_B0;//[8]
	int nCntStep;
	LPSTU_PATTERN2 pstuPattern;
}STU_FILTER2, *LPSTU_FILTER2;

typedef struct _stu_pattern1{//0x4C
	unsigned char ubyPosX_4;
	unsigned char ubyPosY_5;
	unsigned char ubyCx;
	unsigned char ubyCy;
	WORD w_8;
	signed char pbyFilter_A[25];
	float flt_24;
	float flt_28;
	unsigned char puby_2C[32];
}STU_PATTERN1, *LPSTU_PATTERN1;

typedef struct _stu_filter1{
	float flt_10;//0.00999999977648258
	WORD* pwCntStepPattern;
	int nStepCnt;
	float* pfltStepThreshold;
	LPSTU_PATTERN1 pstuPattern;
}STU_FILTER1, *LPSTU_FILTER1;

typedef struct _stu_eval_result{
	float fltX;
	int nIsPassed;
	float fltY;
}EVAL_RESULTS, *LPEVAL_RESULTS;

typedef struct detector_state{
	int piPassedNo[3];
	int nCntPassed;
}STU_DETECTOR_STATE, *LPSTU_DETECTOR_STATE;

typedef struct _stu_flt_pos{
	float fltX;
	float fltY;
}STU_FLT_POS, *LPSTU_FLT_POS;

typedef vector<LPSTU_DETECTED_INFO> ARR_DETECTED_INFO, *LPARR_DETECTED_INFO;

//struct for Stretch
typedef struct _stu_A428E0_arg_8{
	int n_0;
	int n_4;
	int n_8;
	int n_C;
}STU_A428E0_ARG_8, *LPSTU_A428E0_ARG_8;
typedef struct _stu_dst_Img{
	int cx;
	int stride;
	int cy;
	BYTE* pDstImg;
}STU_DST_IMG, *LPSTU_DST_IMG;
typedef struct _stu_filter{
	int n_4;
	char by_34;
	WORD* wFilter;
	int n_58;
	BYTE* pTmpRGBAImg;
	int n_60;
	int* pInt_64;
	signed char by_68;
	signed char by_69;
	int nDx;/*=0*/
	int cx;
	int cy;
	int n_7C;
	LPSTU_DST_IMG pstu_dstImg;
	int cx1;
	int nPerLen;
	int iy;
	int Index;
	WORD* hFilter;
}STU_FILTER, *LPSTU_FILTER;
typedef struct _stu_filter_array{
	WORD* FilterArray;
	int nLen;
}STU_FILTERARRAY, *LPSTU_FILTERARRAY;
typedef struct _stu_filter_mode{
	float c1;
	float b1;
	float a1;
	float d2;
	float c2;
	float b2;
	float a2;
	int nFilterType;
	float fRatio;
}STU_FILTERMODE, *LPSTU_FILTERMODE;

typedef struct _NRECT NRECT, *LPNRECT;
struct _NRECT{
	union{
		int x;
		int l;
	};
	union{ 
		int y;
		int t;
	};
	union{
		int w;
		int r;
	};
	union{
		int h;
		int b;
	};
	LPNRECT pNext;
};

int		EvalSecond(LPSTU_DETECTOR_STATE pstuDetectorState, int* pnIntegralImg, int* pnSqrIntegralImg, int nX, int nY, LPARR_DETECTED_INFO pArrDetectedInfo);
int		PackResult(LPEVAL_RESULTS arg_ebx, float* arg_edx, int nX, int nY, LPARR_DETECTED_INFO parrDetectedInfo);
int		Eval1thHaarFeature2(LPSTU_FILTER0 arg_ecx, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS arg_4);
int		EvalSecondFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS arg_4);
int		MakeIntegralImg();
int		ShrinkImage();
int		ZoomOutTwice();
bool	CreateWorkImage();
int		WeightedAverage(LPNRECT pOutRect, ARR_DETECTED_INFO pDetectedRectArr, int* pnSortIndex, int nOffset, int nNum);
void	PreProc(char* pbyImg, int nImgWidth, int nImgHeight);
int		IsLagerThanScanRect();
void	ChannelDetector(int arg_eax, int arg_0);
int		EvalFirst(LPSTU_DETECTOR_STATE pstuDetectState, int* pnIntegralImg, int* pnSqrIntegralImg);
void	Choose2thStepData(LPSTU_DETECTOR_STATE arg_ebx, LPSTU_DETECTOR_STATE arg_0);
int		ClassifyDetectedRects(ARR_DETECTED_INFO pstuDetectedFaceArr, float fltAreaCoeff, int nScanWidth, int nScanHeight, int** ppnSortNo, int** ppnCntSameClass);
void	CorrectIntegralImg(int arg_4);
int		EvalFirstFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, float arg_4);
int		Eval1thHaarFeature1(LPSTU_FILTER0 arg_ecx, int* pnIntegralImg, float arg_4, LPEVAL_RESULTS arg_8);
int		Eval2ndHaarFeature(LPSTU_FILTER1 arg_ecx, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS arg_4);
int		Eval3rdHaarFeature(LPSTU_FILTER2 arg_ecx, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS arg_4);
int		ExtractFeatureHaar(int arg_eax, LPSTU_PATTERN0 arg_edx, int* arg_0);
bool	IsAvaiableData(float* arg_ecx, float* pflt1, float arg_C);
int     LoadFirstStepData(char *sFilePath);
int     LoadSecondStepData(char *sFilePath);
float	Extract3thHaarFeature(LPSTU_PATTERN2 arg_0, int* arg_edx, int* arg_4, int arg_8, int arg_C, float arg_10, float arg_14);
int		FitImage(BYTE* imgIn, int cxIn, int cyIn, BYTE* imgOut, int cxOut, int cyOut, double flt_Scale);
int		DetectFace_100E5810(char* pbyImg, int nImgWidth, int nImgHeight, LPNRECT *ppOutRect, double fltScale);
WORD*	GetFilterData(int pwidth, int pheight, double flt_Scale, int order);
int		StretchImage_W(LPSTU_FILTER arg_0, BYTE* pRGBAImg, LPSTU_A428E0_ARG_8 arg_8);
void*	sub_97C810(void* arg_0, int size_t);
int		MakeFilter_A3F660(LPSTU_FILTERARRAY arg_esi, LPSTU_FILTERMODE arg_0, int arg_4, int nLenOrigin, int nStartVal, int nLenNormal, float fRatio, int fBias);
void	sub_A3F5B0(LPSTU_FILTERMODE arg_eax, float arg_0, float arg_4);
char*	sub_9BD2E0(int arg_eax, LPSTU_FILTER arg_edx);
void	StretchImage_H(LPSTU_FILTER pstuFilter);
BYTE*	RGBA2Gray(BYTE* pRGBAImg, int cx, int cy);
BYTE*	RGB2Gray(BYTE* pRGBImg, int cx, int cy);

void	FreeRectList(LPNRECT* pRectRoot);
int		DetectFaces(BYTE* pRGBImg, int nImgWidth, int nImgHeight, int nBitCount, LPNRECT *ppOutRect);
int     InitFaceDetector(char *sFilePath);
void	ReleaseFaceDetector();

void ImageStretch(BYTE* imgIn, int cxIn, int cyIn, BYTE* imgOut, int cxOut, int cyOut);
