#include "mainproc.h"
#include "FaceDetect.h"
#include "LandmarkDetect.h"

#include "cvblob.h"
#include "nrutil.h"
#include "Calc.h"

//#include <opencv2\opencv.hpp>
#define BYTE unsigned char

#define PROC_IMG_WIDTH  400
#define PROC_IMG_HEIGHT 400
#define PROC_IMG_WIDTH_S  250
#define PROC_IMG_HEIGHT_S 250

#define PROC_MASK_WIDTH  500
#define PROC_MASK_HEIGHT 500
#define PROC_BIAS_X (PROC_MASK_WIDTH - PROC_IMG_WIDTH) / 2
#define PROC_BIAS_Y (PROC_MASK_HEIGHT - PROC_IMG_HEIGHT) / 2

#define SPEEDUP FALSE


static float Interpolation(float* Im, int cx, int cy, int x1024, int y1024)
{
	float pVal;
	DWORD c = 0;
	int   xq, xr, yq, yr, n;
	float m;

	xq = x1024 >> 10;
	xr = x1024 - (xq << 10);
	yq = y1024 >> 10;
	n = yq * cx + xq;
	yr = y1024 - (yq << 10);

	if (xq < 0 || xq >= cx)	c = 0x10;
	if (yq < 0 || yq >= cy)	c |= 0x20;

	if (c == 0 && xr >= 0 && yr >= 0) {
		if (xq + 1 < cx) {
			if (yq + 1 < cy) {
				m = Im[n + 1] * (1024 - yr) * xr + Im[n + cx + 1] * yr * xr + Im[n + cx] * (1024 - xr)*yr + Im[n] * (1024 - xr)*(1024 - yr);
				m = m < 0 ? m + 1048575 : m;
				pVal = m / (1 << 20);
				return pVal;
			}
		}
		pVal = Im[n]; return pVal;
	}
	return 0;
}

void ImageStretch(float* imgIn, int cxIn, int cyIn, float* imgOut, int cxOut, int cyOut)
{
	int x, y, adr, yy;
	int x_ratio = (cxIn << 10) / cxOut;
	int y_ratio = (cyIn << 10) / cyOut;

	for (y = 0; y < cyOut; y++) {
		adr = y*cxOut;
		yy = y*y_ratio;
		for (x = 0; x < cxOut; x++) {
			imgOut[adr + x] = Interpolation(imgIn, cxIn, cyIn, x*x_ratio, yy);
		}
	}
}


Mat g_Image;
Mat g_Mask;
cv::Rect g_Rect;
static void help()
{
	cout << "\nThis program demonstrates GrabCut segmentation -- select an object in a region\n"
		"and then grabcut will attempt to segment it out.\n"
		"Call:\n"
		"./grabcut <image_name>\n"
		"\nSelect a rectangular area around the object you want to segment\n" <<
		"\nHot keys: \n"
		"\tESC - quit the program\n"
		"\tr - restore the original image\n"
		"\tn - next iteration\n"
		"\n"
		"\tleft mouse button - set rectangle\n"
		"\n"
		"\tCTRL+left mouse button - set GC_BGD pixels\n"
		"\tSHIFT+left mouse button - set GC_FGD pixels\n"
		"\n"
		"\tCTRL+right mouse button - set GC_PR_BGD pixels\n"
		"\tSHIFT+right mouse button - set GC_PR_FGD pixels\n" << endl;
}
const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const int BGD_KEY = EVENT_FLAG_CTRLKEY;
const int FGD_KEY = EVENT_FLAG_SHIFTKEY;

static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
}

class GCApplication
{
public:
	enum { NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
	static const int radius = 2;
	static const int thickness = -1;	
	void reset();
	void setImageAndWinName(const Mat& _image, const string& _winName);
	void showImage() const;
	void mouseClick(int event, int x, int y, int flags, void* param);
	int nextIter();
	int getIterCount() const { return iterCount; }
	cv::Rect rect;
	Mat mask;
	Mat imageMain;
    Mat imageResult;
	Mat bgdModel, fgdModel;	
	uchar rectState, lblsState, prLblsState;
	bool isInitialized;
	vector<cv::Point> rfgdPxls, rbgdPxls;
	void setLblsInMask();
	void setPrevMask();
    vector<cv::Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls, prevFgdPxls, prevBgdPxls;
private:
	void setLblsInMask(int flags, cv::Point p, bool isPr);
	void setRectInMask();
	const string* winName;
	int iterCount;
};

float *Alpha;
void GCApplication::reset()
{
	if (!mask.empty())
		mask.setTo(Scalar::all(GC_BGD));
	rfgdPxls.clear();rbgdPxls.clear();
	bgdPxls.clear(); fgdPxls.clear();
	prBgdPxls.clear();  prFgdPxls.clear();
	prevFgdPxls.clear(), prevBgdPxls.clear();
	memset(Alpha, 0, mask.rows * mask.cols * sizeof(float));
	isInitialized = false;
	rectState = NOT_SET;
	lblsState = NOT_SET;
	prLblsState = NOT_SET;
	iterCount = 0;	
}

void GCApplication::setImageAndWinName(const Mat &_image, const string& _winName)
{
	if (_image.empty() || _winName.empty())
		return;
    _image.copyTo(imageMain);
	winName = &_winName;
	mask.create(imageMain.size(), CV_8UC1);
	reset();
}

void GCApplication::showImage() const
{
	if (imageMain.empty())
		return;
	Mat res;
	Mat binMask;
	if (!isInitialized) {
		imageMain.copyTo(res);
	} else {
//  	getBinMask(mask, binMask);
//  	image->copyTo(res, binMask);
		imageMain.copyTo(res);
	}
	for (int i = 0; i < rfgdPxls.size(); i++) {
		circle(res, rfgdPxls[i], radius, RED, thickness);
	}
	for (int i = 0; i < rbgdPxls.size(); i++) {
		circle(res, rbgdPxls[i], radius, BLUE, thickness);
	}
	vector<cv::Point>::const_iterator it;
	for (it = bgdPxls.begin(); it != bgdPxls.end(); ++it)
		circle(res, *it, radius, BLUE, thickness);
	for (it = fgdPxls.begin(); it != fgdPxls.end(); ++it)
		circle(res, *it, radius, RED, thickness);
	for (it = prBgdPxls.begin(); it != prBgdPxls.end(); ++it)
		circle(res, *it, radius, LIGHTBLUE, thickness);
	for (it = prFgdPxls.begin(); it != prFgdPxls.end(); ++it)
		circle(res, *it, radius, PINK, thickness);
	if (rectState == IN_PROCESS || rectState == SET)
		rectangle(res, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), GREEN, 2);
	//imshow(*winName, res);
	binMask.release();
	res.release();
}

