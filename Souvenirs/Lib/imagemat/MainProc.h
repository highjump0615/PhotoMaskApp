#pragma once
#include <vector>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define BYTE                    unsigned char
#define BOOL                    bool
#define TRUE                    true
#define FALSE                   false
#define WORD unsigned short
#define DWORD unsigned int
#define BYTE unsigned char

#define SafeMemFree(x)              {if(x != NULL) {free(x); x= NULL; }}

typedef BYTE					FT_TYPE;

#define LANDMARK_COUNT		68

struct ItemSort {
	int colIndex;
	int oldIndex;
};

typedef struct SRect {
	int x;
	int y;
	int width;
	int height;
} SRect;

typedef struct SPoint {
	int x;
	int y;
} SPoint;
typedef struct SFaceStru {
	SRect	  rcFace;							// face region
	SPoint    ptCord[LANDMARK_COUNT];
} SFaceStru;

typedef struct CFaceInfo 
{
	int			nFaces;						//	face num
	std::vector<SFaceStru> StruProFace;				//	face structure
	std::vector<SFaceStru> StruFace;				//	face structure
} CFaceInfo;
typedef struct CVBPoint
{
	int x;
	int y;

} CVBPoint;

void* read_file(char* path, int nSize);
int InitImage(BYTE* pImage, int nWidth, int nHeight);
int PushPoints(vector<cv::Point> aryPoint, bool isBackground);
Mat ApplyImage();
