// #include "Const.h"
// #include "Comon.h"
#include "LandmarkDetect.h"
#include <math.h>
#include "MainProc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*******************************************************************/
//----------------- Landmark Points Detection Part -----------------/
/*******************************************************************/
LPRCPRMODEL g_pRcprModel = NULL;
ACTIVE_POINT g_pLandmark[LANDMARK_COUNT] = {0,};
static DWORD pdwRcpr_30[136] = {
	0xBF98CBBD ,0xBF96AF48 ,0xBF91A55B ,0xBF8A17D7,
	0xBF795FD2 ,0xBF4CF619 ,0xBF13B206 ,0xBEA19C34,
	0xBB9F009B ,0x3E9CA42F ,0x3F113604 ,0x3F4A7A17,
	0x3F76E3CF ,0x3F88D9D6 ,0x3F90675A ,0x3F957147,
	0x3F978DBC ,0xBF76F913 ,0xBF51F007 ,0xBF1CDFC1,
	0xBECC36CB ,0xBE501956 ,0x3E46294D ,0x3EC73EC6,
	0x3F1A63BE ,0x3F4F7404 ,0x3F747D10 ,0xBB9F009B,
	0xBB9F009B ,0xBB9F009B ,0xBB9F009B ,0xBE7EEE75,
	0xBE06A40E ,0xBB9F009B ,0x3DF96809 ,0x3E74FE6B,
	0xBF3AC6A7 ,0xBF1A8099 ,0xBEE0F570 ,0xBE993DA6,
	0xBEE3B23B ,0xBF1C014C ,0x3E9445A1 ,0x3EDBFD6B,
	0x3F180497 ,0x3F384AA5 ,0x3F19854A ,0x3EDEBA36,
	0xBEDE67C8 ,0xBE8C6FA3 ,0xBDF407B5 ,0xBB9F009B,
	0x3DE027A2 ,0x3E87779E ,0x3ED96FC3 ,0x3E89D19A,
	0x3DF1E860 ,0xBB9F009B ,0xBE02E43A ,0xBE8EC99E,
	0xBEB9B081 ,0xBDF7600B ,0xBB9F009B ,0x3DE37FF8,
	0x3EB4B87C ,0x3DE6388E ,0xBB9F009B ,0xBDFA18A1,
	0xBED6E103 ,0xBDD77368 ,0x3E4FA845 ,0x3F018B7C,
	0x3F4AE5F9 ,0x3F857F94 ,0x3F9FEECF ,0x3FB464DE,
	0x3FBB0933 ,0x3FB464DE ,0x3F9FEECF ,0x3F857F94,
	0x3F4AE5F9 ,0x3F018B7C ,0x3E4FA845 ,0xBDD77368,
	0xBED6E103 ,0xBF29099B ,0xBF4BB77B ,0xBF56C85C,
	0xBF4F1318 ,0xBF39987C ,0xBF39987C ,0xBF4F1318,
	0xBF56C85C ,0xBF4BB77B ,0xBF29099B ,0xBEFC5BB9,
	0xBE966663 ,0xBDC6A544 ,0x3DD949DF ,0x3E8C2CD1,
	0x3EA07671 ,0x3EB10BC7 ,0x3EA07671 ,0x3E8C2CD1,
	0xBEE2CB69 ,0xBF040630 ,0xBF038A22 ,0xBED2C2C5,
	0xBEC6D3C2 ,0xBEC68AA8 ,0xBED2C2C5 ,0xBF038A22,
	0xBF040630 ,0xBEE2CB69 ,0xBEC68AA8 ,0xBEC6D3C2,
	0x3F2D48D6 ,0x3F171AE5 ,0x3F0AEAF6 ,0x3F12E117,
	0x3F0AEAF6 ,0x3F171AE5 ,0x3F2D48D6 ,0x3F4F4BDA,
	0x3F5E3C4D ,0x3F615844 ,0x3F5E3C4D ,0x3F4F4BDA,
	0x3F2DD40F ,0x3F289862 ,0x3F2B93A9 ,0x3F289862,
	0x3F2DD40F ,0x3F35BF9B ,0x3F3902B8 ,0x3F35BF9B
};

static DWORD g_dwNormalLandmarkVert[56] = {
	0xBE37DF3D, 0x3E37DF3D, 0x247A9427, 0x23C43914,
	0xA385364F, 0xA45D6F66, 0xBE9670D5, 0x3E9670D5,
	0xBF272809, 0xBE8E153B, 0x3E8E153B, 0x3F272809,
	0xBEE1A93F, 0x3EE1A93F, 0x3EA6C297, 0x3EA6C297,
	0x3E59CD3F, 0x3DD58360, 0xBC51BE97, 0xBDFC0C0B,
	0xBE809C87, 0xBE809C87, 0x3E339920, 0x3E2BB65C,
	0x3E2BB65C, 0x3E339920, 0xBF021488, 0xBF021488,
	0x3E16415C, 0x3E16415C, 0x3E761835, 0x3F17CF0E,
	0x3F8631FB, 0x3FA612A0, 0xBD71542C, 0xBD71542C,
	0xBF31B70A, 0xBF0A2748, 0xBF0A2748, 0xBF31B70A,
	0xBEE317D6, 0xBEE317D6, 0x3D80E2E5, 0x3D80E2E5,
	0x3DD1C138, 0x3E53BD9F, 0x3EAD37CC, 0x3ED55D22,
	0x3DBBCC19, 0x3DBBCC19, 0xBE03BF35, 0xBDB54228,
	0xBDB54228, 0xBE03BF35, 0x3CF925F7, 0x3CF925F7
};
static DWORD g_dwNormalLandmarkHorz[56] = {
	0xBDE147AE, 0x3EE66666, 0xBE6B851F, 0x3F800000,
	0x3DE147AE, 0x3EE66666, 0xBE6B851F, 0x3F800000,
	0x00000000, 0x3EAE147B, 0xBE570A3D, 0x3F800000,
	0x00000000, 0x3E6B851F, 0xBE0F5C29, 0x3F800000,
	0x00000000, 0x3DE147AE, 0xBD4CCCCD, 0x3F800000,
	0x00000000, 0x00000000, 0x00000000, 0x3F800000,
	0xBE3851EC, 0xBDCCCCCD, 0xBE851EB8, 0x3F800000,
	0x3E3851EC, 0xBDCCCCCD, 0xBE851EB8, 0x3F800000,
	0xBECCCCCD, 0x3EA3D70A, 0xBEC7AE14, 0x3F800000,
	0xBE2E147B, 0x3E9EB852, 0xBEB851EC, 0x3F800000,
	0x3E2E147B, 0x3E9EB852, 0xBEB851EC, 0x3F800000,
	0x3ECCCCCD, 0x3EA3D70A, 0xBEC7AE14, 0x3F800000,
	0xBE8A3D71, 0xBEAE147B, 0xBEA8F5C3, 0x3F800000,
	0x3E8A3D71, 0xBEAE147B, 0xBEA8F5C3, 0x3F800000
};
static float flt_10225494[121] = {0.0f, 0.0f, 0.64749998f, 0.4883f, 0.54729998f, -0.61900002f, 0.052700002f, 
	-0.389f, 0.37f, 0.86610001f, -0.78189999f, 0.81059998f, -0.486f,
	0.8527f, 0.60909998f, -0.4154f, -0.93309999f, -0.85659999f,
	0.15629999f, 0.29719999f, 0.041200001f, -0.8937f, -0.1736f,
	0.4576f, -0.24079999f, 0.2859f, -0.104f, -0.98720002f, -0.41999999f,
	0.56529999f, -0.86180001f, 0.063500002f, 0.93290001f, -0.88129997f,
	-0.28420001f, 0.59960002f, -0.33489999f, 0.5176f, 0.2145f,
	-0.37729999f, 0.4463f, 0.2516f, -0.88319999f, 0.88639998f,
	0.61409998f, 0.7694f, 0.23710001f, 0.7295f, -0.77399999f,
	-0.4461f, 0.62159997f, 0.94010001f, -0.6638f, 0.56519997f,
	-0.65789998f, -0.3475f, -0.60439998f, 0.5018f, 0.63230002f,
	-0.90539998f, 0.75910002f, 0.73729998f, -0.17299999f, 0.1425f,
	0.50459999f, 0.77319998f, -0.88810003f, 0.83380002f, -0.89209998f,
	0.92369998f, 0.745f, -0.3592f, -0.66689998f, 0.86199999f,
	-0.9558f, -0.63450003f, -0.37059999f, 0.66799998f, 0.7414f,
	0.021f, 0.67839998f, 0.57169998f, -0.30469999f, -0.96340001f,
	-0.91049999f, 0.0546f, -0.1274f, 0.30649999f, 0.80849999f,
	0.3484f, -0.81190002f, -0.96490002f, 0.73449999f, -0.95859998f,
	-0.81760001f, -0.46540001f, -0.19490001f, 0.71539998f, -0.72180003f,
	-0.91589999f, 0.053599998f, -0.30689999f, 0.1f};