void GCApplication::setRectInMask()
{
	CV_Assert(!mask.empty());
	mask.setTo(GC_BGD);
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, imageMain.cols - rect.x);
	rect.height = min(rect.height, imageMain.rows - rect.y);
	(mask(rect)).setTo(Scalar(GC_PR_FGD));
}
void GCApplication::setLblsInMask() {
	for (int i = 0; i < fgdPxls.size(); i++) {
		rfgdPxls.push_back(fgdPxls[i]);
		circle(mask, fgdPxls[i], radius, GC_FGD, thickness);
	}
	for (int i = 0; i < bgdPxls.size(); i++) {
		rbgdPxls.push_back(bgdPxls[i]);
		circle(mask, bgdPxls[i], radius, GC_BGD, thickness);
	}
}
void GCApplication::setLblsInMask(int flags, cv::Point p, bool isPr)
{
	vector<cv::Point> *bpxls, *fpxls;
	uchar bvalue, fvalue;
	if (!isPr)
	{
		bpxls = &bgdPxls;
		fpxls = &fgdPxls;
		bvalue = GC_BGD;
		fvalue = GC_FGD;
	} else	{
		bpxls = &prBgdPxls;
		fpxls = &prFgdPxls;
		bvalue = GC_PR_BGD;
		fvalue = GC_PR_FGD;
	}
	if (flags & BGD_KEY) {
		bpxls->push_back(p);
		//circle(mask, p, radius, bvalue, thickness);
	}
	if (flags & FGD_KEY) {
		fpxls->push_back(p);
		//circle(mask, p, radius, fvalue, thickness);
	}
}
void GCApplication::setPrevMask() {
	fgdPxls.clear();
	fgdPxls.clear();
	fgdPxls = prevFgdPxls;
	bgdPxls = prevBgdPxls;
}
void GCApplication::mouseClick(int event, int x, int y, int flags, void*)
{
	// TODO add bad args check
	switch (event)
	{
	case EVENT_LBUTTONDOWN: // set rect or GC_BGD(GC_FGD) labels
	{
		if (rectState == NOT_SET) {
			rect = g_Rect;
			g_Mask.copyTo(mask);
			rectState = SET;
		}
		bool isb = (flags & BGD_KEY) != 0,
			isf = (flags & FGD_KEY) != 0;
		if (rectState == NOT_SET && !isb && !isf)
		{
			//rectState = IN_PROCESS;
			//rect = cv::Rect(x, y, 1, 1);
		}
		if ((isb || isf) && rectState == SET)
			lblsState = IN_PROCESS;
		prevBgdPxls.clear();
		prevFgdPxls.clear();
		prevFgdPxls = fgdPxls;
		prevBgdPxls = bgdPxls;

	}
	break;
	case EVENT_LBUTTONUP:
		if (rectState == IN_PROCESS)
		{
// 			rect = cv::Rect(cv::Point(rect.x, rect.y), cv::Point(x, y));
// 			rectState = SET;
// 			setRectInMask();
// 			CV_Assert(bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty());
// 			showImage();
		}
		if (lblsState == IN_PROCESS)
		{
			setLblsInMask(flags, cv::Point(x, y), false);
			lblsState = SET;
			showImage();
		}
		break;
	case EVENT_MOUSEMOVE:
		if (rectState == IN_PROCESS)
		{
			rect = cv::Rect(cv::Point(rect.x, rect.y), cv::Point(x, y));
			CV_Assert(bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty());
			showImage();
		}
		else if (lblsState == IN_PROCESS)
		{
			setLblsInMask(flags, cv::Point(x, y), false);
			showImage();
		}
		else if (prLblsState == IN_PROCESS)
		{
			setLblsInMask(flags, cv::Point(x, y), true);
			showImage();
		}
		break;
// 	case EVENT_RBUTTONDOWN: // set GC_PR_BGD(GC_PR_FGD) labels
// 	{
// 		bool isb = (flags & BGD_KEY) != 0,
// 			isf = (flags & FGD_KEY) != 0;
// 		if ((isb || isf) && rectState == SET)
// 			prLblsState = IN_PROCESS;
// 	}
// 	break;
// 	case EVENT_RBUTTONUP:
// 		if (prLblsState == IN_PROCESS)
// 		{
// 			setLblsInMask(flags, cv::Point(x, y), true);
// 			prLblsState = SET;
// 			showImage();
// 		}
// 		break;
	}
}

void RunMatting(const Mat* image, Mat &mask, float *alpha, BYTE *OutImage, int flag);

int GCApplication::nextIter()
{
	if (isInitialized) {
		grabCut(imageMain, mask, rect, bgdModel, fgdModel, 1);
	} else {
		if (rectState != SET)
			return iterCount;
		if (lblsState == SET || prLblsState == SET) {
			grabCut(imageMain, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
		} else {
			grabCut(imageMain, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);
		}			
		isInitialized = true;
	}
#if (SPEEDUP == TRUE)
	Mat OutMat, tMat;
	Mat binMask;
	getBinMask(mask, binMask);
	image->copyTo(OutMat, binMask);
	imshow("Result Image", OutMat);
#else
	BYTE  *OutImg = (BYTE *)malloc(imageMain.rows * imageMain.cols * 4);
	
	int str_wid, str_hei;
	float ratio_x = (float)imageMain.cols / PROC_IMG_WIDTH_S;
	float ratio_y = (float)imageMain.rows / PROC_IMG_HEIGHT_S;
	float ratio = max(ratio_x, ratio_y);
	int Str_wid, Str_hei;	

	Str_wid = (int)(imageMain.cols / ratio);
	Str_hei = (int)(imageMain.rows / ratio);
	Mat img_small;
	resize(imageMain, img_small, img_small.size(), 1.0 / ratio, 1.0 / ratio, INTER_LINEAR);

	Mat mask_small;
	resize(mask, mask_small, mask_small.size(), 1.0 / ratio, 1.0 / ratio, INTER_NEAREST);

	if (iterCount == 0) {
		RunMatting(&img_small, mask_small, Alpha, OutImg, 0);
	} else {
		RunMatting(&img_small, mask_small, Alpha, OutImg, 1);
	}
	
	Mat TmpMat = Mat(img_small.rows, img_small.cols, CV_8UC4, OutImg);
	Mat OutMat;
	resize(TmpMat, OutMat, OutMat.size(), ratio, ratio, INTER_LINEAR);
    
    if(imageResult.data != NULL) {
        imageResult.release();
    }
    OutMat.copyTo(imageResult);
	img_small.release();
	mask_small.release();

	SafeMemFree(OutImg);	
#endif	
	iterCount++;
	bgdPxls.clear(); fgdPxls.clear();
	prBgdPxls.clear(); prFgdPxls.clear();

	return iterCount;
}

GCApplication gcapp;
static void on_mouse(int event, int x, int y, int flags, void* param)
{
	gcapp.mouseClick(event, x, y, flags, param);
}


int ClipRGBImage_T(BYTE* pImgSrc, int nWidthSrc, int nHeightSrc, int nX, int nY, BYTE* pImgDst, int nWidthDst, int nHeightDst){
	if(pImgSrc == NULL || pImgDst == NULL || nX + nWidthDst < 0 || nY + nHeightDst < 0 || nX >= nWidthSrc || nY >= nHeightSrc) {
		return -1;
	}
	int nRealX = nX, nRealY=nY, nRealWidth=nWidthDst, nRealHeight = nHeightDst;
	if(nX < 0) {
		nRealX = 0;
		nRealWidth = nWidthDst - nX;
	}
	if(nY < 0) {
		nRealY = 0;
		nRealHeight = nHeightDst - nY;
	}
	if(nX + nWidthDst > nWidthSrc) {
		nRealWidth = nWidthSrc - nX;
	}
	if(nY + nHeightDst > nHeightSrc) {
		nRealHeight = nHeightSrc - nY;
	}
	memset(pImgDst, 0, sizeof(BYTE) * nWidthDst * nHeightDst * 3);
	for(int i=nRealY; i<nRealY+nRealHeight; i++) {
		memcpy(pImgDst + (i-nRealY) * nWidthDst * 3, pImgSrc + i*nWidthSrc*3 + nRealX*3, nRealWidth * 3);
	}

	return 0;
}


void ColorStrech(BYTE*inRGBImage, int cx, int cy, BYTE* outRGBImg, int wid, int hei)
{
	BYTE* pRImage = (BYTE *)calloc(cx*cy, sizeof(BYTE));
	BYTE* pGImage = (BYTE *)calloc(cx*cy, sizeof(BYTE));
	BYTE* pBImage = (BYTE *)calloc(cx*cy, sizeof(BYTE));
	BYTE* RImg = (BYTE *)calloc(wid*hei, sizeof(BYTE));
	BYTE* GImg = (BYTE *)calloc(wid*hei, sizeof(BYTE));
	BYTE* BImg = (BYTE *)calloc(wid*hei, sizeof(BYTE));

	int i;
	for (i = 0; i < cy*cx; i++){
		pBImage[i] = inRGBImage[3*i];
		pGImage[i] = inRGBImage[3*i + 1];
		pRImage[i] = inRGBImage[3*i + 2];
	}
	ImageStretch(pRImage, cx, cy, RImg, wid, hei);	
	ImageStretch(pGImage, cx, cy, GImg, wid, hei);
	ImageStretch(pBImage, cx, cy, BImg, wid, hei);
// 	ImageSmoothing(RImg, wid, hei, 2);
// 	ImageSmoothing(GImg, wid, hei, 2);
// 	ImageSmoothing(BImg, wid, hei, 2);
	for (i = 0; i < wid*hei; i++){
		outRGBImg[3*i] =   BImg[i] ;
		outRGBImg[3*i + 1] = GImg[i]; 
		outRGBImg[3*i + 2] = RImg[i]; 
	}
	free(pRImage); free(pGImage); free(pBImage);
	free(RImg); free(GImg); free(BImg);
}

int DetectLandMark(BYTE* i_pImage, int i_nWidth, int i_nHeight, SFaceStru* facestru)
{
	if (i_pImage == NULL)
		return -1;
	SRect rcFace;
	SPoint cen;
	int BetEye = 0;
	BetEye = (int)(facestru->rcFace.width / 2.5 + 0.5);
	cen.x = facestru->rcFace.x + facestru->rcFace.width / 2;
	cen.y = (int)(facestru->rcFace.y +  1.05 * BetEye + 0.5);		

	rcFace.x = (int)(cen.x - 1.0 * BetEye + 0.5);
	rcFace.y = (int)(cen.y - 0.6 * BetEye + 0.5);
	rcFace.width = (int)(BetEye * 2.0 + 0.5);
	rcFace.height = (int)(BetEye * 2.0 + 0.5);		
	LandMarkExtract(i_pImage, i_nWidth, i_nHeight, rcFace, facestru);	
	return 0;
}

int bin_erode(BYTE *im, int width, int height)
{
	int i, j, i1, j1, adr, st_x, st_y, end_x, end_y;

	if (im == 0) return 0;
	BYTE *im_tmp = (BYTE *)calloc(width * height, sizeof(BYTE));
	memset(im_tmp, 0, width * height * sizeof(BYTE));
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			adr = i * width + j;
			if (im[adr] == 0) {
				continue;
			}
			st_x = max(0, j - 1); st_y = max(0, i - 1);
			end_x = min(width - 1, j + 1); end_y = min(height - 1, i + 1);
			for (i1 = st_y; i1 <= end_y; i1++) {
				for (j1 = st_x; j1 <= end_x; j1++) {
					if (im[i1 * width + j1] == 0) {
						goto TT;
					}
				}
			}
			im_tmp[adr] = 1;
TT:         j = j;
		}
	}
	memcpy(im, im_tmp, width * height * sizeof(BYTE));
	SafeMemFree(im_tmp);

	return 1;
}

extern unsigned long *g_ija;
extern double *g_sa;
extern float *gf_sa;

 void GetMean_float(float **Mat, int row, int col, float **Mean)
 {
 	int i, j;
 	float s = 0;
 
 	for (j = 1; j <= col; j++) {
 		s = 0;
 		for (i = 1; i <= row; i++) {
 			s += Mat[i][j];
 		}
 		Mean[j][1] = s / row;
 	}
 }

void GetMean(double **Mat, int row, int col, double **Mean)
{
	int i, j;
	double s = 0;

	for (j = 1; j <= col; j++) {
		s = 0;
		for (i = 1; i <= row; i++) {
			s += Mat[i][j];
		}
		Mean[j][1] = s / row;
	}
}