static BYTE g_pbyUsefulLandmarkIndexs[14] = {0x15, 0x16, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x23, 0x24, 0x27, 0x2A, 0x2D, 0x30, 0x36};
#define	PI								3.1415926535897932384626433832795028841971693993751

LPRCPRMODEL FillRcprModel(char* path)
{
    int i, j;
    LPRCPRMODEL rst = 0;
    char strPathDat[1024];
    
    float* hgPn4_flt4    = NULL;//(float*)calloc(87040000/4, sizeof(float));
    short* hgPn4_sh8    = NULL;//(short*)calloc(53248/2, sizeof(short));
    short* hgPn4_shC    = NULL;//(short*)calloc(53248/2, sizeof(short));
    float* hgPn4_flt10    = NULL;//(float*)calloc(102400/4, sizeof(float));
    short* hg_psh_10    = NULL;//(short*)calloc(8192/2, sizeof(short));
    short* hg_psh_14    = NULL;//(short*)calloc(8192/2, sizeof(short));
    float* hg_pflt_18    = NULL;//(float*)calloc(16384/4, sizeof(float));
    
    sprintf(strPathDat, "%s/rcpr_38_pn_4_flt_4.mem", path);
    hgPn4_flt4 = (float*)read_file(strPathDat, 87040000); //data\rcpr_38_pn_4_flt_4.mem
    if (hgPn4_flt4 == NULL) return NULL;
    
    sprintf(strPathDat, "%s/rcpr_38_pn_4_sh_8.mem", path);
    hgPn4_sh8 = (short*)read_file(strPathDat, 53248); //data\rcpr_38_pn_4_sh_8.mem
    if (hgPn4_sh8 == NULL) return NULL;
    
    sprintf(strPathDat, "%s/rcpr_38_pn_4_sh_C.mem", path);
    hgPn4_shC = (short*)read_file(strPathDat, 53248); //data\rcpr_38_pn_4_sh_C.mem
    if (hgPn4_shC == NULL) return NULL;
    
    sprintf(strPathDat, "%s/rcpr_38_pn_4_flt_10.mem", path);
    hgPn4_flt10 = (float*)read_file(strPathDat, 102400); //data\rcpr_38_pn_4_flt_10.mem
    if (hgPn4_flt10 == NULL) return NULL;
    
    sprintf(strPathDat, "%s/rcpr_38_psh_10.mem", path);
    hg_psh_10 = (short*)read_file(strPathDat, 8192); //data\rcpr_38_psh_10.mem
    if (hg_psh_10 == NULL) return NULL;
    
    sprintf(strPathDat, "%s/rcpr_38_psh_14.mem", path);
    hg_psh_14 = (short*)read_file(strPathDat, 8192); //data\rcpr_38_psh_14.mem
    if (hg_psh_14 == NULL) return NULL;
    
    sprintf(strPathDat, "%s/rcpr_38_pflt_18.mem", path);
    hg_pflt_18 = (float*)read_file(strPathDat, 16384); //data\rcpr_38_pflt_18.mem
    if (hg_pflt_18 == NULL) return NULL;
    
    rst = (LPRCPRMODEL)malloc(sizeof(RCPRMODEL));
    if (rst == NULL) return NULL;
    
    rst->nCntLandmark_24 = LANDMARK_COUNT;
    rst->n_28 = LANDMARK_COUNT * 2;
    rst->flt_2C = 2.0;
    rst->pflt_30 = (float*)&pdwRcpr_30;
    rst->nCntStu_38 = 10;
    rst->pstu_38 = (LPRCPR_38)malloc(sizeof(RCPR_38) * rst->nCntStu_38);
    
    for (i = 0; i < rst->nCntStu_38; i++) {
        rst->pstu_38[i].nCntStu_4 = 500;
        rst->pstu_38[i].pstu_4 = (LPRCPR_38_4)malloc(sizeof(RCPR_38_4) * rst->pstu_38[i].nCntStu_4);
        for (j = 0; j < rst->pstu_38[i].nCntStu_4; j++) {
            rst->pstu_38[i].pstu_4[j].nCnt_0 = 5;
            rst->pstu_38[i].pstu_4[j].pflt_4 = hgPn4_flt4; hgPn4_flt4 += rst->n_28 * 32;
            rst->pstu_38[i].pstu_4[j].psh_8 = hgPn4_sh8; hgPn4_sh8 += rst->pstu_38[i].pstu_4->nCnt_0;
            rst->pstu_38[i].pstu_4[j].psh_C = hgPn4_shC; hgPn4_shC += rst->pstu_38[i].pstu_4->nCnt_0;
            rst->pstu_38[i].pstu_4[j].pflt_10 = hgPn4_flt10; hgPn4_flt10 += rst->pstu_38[i].pstu_4->nCnt_0;
        }
        rst->pstu_38[i].n_8 = 4;
        rst->pstu_38[i].n_C = 400;
        rst->pstu_38[i].psh_10 = hg_psh_10; hg_psh_10 += rst->pstu_38[i].n_C;
        rst->pstu_38[i].psh_14 = hg_psh_14; hg_psh_14 += rst->pstu_38[i].n_C;
        rst->pstu_38[i].pflt_18 = hg_pflt_18; hg_pflt_18 += rst->pstu_38[i].n_C;
    }
    
    return rst;
}
int InitRCPRModel(char* path){
	g_pRcprModel = 	FillRcprModel(path);
	if(!g_pRcprModel) {
		return -1;
	}
	return 0;
}
int ReleaseRCPRModel(){
    int i;
    
    for (i = 0; i < g_pRcprModel->nCntStu_38; i++) {
        if (i == 0) {
            SafeMemFree(g_pRcprModel->pstu_38[i].pstu_4[0].pflt_4);
            SafeMemFree(g_pRcprModel->pstu_38[i].pstu_4[0].psh_8);
            SafeMemFree(g_pRcprModel->pstu_38[i].pstu_4[0].psh_C);
            SafeMemFree(g_pRcprModel->pstu_38[i].pstu_4[0].pflt_10);
            SafeMemFree(g_pRcprModel->pstu_38[i].psh_10);
            SafeMemFree(g_pRcprModel->pstu_38[i].psh_14);
            SafeMemFree(g_pRcprModel->pstu_38[i].pflt_18);
        }
        SafeMemFree(g_pRcprModel->pstu_38[i].pstu_4);
    }
    SafeMemFree(g_pRcprModel->pstu_38);
    SafeMemFree(g_pRcprModel);
    
    return 0;
}
int MakeTmpTbl30(LPRCPRMODEL hRcprModel, int nCntTmp, float fltCoeff, float** ppfltTmpTbl30)
{
	float* pfltTmpTbl30 = 0;
	int rst = 0;
	int i, j;

	pfltTmpTbl30 = (float*)calloc(hRcprModel->n_28 * nCntTmp, sizeof(float));
	if(pfltTmpTbl30 == NULL)
		return rst;
	for(i = 0; i < nCntTmp; i++) {
		memcpy(&pfltTmpTbl30[i * hRcprModel->n_28], hRcprModel->pflt_30, hRcprModel->n_28 * sizeof(float));
		for(j = 0; j < hRcprModel->nCntLandmark_24; j++){
			pfltTmpTbl30[i * hRcprModel->n_28 + j] += fltCoeff * flt_10225494[i * 2];
			pfltTmpTbl30[i * hRcprModel->n_28 + hRcprModel->nCntLandmark_24 + j] += fltCoeff * flt_10225494[i * 2 + 1];
		}
	}
	*ppfltTmpTbl30 = pfltTmpTbl30;
	return rst;
}
int MakeTmpTbl34(LPRCPRMODEL hRcprModel, int nCntTmp, float* pfltTmpTbl30, float* pfltNormalEyeRect, float fltSin, float fltCos, float* pfltTmpTbl34)
{
	int i, j;
	float fltRight = 0;
	float fltBottom = 0;
	float flt;

	fltRight = (float)(pfltNormalEyeRect[0] + pfltNormalEyeRect[2] * 0.5);
	fltBottom = (float)(pfltNormalEyeRect[1] + pfltNormalEyeRect[3] * 0.5);
	for(i = 0; i < nCntTmp; i++) { 
		for(j = 0; j < hRcprModel->nCntLandmark_24; j++){
			flt = pfltTmpTbl30[hRcprModel->n_28 * i + j] * fltCos;
			flt -= pfltTmpTbl30[hRcprModel->n_28 * i + hRcprModel->nCntLandmark_24 + j] * fltSin;
			flt = (float)(flt * pfltNormalEyeRect[2] * 0.5);
			flt = flt + fltRight;
			pfltTmpTbl34[hRcprModel->n_28 * i + j] = flt;

			flt = pfltTmpTbl30[hRcprModel->n_28 * i + j] * fltSin;
			flt += pfltTmpTbl30[hRcprModel->n_28 * i + hRcprModel->nCntLandmark_24 + j] * fltCos;
			flt = (float)(flt * pfltNormalEyeRect[3] * 0.5);
			flt += fltBottom;
			pfltTmpTbl34[hRcprModel->n_28 * i + hRcprModel->nCntLandmark_24 + j] = flt;
		}
	}
	return 0;
}
int MakeTmpTblFromRCPR(LPRCPRMODEL hRcprModel, int nCntTmp, float* pfltTmpTbl34, int w, int h, int s, BYTE* pImage, LPRCPR_38 pRCPR_38, float* pfltTmpTbl)
{
	int rst = 0;
	int i, j;
	double fltX;
	double fltY;
	int nX, nY;
	double flt1, flt2, flt3, flt4;

	if(nCntTmp <= 0) return 0;
	for(i = 0; i < nCntTmp; i++) {
		for(j = 0; j < pRCPR_38->n_C; j++) {

			fltY = pfltTmpTbl34[hRcprModel->nCntLandmark_24 + hRcprModel->n_28 * i + pRCPR_38->psh_10[j]];
			flt4 = pfltTmpTbl34[hRcprModel->n_28 * i + pRCPR_38->psh_14[j]] - pfltTmpTbl34[hRcprModel->n_28 * i + pRCPR_38->psh_10[j]];
			flt2 = pfltTmpTbl34[hRcprModel->nCntLandmark_24 + hRcprModel->n_28 * i + pRCPR_38->psh_14[j]] - pfltTmpTbl34[hRcprModel->nCntLandmark_24 + hRcprModel->n_28 * i + pRCPR_38->psh_10[j]];
			flt3 = pRCPR_38->pflt_18[j];
			flt1 = flt4 / 2;
			fltX = flt1 + pfltTmpTbl34[hRcprModel->n_28 * i + pRCPR_38->psh_10[j]] + flt3 * flt1;
			if(fabs(flt1) <= 0.5) {
				flt3 += 1.0;
				flt2 *= 0.5;
				flt2 *= flt3;
				fltY += flt2;
			}else{
				flt2 /= flt4;
				flt1 = flt2 * pfltTmpTbl34[hRcprModel->n_28 * i + pRCPR_38->psh_10[j]];
				fltY -= flt1;
				flt2 *= fltX;
				fltY += flt2;
			}
			if(fltX > 1) {
				nX = (int)fltX - 1;
			}else{
				nX = 0;
			}
			if(fltY > 1) {
				nY = (int)fltY - 1;
			}else{
				nY = 0;
			}
			if(nX >= w)
				nX = w - 1;
			if(nY >= h)
				nY = h - 1;
			pfltTmpTbl[i * pRCPR_38->n_C + j] = (float)pImage[nX + nY * s] / 255;
		}
	}
	return 0;
}
int ChangeTmpTbl30(LPRCPRMODEL hRcprModel, int nCntTmp, float* pfltTmpTbl, LPRCPR_38 pRCPR_38, float* pfltTmpTbl30)
{
	int rst = 0;
	int i, j, k;
	float fltRCPR_38Ele, fltTmpTblEle;
	int iStu_4_pflt_4 = 0;

	if(nCntTmp <= 0)
		return rst;
	for(i = 0; i < nCntTmp; i++) {
		for(j = 0; j < pRCPR_38->nCntStu_4; j++) {
			iStu_4_pflt_4 = 0;
			for(k = 0; k < pRCPR_38->pstu_4[j].nCnt_0; k++){
				fltRCPR_38Ele = pRCPR_38->pstu_4[j].pflt_10[k];
				fltTmpTblEle = pfltTmpTbl[pRCPR_38->n_C * i + pRCPR_38->pstu_4[j].psh_8[k]] - pfltTmpTbl[pRCPR_38->n_C * i + pRCPR_38->pstu_4[j].psh_C[k]];
				iStu_4_pflt_4 *= 2;
				if(fltRCPR_38Ele > fltTmpTblEle) iStu_4_pflt_4++;
			}
			for(k = 0; k < hRcprModel->n_28; k++) {
				pfltTmpTbl30[i * hRcprModel->n_28 + k] += pRCPR_38->pstu_4[j].pflt_4[iStu_4_pflt_4 * hRcprModel->n_28 + k];
			}
		}
	}
	return rst;
}
int ChangeTmpTbl34(LPRCPRMODEL hRcprModel, int nCntTmp, float* pfltTmpTbl30, float* pfltTmpTbl34, float* pfltNormalEyeRect, float fltSin, float fltCos, BYTE* pImage, int w, int h, int s)
{
	int rst = 0;
	int i;
	float* pfltTmpTbl = (float*)calloc(hRcprModel->pstu_38[0].n_C * nCntTmp, sizeof(float));
	if(rst < 0)
		return rst;
	for(i = 0; i < hRcprModel->nCntStu_38; i++) {
		rst = MakeTmpTblFromRCPR(hRcprModel, nCntTmp, pfltTmpTbl34, w, h, s, pImage, &hRcprModel->pstu_38[i], pfltTmpTbl);
		if(rst < 0) {
			free(pfltTmpTbl);
			return rst;
		}
		rst = ChangeTmpTbl30(hRcprModel, nCntTmp, pfltTmpTbl, &hRcprModel->pstu_38[i], pfltTmpTbl30);
		if(rst < 0) {
			free(pfltTmpTbl);
			return rst;
		}
		rst = MakeTmpTbl34(hRcprModel, nCntTmp, pfltTmpTbl30, pfltNormalEyeRect, fltSin, fltCos, pfltTmpTbl34);
		if(rst < 0) {
			free(pfltTmpTbl);
			return rst;
		}
	}
	free(pfltTmpTbl);
	return rst;
}
int MakeTmpTblFromTmpTbl34(int nCntTmp, int nCntTmpTbl38, float* arg_8, float** ppTmpTbl)
{
	int rst = 0;
	int i, j;
	int nCntActivePointByEle;
	int nCntActivePoint;
	float *pTmpTbl;

	pTmpTbl = (float*)calloc(nCntTmp * nCntTmpTbl38, sizeof(float));
	if(rst < 0)
		return rst;

	nCntActivePoint = (nCntTmpTbl38 - 4) / 4 + 1;
	nCntActivePointByEle = nCntActivePoint * 4;
	for(i = 0; i < nCntTmp; i++) {
		if(nCntTmpTbl38 >= 4){
			for(j = 0; j < nCntActivePoint; j++) {
				pTmpTbl[i + nCntTmp * 4 * j] = arg_8[i * nCntTmpTbl38 + j * 4];
				pTmpTbl[nCntTmp + i + nCntTmp * 4 * j] = arg_8[i * nCntTmpTbl38 + j * 4 + 1];
				pTmpTbl[nCntTmp * 2 + i + nCntTmp * 4 * j] = arg_8[i * nCntTmpTbl38 + j * 4 + 2];
				pTmpTbl[nCntTmp * 3 + i + nCntTmp * 4 * j] = arg_8[i * nCntTmpTbl38 + j * 4 + 3];
			}
			for(j = 0; j < nCntTmpTbl38 - nCntActivePointByEle; j++) {
				pTmpTbl[nCntActivePointByEle * nCntTmp + i + nCntTmp * j] = arg_8[nCntActivePointByEle + i * nCntTmpTbl38];
			}
		}
	}
	*ppTmpTbl = pTmpTbl;
	return rst;
}
float GetMidValAfterSortTbl(int nCntTmpTbl, float* pfltTmpTbl)
{
	float fltRst = 0;
	int i, j;
	for(i = 0; i < nCntTmpTbl - 1; i++){
		for(j = i + 1; j < nCntTmpTbl; j++){
			if(pfltTmpTbl[i] > pfltTmpTbl[j]){
				fltRst = pfltTmpTbl[i];
				pfltTmpTbl[i] = pfltTmpTbl[j];
				pfltTmpTbl[j] = fltRst;
			}
		}
	}
	fltRst = pfltTmpTbl[nCntTmpTbl / 2];
	return fltRst;
}
int MakeTmpTbl38and2C(LPRCPRMODEL hRcprModel, int nCntTmp, int nCntTmpTbl38, float* pfltTmpTbl30, float** ppfltTmpTbl38, float** ppfltTmpTbl2C)
{
	int rst = 0;
	int i, j;
	float* pfltTmpTbl38 = 0;
	float* pfltTmpTbl2C = 0;
	float* pfltTmpTbl = 0;
	float flt1 = 0;
	float flt2 = 0;
	float fltEle, fltEle1;

	rst = MakeTmpTblFromTmpTbl34(nCntTmp, nCntTmpTbl38, pfltTmpTbl30, &pfltTmpTbl);
	if(rst < 0)
		return rst;
	pfltTmpTbl38 = (float*)calloc(nCntTmpTbl38, sizeof(float));
	if(pfltTmpTbl38 == NULL){
		SafeMemFree(pfltTmpTbl);
		return rst;
	}
	for(i = 0; i < nCntTmpTbl38; i++) {
		pfltTmpTbl38[i] = GetMidValAfterSortTbl(nCntTmp, &pfltTmpTbl[i * nCntTmp]);
	}
	if(nCntTmpTbl38 != hRcprModel->nCntLandmark_24 * 2){
		SafeMemFree(pfltTmpTbl);
		SafeMemFree(pfltTmpTbl38);
		return -1;
	}
	pfltTmpTbl2C = (float*)calloc(hRcprModel->nCntLandmark_24, sizeof(float));
	if(pfltTmpTbl2C == NULL){
		SafeMemFree(pfltTmpTbl);
		SafeMemFree(pfltTmpTbl38);
		SafeMemFree(pfltTmpTbl2C);
		return rst;
	}
	for(i = 0; i < hRcprModel->nCntLandmark_24; i++){
		flt1 = 0;
		flt2 = 0;
		fltEle = 0;
		for(j = 0; j < nCntTmp; j++){
			fltEle1 = (pfltTmpTbl38[i] - pfltTmpTbl30[i + j * nCntTmpTbl38]) * (pfltTmpTbl38[i] - pfltTmpTbl30[i + j * nCntTmpTbl38]);
			fltEle1 += (pfltTmpTbl38[i + hRcprModel->nCntLandmark_24] - pfltTmpTbl30[i + j * nCntTmpTbl38 + hRcprModel->nCntLandmark_24])*
				(pfltTmpTbl38[i + hRcprModel->nCntLandmark_24] - pfltTmpTbl30[i + j * nCntTmpTbl38 + hRcprModel->nCntLandmark_24]);
			fltEle = sqrt(fltEle1);
			flt1 += fltEle;
			flt2 += fltEle1;
		}
		fltEle = sqrt(flt2 / nCntTmp - (flt1 / nCntTmp) * (flt1 / nCntTmp));
		pfltTmpTbl2C[i] = fltEle;
	}
	*ppfltTmpTbl38 = pfltTmpTbl38;
	*ppfltTmpTbl2C = pfltTmpTbl2C;
	SafeMemFree(pfltTmpTbl);
	return rst;
}
int FillGridLandmark(LPRCPRMODEL hRcprModel, BYTE* pImage, int w, int h, int s, LPACTIVE_POINT pRightEyeCenter, LPACTIVE_POINT pLeftEyeCenter, DWORD nCntTmp, int nCntLandmark, LPACTIVE_POINT pLandmark){
	float* pfltTmpTbl_38 = 0;
	float* pfltTmpTbl_34 = 0;
	float* pfltTmpTbl_30 = 0;
	float* pfltTmpTbl_2C = 0;
	float flt2 = 0;
	float flt1 = 0;
	float fltDistanceBetweenEyes_14 = 0;
	float pfltNormalEyeRect[4];
	float xmm1;
	int rst = 0;
	int i;

	if(!hRcprModel){
		return -11;
	}
	if(!pImage || w <= 0 || h <= 0 || s < w) {
		return -11;
	}
	if(!pRightEyeCenter) {
		return -11;
	}
	if(!pLeftEyeCenter) {
		return -11;
	}
	if(!pLandmark) {
		return -11;
	}
	if(pRightEyeCenter->active == 254 || pRightEyeCenter->active == 255){
		return -11;
	}
	if(pLeftEyeCenter->active == 254 || pLeftEyeCenter->active == 255){
		return -11;
	}
	if(hRcprModel->nCntLandmark_24 != nCntLandmark) {
		return -11;
	}
	flt1 = (float)(pRightEyeCenter->x - pLeftEyeCenter->x);
	flt2 = (float)(pLeftEyeCenter->y - pRightEyeCenter->y);
	fltDistanceBetweenEyes_14 = hRcprModel->flt_2C * sqrt(flt1 * flt1 + flt2 * flt2);
	pfltNormalEyeRect[2] = fltDistanceBetweenEyes_14;
	pfltNormalEyeRect[3] = fltDistanceBetweenEyes_14;
	pfltNormalEyeRect[0] = (flt2 - flt1) / 2 + pRightEyeCenter->x - fltDistanceBetweenEyes_14 / 2;
	pfltNormalEyeRect[1] = (flt2 + flt1) / 2 + pRightEyeCenter->y - fltDistanceBetweenEyes_14 / 2;
	flt2 = atan2(flt2, flt1);
	flt1 = sin(flt2);
	flt2 = cos(flt2);
	rst = MakeTmpTbl30(hRcprModel, nCntTmp, 0.0625, &pfltTmpTbl_30);
	if(rst < 0) return rst;

	pfltTmpTbl_34 = (float*)calloc(hRcprModel->n_28 * nCntTmp, sizeof(float));
	if(pfltTmpTbl_34 == NULL) {
		SafeMemFree(pfltTmpTbl_30);
		return rst;
	}
	rst = MakeTmpTbl34(hRcprModel, nCntTmp, pfltTmpTbl_30, pfltNormalEyeRect, flt1, flt2, pfltTmpTbl_34);
	if(rst < 0) {
		SafeMemFree(pfltTmpTbl_30);
		SafeMemFree(pfltTmpTbl_34);
		return rst;
	}
	rst = ChangeTmpTbl34(hRcprModel, nCntTmp, pfltTmpTbl_30, pfltTmpTbl_34, pfltNormalEyeRect, flt1, flt2, pImage, w, h, s);
	if(rst < 0) {
		SafeMemFree(pfltTmpTbl_30);
		SafeMemFree(pfltTmpTbl_34);
		return rst;
	}
	rst = MakeTmpTbl38and2C(hRcprModel, nCntTmp, hRcprModel->n_28, pfltTmpTbl_34, &pfltTmpTbl_38, &pfltTmpTbl_2C);
	if(rst < 0) {
		SafeMemFree(pfltTmpTbl_30);
		SafeMemFree(pfltTmpTbl_34);
		return rst;
	}
	for(i = 0; i < nCntLandmark; i++){
		xmm1 = pfltTmpTbl_2C[i] / fltDistanceBetweenEyes_14;
		if(xmm1 > 0.1) xmm1 = (float)0.1;
		pLandmark[i].active = (BYTE)((0.1 - xmm1) * 100 / 0.1 + 0.5);
		xmm1 = pfltTmpTbl_38[i];
		if(xmm1 < 0)
			xmm1 -= 0.5;
		else
			xmm1 += 0.5;
		pLandmark[i].x = (short)xmm1;
		xmm1 = pfltTmpTbl_38[hRcprModel->nCntLandmark_24 + i];
		if(xmm1 < 0)
			xmm1 -= 0.5;
		else
			xmm1 += 0.5;
		pLandmark[i].y = (short)xmm1;
	}
	SafeMemFree(pfltTmpTbl_2C);
	SafeMemFree(pfltTmpTbl_30);
	SafeMemFree(pfltTmpTbl_34);
	SafeMemFree(pfltTmpTbl_38);
	return rst;
}
int DetectEye(LPACTIVE_POINT lpLandmark, BYTE *pbyEyeIndex, LPACTIVE_POINT lpEyePoint){
	int nIndex = 4;
	int nX = 0, nY = 0, nActive= 0;
	for (int i = 0; i < nIndex; i++) {
		nX += lpLandmark[pbyEyeIndex[i]].x;
		nY += lpLandmark[pbyEyeIndex[i]].y;
		nActive += lpLandmark[pbyEyeIndex[i]].active;
	}
	lpEyePoint->x = (2*nX + nIndex)/ (2*nIndex);
	lpEyePoint->y = (2*nY +nIndex) / (2*nIndex);
	lpEyePoint->active = (2*nActive +nIndex) / (2*nIndex);
	return 0;
}
int DetectRightEye(LPACTIVE_POINT lpLandmark, LPACTIVE_POINT lpRightEye){
	BYTE lpEyeIndex[4] = {0x2B, 0x2C, 0x2E, 0x2F};
	return DetectEye(lpLandmark, lpEyeIndex, lpRightEye);
}
int DetectLeftEye(LPACTIVE_POINT lpLandmark, LPACTIVE_POINT lpLeftEye){
	BYTE lpEyeIndex[4] = {0x25, 0x26, 0x28, 0x29};
	return DetectEye(lpLandmark, lpEyeIndex, lpLeftEye);
}
int GetAnglesFinal(double* pdblInput, double* pdblOutput)
{
	double pdblTbl1[8] = {0,};
	double pdblTbl2[9] = {0,};
	double pdblTbl3[9] = {0,};
	float pfltTbl1[16] = {0,};
	float pfltTbl2[16] = {0,};
	double dbl;
	int i;

	dbl = 1 / sqrt(pdblInput[8] * pdblInput[8] + pdblInput[7] * pdblInput[7] + 2.220446049250313e-16);
	pdblTbl1[2] = pdblInput[7] * dbl;
	pdblTbl1[1] = pdblInput[8] * dbl;
	pdblTbl1[0] = -pdblTbl1[2];
	for(i = 0; i < 9; i += 3){
		pdblTbl3[i]     = pdblInput[i];
		pdblTbl3[i + 1] = pdblInput[i + 1] * pdblTbl1[1] + pdblInput[i + 2] * pdblTbl1[0];
		pdblTbl3[i + 2] = pdblInput[i + 1] * pdblTbl1[2] + pdblInput[i + 2] * pdblTbl1[1];
	}
	pdblTbl3[7] = 0;
	dbl = 1 / sqrt(pdblTbl3[6] * pdblTbl3[6] + pdblTbl3[8] * pdblTbl3[8] + 2.220446049250313e-16);
	pdblTbl1[0] = -pdblTbl3[6];
	pdblTbl1[3] = pdblTbl3[8] * dbl;
	dbl *= pdblTbl1[0];
	pdblTbl1[4] = dbl;
	pdblTbl1[7] = -dbl;
	for(i = 0; i < 9; i+= 3){
		pdblTbl2[i] = pdblTbl3[i] * pdblTbl1[3] + pfltTbl1[2] * pdblTbl3[i + 1] + pdblTbl3[i + 2] * pdblTbl1[4];
		pdblTbl2[i + 1] = pfltTbl1[0] * pdblTbl3[i] + pdblTbl3[i + 1] + pfltTbl1[6] * pdblTbl3[i + 1];
		pdblTbl2[i + 2] = pdblTbl1[7] * pdblTbl3[i] + pfltTbl1[4] * pdblTbl3[i + 1] + pdblTbl3[i + 2] * pdblTbl1[3];
	}
	pdblTbl2[6] = 0;
	dbl = 1 / sqrt(pdblTbl2[4] * pdblTbl2[4] + pdblTbl2[3] * pdblTbl2[3] + 2.220446049250313e-16);
	pdblTbl1[0] = pdblTbl2[4] * dbl;
	pdblTbl1[5] = pdblTbl2[3] * dbl;
	pdblTbl1[6] = -pdblTbl1[5];
	for(i = 0; i < 9; i+= 3){
		pdblTbl3[i] = pdblTbl2[i] * pdblTbl1[0] + pdblTbl2[i + 1] * pdblTbl1[6] + pdblTbl2[i + 2] * pfltTbl2[5];
		pdblTbl3[i + 1] = pdblTbl2[i] * pdblTbl1[5] + pdblTbl2[i + 1] * pdblTbl1[0] + pdblTbl2[i + 2] * pfltTbl2[6];
		pdblTbl3[i + 2] = pdblTbl2[i] * pfltTbl2[1] + pdblTbl2[i + 1] * pfltTbl2[4] + pdblTbl2[i + 2];
	}
	dbl = pdblTbl1[1];
	if(pdblTbl3[0] >= 0){
		if(pdblTbl3[4] < 0){
			pdblTbl1[5] = pdblTbl1[6];
			dbl = -pdblTbl1[1];
			pdblTbl1[4] = pdblTbl1[7];
			pdblTbl1[2] = -pdblTbl1[2];
		}
	}else{
		if(pdblTbl3[4] >= 0) {
			pdblTbl1[3] = - pdblTbl1[3];
			pdblTbl1[5] = pdblTbl1[6];
			pdblTbl1[4] = -pdblTbl1[4];
		}else{
			pdblTbl1[0] = -pdblTbl1[0];
			pdblTbl1[5] = -pdblTbl1[5];
		}
	}

	pdblOutput[0] = acos(dbl) * (pdblTbl1[2] >= 0 ? 1 : -1) * 57.29577951308232;
	pdblOutput[1] = acos(pdblTbl1[3]) * (pdblTbl1[4] >= 0 ? 1: -1) * 57.29577951308232;
	pdblOutput[2] = acos(pdblTbl1[0]) * (pdblTbl1[5] >= 0 ? 1: -1) * 57.29577951308232;
	return 0;
}
int GetAnglesFromLandmark14(int nCntLandmark, float* pfltLandmark, int nCntRepeat, float* pfltCenter, float fltThreshold, float* pfltYaw, float* pfltRoll, float* pfltPitch)
{
	double dbl0, dbl1, dbl2, dbl3, dbl4, dbl5;
	double* pdblTbl1 = 0;
	double* pdblTbl2 = 0;
	double* pdblTbl3 = 0;
	double pdblRst[4];
	double pdblCoord1[4];
	double pdblCoord2[4];
	double pdblCoord3[4];
	double pdblTbl4[9];
	float fltTmp;
	double dblTmp;
	float* pfltNormalLandmarkHorz = (float*)g_dwNormalLandmarkHorz;
	float* pfltNormalLandmarkVert = (float*)g_dwNormalLandmarkVert;
	int i, j, k = 0;
	int rst = 0;

	pdblTbl3 = (double*)calloc(nCntLandmark * 2, sizeof(double));
	if(pdblTbl3 == NULL)
		return rst;
	for(i = 0; i < nCntLandmark; i++) {
		fltTmp = (pfltLandmark[i * 2] - pfltCenter[0]) / fltThreshold;
		pdblTbl3[i * 2] = fltTmp;
		fltTmp = (pfltLandmark[i * 2 + 1] - pfltCenter[1]) / fltThreshold;
		pdblTbl3[i * 2 + 1] = fltTmp;
	}
	pdblTbl2 = (double*)calloc(nCntLandmark * 2, sizeof(double));
	if(rst < 0) {
		SafeMemFree(pdblTbl3);
		return rst;
	}
	for(i = 0; i < nCntLandmark; i++) {
		pdblTbl2[i * 2] = pdblTbl3[i * 2];
		pdblTbl2[i * 2 + 1] = pdblTbl3[i * 2 + 1];
	}
	pdblTbl1 = (double*)calloc(nCntLandmark * 2, sizeof(double));
	if(rst < 0) {
		SafeMemFree(pdblTbl3);
		SafeMemFree(pdblTbl2);
		return rst;
	}

	for(k = 0; k < nCntRepeat; k++) {
		for(i = 0; i < 4; i++) {
			pdblCoord3[i] = 0;
			pdblCoord1[i] = 0;
			for(j = 0; j < nCntLandmark; j++) {
				dblTmp = pfltNormalLandmarkVert[i * nCntLandmark + j];
				pdblCoord3[i] += dblTmp * pdblTbl2[j * 2];
				pdblCoord1[i] += dblTmp * pdblTbl2[j * 2 + 1];
			}
		}
		dblTmp = 1 / sqrt(pdblCoord1[0] * pdblCoord1[0] + pdblCoord1[1] * pdblCoord1[1] + pdblCoord1[2] * pdblCoord1[2]);
		dblTmp *= 1 / sqrt(pdblCoord3[0] * pdblCoord3[0] + pdblCoord3[1] * pdblCoord3[1] + pdblCoord3[2] * pdblCoord3[2]);
		dblTmp = sqrt(dblTmp);
		for(i = 0; i < 4; i++) {
			pdblRst[i] = pdblCoord3[i] * dblTmp;
			pdblCoord2[i] = pdblCoord1[i] * dblTmp;
		}
		for(i = 0; i < 3; i++) {
			if(pdblRst[i] > 1) {
				pdblRst[i] = 1;
			}else if(pdblRst[i] < -1) {
				pdblRst[i] = -1;
			}
			if(pdblCoord2[i] > 1) {
				pdblRst[i] = 1;
			}else if(pdblCoord2[i] < -1) {
				pdblCoord2[i] = -1;
			}
		}
		dbl4 = pdblCoord2[2] * pdblRst[1] - pdblCoord2[1] * pdblRst[2];
		dbl3 = pdblCoord2[0] * pdblRst[2] - pdblCoord2[2] *pdblRst[0];
		dbl2 = pdblCoord2[1] * pdblRst[0] - pdblCoord2[0] * pdblRst[1];
		dbl5 = 0;
		for(i = 0; i < nCntLandmark; i++) {
			pdblTbl1[i] = pfltNormalLandmarkHorz[i * 4 + 0] * dbl4 / dblTmp;
			pdblTbl1[i] += pfltNormalLandmarkHorz[i * 4 + 1] * dbl3 / dblTmp;
			pdblTbl1[i] += pfltNormalLandmarkHorz[i * 4 + 2] * dbl2 / dblTmp;
			pdblTbl1[i] += pfltNormalLandmarkHorz[i * 4 + 3];
		}
		for(i = 0; i < nCntLandmark; i++) {
			dbl0 = pdblTbl2[i * 2];
			dbl1 = pdblTbl2[i * 2 + 1];
			pdblTbl2[i * 2] = pdblTbl3[i * 2] * pdblTbl1[i];
			pdblTbl2[i * 2 + 1] = pdblTbl3[i * 2 + 1] * pdblTbl1[i];
			dbl0 = pdblTbl2[i * 2] - dbl0;
			dbl1 = pdblTbl2[i * 2 + 1] -dbl1;
			dbl5 += dbl0 * dbl0 + dbl1 * dbl1;
		}
		if(fltThreshold * fltThreshold * dbl5 <= 0.009999999776482582)
			break;
	};
	pdblTbl4[0] = pdblRst[0];
	pdblTbl4[1] = pdblRst[1];
	pdblTbl4[2] = pdblRst[2];
	pdblTbl4[3] = -pdblCoord2[0];
	pdblTbl4[4] = -pdblCoord2[1];
	pdblTbl4[5] = -pdblCoord2[2];
	pdblTbl4[6] = -dbl4;
	pdblTbl4[7] = -dbl3;
	pdblTbl4[8] = -dbl2;
	GetAnglesFinal(pdblTbl4, pdblRst);
	*pfltYaw = (float)(-pdblRst[2]);
	*pfltRoll = (float)(-pdblRst[1]);
	*pfltPitch = (float)(pdblRst[0]);
	SafeMemFree(pdblTbl3);
	SafeMemFree(pdblTbl2);
	SafeMemFree(pdblTbl1);
	return 0;
}
int GetAnglesFromLandmark68(LPACTIVE_POINT pLandmark, BYTE* pbyUsefulLandmarkIndexs, SRect* lpFaceRect, float* pfltYaw, float* pfltRoll, float* pfltPitch)
{
	float pfltCenterPos[2];
	float pfltLandmark[CNT_USING_LANDMARK * 2];
	int szLen;
	int i;
	int rst = 0;

	for(i = 0; i < CNT_USING_LANDMARK; i++) {
		if(pLandmark[pbyUsefulLandmarkIndexs[i]].active >= 0xFE)
			return 0;
		pfltLandmark[i * 2] = pLandmark[pbyUsefulLandmarkIndexs[i]].x;
		pfltLandmark[i * 2 + 1] = pLandmark[pbyUsefulLandmarkIndexs[i]].y;
	}
	szLen = lpFaceRect->width;
	if(szLen < 0)
		szLen++;
	pfltCenterPos[0] = (float)(szLen / 2 + lpFaceRect->x);
	szLen = lpFaceRect->height;
	if(szLen < 0)
		szLen++;
	pfltCenterPos[1] = (float)(szLen / 2 + lpFaceRect->y);
	rst = GetAnglesFromLandmark14(CNT_USING_LANDMARK, pfltLandmark, 100, pfltCenterPos, (float)(lpFaceRect->width * 20), pfltYaw, pfltRoll, pfltPitch);
	return rst;
}
int GetAngles(LPRCPRMODEL pRcprModel, BYTE* pImage, int nWidth, int nHeight, int s, LPACTIVE_POINT pRightEyeCenter, LPACTIVE_POINT pLeftEyeCenter, SRect* lpFaceRect, float* pfltAngleYaw, float* pfltAngleRoll, float* pfltAnglePitch)
{
	int rst = 0;
	int i;
	for(i = 0; i < LANDMARK_COUNT; i++) {
		g_pLandmark[i].active = 0xFE;
		g_pLandmark[i].index = 0x400 + i;
		g_pLandmark[i].x = 0;
		g_pLandmark[i].y = 0;
	}
	rst = FillGridLandmark(pRcprModel, pImage, nWidth, nHeight, s, pRightEyeCenter, pLeftEyeCenter, 15, LANDMARK_COUNT, g_pLandmark);
	if(rst < 0) return rst;
	rst = DetectLeftEye(g_pLandmark, pLeftEyeCenter);
	if(rst < 0) return rst;
	rst = DetectRightEye(g_pLandmark, pRightEyeCenter);
	if(rst < 0) return rst;
	rst = GetAnglesFromLandmark68(g_pLandmark,g_pbyUsefulLandmarkIndexs, lpFaceRect, pfltAngleYaw, pfltAngleRoll, pfltAnglePitch);
	return rst;
}
int GetEyesFromFaceRect(SRect* pFaceRect, float dblYaw, float dblRoll, float dblPitch, POINT_FLOAT *pRightEye, POINT_FLOAT *pLeftEye)
{
	int result = 0;
	float sin_value, cos_value;
	if (pRightEye != 0){
		if (pLeftEye!= 0){
			cos_value = (float)cos(dblPitch * PI / 180);
			sin_value = (float)sin(dblPitch * PI / 180);
			pRightEye->x =  (float)(((pFaceRect->width*0.25+pFaceRect->x - pFaceRect->width*dblYaw/180)-(pFaceRect->width*0.5+pFaceRect->x))*cos_value-((pFaceRect->height*0.25+pFaceRect->y)-(pFaceRect->height*0.5+pFaceRect->y))*sin_value+pFaceRect->width*0.5 + pFaceRect->x);
			pRightEye->y =  (float)(((pFaceRect->height*0.25+pFaceRect->y) - (pFaceRect->height*0.5+pFaceRect->y))*cos_value+((pFaceRect->width*0.25+pFaceRect->x-pFaceRect->width * dblYaw/180)-(pFaceRect->width*0.5+pFaceRect->x))*sin_value + pFaceRect->height*0.5 + pFaceRect->y);
			pLeftEye->x = (float)(((pFaceRect->width*0.75+pFaceRect->x - pFaceRect->width*dblYaw/180)-(pFaceRect->width*0.5+pFaceRect->x))*cos_value-((pFaceRect->height*0.25+pFaceRect->y)-(pFaceRect->height*0.5+pFaceRect->y))*sin_value+pFaceRect->width*0.5 + pFaceRect->x);
			pLeftEye->y = (float)(((pFaceRect->height*0.25+pFaceRect->y) - (pFaceRect->height*0.5+pFaceRect->y))*cos_value+((pFaceRect->width*0.75+pFaceRect->x-pFaceRect->width*dblYaw/180)-(pFaceRect->width*0.5+pFaceRect->x))*sin_value+pFaceRect->height*0.5 + pFaceRect->y);
		}else{
			result = -11;
		}
	}else{
		result = -11;
	}
	return result;
}