#if 1
void getLaplacian1(float *pfImage, BYTE *consts, int width, int height, int BitCount, int &Num)
{	
	int i, j, adr, adr1, k, l, m, c, n, image_size = width * height;
	int win_size = 1, neb_size, tlen, len;
	double epsilon = 0.0000001;
	int **indsM = imatrix(1, height, 1, width);
	int *win_inds;

	neb_size = (int)SQR(win_size * 2 + 1);
	double **winI, *tvals_1;
	double **tvals   = dmatrix(1, neb_size, 1, neb_size);
	double **TmpMat1 = dmatrix(1, neb_size, 1, neb_size);
	double **TmpMat2 = dmatrix(1, neb_size, 1, neb_size);
	double **win_var = dmatrix(1, 3, 1, 3);
	double **win_mu  = dmatrix(1, 3, 1, 1);

	tvals_1 = dvector(1, SQR(neb_size));
	int *Ind_Tbl_row = ivector(1, SQR(neb_size));
	int *Ind_Tbl_col = ivector(1, SQR(neb_size));

	for (i = 1; i <= SQR(neb_size); i++) {
		adr = i % neb_size;
		if (adr == 0) {
			adr = neb_size;
		}
		Ind_Tbl_row[i] = adr;
		Ind_Tbl_col[i] = (i - 1) / neb_size + 1;
	}

	double ttt = 0;
	n = height;
	m = width;
	bin_erode(consts, width, height);

	for (i = 1; i <= height; i++) {
		for (j = 1; j <= width; j++) {
			indsM[i][j] = (j - 1) * height + i;
		}
	}
	tlen = 0;
	for (i = win_size; i < height - win_size; i++) {
		for (j = win_size; j < width - win_size; j++) {
			tlen += 1 - consts[i * width + j];
		}
	}
	tlen *= SQR(neb_size);

	win_inds = ivector(1, neb_size);
	winI = dmatrix(1, neb_size, 1, 3);

	len = 0;
	int *Num_PerRow = (int *)calloc((image_size + 1), sizeof(int));
	ItemSort **Cols_PerRow = (ItemSort **)malloc((image_size + 1) * sizeof(ItemSort *));
	double **Dta_PerRow = (double **)malloc((image_size + 1) * sizeof(double *));
	for (i = 0; i <= image_size; i++) {
		Cols_PerRow[i] = (ItemSort *)malloc(90 * sizeof(ItemSort));
		Dta_PerRow[i]  = (double *)malloc(90 * sizeof(double));
	}
	for (j = 1 + win_size; j <= width - win_size; j++) {
		for (i = win_size + 1; i <= height - win_size; i++) {
			if (consts[(i - 1) * width + j - 1] == 1) {
				continue;
			}
			adr = 1;
			for (l = j - win_size; l <= j + win_size; l++) {
				for (k = i - win_size; k <= i + win_size; k++) {
					win_inds[adr] = indsM[k][l];
					adr++;
				}
			}
			adr = 1;
			for (l = j - win_size; l <= j + win_size; l++) {
				for (k = i - win_size; k <= i + win_size; k++) {
					adr1 = (k - 1) * width + l - 1;
					for (c = 1; c <= BitCount; c++) {
						winI[adr][c] = pfImage[BitCount * adr1 + c - 1];
					}
					adr++;
				}
			}
			GetMean(winI, neb_size, BitCount, win_mu);
			TranseMatMul(winI, winI, BitCount, neb_size, BitCount, TmpMat1);
			MatMulTranse(win_mu, win_mu, BitCount, 1, BitCount, TmpMat2);

			for (k = 1; k <= BitCount; k++) {
				for (l = 1; l <= BitCount; l++) {
					if (k == l) {
						win_var[k][l] = (double)TmpMat1[k][l] / neb_size - (double)TmpMat2[k][l] + (double)epsilon / neb_size;
					}
					else {
						win_var[k][l] = (double)TmpMat1[k][l] / neb_size - (double)TmpMat2[k][l];
					}
				}
			}
			InvMatrix(win_var, BitCount, TmpMat1);

			for (k = 1; k <= neb_size; k++) {
				for (c = 1; c <= BitCount; c++) {
					winI[k][c] -= win_mu[c][1];
				}
			}
			Mat_Mul(winI, TmpMat1, neb_size, BitCount, BitCount, TmpMat2);
			MatMulTranse(TmpMat2, winI, neb_size, BitCount, neb_size, tvals);
			adr = 1;
			for (k = 1; k <= neb_size; k++) {
				for (l = 1; l <= neb_size; l++) {
					tvals_1[adr] = (tvals[k][l] + 1) / neb_size;
					adr++;
				}
			}
			for (k = 1; k <= SQR(neb_size); k++) {
				int row = win_inds[Ind_Tbl_row[k]];
				Num_PerRow[row]++;
				Cols_PerRow[row][Num_PerRow[row]].colIndex = win_inds[Ind_Tbl_col[k]];
				Cols_PerRow[row][Num_PerRow[row]].oldIndex = Num_PerRow[row];
				Dta_PerRow[row][Num_PerRow[row]] = tvals_1[k];
			}
			len += SQR(neb_size);
		}
	}
// 	int yyy = 0;
// 	for (i = 0; i <= image_size; i++) {
// 		yyy = max(yyy, Num_PerRow[i]);
// 	}
	Num = tlen + image_size;
	g_sa  = dvector(1, Num);	
	g_ija = lvector(1, Num);	
	sprsin_FromCordiToRow(Dta_PerRow, Cols_PerRow, Num_PerRow, image_size, g_sa, g_ija);
	double *Diag = dvector(1, image_size);
	sprsum_col(g_sa, g_ija, Diag, image_size);
	for (i = 1; i <= image_size; i++) {
		g_sa[i] = Diag[i] - g_sa[i];
	}
	for (i = image_size + 1; i <= g_ija[image_size + 1] - 1; i++) {
		g_sa[i] = -g_sa[i];
	}
	
	// 	for (int i = 0; i < height; i++) {
	// 	 	for (int j = 0; j < width; j++)	{
	// 	 		double a;
	// 	 		a = consts[i * width + j];
	// 	 		fwrite(&a, 1, sizeof(double), fp);
	// 	 	}
	// 	}	
	// 	fclose(fp);
	

	free_imatrix(indsM, 1, height, 1, width);
	free_ivector(win_inds, 1, neb_size);
	free_dmatrix(winI, 1, neb_size, 1, 3);
	free_dmatrix(tvals, 1, neb_size, 1, neb_size);
	free_dmatrix(TmpMat1, 1, neb_size, 1, neb_size);
	free_dmatrix(TmpMat2, 1, neb_size, 1, neb_size);
	free_ivector(Ind_Tbl_row, 1, neb_size);
	free_ivector(Ind_Tbl_col, 1, neb_size);
	free_dmatrix(win_var, 1, 3, 1, 3);
	free_dmatrix(win_mu, 1, 3, 1, 1);
	free_dvector(tvals_1, 1, neb_size);
	free_dvector(Diag, 1, image_size);
	for (i = 0; i <= image_size; i++) {
		SafeMemFree(Cols_PerRow[i]);
		SafeMemFree(Dta_PerRow[i]);
	}
	SafeMemFree(Cols_PerRow);
	SafeMemFree(Dta_PerRow);
	SafeMemFree(Num_PerRow);
}
#endif

#if 0
void solveAlpha(float *pfImage, BYTE *consts_map, BYTE *consts_vals, int width, int height, int BitCount, float *alpha)
{
	int    i, j, adr, ind, Num;
	int    img_size = width * height;
	double lambda = 100;

	double *Diag = dvector(1, img_size);
	double *Bias = dvector(1, img_size);

	float *fDiag = fvector(1, img_size);
	float *fBias = fvector(1, img_size);

	adr = 1;
	for (j = 1; j <= width; j++) {
		for (i = 1; i <= height; i++) {
			ind = (i - 1) * width + j - 1;
			Diag[adr] = consts_map[ind];
			Bias[adr] = lambda * consts_map[ind] * consts_vals[ind];
			fBias[adr] = Bias[adr];
			adr++;
		}
	}
	getLaplacian1(pfImage, consts_map, width, height, BitCount, Num);

	gf_sa = fvector(1, Num);
	for (i = 1; i <= img_size; i++)	{
		g_sa[i] = g_sa[i] + lambda * Diag[i];		
	}
	for (i = 1; i <= Num; i++) {
		gf_sa[i] = g_sa[i];
	}

// 	FILE *fp = fopen("E:\\test_m.bin", "rb");
// 	double sum = 0, a;
// 	fread(&a, 1, sizeof(double), fp);
// 	int size = (int(a + 0.5));
// 	double *data = dvector(1, size);
// 	int *row = ivector(1, size);
// 	int *col = ivector(1, size);
// 	double *sa = dvector(1, img_size + size);
// 	unsigned long *ija= lvector(1, img_size + size);
// 	for (int i = 1; i <= size; i++) {
// 		fread(&a, 1, sizeof(double), fp);
// 		row[i] = (int)(a + 0.5);
// 
// 		fread(&a, 1, sizeof(double), fp);
// 		col[i] = (int)(a + 0.5);
// 
// 		fread(&a, 1, sizeof(double), fp);
// 		data[i] = a;
// 	}
// 	fclose(fp);
// 	sprsin_FromCordiToRow(data, row, col, size, img_size, sa, ija);
// 	sum = 0;
// 	for (i = 1; i <= g_ija[img_size + 1] - 1; i++) {
// 		if (i == img_size + 1) {
// 			continue;
// 		}
// 		sum += fabs(sa[i] - g_sa[i]);
// 		sum += fabs(ija[i] - g_ija[i]);
// 		if (sum > 1) {
// 			sum = sum;
// 		}
// 	}
// 	free_dvector(data, 1, size);
// 	free_ivector(row, 1, size);
// 	free_ivector(col, 1, size);
	
	double err, tol = 0.0001;
	int itmax = 5000, iter = 10000, itol = 1;
	

	float ferror = 0, ftol = 0.0001;
	memset(fDiag + 1, 0, img_size * sizeof(float));

	int cx, cy, bit;
	CImageProc proc;
	adr = 0;
	BYTE* pImage = proc.ReadBitmapFile("E:\\consts_vals1.bmp", cx, cy, bit);
	for (i = 1; i <= height; i++) {
		for (j = 1; j <= width; j++) {
			fDiag[j * height + i] = pImage[adr] / 255.0;
			adr++;
		}
	}
	SafeMemFree(pImage);

	AfxMessageBox("OK1", MB_OK);
	DWORD tim = GetTickCount();
// 	memset(Diag + 1, 0, img_size * sizeof(double));
// 	ConjugateGradient_DiagPreco(img_size, Bias, Diag, tol, itmax, &iter, &err);


	ConjugateGradient_DiagPreco(img_size, fBias, fDiag, ftol, itmax, &iter, &ferror);
	//ConjugateGradient_DiagPreco_speed(img_size, fBias, fDiag, ftol, itmax, &iter, &ferror);
	tim = GetTickCount() - tim;
	char str[MAX_PATH];
	sprintf(str, "%d", tim);
	AfxMessageBox(str, MB_OK);


// 	tol /= 100;
// 	linbcg(img_size, Bias, Diag, itol, tol, itmax, &iter, &err);

 	BYTE *pshow = (BYTE *)malloc(img_size * sizeof(BYTE));
 	adr = 0;
 	for (i = 1; i <= height; i++) {
 		for (j = 1; j <= width; j++) {	
 			if (fDiag[j * height + i] < 0) {
 				fDiag[j * height + i] = 0;
 			}
 			if (fDiag[j * height + i] > 1) {
 				fDiag[j * height + i] = 1;
 			}
 			int tmp = (int)(255 * fDiag[j * height + i]);
 			pshow[adr++] = tmp;
 		}
 	}
 	WriteBitmapFile("E:\\ppp.bmp", pshow, width, height, 8);
 	SafeMemFree(pshow);

// 	FILE *fp = fopen("E:\\test_m.bin", "rb");
// 	double sum = 0, a;
// 	for (int i = 1; i <= img_size; i++) {
// 		fread(&a, 1, sizeof(double), fp);
// 		sum += fabs(a - Diag[i]);
// 		if (fabs(a) > 0.001) {
// 			i = i;
// 		}
// 	}
// 	fclose(fp);
	
	free_dvector(g_sa, 1, Num);
	free_lvector(g_ija, 1, Num);
	free_dvector(Diag, 1, img_size);
	free_dvector(Bias, 1, img_size);
	free_vector(fDiag, 1, img_size);
	free_vector(fBias, 1, img_size);
}
#endif