int LandMarkExtract(BYTE* pImage, int imgWidth, int imgHeight, SRect faceRect, SFaceStru* pFaceStru)
{
	int gap = 10;
	if(pImage == NULL) return -1;
	if ((faceRect.x+faceRect.width) > imgWidth || (faceRect.y+faceRect.height) > imgHeight || faceRect.x <= 0 || faceRect.y <= 0 ){
		return -1;
	}

	FACE_ATTRIBUTE faceAttribute;
	LPFACE_ATTRIBUTE pFaceAttribute = &faceAttribute;
	EYES_POINT eyesPoint;
	eyesPoint.leftEye.x = 0;

	ACTIVE_POINT leftEyeCenter = {450, 0, 0, 25};
	ACTIVE_POINT rightEyeCenter = {449, 0, 0, 25};
	POINT_FLOAT eyeLeft, eyeRight;
	GetEyesFromFaceRect(&faceRect, 0, 0, 0, &eyeLeft, &eyeRight);
	leftEyeCenter.x = (short)(eyeLeft.x + 0.5);
	leftEyeCenter.y = (short)(eyeLeft.y + 0.5);
	rightEyeCenter.x = (short)(eyeRight.x + 0.5);
	rightEyeCenter.y = (short)(eyeRight.y + 0.5);
	GetAngles(g_pRcprModel, pImage, imgWidth, imgHeight, imgWidth, &rightEyeCenter, &leftEyeCenter, &faceRect, &pFaceAttribute->dblRoll, &pFaceAttribute->dblYaw, &pFaceAttribute->dblPitch);

	if(pFaceStru != NULL) {
// 		for(int i=0; i<LANDMARK_COUNT; i++) {
// 			if(g_pLandmark[i].x > imgWidth - gap || g_pLandmark[i].y > imgHeight - gap || g_pLandmark[i].x < gap || g_pLandmark[i].y < gap) 
// 				return -1;
// 		}

		for(int i=0; i<LANDMARK_COUNT; i++) {			
			pFaceStru->ptCord[i].x = g_pLandmark[i].x;
			pFaceStru->ptCord[i].y = g_pLandmark[i].y;

		}
// 		pFaceStru->ptLEye.x = leftEyeCenter.x;
// 		pFaceStru->ptLEye.y = leftEyeCenter.y;
// 		pFaceStru->ptREye.x = rightEyeCenter.x;
// 		pFaceStru->ptREye.y = rightEyeCenter.y;
// 		pFaceStru->PoseAngle.dblYaw = pFaceAttribute->dblYaw;
// 		pFaceStru->PoseAngle.dblRoll = pFaceAttribute->dblRoll;
// 		pFaceStru->PoseAngle.dblPitch = pFaceAttribute->dblPitch;
	}
	return 0;
}