void solveAlpha(float *pfImage, BYTE *consts_map, BYTE *consts_vals, int width, int height, int BitCount, float *alpha, float *InitVal = NULL)
{
	int    i, j, adr, ind, Num;
	int    img_size = width * height;
	double lambda = 10;
	float  *Diag = fvector(1, img_size);
	float  *Bias = fvector(1, img_size);

	adr = 1;
	for (j = 1; j <= width; j++) {
		for (i = 1; i <= height; i++) {
			ind = (i - 1) * width + j - 1;
			Diag[adr] = consts_map[ind];
			Bias[adr] = lambda * consts_map[ind] * consts_vals[ind];			
			adr++;
		}
	}
	getLaplacian1(pfImage, consts_map, width, height, BitCount, Num);

	gf_sa = fvector(1, Num);
	for (i = 1; i <= img_size; i++) {
		g_sa[i] = g_sa[i] + lambda * Diag[i];
	}
	for (i = 1; i <= Num; i++) {
		gf_sa[i] = (float)g_sa[i];
	}
	int itmax = 1000, iter = 0, itol = 1;
	float ferror = 0, ftol = 0.0001f;
	if (InitVal == NULL) {
		memset(Diag + 1, 0, img_size * sizeof(float));
	} else {
		memcpy(Diag + 1, InitVal, img_size * sizeof(float));
	}
	ConjugateGradient_DiagPreco(img_size, Bias, Diag, ftol, itmax, &iter, &ferror);

	adr = 0; int tmp = 0;
	for (i = 1; i <= height; i++) {
		for (j = 1; j <= width; j++) {
			int ind = (j - 1) * height + i - 1 + 1;
			if (Diag[ind] < 0.02) {
				Diag[ind] = 0;
			}
			if (Diag[ind] > 1) {
				Diag[ind] = 1;
			}
			alpha[adr] = Diag[ind];	
			adr++;
		}
	}

	free_dvector(g_sa, 1, Num);
	free_lvector(g_ija, 1, Num);
	free_vector(gf_sa, 1, Num);
	free_vector(Diag, 1, img_size);	
	free_vector(Bias, 1, img_size);		
}

void solveAlphaC2F(float *pfImage, BYTE *consts_map, BYTE *consts_vals, int width, int height, int BitCount, int levels_num, int active_levels_num, float *alpha)
{
	int erode_mask_w = 1;
	float thr_alpha = 0.02f;

	if (active_levels_num >= levels_num) {
		solveAlpha(pfImage, consts_map, consts_vals, width, height, BitCount, alpha);
	}
}

void RunMatting(const Mat* image, Mat &mask, float *alpha, BYTE *OutImage, int flag)
{
	int i, j, k, adr, bit_count = image->channels(), ind;
	int width, height;

	width = image->cols;
	height = image->rows;

	float *pfImage = (float *)malloc(width * height * bit_count * sizeof(float));
	float *pfInit = (float *)malloc(width * height * bit_count * sizeof(float));
	BYTE *consts_map = (BYTE *)malloc(width * height * sizeof(BYTE));
	BYTE *consts_vals = (BYTE *)calloc(width * height, sizeof(BYTE));

	memset(consts_map, 0, width * height * sizeof(BYTE));
	memset(consts_vals, 0, width * height * sizeof(BYTE));
	adr = 0; ind = 0;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			Vec3b bbb = image->at<Vec3b>(i, j);
			for (k = 0; k < bit_count; k++) {
				pfImage[adr + k] = (float)bbb[2 - k] / 255;
			}
			BYTE val = mask.at<char>(i, j);
			if (val == GC_FGD || val == GC_BGD) {
				consts_map[ind] = 1;
				if (val == 1) {
					consts_vals[ind] = 1;
				}
			} else {
				consts_map[ind] = 0;
			}
			if (flag == 0) {
				if (val == GC_BGD || val == GC_PR_BGD) {
					pfInit[j * height + i] = 0;
				}
				if (val == GC_FGD || val == GC_PR_FGD) {
					pfInit[j * height + i] = 1;
				}
			} else {
				pfInit[j * height + i] = Alpha[i * width + j];
			}
			adr += bit_count;
			ind++;
		}
	}
	//WriteBitmapFile("E:\\consts_map.bmp", pfInit, height, width, 8);
	int rgb[3] = { 0, 255, 0 };
	solveAlpha(pfImage, consts_map, consts_vals, width, height, bit_count, alpha, pfInit);

	float alpha1 = 0;
	adr = 0;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			alpha1 = alpha[adr];
			Vec3b bbb = image->at<Vec3b>(i, j);
			for (k = 0; k < bit_count; k++) {
				OutImage[4 * adr + (bit_count - k - 1)] = alpha1 * bbb[k];
			}
            OutImage[4*adr + 3] = MAX(0, MIN(255, (alpha1) * 255));

			BYTE val = mask.at<char>(i, j);
			if (val != GC_FGD && val != GC_BGD) {
				if (alpha1 > 0.2) {
					mask.at<char>(i, j) = GC_PR_FGD;
				}
				if (alpha1 < 0.1) {
					mask.at<char>(i, j) = GC_PR_BGD;
				}
			}
			adr++;
		}
	}
	//WriteBitmapFile("E:\\OutImage.bmp", OutImage, width, height, 24);
	SafeMemFree(consts_map);
	SafeMemFree(consts_vals);
	SafeMemFree(pfImage);
	SafeMemFree(pfInit);

}

void RunMatting_Test(BYTE *pImage, BYTE *pImage_map, int width, int height, int BitCount, float *alpha)
{
	int i, j, adr;
	float diff;
	float *pfImage = (float *)malloc(width * height * 3 * sizeof(float));
	float *pfImage_map = (float *)malloc(width * height * 3 * sizeof(float));
	BYTE *consts_map = (BYTE *)malloc(width * height * sizeof(BYTE));
	BYTE *consts_vals = (BYTE *)malloc(width * height * sizeof(BYTE));
	int active_levels_num = 1;
	int level_num = 1;

	memset(consts_map, 0, width * height * sizeof(BYTE));
	memset(consts_vals, 0, width * height * sizeof(BYTE));
	
	if (BitCount == 3) {
		adr = 0;
		for (i = 0; i < width * height; i++) {
			diff = 0;
			for (j = 0; j < 3; j++) {
				pfImage[adr + j] = (float)pImage[adr - j + 2] / 255;
				pfImage_map[adr + j] = (float)pImage_map[adr - j + 2] / 255;
				diff += fabs(pfImage[adr + j] - pfImage_map[adr + j]);
			}
			if (diff > 0.001f) {
				consts_map[i] = 1;
				double gray = 0.299f *  pfImage_map[adr] + 0.587f * pfImage_map[adr + 1] + 0.114f * pfImage_map[adr + 2];
				if (gray > 0.1) {
					consts_vals[i] = 1;
				}
			}
			adr += 3;
		}
	} else {
		for (i = 0; i < width * height; i++) {
			diff = abs(pfImage[i] - pfImage_map[i]);
			if (diff > 0.001f) {
				consts_map[i] = 1;
				if (pfImage_map[i] > 0.1f) {
					consts_vals[i] = 1;
				}
			}
		}
	}
	//WriteBitmapFile("E:\\consts_vals.bmp", consts_map, width, height, 8);

	solveAlphaC2F(pfImage, consts_map, consts_vals, width, height, BitCount, level_num, active_levels_num, alpha);

	SafeMemFree(consts_map);
	SafeMemFree(consts_vals);
}


int Bresenham(float x1, float y1, float x2, float y2, BYTE *Tmp, int width, int height)
{
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep) {
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;
	const int maxX = (int)x2;

	int num = 0;
	for (int x = (int)x1; x < maxX; x++) {
		int adr;
		if (steep) {
			adr = x * width + y;
			Tmp[adr] = 255;
			num++;
		}
		else {
			adr = y * width + x;
			Tmp[adr] = 255;
			num++;
		}
		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}

	return num;
}

void RGB2Gray(BYTE* pRGBImg, BYTE* pGray, int cx, int cy)
{
    if (pGray == NULL) return;
    BYTE r,g,b;
    int x, y, lVal, nAddr = 0;
    for (y = 0; y < cy; y++) {
        for (x = 0; x < cx; x++) {
            b = pRGBImg[3 * (y * cx + x) + 0];
            g = pRGBImg[3 * (y * cx + x) + 1];
            r = pRGBImg[3 * (y * cx + x) + 2];
            lVal = min((b * 117 + g * 601 + r * 306) >> 10, 255);
            pGray[nAddr++] = (BYTE)lVal;
        }
    }
}
void* read_file(char* path, int nSize)
{
    BYTE* buf = (BYTE*)malloc(nSize);
    if (buf == NULL) {
        return NULL;
    }
    
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        return NULL;
    }
    
    fread(buf, 1, nSize, fp);
    
    fclose(fp);
    
    return buf;
}

void write_file(char* path, void* buf, int nSize)
{
    
    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        return;
    }
    
    fwrite(buf, 1, nSize, fp);
    
    fclose(fp);
}

static const int radius = 2;
static const int thickness = -1;

int InitImage(BYTE* pImage, int nWidth, int nHeight)
{
	if (pImage == NULL || nWidth == 0 || nHeight == 0) return -1;

    BYTE* pGrayImg = (BYTE*)calloc(nWidth*nHeight, sizeof(BYTE));
	RGB2Gray(pImage, pGrayImg, nWidth, nHeight);
	CFaceInfo faceInfo;
	int i = 0, j;
	memset( &faceInfo, 0, sizeof( CFaceInfo));
	LPNRECT pRecT = NULL, piRect;
    Mat mat = Mat(nHeight, nWidth, CV_8UC3, pImage);
	if ( DetectFaces(pImage, nWidth, nHeight, 24, &pRecT) != 0){
		return -1;
	}
	for (piRect = pRecT; piRect != NULL; piRect = piRect->pNext, i++){
		SFaceStru FaceStru;
		int wid = piRect->r - piRect->l;
		int hei = piRect->b - piRect->t;

		FaceStru.rcFace.x = (int)(piRect->l - wid * 1.8 / 2);
		if(FaceStru.rcFace.x < 0) FaceStru.rcFace.x = 0;
		FaceStru.rcFace.y = (int)(piRect->t - hei * 1.8 / 2);
		if(FaceStru.rcFace.y < 0) FaceStru.rcFace.y = 0;		
		int leftlen = (int)(piRect->r + wid * 1.8 / 2);

		if(leftlen > nWidth) leftlen = nWidth;

		FaceStru.rcFace.width = leftlen - FaceStru.rcFace.x;
		int downLen = (int)(piRect->b + hei * 3 / 3);
		if(downLen > nHeight) downLen = nHeight;
		FaceStru.rcFace.height = downLen - FaceStru.rcFace.y;
		faceInfo.StruProFace.push_back(FaceStru);

		FaceStru.rcFace.x = piRect->l;
		FaceStru.rcFace.y = piRect->t;
		FaceStru.rcFace.width = wid;
		FaceStru.rcFace.height= hei;
		faceInfo.StruFace.push_back(FaceStru);
		faceInfo.nFaces++;
	}
	if(faceInfo.nFaces == 0) return -1;
	FreeRectList(&pRecT);
 	DetectLandMark(pGrayImg, nWidth, nHeight, &faceInfo.StruFace[0]);
	SafeMemFree(pGrayImg);
 
	int Pro_W = faceInfo.StruProFace[0].rcFace.width;
	int Pro_H = faceInfo.StruProFace[0].rcFace.height;

	BYTE* pImgDst = (BYTE *)malloc(Pro_W * Pro_H * 3 * sizeof(BYTE));
	BYTE* pProRGBImg = (BYTE *)malloc(PROC_IMG_WIDTH * PROC_IMG_HEIGHT * 3 * sizeof(BYTE));
	BYTE* pOutBinImg = (BYTE *)malloc(PROC_MASK_WIDTH * PROC_MASK_HEIGHT * sizeof(BYTE));
	ClipRGBImage_T(pImage, nWidth, nHeight, faceInfo.StruProFace[0].rcFace.x, faceInfo.StruProFace[0].rcFace.y, pImgDst, Pro_W, Pro_H);
	
	SRect ref;
	ref.x = faceInfo.StruFace[0].rcFace.x - faceInfo.StruProFace[0].rcFace.x;
	ref.y = faceInfo.StruFace[0].rcFace.y - faceInfo.StruProFace[0].rcFace.y;
	ref.width = faceInfo.StruFace[0].rcFace.width;
	ref.height = faceInfo.StruFace[0].rcFace.height;
	float ratio_x = (float)Pro_W / PROC_IMG_WIDTH;
	float ratio_y = (float)Pro_H / PROC_IMG_HEIGHT;
	float ratio = max(ratio_x, ratio_y);
	int Str_wid, Str_hei;

	Str_wid = (int)(Pro_W / ratio);
	Str_hei = (int)(Pro_H / ratio);

	ref.x = (int)(ref.x / ratio);
	ref.y = (int)(ref.y / ratio);
	ref.width = (int)(ref.width / ratio);
	ref.height = (int)(ref.height / ratio);
	SFaceStru proface;
	proface.rcFace = ref;

	SPoint cen;
	cen.x = 0;
	cen.y = 0;
	for (i = 0; i < LANDMARK_COUNT; i++) {
		proface.ptCord[i].x = faceInfo.StruFace[0].ptCord[i].x - faceInfo.StruProFace[0].rcFace.x;
		proface.ptCord[i].y = faceInfo.StruFace[0].ptCord[i].y - faceInfo.StruProFace[0].rcFace.y;
		proface.ptCord[i].x = max(0, min(Str_wid - 1, (int)(proface.ptCord[i].x / ratio)));
		proface.ptCord[i].y = max(0, min(Str_hei- 1, (int)(proface.ptCord[i].y / ratio)));

		cen.x += proface.ptCord[i].x;
		cen.y += proface.ptCord[i].y;
	}
	cen.x /= LANDMARK_COUNT;
	cen.y /= LANDMARK_COUNT;
	SPoint Landmark[LANDMARK_COUNT];
	for (i = 0; i < LANDMARK_COUNT; i++) {
		if (i < 17) {
			Landmark[i].x = cen.x + (proface.ptCord[i].x - cen.x) * 0.8;
			Landmark[i].y = cen.y + (proface.ptCord[i].y - cen.y) * 0.8;
		} else if (i < 27) {
			Landmark[i].x = cen.x + (proface.ptCord[i].x - cen.x) * 0.9;
			Landmark[i].y = cen.y + (proface.ptCord[i].y - cen.y) * 1.6;
		} else {
			Landmark[i].x = proface.ptCord[i].x;
			Landmark[i].y = proface.ptCord[i].y;
		}		
	}
	ColorStrech(pImgDst, Pro_W, Pro_H, pProRGBImg, Str_wid, Str_hei);
 	SafeMemFree(pImgDst);	

	cv::Rect rect;
	Mat mask = Mat::zeros(Str_hei, Str_wid, CV_8UC1);
	Alpha = (float *)malloc(Str_wid * Str_hei * sizeof(float));

	int margin_x = 10, margin_y = 10;

	rect.x = margin_x;
	rect.y = margin_y;
	rect.width  = Str_wid - 2 * margin_x;
	rect.height = Str_hei - 2 * margin_y;

	mask.setTo(GC_PR_BGD);
	(mask(rect)).setTo(Scalar(GC_PR_FGD));	
	BYTE *Tmp = (BYTE *)calloc(Str_wid * Str_hei, sizeof(BYTE));
	for (i = 0; i < LANDMARK_COUNT - 1; i++) {
		Bresenham(Landmark[i].x, Landmark[i].y, Landmark[i + 1].x, Landmark[i + 1].y, Tmp, Str_wid, Str_hei);
	}
	int adr = 0;
	for (i = 0; i < Str_hei; i++) {
		for (j = 0; j < Str_wid; j++) {
			if (Tmp[adr] != 0) {
				cv::Point p;
				p.x = j;
				p.y = i;
				uchar fvalue = GC_FGD;
				circle(mask, p, radius, fvalue, thickness);	
			}
			adr++;
		}
	}
	SafeMemFree(Tmp);

	cv::Mat inImg(Str_hei, Str_wid, CV_8UC(3), pProRGBImg);
	g_Rect.x = 0;
	g_Rect.y = 0;
	g_Rect.width = mask.cols;
	g_Rect.height = mask.rows;
	inImg.copyTo(g_Image);
	mask.copyTo(g_Mask);
	
    gcapp.setImageAndWinName(inImg, "main");
    gcapp.showImage();

    gcapp.rect = g_Rect;
    g_Mask.copyTo(gcapp.mask);
    gcapp.rectState = gcapp.SET;
    gcapp.lblsState = gcapp.SET;
    
    return 0;
	const string winName = "image";	
//----------------Add new callbacks--------
	setMouseCallback(winName, on_mouse, 0);
	gcapp.setImageAndWinName(inImg, winName);
	gcapp.showImage();
	for (;;)
	{
		char c = (char)waitKey(0);
		switch (c)
		{
		case 26:
			gcapp.setPrevMask();
			gcapp.showImage();
			break;
		case 27:
			goto exit_main;
		case 'r':
			cout << endl;
			gcapp.reset();
			gcapp.showImage();
			break;
		case 'c':
			gcapp.setLblsInMask();
			int iterCount = gcapp.getIterCount();
			cout << "<" << iterCount << "... ";
			int newIterCount = gcapp.nextIter();
			if (newIterCount > iterCount) {
				imshow("Mask Image", gcapp.mask * 255 / 3);
				gcapp.showImage();
				cout << iterCount << ">" << endl;
			}
			else
				cout << "rect must be determined>" << endl;
			break;
		}
		if (cvGetWindowHandle(winName.c_str()) == NULL) {
			goto exit_main;
		}
	}
exit_main:
	destroyWindow(winName);
 	SafeMemFree(pImage);
	SafeMemFree(Alpha);
	faceInfo.StruFace.clear();
	faceInfo.StruProFace.clear();
	SafeMemFree(pOutBinImg);
 	SafeMemFree(pProRGBImg);
	inImg.release();
	g_Image.release();
	mask.release();	

	return 0; 
}
int PushPoints(vector<cv::Point> aryPoint, bool isBackground){
    if(isBackground == true) {
        gcapp.bgdPxls.clear();
        for(int i=0; i<aryPoint.size(); i++) {
            gcapp.bgdPxls.push_back(aryPoint[i]);
        }
    } else {
        gcapp.fgdPxls.clear();
        for(int i=0; i<aryPoint.size(); i++) {
            gcapp.fgdPxls.push_back(aryPoint[i]);
        }
    }
    gcapp.setLblsInMask();
    return 0;
}
Mat ApplyImage() {
    gcapp.setLblsInMask();
    int iterCount = gcapp.getIterCount();
    cout << "<" << iterCount << "... ";
    int newIterCount = gcapp.nextIter();
    if (newIterCount > iterCount) {
        //imshow("Mask Image", gcapp.mask * 255 / 3);
        gcapp.showImage();
        cout << iterCount << ">" << endl;
    }
    return gcapp.imageResult;
}
