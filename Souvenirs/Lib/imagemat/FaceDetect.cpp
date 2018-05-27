#include <vector>
#include "FaceDetect.h"
#include <math.h>

#if (__i386__ || __x86_64__)
#include "emmintrin.h"
#else
#include "SSE2NEON.h"
#endif

char g_pbyOrgImg[MAX_IMG_SIZE];
int g_nOrgWidth;
int g_nOrgHeight;
int g_nOrgImgSize;

char g_pbyShrinkedImg[MAX_IMG_SIZE];
int g_pnIntegralImg[MAX_IMG_SIZE * 4];
int g_pnSqrIntegralImg[MAX_IMG_SIZE * 4];
int g_nShrinkedWidth;
int g_nShrinkedHeight;
int g_nIntegralImgWidth;
int g_nIntegralImgHeight;

float g_fltShrinkRate;
float g_fltMaxShrinkRate;

int g_nZoomOutCnt;
int g_nChannelCnt;

float dbl_101E930C = 3.355443995e8f;
float fltMin = (float)-3.4028235e38;
float flt_101E930C = 4.2949673e9f;
float flt_101E93E4 = 0.1f;
float flt_101E9450 = 16384.0f;
float fltZoomScale = 0.0000152587890625f;

STU_FILTER0 g_stuFilter0[3];
STU_FILTER1 g_stuFilter1[3];
STU_FILTER2 g_stuFilter2[3];
STU_FILTER0 g_IstuFilter0[3];
STU_FILTER1 g_IstuFilter1[3];
STU_FILTER2 g_IstuFilter2[3];

float g_pfltStepThreshold0_0_0[STEP_CNT0_0] = {-0.7421879768371582000f, -0.7734379768371582000f, -0.7617189884185791000f, -0.6074219942092895500f, -0.3808589875698089600f, -0.1777340024709701500f, 0.0146484002470970150f, 0.2158199995756149300f, 0.4169920086860656700f, 0.5356450080871582000f};
float g_pfltStepThreshold0_0_12[STEP_CNT0_0] = {-0.8046879768371582000f, -0.8945310115814209000f, -0.7304689884185791000f, -0.5253909826278686500f, -0.2285159975290298500f, 0.0996093973517417910f, 0.4345700144767761200f, 0.7373049855232238800f, 1.0493199825286865000f, 1.2973599433898926000f};
float g_pfltStepThreshold0_1_0[STEP_CNT0_1] = {1.34131002426147f, 2.01623201370239f, 2.54932761192322f};
float g_pfltStepThreshold0_1_12[STEP_CNT0_1] = {2.11377000808716f, 2.84326004981995f, 3.43701004981995f};
float g_pfltFloorThreshold0_2_0[STEP_CNT0_2] = {3.1821300983429f, 3.76514005661011f};
float g_pfltCeilThreshold0_2[STEP_CNT0_2] = {3.4028200183756568600e+38f, 3.4028200183756568600e+38f};
float g_pfltFloorThreshold0_2_12[STEP_CNT0_2] = {4.00049018859863f, 4.58936023712158f};

WORD g_pwCntStepPattern0_0[STEP_CNT0_0] = {0x2, 0x4, 0x6, 0x9, 0xE, 0x17, 0x25, 0x3B, 0x5F, 0x97};
WORD g_pwCntStepPattern0_1[STEP_CNT0_1] = {40, 62, 98};
WORD g_pwCntStepPattern0_2[STEP_CNT0_2] = {0x28, 0x3C};


float g_pfltStepThreshold1_0_0[STEP_CNT1_0] = {-2.1328101158142090000f, -1.9335900545120239000f, -1.6523400545120239000f, -1.3085900545120239000f,	-0.9589840173721313500f, -0.6621090173721313500f, -0.3105469942092895500f, 0.0283202994614839550f, 0.3837890028953552200f, 0.7197269797325134300f, 1.0107400417327881000f, 1.2783199548721313000f,	1.5048799514770508000f, 1.6923799514770508000f, 1.8154300451278687000f, 1.8232400417327881000f};
float g_pfltStepThreshold1_0_12[STEP_CNT1_0] = {-1.7890599966049194000f, -1.5273400545120239000f, -1.2695300579071045000f, -0.9121090173721313500f,	-0.5371090173721313500f, -0.0957031026482582090f, 0.3408200144767761200f, 0.7470700144767761200f, 1.2001999616622925000f, 1.5947300195693970000f, 1.9736299514770508000f, 2.3740200996398926000f, 2.7114300727844238000f, 3.0258800983428955000f, 3.2924799919128418000f, 3.5688500404357910000f};
float g_pfltStepThreshold1_1_0[STEP_CNT1_1] = {2.66308999061584f, 3.33495998382568f, 3.94042992591858f, 4.46387004852295f, 4.93065977096558f, 5.30762004852295f};
float g_pfltStepThreshold1_1_12[STEP_CNT1_1] = {4.36182022094727f, 5.02002000808716f, 5.58349990844727f, 6.11670017242432f, 6.59912014007568f, 7.03075981140137f};
float g_pfltFloorThreshold1_2_0[STEP_CNT1_2] = {6.09668016433716f, 6.84472990036011f, 7.54442977905273f, 8.15966987609863f};
float g_pfltCeilThreshold1_2_0[STEP_CNT1_2] = {20.8388996124268f, 15.5635004043579f, 12.5298004150391f, 10.8275995254517f};
float g_pfltFloorThreshold1_2_12[STEP_CNT1_2] = {7.71045017242432f, 8.34716987609863f, 8.95557022094727f, 9.41748046875f};
float g_pfltCeilThreshold1_2_12[STEP_CNT1_2] = {17.5443992614746f, 13.6625995635986f, 11.6781997680664f, 10.8002996444702f};

WORD g_pwCntStepPattern1_0[STEP_CNT1_0] = {0x4, 0x5, 0x7, 0x9, 0xC, 0xF, 0x13, 0x19, 0x1F, 0x28, 0x34, 0x42, 0x55, 0x6D, 0x8C, 0xB4};
WORD g_pwCntStepPattern1_1[STEP_CNT1_1] = {0x14, 0x1B, 0x25, 0x33, 0x46, 0x5F};
WORD g_pwCntStepPattern1_2[STEP_CNT1_2] = {0x19, 0x2A, 0x45, 0x73};

float g_pflt_2step_3_0[8] = {0.0f, 0.0f, 0.0f, 30.00000f, 35.00000f, 11.25000f, 0.9900000f, -1.000000f};
float g_pflt_2step_3_1[8] = {0.0f, 35.0f, 0.0f, 30.00000f, 35.00000f, 15.00000f, 2.50999999046326f, -1.000000f};
float g_pflt_2step_3_2[8] = {0.0f, -35.0f, 0.0f, 30.00000f, 35.00000f, 11.25000f, 0.9900000f, -1.000000f};

DWORD dword_101ABE60[256]={
	0x3901679F, 0x3914DCB5, 0x392B0EAE, 0x39445845,
	0x39612005, 0x3980EB78, 0x39937DDD, 0x39A88DCB,
	0x39C06981, 0x39DB681B, 0x39F9E9D5, 0x3A0E2CC3,
	0x3A219671, 0x3A37731E, 0x3A500A66, 0x3A6BAB45,
	0x3A85562F, 0x3A96B66E, 0x3AAA2A72, 0x3ABFEB46,
	0x3AD83728, 0x3AF351FA, 0x3B08C2D8, 0x3B199154,
	0x3B2C4014, 0x3B40FE5E, 0x3B57FF5F, 0x3B717A5A,
	0x3B86D57A, 0x3B9668AD, 0x3BA7994A, 0x3BBA8C56,
	0x3BCF697F, 0x3BE65B36, 0x3BFF8EC1, 0x3C0D9A35,
	0x3C1CBFC5, 0x3C2D5354, 0x3C3F71C6, 0x3C5339AC,
	0x3C68CB49, 0x3C80244D, 0x3C8CEAAF, 0x3C9ACB8C,
	0x3CA9DA84, 0x3CBA2C1C, 0x3CCBD5B5, 0x3CDEED8B,
	0x3CF38AAF, 0x3D04E281, 0x3D10DA94, 0x3D1DBA41,
	0x3D2B8E8D, 0x3D3A64CD, 0x3D4A4A9D, 0x3D5B4DD5,
	0x3D6D7C7E, 0x3D807263, 0x3D8ACA79, 0x3D95CDAA,
	0x3DA18317, 0x3DADF1DA, 0x3DBB20F4, 0x3DC91747,
	0x3DD7DB8D, 0x3DE77449, 0x3DF7E7C0, 0x3E049DF5,
	0x3E0DBB34, 0x3E174E39, 0x3E21596A, 0x3E2BDEEA,
	0x3E36E098, 0x3E426005, 0x3E4E5E6E, 0x3E5ADCB5,
	0x3E67DB5C, 0x3E755A7C, 0x3E81ACE0, 0x3E88EC31,
	0x3E906A8E, 0x3E982718, 0x3EA020AC, 0x3EA855E2,
	0x3EB0C50F, 0x3EB96C3C, 0x3EC2492D, 0x3ECB5958,
	0x3ED499EC, 0x3EDE07C9, 0x3EE79F87, 0x3EF15D73,
	0x3EFB3D90, 0x3F029DCD, 0x3F07A983, 0x3F0CBF84,
	0x3F11DD4A, 0x3F17002D, 0x3F1C256D, 0x3F214A2C,
	0x3F266B77, 0x3F2B8642, 0x3F309771, 0x3F359BD8,
	0x3F3A903C, 0x3F3F715B, 0x3F443BEE, 0x3F48ECA8,
	0x3F4D8041, 0x3F51F375, 0x3F564309, 0x3F5A6BCF,
	0x3F5E6AAA, 0x3F623C90, 0x3F65DE93, 0x3F694DDD,
	0x3F6C87BB, 0x3F6F899C, 0x3F725117, 0x3F74DBEC,
	0x3F772808, 0x3F793387, 0x3F7AFCBB, 0x3F7C8227,
	0x3F7DC286, 0x3F7EBCCD, 0x3F7F7028, 0x3F7FDC03,
	0x3F800000, 0x3F7FDC03, 0x3F7F7028, 0x3F7EBCCD,
	0x3F7DC286, 0x3F7C8227, 0x3F7AFCBB, 0x3F793387,
	0x3F772808, 0x3F74DBEC, 0x3F725117, 0x3F6F899C,
	0x3F6C87BB, 0x3F694DDD, 0x3F65DE93, 0x3F623C90,
	0x3F5E6AAA, 0x3F5A6BCF, 0x3F564309, 0x3F51F375,
	0x3F4D8041, 0x3F48ECA8, 0x3F443BEE, 0x3F3F715B,
	0x3F3A903C, 0x3F359BD8, 0x3F309771, 0x3F2B8642,
	0x3F266B77, 0x3F214A2C, 0x3F1C256D, 0x3F17002D,
	0x3F11DD4A, 0x3F0CBF84, 0x3F07A983, 0x3F029DCD,
	0x3EFB3D90, 0x3EF15D73, 0x3EE79F87, 0x3EDE07C9,
	0x3ED499EC, 0x3ECB5958, 0x3EC2492D, 0x3EB96C3C,
	0x3EB0C50F, 0x3EA855E2, 0x3EA020AC, 0x3E982718,
	0x3E906A8E, 0x3E88EC31, 0x3E81ACE0, 0x3E755A7C,
	0x3E67DB5C, 0x3E5ADCB5, 0x3E4E5E6E, 0x3E426005,
	0x3E36E098, 0x3E2BDEEA, 0x3E21596A, 0x3E174E39,
	0x3E0DBB34, 0x3E049DF5, 0x3DF7E7C0, 0x3DE77449,
	0x3DD7DB8D, 0x3DC91747, 0x3DBB20F4, 0x3DADF1DA,
	0x3DA18317, 0x3D95CDAA, 0x3D8ACA79, 0x3D807263,
	0x3D6D7C7E, 0x3D5B4DD5, 0x3D4A4A9D, 0x3D3A64CD,
	0x3D2B8E8D, 0x3D1DBA41, 0x3D10DA94, 0x3D04E281,
	0x3CF38AAF, 0x3CDEED8B, 0x3CCBD5B5, 0x3CBA2C1C,
	0x3CA9DA84, 0x3C9ACB8C, 0x3C8CEAAF, 0x3C80244D,
	0x3C68CB49, 0x3C5339AC, 0x3C3F71C6, 0x3C2D5354,
	0x3C1CBFC5, 0x3C0D9A35, 0x3BFF8EC1, 0x3BE65B36,
	0x3BCF697F, 0x3BBA8C56, 0x3BA7994A, 0x3B9668AD,
	0x3B86D57A, 0x3B717A5A, 0x3B57FF5F, 0x3B40FE5E,
	0x3B2C4014, 0x3B199154, 0x3B08C2D8, 0x3AF351FA,
	0x3AD83728, 0x3ABFEB46, 0x3AAA2A72, 0x3A96B66E,
	0x3A85562F, 0x3A6BAB45, 0x3A500A66, 0x3A37731E,
	0x3A219671, 0x3A0E2CC3, 0x39F9E9D5, 0x39DB681B,
	0x39C06981, 0x39A88DCB, 0x39937DDD, 0x3980EB78,
	0x39612005, 0x39445845, 0x392B0EAE, 0x3914DCB5
};
DWORD dword_101ABA60[256]= {
	0x00000000, 0x3CC90AB0, 0x3D48FB2F, 0x3D96A905,
	0x3DC8BD36, 0x3DFAB273, 0x3E164083, 0x3E2F10A2,
	0x3E47C5C2, 0x3E605C13, 0x3E78CFCC, 0x3E888E93,
	0x3E94A031, 0x3EA09AE5, 0x3EAC7CD4, 0x3EB8442A,
	0x3EC3EF15, 0x3ECF7BCA, 0x3EDAE880, 0x3EE63375,
	0x3EF15AEA, 0x3EFC5D27, 0x3F039C3D, 0x3F08F59B,
	0x3F0E39DA, 0x3F13682A, 0x3F187FC0, 0x3F1D7FD1,
	0x3F226799, 0x3F273656, 0x3F2BEB4A, 0x3F3085BB,
	0x3F3504F3, 0x3F396842, 0x3F3DAEF9, 0x3F41D870,
	0x3F45E403, 0x3F49D112, 0x3F4D9F02, 0x3F514D3D,
	0x3F54DB31, 0x3F584853, 0x3F5B941A, 0x3F5EBE05,
	0x3F61C598, 0x3F64AA59, 0x3F676BD8, 0x3F6A09A7,
	0x3F6C835E, 0x3F6ED89E, 0x3F710908, 0x3F731447,
	0x3F74FA0B, 0x3F76BA07, 0x3F7853F8, 0x3F79C79D,
	0x3F7B14BE, 0x3F7C3B28, 0x3F7D3AAC, 0x3F7E1324,
	0x3F7EC46D, 0x3F7F4E6D, 0x3F7FB10F, 0x3F7FEC43,
	0x3F800000, 0x3F7FEC43, 0x3F7FB10F, 0x3F7F4E6D,
	0x3F7EC46D, 0x3F7E1324, 0x3F7D3AAC, 0x3F7C3B28,
	0x3F7B14BE, 0x3F79C79D, 0x3F7853F8, 0x3F76BA07,
	0x3F74FA0B, 0x3F731447, 0x3F710908, 0x3F6ED89E,
	0x3F6C835E, 0x3F6A09A7, 0x3F676BD8, 0x3F64AA59,
	0x3F61C598, 0x3F5EBE05, 0x3F5B941A, 0x3F584853,
	0x3F54DB31, 0x3F514D3D, 0x3F4D9F02, 0x3F49D112,
	0x3F45E403, 0x3F41D870, 0x3F3DAEF9, 0x3F396842,
	0x3F3504F3, 0x3F3085BB, 0x3F2BEB4A, 0x3F273656,
	0x3F226799, 0x3F1D7FD1, 0x3F187FC0, 0x3F13682A,
	0x3F0E39DA, 0x3F08F59B, 0x3F039C3D, 0x3EFC5D27,
	0x3EF15AEA, 0x3EE63375, 0x3EDAE880, 0x3ECF7BCA,
	0x3EC3EF15, 0x3EB8442A, 0x3EAC7CD4, 0x3EA09AE5,
	0x3E94A031, 0x3E888E93, 0x3E78CFCC, 0x3E605C13,
	0x3E47C5C2, 0x3E2F10A2, 0x3E164083, 0x3DFAB273,
	0x3DC8BD36, 0x3D96A905, 0x3D48FB2F, 0x3CC90AB0,
	0x250D400C, 0xBCC90AB0, 0xBD48FB2F, 0xBD96A905,
	0xBDC8BD36, 0xBDFAB273, 0xBE164083, 0xBE2F10A2,
	0xBE47C5C2, 0xBE605C13, 0xBE78CFCC, 0xBE888E93,
	0xBE94A031, 0xBEA09AE5, 0xBEAC7CD4, 0xBEB8442A,
	0xBEC3EF15, 0xBECF7BCA, 0xBEDAE880, 0xBEE63375,
	0xBEF15AEA, 0xBEFC5D27, 0xBF039C3D, 0xBF08F59B,
	0xBF0E39DA, 0xBF13682A, 0xBF187FC0, 0xBF1D7FD1,
	0xBF226799, 0xBF273656, 0xBF2BEB4A, 0xBF3085BB,
	0xBF3504F3, 0xBF396842, 0xBF3DAEF9, 0xBF41D870,
	0xBF45E403, 0xBF49D112, 0xBF4D9F02, 0xBF514D3D,
	0xBF54DB31, 0xBF584853, 0xBF5B941A, 0xBF5EBE05,
	0xBF61C598, 0xBF64AA59, 0xBF676BD8, 0xBF6A09A7,
	0xBF6C835E, 0xBF6ED89E, 0xBF710908, 0xBF731447,
	0xBF74FA0B, 0xBF76BA07, 0xBF7853F8, 0xBF79C79D,
	0xBF7B14BE, 0xBF7C3B28, 0xBF7D3AAC, 0xBF7E1324,
	0xBF7EC46D, 0xBF7F4E6D, 0xBF7FB10F, 0xBF7FEC43,
	0xBF800000, 0xBF7FEC43, 0xBF7FB10F, 0xBF7F4E6D,
	0xBF7EC46D, 0xBF7E1324, 0xBF7D3AAC, 0xBF7C3B28,
	0xBF7B14BE, 0xBF79C79D, 0xBF7853F8, 0xBF76BA07,
	0xBF74FA0B, 0xBF731447, 0xBF710908, 0xBF6ED89E,
	0xBF6C835E, 0xBF6A09A7, 0xBF676BD8, 0xBF64AA59,
	0xBF61C598, 0xBF5EBE05, 0xBF5B941A, 0xBF584853,
	0xBF54DB31, 0xBF514D3D, 0xBF4D9F02, 0xBF49D112,
	0xBF45E403, 0xBF41D870, 0xBF3DAEF9, 0xBF396842,
	0xBF3504F3, 0xBF3085BB, 0xBF2BEB4A, 0xBF273656,
	0xBF226799, 0xBF1D7FD1, 0xBF187FC0, 0xBF13682A,
	0xBF0E39DA, 0xBF08F59B, 0xBF039C3D, 0xBEFC5D27,
	0xBEF15AEA, 0xBEE63375, 0xBEDAE880, 0xBECF7BCA,
	0xBEC3EF15, 0xBEB8442A, 0xBEAC7CD4, 0xBEA09AE5,
	0xBE94A031, 0xBE888E93, 0xBE78CFCC, 0xBE605C13,
	0xBE47C5C2, 0xBE2F10A2, 0xBE164083, 0xBDFAB273,
	0xBDC8BD36, 0xBD96A905, 0xBD48FB2F, 0xBCC90AB0
	//	0x3901679F, 0x3914DCB5, 0x392B0EAE, 0x39445845
};
float* flt_101ABE60 = (float*)dword_101ABE60;
float* flt_101ABA60 = (float*)dword_101ABA60;
float flt_C7C838 = 0.40000001f;
WORD* g_Filter1;
WORD* g_Filter2;
BYTE* g_pTmpImg;
int** g_pArrayPtr;

#ifdef DEBUG_MODE 
void WriteDebugInfo(const char* path, void* pBuf, int nSize)
{
	FILE* fp = NULL;
	fp = fopen(path, "wb");
	if(!fp)
		return;
	fwrite(pBuf, nSize, 1, fp);
	fclose(fp);
}
#endif

int Eval1thHaarFeature2(LPSTU_FILTER0 pstuFilter0, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{

	int cy = SCAN_HEIGHT * g_nIntegralImgWidth;
	WORD* pwStepPattern;
	float fltY;
	int nVal;
	int nSum;
	float fltSqrIntegralVal;
	float fltIntegralVal;
	int i, j;
	LPSTU_PATTERN0 pPatternAddr = pstuFilter0->pstuPattern;

	fltIntegralVal = (float)(pnIntegralImg[SCAN_WIDTH + cy] - pnIntegralImg[SCAN_WIDTH] - pnIntegralImg[cy] + pnIntegralImg[0]);
	if(fltIntegralVal < 0)
		fltIntegralVal += flt_101E930C;
	//loc_100EAAB7
	fltSqrIntegralVal = (float)(pnSqrIntegralImg[SCAN_WIDTH + cy] - pnSqrIntegralImg[SCAN_WIDTH] - pnSqrIntegralImg[cy] + pnSqrIntegralImg[0]);
	if(fltSqrIntegralVal < 0)
		fltSqrIntegralVal += flt_101E930C;
	fltIntegralVal = pstuFilter0->fltFilterCoeff * fltIntegralVal;
	fltIntegralVal = pstuFilter0->fltFilterCoeff * fltSqrIntegralVal - fltIntegralVal * fltIntegralVal;
	pstuResult->nIsPassed = 0;
	if(pstuFilter0->fltFilterLimit > fltIntegralVal){
		pstuResult->nIsPassed = -1;
		return -1;
	}
	//loc_100EAB18
	fltSqrIntegralVal = flt_101E9450 / sqrt(fltIntegralVal);
	fltSqrIntegralVal = (fltSqrIntegralVal >= 0) ? fltSqrIntegralVal + 0.5f : fltSqrIntegralVal - 0.5f;
	nVal = (int)fltSqrIntegralVal;
	pwStepPattern = pstuFilter0->pwCntStepPattern;
	fltY = pstuResult->fltY;
	for(i = 0; i < pstuFilter0->nStepCnt; i++){//loc_100EAB78 var_C
		nSum = 0;
		for(j = 0; j < pwStepPattern[i]; j++){//loc_100EAB92 ebp
			cy = ExtractFeatureHaar(g_nIntegralImgWidth, pPatternAddr, pnIntegralImg) * nVal;
			cy += pPatternAddr->n_1C;
			cy /= 16777216;
			if(cy > 15)
				cy = 15;
			else if(cy < 0)
				cy = 0;
			cy = pPatternAddr->pby_C[cy];
			nSum += (cy << pPatternAddr->by_9);
			pPatternAddr++;
		}
		fltSqrIntegralVal = fltZoomScale * nSum + pstuResult->fltX;
		pstuResult->fltX = fltSqrIntegralVal;
		if(pstuFilter0->pfltStepThreshold[i]/*trainneddata*/ > fltSqrIntegralVal){
			pstuResult->nIsPassed = -1;
			break;//face determine
		}
		fltY += fltSqrIntegralVal - pstuFilter0->pfltStepThreshold[i];
	}
	pstuResult->fltX = fltSqrIntegralVal;
	pstuResult->fltY = fltY;
	return pstuResult->nIsPassed;
}

int EvalSecondFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	int rst = 0;
	if(Eval1thHaarFeature2(&g_IstuFilter0[index], pnIntegralImg, pnSqrIntegralImg, pstuResult) == 0)
		if(Eval2ndHaarFeature(&g_IstuFilter1[index], pnIntegralImg, pnSqrIntegralImg, pstuResult) == 0)
			Eval3rdHaarFeature(&g_IstuFilter2[index], pnIntegralImg, pnSqrIntegralImg, pstuResult);
	return rst;
}

int PackResult(LPEVAL_RESULTS pEvalResult, float* pflt, int nX, int nY, LPARR_DETECTED_INFO parrDetectedInfo)
{
	LPSTU_DETECTED_INFO pDetectedInfo;
	if(pEvalResult->nIsPassed != -1){//add region//100E2AB6
		if(fltMin >= pEvalResult->fltY)
			return -1;
		pDetectedInfo = new STU_DETECTED_INFO;
		pDetectedInfo->fltX = (float)(nX - BORDER_WIDTH) * g_fltShrinkRate;//sub_1004B3D0
		pDetectedInfo->fltY = (nY - BORDER_HEIGHT) * g_fltShrinkRate;
		pDetectedInfo->fltScale = g_fltShrinkRate;
		pDetectedInfo->flt_20 = pflt[2];
		pDetectedInfo->flt_24 = pEvalResult->fltY;
		parrDetectedInfo->push_back(pDetectedInfo);
	}
	return 0;
}

int EvalSecond(LPSTU_DETECTOR_STATE pstuDetectorState, int* pnIntegralImg, int* pnSqrIntegralImg, int nX, int nY, LPARR_DETECTED_INFO pArrDetectedInfo)
{
	int i;
	int iPassedNo;

	EVAL_RESULTS stuResult;
	for(i = 0; i < pstuDetectorState->nCntPassed/*2*/; i++){//loc_100EDEA6	ebp
		iPassedNo = pstuDetectorState->piPassedNo[i];//0,1,2
		memset(&stuResult, 0, sizeof(EVAL_RESULTS));
		stuResult.nIsPassed = -1;
		EvalSecondFeatures(iPassedNo, pnIntegralImg, pnSqrIntegralImg, &stuResult);//arg_0->4C, arg_4, pEBX
		PackResult(&stuResult, g_IstuFilter2[iPassedNo].pflt_B0, nX, nY, pArrDetectedInfo);//edx, ecx, edi, i
	}
	return 0;
}

int MakeIntegralImg()
{//void vlf_AdvancedDetector::createSectionIntImages() const
	int nIntegral;
	int nSqrIntegral;
	int* pnIntegralImgAddr, *pnSqrIntegralImgAddr, *pnIntegralImgAddr1, *pnSqrIntegralImgAddr1;
	int i, j;
	unsigned char* pubyShrinkedImgAddr;

	//loc_100E37B7
	g_nIntegralImgWidth = g_nShrinkedWidth + 1;
	g_nIntegralImgHeight = g_nShrinkedHeight + 1;
	if(g_nIntegralImgHeight > 0x3D)
		g_nIntegralImgHeight = 0x3D;
	//loc_100E3815
	pnIntegralImgAddr = g_pnIntegralImg;
	pnSqrIntegralImgAddr = g_pnSqrIntegralImg;
	pubyShrinkedImgAddr = (unsigned char*)g_pbyShrinkedImg;
	memset(pnIntegralImgAddr, 0, sizeof(int) * g_nIntegralImgWidth);
	memset(pnSqrIntegralImgAddr, 0, sizeof(int) * g_nIntegralImgWidth);
	pnIntegralImgAddr1 = pnIntegralImgAddr;
	pnSqrIntegralImgAddr1 = pnSqrIntegralImgAddr;
	pnIntegralImgAddr += g_nIntegralImgWidth;
	pnSqrIntegralImgAddr += g_nIntegralImgWidth;
	//loc_100E3847
	for(i = 0; i < g_nShrinkedHeight; i++){//loc_100E3853	var_C
		*pnIntegralImgAddr = 0;
		*pnSqrIntegralImgAddr = 0;
		pnIntegralImgAddr++;
		pnSqrIntegralImgAddr++;
		pnIntegralImgAddr1++;
		pnSqrIntegralImgAddr1++;
		nIntegral = 0;
		nSqrIntegral = 0;
		for(j = 0; j < g_nShrinkedWidth; j++){//loc_100E3880
			nSqrIntegral += (*pubyShrinkedImgAddr) * (*pubyShrinkedImgAddr);
			nIntegral += (*pubyShrinkedImgAddr);
			*pnIntegralImgAddr = (*pnIntegralImgAddr1) + nIntegral;
			*pnSqrIntegralImgAddr = (*pnSqrIntegralImgAddr1) + nSqrIntegral;
			pubyShrinkedImgAddr++;
			pnIntegralImgAddr++;
			pnSqrIntegralImgAddr++;
			pnIntegralImgAddr1++;
			pnSqrIntegralImgAddr1++;
		}
	}
	g_nChannelCnt = 0;
#ifdef DEBUG_MODE
// 	WriteDebugInfo("g:\\debug_int_new.dat", g_pnIntegralImg, g_nIntegralImgWidth * g_nIntegralImgHeight * 4);
// 	WriteDebugInfo("g:\\debug_sqr_new.dat", g_pnSqrIntegralImg, g_nIntegralImgWidth * g_nIntegralImgHeight * 4);
#endif
	return 0;
}

int ShrinkImage()
{
	float fltShrinkRate;
	int nNewWidth;
	int nNewHeight;
	int iZoomedShrinkRate = 0;
	int nZoomedShrinkRate;
	int nZoomOutTimes = 1;
	int nOffset;
	int nWeight1;
	int nWeight2;
	int nDiffWidth;
	int nCnt;
	int nBase;
	int i, j;
	int nVal;
	WORD pwImg[MAX_IMG_WIDTH];
	unsigned char* pubyImgAddr;
	char* pbyImgAddr;

	//Calc 2 ^ g_nZoomOutCnt to get nZoomOutTimes
	nCnt = g_nZoomOutCnt > 0 ? g_nZoomOutCnt : - g_nZoomOutCnt;
	nBase = 2;
	while(nCnt){
		if(nCnt & 1)
			nZoomOutTimes *= nBase;
		nCnt >>= 1;
		nBase *= nBase;
	}
	if(g_nZoomOutCnt > 0)
		fltShrinkRate =  g_fltShrinkRate / nZoomOutTimes;
	else
		fltShrinkRate = g_fltShrinkRate;

	//loc_100E30EF
	//Zoom in the state
	nZoomedShrinkRate = (int)(65536.0f * fltShrinkRate);
	nNewWidth = ((g_nOrgWidth << 16) - 1) / nZoomedShrinkRate;
	nNewHeight = ((g_nOrgHeight << 16) - 1) / nZoomedShrinkRate;

	//loc_100E3175
	g_nShrinkedWidth = nNewWidth + BORDER_WIDTH * 2;
	g_nShrinkedHeight = nNewHeight + BORDER_HEIGHT * 2;
	pbyImgAddr = g_pbyShrinkedImg + BORDER_HEIGHT * g_nShrinkedWidth + BORDER_WIDTH;//eax
	for(i = 0; i < nNewHeight; i++){//loc_100E31D0 var_3C
		pubyImgAddr = (unsigned char*)((iZoomedShrinkRate >> 16) * g_nOrgWidth + g_pbyOrgImg);
		nWeight2 = iZoomedShrinkRate & 0xFFFF;
		nWeight1 = 65536 - nWeight2;
		iZoomedShrinkRate += nZoomedShrinkRate;
		for(j = 0; j < g_nOrgWidth; j++){//loc_100E3238 j = var_arg_0
			nVal = pubyImgAddr[j] * nWeight1;
			nVal += nWeight2 * pubyImgAddr[g_nOrgWidth + j];
			pwImg[j] = nVal >> 10;
		}
		//loc_100E327E
		nVal = 0;
		for(j = 0; j < nNewWidth; j++){//j = var_arg_0
			nWeight1 = nVal >> 16;
			nWeight2 = nVal & 0xFFFF;
			pbyImgAddr[j] = (pwImg[nWeight1 + 1] * nWeight2 + (65536 - nWeight2) * pwImg[nWeight1]) >> 22;
			nVal += nZoomedShrinkRate;
		}
		pbyImgAddr += g_nShrinkedWidth;
	}
	//loc_100E32F7
	if(nNewWidth > 1 && nNewHeight > 1){//
		//Shrink Borders
		nZoomedShrinkRate = BORDER_WIDTH;
		nCnt = (int)(65536.0 * WEIGHT_COEFFICIENT);//ecx
		iZoomedShrinkRate = (int)((1.0 - WEIGHT_COEFFICIENT) * 128.0 * 65536.0);
		nOffset = (BORDER_HEIGHT + nNewHeight - 1) * g_nShrinkedWidth;
		nDiffWidth = - g_nShrinkedWidth;
		nWeight2 = BORDER_HEIGHT * g_nShrinkedWidth;
		for(i = 0; i < BORDER_HEIGHT; i++){//		var_24 = arg_0->nBorderHeight_40;	loc_100E33B1
			pubyImgAddr = (unsigned char*)&g_pbyShrinkedImg[nWeight2 + nZoomedShrinkRate];
			pbyImgAddr = &g_pbyShrinkedImg[nWeight2 - g_nShrinkedWidth + nZoomedShrinkRate];
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr++;
			for(j = 0; j < nNewWidth - 2; j++){//loc_100E33E0
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr++;
				pubyImgAddr++;
			}
			//loc_100E340A
			*pbyImgAddr = (pubyImgAddr[1] * nCnt + iZoomedShrinkRate) >> 16;
			pubyImgAddr = (unsigned char*)g_pbyShrinkedImg + nOffset + nZoomedShrinkRate;
			pbyImgAddr = (char*)pubyImgAddr + g_nShrinkedWidth;
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr++;
			for(j = 0; j < nNewWidth - 2; j++){//loc_100E3450
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr++;
				pubyImgAddr++;
			}
			//loc_100E347B
			*pbyImgAddr = (pubyImgAddr[1] * nCnt + iZoomedShrinkRate) >> 16;
			nOffset += g_nShrinkedWidth;
			nWeight2 += nDiffWidth;
		}
		//loc_100E34A8
		//Shrink contents
		for(i = 0; i < nZoomedShrinkRate; i++){//loc_100E34C1	i = var_3C
			pubyImgAddr = (unsigned char*)&g_pbyShrinkedImg[nZoomedShrinkRate - i];
			pbyImgAddr = &g_pbyShrinkedImg[nZoomedShrinkRate - i - 1];
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr += g_nShrinkedWidth;
			for(j = 0; j < g_nShrinkedHeight - 2; j++){//loc_100E33E0
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[g_nShrinkedWidth * 2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr+= g_nShrinkedWidth;
				pubyImgAddr+= g_nShrinkedWidth;
			}
			//loc_100E340A
			*pbyImgAddr = (pubyImgAddr[g_nShrinkedWidth] * nCnt + iZoomedShrinkRate) >> 16;
			pubyImgAddr = (unsigned char*)g_pbyShrinkedImg + nZoomedShrinkRate + i + nNewWidth - 1;
			pbyImgAddr = (char*)pubyImgAddr + 1;
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr += g_nShrinkedWidth;
			for(j = 0; j < g_nShrinkedHeight - 2; j++){//loc_100E3450
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[g_nShrinkedWidth * 2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr+= g_nShrinkedWidth;
				pubyImgAddr+= g_nShrinkedWidth;
			}
			//loc_100E347B
			*pbyImgAddr = (pubyImgAddr[g_nShrinkedWidth] * nCnt + iZoomedShrinkRate) >> 16;
		}
#ifdef DEBUG_MODE
		WriteDebugInfo("g:\\debugNew.dat", g_pbyShrinkedImg, g_nShrinkedWidth * g_nShrinkedHeight);
#endif
	}else{//loc_100E35B8
		pbyImgAddr = &g_pbyShrinkedImg[g_nShrinkedWidth - 1];//ecx
		for(i = 0; i < g_nShrinkedHeight; i++){//loc_100E35E4
			if(i < BORDER_HEIGHT || i < g_nShrinkedWidth - BORDER_HEIGHT){//loc_100E3613
				if(g_nShrinkedWidth > 0)
					memset(g_pbyShrinkedImg, 0x80, g_nShrinkedWidth);
			}else{
				for(j = 0; j < BORDER_WIDTH; j++){//loc_100E3600
					g_pbyShrinkedImg[j] = (char)0x80;
					*pbyImgAddr = (char)0x80;
					pbyImgAddr--;
				}
			}
		}
	}
	return 0;
}

int ZoomOutTwice()
{
	int iImg = 0;
	int nBeforeWidth;
	int nVal;
	int i, j;

	nBeforeWidth = g_nOrgWidth;
	g_nOrgWidth = nBeforeWidth / 2;
	g_nOrgHeight = g_nOrgHeight / 2;
	if(g_nZoomOutCnt >= 0){//loc_100E2FC4
		for(i = 0; i < g_nOrgHeight; i++){//loc_100E2FD0
			for(j = 0; j < g_nOrgWidth; j++){//ebp
				nVal = (unsigned char)g_pbyOrgImg[iImg + j * 2 + nBeforeWidth + 1];
				nVal += (unsigned char)g_pbyOrgImg[iImg + j * 2 + 1];
				nVal += (unsigned char)g_pbyOrgImg[iImg + j * 2 + nBeforeWidth];
				nVal += (unsigned char)g_pbyOrgImg[iImg + j * 2] + 2;
				g_pbyOrgImg[i * g_nOrgWidth + j] = nVal >> 2;
			}
			iImg += 2 * nBeforeWidth;
		}
	}else{
		for(i = 0; i < g_nOrgHeight; i++){
			for(j = 0; j < g_nOrgWidth; j++){
				g_pbyOrgImg[i * g_nOrgWidth + j] = g_pbyOrgImg[j * 2];
			}
		}
	}
	g_nZoomOutCnt++;
	return 0;
}

bool CreateWorkImage()
{
	double dbl_exp, dbl_val;
	g_fltShrinkRate = SHRINK_COEFFICIENT * g_fltShrinkRate;
	if(g_fltShrinkRate > g_fltMaxShrinkRate){
		return false;
	}
	//loc_100E3DF3
	dbl_exp = g_nZoomOutCnt + 1;
	dbl_val = pow(2.0, dbl_exp);
	while(dbl_val < g_fltShrinkRate){
		ZoomOutTwice();
		dbl_exp = g_nZoomOutCnt + 1;
		dbl_val = pow(2.0, dbl_exp);
	}
 	ShrinkImage();
 	MakeIntegralImg();
	return true;				   
}

int WeightedAverage(LPNRECT pOutRect, ARR_DETECTED_INFO pDetectedRectArr, int* pnSortIndex, int nOffset, int nNum)
{
	float fltDist;
	int iMaxNo;
	float fltMax_24;
	float fltScaleSize;
	float fltScalPos;
	float flt_24;
	float fltLogScale;
	float fltLogScaleForMaxNo;
	float fltCoeffSize;
	STU_FLT_POS pos;
	STU_FLT_POS posCenter;
	STU_FLT_POS posDiff;
	STU_FLT_POS posMax;

	int i, iSortIndex;
	int nDetectNum = pDetectedRectArr.size();

	if(nNum + nOffset > nDetectNum){
		return -1;//"%s:\noffset + size out of range"
	}//loc_100E42E7:                           
	fltMax_24 = fltMin;
	iMaxNo = 0;
	for(i = 0; i < nNum; i++){//loc_100E4360 -> loc_100E4519
		iSortIndex = pnSortIndex[nOffset + i];
		if(fltMax_24 < pDetectedRectArr[iSortIndex]->flt_24){
			fltMax_24 = pDetectedRectArr[iSortIndex]->flt_24;
			iMaxNo = iSortIndex;
		}
	}
	//loc_100E4519:                          
	posMax.fltX = pDetectedRectArr[iMaxNo]->fltScale * SCAN_WIDTH * 0.5f + pDetectedRectArr[iMaxNo]->fltX;
	posMax.fltY = pDetectedRectArr[iMaxNo]->fltScale * SCAN_HEIGHT * 0.5f + pDetectedRectArr[iMaxNo]->fltY;
	fltLogScaleForMaxNo = log(pDetectedRectArr[iMaxNo]->fltScale);
	fltScaleSize = pDetectedRectArr[iMaxNo]->flt_20;
	posCenter.fltX = 0;
	posCenter.fltY = 0;
	fltCoeffSize = 0;
	fltScalPos = 0;
	for(i = 0; i < nNum; i++){//loc_100E4624
		iSortIndex = pnSortIndex[nOffset + i];
		flt_24 = pDetectedRectArr[iSortIndex]->flt_20 - fltScaleSize;
		while(flt_24 > 180.0){
			flt_24 -= 360.0;
		}//loc_100E467B
		while(flt_24 < -180.0){
			flt_24 += 360.0;
		}//loc_100E46A3
		if(flt_24 <= 0)
			flt_24 = -flt_24;
		fltLogScale = log(pDetectedRectArr[iSortIndex]->fltScale);
		pos.fltX = pDetectedRectArr[iSortIndex]->fltScale * SCAN_WIDTH * 0.5f + pDetectedRectArr[iSortIndex]->fltX;
		pos.fltY = pDetectedRectArr[iSortIndex]->fltScale * SCAN_HEIGHT * 0.5f + pDetectedRectArr[iSortIndex]->fltY;
		//loc_100E46F8
		if(FIRST_THRESHOLD >= flt_24){//loc_100E4780
			flt_24 = fltLogScale - fltLogScaleForMaxNo;
			if(flt_24 <= 0)
				flt_24 = -flt_24;
			//100E472E
			if(mac_flt_58 >= flt_24){
				posDiff.fltY = posMax.fltY - pos.fltY;
				posDiff.fltX = posMax.fltX - pos.fltX;
				fltDist = sqrt(posDiff.fltX * posDiff.fltX + posDiff.fltY * posDiff.fltY);
				if(mac_flt_5C * SCAN_WIDTH * pDetectedRectArr[iSortIndex]->fltScale >= fltDist){
					flt_24 = pDetectedRectArr[iSortIndex]->flt_24;
					fltScalPos += flt_24;
					posCenter.fltX += pos.fltX * flt_24;
					posCenter.fltY += pos.fltY * flt_24;
					fltCoeffSize += flt_24 * fltLogScale;
				}
			}
		}
	}
	fltScalPos = fltScalPos > 0 ? 1 / fltScalPos : 0;
	fltScaleSize = exp(fltScalPos * fltCoeffSize);
	pOutRect->l = (int)(posCenter.fltX * fltScalPos - 0.5 * SCAN_WIDTH * fltScaleSize);
	pOutRect->t = (int)(posCenter.fltY * fltScalPos - 0.5 * SCAN_HEIGHT * fltScaleSize);
	pOutRect->r = (int)(pOutRect->l + fltScaleSize * SCAN_WIDTH);
	pOutRect->b = (int)(pOutRect->t + fltScaleSize * SCAN_HEIGHT);
	return 0;
}

void PreProc(char* pbyImg, int nImgWidth, int nImgHeight)
{
	float fltMaxShrinkHorzRate, fltMaxShrinkVertRate;
	int nHalfScanW, nRealScanW, nRealScanH, nHalfScanH;
	//Malloc and copy image to arg_esi->stu_248->pbyImg_4
	g_nOrgWidth = nImgWidth;
	g_nOrgHeight = nImgHeight;
	g_nOrgImgSize = nImgWidth * nImgHeight;

	g_fltShrinkRate = FIRST_SHRINK_RATE;
	memcpy(g_pbyOrgImg, pbyImg, g_nOrgImgSize);
	nRealScanW = SCAN_WIDTH - BORDER_WIDTH * 2 + 1;
	nRealScanH = SCAN_HEIGHT - BORDER_HEIGHT * 2 + 1;
	nHalfScanW = SCAN_WIDTH >> 1;
	nHalfScanH = SCAN_HEIGHT >> 1;
	fltMaxShrinkHorzRate = (float)(nRealScanW > nHalfScanW ? nRealScanW : nHalfScanW);
	fltMaxShrinkVertRate = (float)(nRealScanH > nHalfScanH ? nRealScanH : nHalfScanH);
	fltMaxShrinkHorzRate = (float)g_nOrgWidth / fltMaxShrinkHorzRate;
	fltMaxShrinkVertRate = (float)g_nOrgHeight / fltMaxShrinkVertRate;

	//100E3CA1
	g_fltMaxShrinkRate = fltMaxShrinkHorzRate < fltMaxShrinkVertRate ? fltMaxShrinkHorzRate : fltMaxShrinkVertRate;
	if(LIMIT_OF_SHRINK_RATE <= g_fltMaxShrinkRate)
		g_fltMaxShrinkRate = LIMIT_OF_SHRINK_RATE;
	g_nZoomOutCnt = 0;
	ShrinkImage();
	MakeIntegralImg();
	return;
}

int IsLagerThanScanRect()
{
	if(g_nShrinkedWidth < SCAN_WIDTH)
		return 0;
	if(g_nShrinkedHeight < SCAN_HEIGHT)
		return 0;
	return 1;
}

void ChannelDetector(int nTop, int nHeight)
{
	int nY;
	if(60 >= nHeight){
		nY = nTop - g_nChannelCnt + nHeight;
		if(nY > 60){//100E3A91 
			nY = nTop  + 60;
			if(nY > g_nShrinkedHeight){//100E3AA5
				nY = g_nShrinkedHeight - g_nChannelCnt - 60;
			}else{
				nY = nTop - g_nChannelCnt;
			}//100E3AAB
			if(nY > 0){//100E3AD5
				CorrectIntegralImg(nY);
			}
		}
	}
}

int EvalFirst(LPSTU_DETECTOR_STATE pstuDetectState, int* pnIntegralImg, int* pnSqrIntegralImg)
{
	int nRB;//Right Bottom Position
	int nLB;//Left Bottom Position
	float flt_val;
	float fltIntegralAvg;
	float fltSqrIntegralAvg;
	int nSqrIntegral, nIntegral;

	nLB = g_nIntegralImgWidth * SCAN_HEIGHT;
	nRB = nLB + SCAN_WIDTH;
	nIntegral = pnIntegralImg[nRB];
	nIntegral -= pnIntegralImg[SCAN_WIDTH];
	nIntegral -= pnIntegralImg[nLB];
	nIntegral += pnIntegralImg[0];
	fltIntegralAvg = (float)nIntegral;
	if(fltIntegralAvg < 0)
		fltIntegralAvg += flt_101E930C;
	//100EDCDD
	fltIntegralAvg = fltIntegralAvg * INTEGRAL_AVG_COEFF;
	nSqrIntegral = pnSqrIntegralImg[nRB];
	nSqrIntegral -= pnSqrIntegralImg[SCAN_WIDTH];
	nSqrIntegral -= pnSqrIntegralImg[nLB];
	nSqrIntegral += pnSqrIntegralImg[0];
	fltSqrIntegralAvg = (float)nSqrIntegral;
	if(fltSqrIntegralAvg < 0)
		fltSqrIntegralAvg += flt_101E930C;
	fltSqrIntegralAvg = fltSqrIntegralAvg * INTEGRAL_AVG_COEFF;
	
	fltIntegralAvg = pow(fltIntegralAvg, 2);
	fltSqrIntegralAvg = fltSqrIntegralAvg - fltIntegralAvg;
	if(fltSqrIntegralAvg <= FIRST_THRESHOLD)
		return 0;
	//100EDD3F
	flt_val = 1 / sqrt(fltSqrIntegralAvg);
	pstuDetectState->nCntPassed = 0;
	for(int i = 0; i < CNT_DETECT_STEP; i ++)
	{//100EDD60 
		if(EvalFirstFeatures(i, pnIntegralImg, pnSqrIntegralImg, flt_val) >= 0)
		{
			pstuDetectState->piPassedNo[pstuDetectState->nCntPassed] = i;
			pstuDetectState->nCntPassed++;
		}
		//100EDD90
	}
	//100EDD98
	return pstuDetectState->nCntPassed;
}

void Choose2thStepData(LPSTU_DETECTOR_STATE pstuSecondStep, LPSTU_DETECTOR_STATE pstuFirstStep)
{
	int nCntFirstPassed;
	int i, j;
	float* pflt_ESI;
	int iPassedDataNo;
	
	nCntFirstPassed = pstuFirstStep->nCntPassed;
	pstuSecondStep->nCntPassed = 0;
	for(i = 0; i < CNT_DETECT_STEP; i ++){//100EDDD5
		pflt_ESI = g_IstuFilter2[i].pflt_B0;
		for(j = 0; j < nCntFirstPassed; j ++){//100EDDF0
			iPassedDataNo = pstuFirstStep->piPassedNo[j];
			if(IsAvaiableData(g_IstuFilter2[iPassedDataNo].pflt_B0, pflt_ESI, flt_101E93E4)){
				pstuSecondStep->piPassedNo[pstuSecondStep->nCntPassed] = i;
				pstuSecondStep->nCntPassed ++;
				break;
			}
		}
	}
}

int ClassifyDetectedRects(ARR_DETECTED_INFO pstuDetectedFaceArr, float fltAreaCoeff, int nScanWidth, int nScanHeight, int** ppnSortNo, int** ppnCntSameClass)
{
	int nCntCandidate = 0;
	float fltAreaA;
	float fltAreaB;
	float flt_tmp;
	int j, k, i = 0, nCntNeighbourRect;
	int *pnCntSameClass, *pnSortNo;
	float fltAX1, fltAY1, fltAX2, fltAY2;
	float fltBX1, fltBY1, fltBX2, fltBY2;
	int nDetectCnt = pstuDetectedFaceArr.size();

	if(nDetectCnt <= 0)
		return 0;
	
	pnSortNo = new int[nDetectCnt];
	pnCntSameClass = new int[nDetectCnt];
	for(j = 0; j < nDetectCnt; j++){
		pnSortNo[j] = j;
		pnCntSameClass[j] = 1;
	}

	*ppnSortNo = pnSortNo;
	*ppnCntSameClass = pnCntSameClass;

	if(nDetectCnt == 1)
		return 1;
	
	while(i < nDetectCnt){//100F9607
		fltAreaB = fltMin;
		nCntNeighbourRect = 0;
		for(j = i; j < nDetectCnt; j++){//100F9710
			flt_tmp = pstuDetectedFaceArr[pnSortNo[j]]->flt_24;
			if(fltAreaB < flt_tmp){//100F9730
				fltAreaB = flt_tmp;
				nCntNeighbourRect = j;
			}//100F9738
		}
		//100F9744
		k = pnSortNo[nCntNeighbourRect];
		fltAX1 = pstuDetectedFaceArr[k]->fltX;
		fltAY1 = pstuDetectedFaceArr[k]->fltY;
		fltAX2 = pstuDetectedFaceArr[k]->fltScale * SCAN_WIDTH + fltAX1;
		fltAY2 = pstuDetectedFaceArr[k]->fltScale * SCAN_HEIGHT + fltAY1;
		pnSortNo[nCntNeighbourRect] = pnSortNo[i];
		pnSortNo[i] = k;
		i ++;
		nCntNeighbourRect = 1;
		fltAreaA = (fltAY2 - fltAY1) * (fltAX2 - fltAX1);
		for(j = i; j < nDetectCnt; j++){//100F97F4
			k = pnSortNo[j];
			fltBX1 = pstuDetectedFaceArr[k]->fltX;
			fltBY1 = pstuDetectedFaceArr[k]->fltY;
			fltBX2 = pstuDetectedFaceArr[k]->fltScale * SCAN_WIDTH + fltBX1;
			fltBY2 = pstuDetectedFaceArr[k]->fltScale * SCAN_HEIGHT + fltBY1;
			fltAreaB = (fltBY2 - fltBY1) * (fltBX2 - fltBX1);
			fltBX1 = fltAX1 > fltBX1 ? fltAX1 : fltBX1;
			fltBY1 = fltAY1 > fltBY1 ? fltAY1 : fltBY1;
			fltBX2 = fltAX2 < fltBX2 ? fltAX2 : fltBX2;
			fltBY2 = fltAY2 < fltBY2 ? fltAY2 : fltBY2;
			if(fltBX1 > fltBX2)
				fltBX2 = fltBX1;
			if(fltBY1 > fltBY2)
				fltBY2 = fltBY1;
			flt_tmp = fltAreaA;
			if(fltAreaB <= fltAreaA){//100F98C0
				flt_tmp = fltAreaB;
			}//100F98C4
			fltAreaB = (fltBY2 - fltBY1) * (fltBX2 - fltBX1);
			if(flt_tmp * fltAreaCoeff < fltAreaB){//100F98F5
				pnSortNo[j] = pnSortNo[i];
				pnSortNo[i] = k;
				i ++;
				nCntNeighbourRect ++;
			}
		}//100F9945
		pnCntSameClass[nCntCandidate] = nCntNeighbourRect;
		nCntCandidate ++;
	}
	return nCntCandidate;
}

void CorrectIntegralImg(int nY)
{
	int	iIntegral;
	int	iSqrIntegral;
	int n, nHeight, cx, cy;
	int *pnIntegralImgAddr, *pnSqrIntegralImgAddr, *pnIntegralImgAddr_1, *pnSqrIntegralImgAddr_1;
	int i, j;
	char *pbyShrinkedImgAddr;
	cy = 60;
	if(cy + g_nChannelCnt + nY > g_nShrinkedHeight){//100E392C
		cy = g_nShrinkedHeight - g_nChannelCnt - nY;
	}
	//100E3932
	pnIntegralImgAddr = g_pnIntegralImg;
	pnSqrIntegralImgAddr = g_pnSqrIntegralImg;
	pbyShrinkedImgAddr = g_pbyShrinkedImg + (g_nChannelCnt + cy) * g_nShrinkedWidth;
	nHeight = cy + 1;
	cx = g_nShrinkedWidth + 1;
	n = cx * nY;
	for(nHeight -= nY; nHeight > 0; nHeight--){
		memcpy(pnIntegralImgAddr, pnIntegralImgAddr + n, cx * 4);
		memcpy(pnSqrIntegralImgAddr, pnSqrIntegralImgAddr + n, cx * 4);
		pnIntegralImgAddr += cx;
		pnSqrIntegralImgAddr += cx;
	}
	for(i = 0; i < nY; i++){//100E39B8
		*(pnIntegralImgAddr++) = 0;
		*(pnSqrIntegralImgAddr++) = 0;
		iIntegral = 0;
		iSqrIntegral = 0;
		pnSqrIntegralImgAddr_1 = pnSqrIntegralImgAddr - cx;
		pnIntegralImgAddr_1 = pnIntegralImgAddr - cx;
		for(j = 0; j < g_nShrinkedWidth; j++){//100E39E4
			n = *(unsigned char*)(pbyShrinkedImgAddr++);
			iIntegral += n;
			iSqrIntegral += n * n;
			*(pnIntegralImgAddr++) = *(pnIntegralImgAddr_1++) + iIntegral;
			*(pnSqrIntegralImgAddr++) = *(pnSqrIntegralImgAddr_1++) + iSqrIntegral;
		}
	}
	g_nChannelCnt += nY;
}

int EvalFirstFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, float fltInitX)
{
	EVAL_RESULTS stuResult = {0, -1, 0};
	if(Eval1thHaarFeature1(&g_stuFilter0[index], pnIntegralImg, fltInitX, &stuResult) == 0)
		if(Eval2ndHaarFeature(&g_stuFilter1[index], pnIntegralImg, pnSqrIntegralImg, &stuResult) == 0)
			Eval3rdHaarFeature(&g_stuFilter2[index], pnIntegralImg, pnSqrIntegralImg, &stuResult);
	return stuResult.nIsPassed;
}
//verify_region_1
int Eval1thHaarFeature1(LPSTU_FILTER0 pstuFilter0, int* pnIntegralImg, float fltInitX, LPEVAL_RESULTS pstuResult)
{
	int nHaar;
	int n;
	WORD* pwStepPattern;
	int nStepCnt;
	float fltX, fltY;
	int nSum;
	int i, index = 0;

	fltX = fltInitX * flt_101E9450;
	if(fltX > 0)
		fltX += 0.5;
	else
		fltX -= 0.5;
	n = (int)fltX;
	nStepCnt = pstuFilter0->nStepCnt;
	pwStepPattern = pstuFilter0->pwCntStepPattern;/*cnt=10:2, 4, 6, 8, E, 17, 25, 3B, 5F, 97*/
	fltX = pstuResult->fltX;
	fltY = pstuResult->fltY;
	pstuResult->nIsPassed = 0;
	for(i = 0; i < nStepCnt/*0xA*/; i++){//100EACC9
		nSum = 0;
		for(int j = 0; j < pwStepPattern[i]; j++){//100EACE1
			nHaar = ExtractFeatureHaar(g_nIntegralImgWidth, &pstuFilter0->pstuPattern[index], pnIntegralImg);
			nHaar = nHaar * n + pstuFilter0->pstuPattern[index].n_1C;
			nHaar = nHaar >> 0x18;
			if(nHaar < 0){
				nHaar = 0;
			}else if(nHaar > 15){
				nHaar = 15;
			}//100EAD0E
			nSum += pstuFilter0->pstuPattern[index].pby_C[nHaar] << pstuFilter0->pstuPattern[index++].by_9;
		}//100EAD22
		fltX += fltZoomScale * nSum;
		if(pstuFilter0->pfltStepThreshold[i] > fltX){//100EAD80
			pstuResult->nIsPassed = -1;
			break;
		}//100EAD69
		fltY += (fltX - pstuFilter0->pfltStepThreshold[i]);
	}
	//100EAD82
	pstuResult->fltX = fltX;
	pstuResult->fltY = fltY;
	return pstuResult->nIsPassed;
}

float Extract2ndHaarFeature_100F5360(LPSTU_PATTERN1 pstuPattern, int* pnIntegralImg, int* pnSqrIntegralImg, int nIntegralWidth, float fltAvgCoeff)
{
	int nBR;
	int nBL;
	int nTR;
	int nOffX;
	int nIntegralVal;
	int* pnIntegralImgAddr;
	int* pnSqrIntegralImgAddr;
	float fltThreshold;
	float fltIntegralVal;
	float fltSqrIntegralVal;
	int i, j;
	int index = 0;
	int cx = pstuPattern->ubyCx;
	int cy = pstuPattern->ubyCy * nIntegralWidth;
	int nSum = 0;

	//Calc the integral rect(arg_edx(x_4, y_5, w_6, h_7) in pn_ecx to n_ebx
	nOffX = pstuPattern->ubyPosY_5 * nIntegralWidth + pstuPattern->ubyPosX_4;
	pnIntegralImgAddr = pnIntegralImg + nOffX;
	pnSqrIntegralImgAddr = pnSqrIntegralImg + nOffX;
	nTR = cx * 4;//TopRight
	nBL = cy * 4;//BottomLeft
	nBR = nTR + nBL;//BottomRight
	nIntegralVal = pnIntegralImgAddr[nBR] - pnIntegralImgAddr[nTR] - pnIntegralImgAddr[nBL] + pnIntegralImgAddr[0];
	fltIntegralVal = (float)nIntegralVal;
	if(fltIntegralVal < 0)
		fltIntegralVal += flt_101E930C;
	//loc_100F53C9
	nIntegralVal = pstuPattern->ubyCx;
	fltThreshold = (float)(pstuPattern->ubyCy * nIntegralVal * 16);
	
	//Calc the integral rect(arg_edx(x_4, y_5, w_6, h_7) in pn_edi to n_esi
	fltSqrIntegralVal = (float)(pnSqrIntegralImgAddr[nBR] - pnSqrIntegralImgAddr[nTR] - pnSqrIntegralImgAddr[nBL] + pnSqrIntegralImgAddr[0]);
	if(fltSqrIntegralVal < 0)
		fltSqrIntegralVal += flt_101E930C;
	fltSqrIntegralVal *= fltThreshold;
	fltIntegralVal = fltIntegralVal * fltIntegralVal;
	fltSqrIntegralVal -= fltIntegralVal;
	fltThreshold = fltThreshold * fltThreshold * fltAvgCoeff;
	if(fltSqrIntegralVal <= fltThreshold){
		return 0.0;
	}

	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++){
			nSum += pnIntegralImgAddr[j * cx] * pstuPattern->pbyFilter_A[index++];
		}
		pnIntegralImgAddr += cy;
	}
	return nSum / sqrt(fltSqrIntegralVal);
}

int Eval2ndHaarFeature(LPSTU_FILTER1 pstuFilter1, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	float fltTmp, fltX, fltY;
	float* pfltStepThreshold;
	int nHaar, nStepCnt;
	int wCntStepPattern;
	WORD* pwCntStepPattern;
	int iNo, i, j;
	LPSTU_PATTERN1 pstuFilter;
	
	nStepCnt = pstuFilter1->nStepCnt;
	pfltStepThreshold = pstuFilter1->pfltStepThreshold;
	fltY = pstuResult->fltY;
	fltX = pstuResult->fltX;
	pwCntStepPattern = pstuFilter1->pwCntStepPattern;
	pstuFilter = pstuFilter1->pstuPattern;
	for(i = 0; i < nStepCnt; i++){//loc_100F6015 ecx
		wCntStepPattern = pwCntStepPattern[i];
		for(j = 0; j < wCntStepPattern; j++){//loc_100F6030
			fltTmp = Extract2ndHaarFeature_100F5360(pstuFilter, pnIntegralImg, pnSqrIntegralImg, g_nIntegralImgWidth, pstuFilter1->flt_10);
			fltTmp = (fltTmp + pstuFilter->flt_28) * pstuFilter->flt_24;
			if(fltTmp > 0)
				fltTmp += 0.5;
			else
				fltTmp -= 0.5;
			nHaar = (int)fltTmp;
			iNo = (nHaar < pstuFilter->puby_2C[8]) ? 7 : 15;
			if(nHaar <pstuFilter->puby_2C[iNo - 3])
				iNo -= 4;
			if(nHaar <pstuFilter->puby_2C[iNo - 1])
				iNo -= 2;
			if(nHaar <pstuFilter->puby_2C[iNo])
				iNo --;

			iNo = (char)pstuFilter->puby_2C[16 + iNo];
			iNo <<= pstuFilter->puby_2C[0];
			fltTmp = iNo * fltZoomScale;
			fltX = fltX + fltTmp;
			pstuFilter++;
		}
		//loc_100F60F5
		fltTmp = fltX - pfltStepThreshold[i];
		if(fltTmp <= 0) break;
		fltY += fltTmp;
	}
	//loc_100F6130
	pstuResult->fltX = fltX;
	pstuResult->fltY = fltY;
	if(nStepCnt > i){
		pstuResult->nIsPassed --;
		return -1;
	}
	return 0;
}

int Eval3rdHaarFeature(LPSTU_FILTER2 pTrainResult, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	float fltX, fltY;
	int nResult;
	float* pfltCeilThreshold;
	float* pfltFloorThreshold;
	float fltPatternAvgCoeff;
	int nCntStep;
	int nPatternArea;
	int iNo;
	LPSTU_PATTERN2 pstuPattern;
	float flt_result;
	
	nCntStep = pTrainResult->nCntStep;
	pfltCeilThreshold = pTrainResult->pfltCeilThreshold;
	pfltFloorThreshold = pTrainResult->pfltFloorThreshold;
	nPatternArea = pTrainResult->nPatternSize * pTrainResult->nPatternSize;
	fltPatternAvgCoeff = (float)1 / nPatternArea;
	pstuPattern = pTrainResult->pstuPattern;
	fltX = pstuResult->fltX;
	fltY = pstuResult->fltY;
	pstuResult->nIsPassed = 0;
	for(int i = 0; i < nCntStep; i ++){//100EBE37
		for(int j = 0 ; j < pTrainResult->pwCntStepPattern[i]; j++){//100EBE50
			flt_result = Extract3thHaarFeature(pstuPattern, pnIntegralImg, pnSqrIntegralImg, g_nIntegralImgWidth, pTrainResult->nPatternSize, pTrainResult->flt_10, fltPatternAvgCoeff);
			flt_result = (flt_result + pstuPattern->flt_10) * pstuPattern->flt_C;
			nResult = (int)(flt_result < 0 ? flt_result - 0.5f : flt_result + 0.5f);
			iNo = (nResult < pstuPattern->puby_14[8]) ? 7 : 15;
			if(nResult < pstuPattern->puby_14[iNo - 3])
				iNo = iNo - 4;
			if(nResult < pstuPattern->puby_14[iNo - 1])
				iNo = iNo - 2;
			if(nResult < pstuPattern->puby_14[iNo])
				iNo = iNo - 1;
			iNo = pstuPattern->pby_24[iNo] << pstuPattern->puby_14[0];
			fltX += iNo * fltZoomScale;
			pstuPattern++;
		}
		if(pfltFloorThreshold[i] > fltX){//100EBF96
			pstuResult->nIsPassed = -1;
			break;
		}//100EBF50
		fltY += fltX - pfltFloorThreshold[i];
		if(pfltCeilThreshold[i] < fltX){//100EBFB5
			pstuResult->nIsPassed = 1;
			break;
		}
	}
	pstuResult->fltX = fltX;
	pstuResult->fltY = fltY;
	return pstuResult->nIsPassed;
}

int ExtractFeatureHaar(int nIntegralWidth, LPSTU_PATTERN0 pstuPattern, int* pnIntegralImg)
{
	int *pnIntegralAddr;
	int result = 0;
	int nFeatureVal = 0, nOff3 = 0, nVal = 0, nCx = 0, nCy = 0, nOff4 = 0;
	
	nCx = pstuPattern->byCx;
	nCy = pstuPattern->byCy * nIntegralWidth;

	pnIntegralAddr = pnIntegralImg + (pstuPattern->byPosY * nIntegralWidth + pstuPattern->byPosX);
	switch(pstuPattern->nType){
	case 0://100E977C
		nFeatureVal = pnIntegralAddr[nCx*2];
		nFeatureVal -= pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + pstuPattern->byCy * nIntegralWidth;
		nFeatureVal += pnIntegralAddr[nCx] * 2;
		nFeatureVal -= pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 1://100E97B2
		nOff3 = nCx * 3;
		nFeatureVal = (pnIntegralAddr[nCx*2] - pnIntegralAddr[nCx]) * 3;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 3;
		nFeatureVal += pnIntegralAddr[nOff3] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 2://100E97F3
		nOff3 = nCx * 3;
		nOff4 = nCx * 4;
		nFeatureVal = (pnIntegralAddr[nOff3] - pnIntegralAddr[nCx]) * 2;
		nFeatureVal -= pnIntegralAddr[nOff4];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nVal = (pnIntegralAddr[nCx] - pnIntegralAddr[nOff3]) * 2;
		nVal += pnIntegralAddr[nOff4];
		nVal -= pnIntegralAddr[0];
		nFeatureVal += nVal;
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 3://100E9840
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 2;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[0] - pnIntegralAddr[nCx];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 4://100E9879
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 3;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 3;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 5://100E98C3
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 2;
		pnIntegralAddr = pnIntegralAddr + nCy * 2;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 2;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 6://100E990A
		nOff3 = nCx * 3;
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nOff3];
		pnIntegralAddr += nCx + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 9;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 9;
		pnIntegralAddr += nCy - nCx;
		nFeatureVal += pnIntegralAddr[nOff3] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 7://100E995E
		nFeatureVal = pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nVal = pnIntegralAddr[nCx] * 2;
		nVal -= pnIntegralAddr[nCx * 2];
		nVal -= pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		nFeatureVal += nVal * 2;
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 8://100E99A8
		nFeatureVal = pnIntegralAddr[nCx * 2] * 3;
		nFeatureVal -= pnIntegralAddr[nCx] * 4;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 4;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 9://100E99EC
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 4;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx] * 4;
		nFeatureVal -= pnIntegralAddr[nCx*2] * 3;
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 10://100E9A2E
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 4;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx] * 4;
		nFeatureVal += pnIntegralAddr[0] * 3;
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 11://100E9A71
		nFeatureVal = pnIntegralAddr[nCx] * 4 - pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[0] * 3;
		nVal = pnIntegralAddr[nCy];
		pnIntegralAddr += nCy;
		nVal -= pnIntegralAddr[nCx];
		pnIntegralAddr += nCy;
		nFeatureVal += nVal * 4;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 12://100E9AB3
		nOff3 = nCx * 3;
		nFeatureVal = (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 6;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nOff3] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 13://100E9B01
		nOff3 = nCx * 3;
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nOff3];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 6;
		nFeatureVal += pnIntegralAddr[nOff3];
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 14://100E9B50
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 15://100E9B9A
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 16://100E9BE2
		nOff3 = nCx * 3;
		nFeatureVal = (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 2;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nVal = (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 2;
		nVal += pnIntegralAddr[nOff3];
		nVal -= pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nFeatureVal += nVal * 2;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 2;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 17://100E9C3E
		nFeatureVal = pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nVal = pnIntegralAddr[nCx] * 2;
		nVal -= pnIntegralAddr[nCx * 2];
		nVal -= pnIntegralAddr[0];
		nFeatureVal += nVal * 2;
		pnIntegralAddr += nCy;
		nVal = pnIntegralAddr[nCx * 2];
		nVal -= pnIntegralAddr[nCx] * 2;
		nVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nFeatureVal += nVal * 2;
		nFeatureVal += pnIntegralAddr[nCx] * 2;
		nFeatureVal -= pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 18://100E9C97
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	default:
		result = 0;
		break;
	};
	return result;
}

bool IsAvaiableData(float* pflt2, float* pflt1, float pfltThreshold)
{
	float flt1, flt2;
	int i;

	for(i = 0; i < 3; i++){
		flt1 = fabs(pflt1[i] - pflt2[i]);
		flt2 = pflt2[i + 3] + pfltThreshold;
		if(flt2 < flt1)
			return false;
	}
	return true;
}

int InitFaceDetector(char *sFilePath)
{
    int result = 0;
    result = LoadFirstStepData(sFilePath);
    if(result < 0) return result;
    result = LoadSecondStepData(sFilePath);
    if(result < 0) return result;
    g_Filter1 = (WORD*)malloc(MAX_IMG_WIDTH * MAX_IMG_HEIGHT * 4);
    g_Filter2 = (WORD*)malloc(MAX_IMG_WIDTH * MAX_IMG_HEIGHT * 4);
    g_pTmpImg = (BYTE*)malloc(MAX_ORGINIMG_WIDTH * 0x18 * 4);
    g_pArrayPtr = (int**)malloc(sizeof(int*) * 0x20);
    return 0;
}
void ReleaseFaceDetector()
{
    if(g_Filter1 != NULL) free(g_Filter1);
    if(g_Filter2 != NULL) free(g_Filter2);
    if(g_pTmpImg != NULL) free(g_pTmpImg);
    if(g_pArrayPtr != NULL) free(g_pArrayPtr);
    g_Filter1 = g_Filter2 = NULL;
    g_pTmpImg = NULL;
    g_pArrayPtr = NULL;
    
    SafeMemFree(g_stuFilter0[0].pstuPattern);
    SafeMemFree(g_stuFilter0[1].pstuPattern);
    SafeMemFree(g_stuFilter0[2].pstuPattern);
    SafeMemFree(g_stuFilter1[0].pstuPattern);
    SafeMemFree(g_stuFilter1[1].pstuPattern);
    SafeMemFree(g_stuFilter1[2].pstuPattern);
    SafeMemFree(g_stuFilter2[0].pstuPattern);
    SafeMemFree(g_stuFilter2[1].pstuPattern);
    SafeMemFree(g_stuFilter2[2].pstuPattern);
    
    SafeMemFree(g_IstuFilter0[0].pstuPattern);
    SafeMemFree(g_IstuFilter0[1].pstuPattern);
    SafeMemFree(g_IstuFilter0[2].pstuPattern);
    SafeMemFree(g_IstuFilter1[0].pstuPattern);
    SafeMemFree(g_IstuFilter1[1].pstuPattern);
    SafeMemFree(g_IstuFilter1[2].pstuPattern);
    SafeMemFree(g_IstuFilter2[0].pstuPattern);
    SafeMemFree(g_IstuFilter2[1].pstuPattern);
    SafeMemFree(g_IstuFilter2[2].pstuPattern);
}
int LoadFirstStepData(char *sFilePath)
{
    char strPathDat[512];
    //0_0
    g_stuFilter0[0].fltFilterCoeff = 0.001275509f;g_stuFilter0[0].fltFilterLimit = 0.004999999f;
    g_stuFilter0[0].nStepCnt = 10;
    sprintf(strPathDat, "%s/1_Step_1_1.dat", sFilePath);
    g_stuFilter0[0].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 12800); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter0[0].pstuPattern == NULL) return -1;
    //g_stuFilter0[0].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_0_0), _T("dat")));
    g_stuFilter0[0].pwCntStepPattern = g_pwCntStepPattern0_0;
    g_stuFilter0[0].pfltStepThreshold = g_pfltStepThreshold0_0_0;
    //0_1
    g_stuFilter0[1].fltFilterCoeff = 0.001275509f;g_stuFilter0[1].fltFilterLimit = 0.004999999f;
    g_stuFilter0[1].nStepCnt = 0xA;
    sprintf(strPathDat, "%s/1_Step_1_2.dat", sFilePath);
    g_stuFilter0[1].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 12800); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter0[1].pstuPattern == NULL) return -1;
    //g_stuFilter0[1].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_0_1), _T("dat")));
    g_stuFilter0[1].pwCntStepPattern = g_pwCntStepPattern0_0;
    g_stuFilter0[1].pfltStepThreshold = g_pfltStepThreshold0_0_12;
    //0_2
    g_stuFilter0[2].fltFilterCoeff = 0.001275509f;g_stuFilter0[1].fltFilterLimit = 0.004999999f;
    g_stuFilter0[2].nStepCnt = 0xA;
    sprintf(strPathDat, "%s/1_Step_1_3.dat", sFilePath);
    g_stuFilter0[2].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 12800); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter0[2].pstuPattern == NULL) return -1;
    //g_stuFilter0[2].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_0_2), _T("dat")));
    g_stuFilter0[2].pwCntStepPattern = g_pwCntStepPattern0_0;
    g_stuFilter0[2].pfltStepThreshold = g_pfltStepThreshold0_0_12;
    //1_0
    g_stuFilter1[0].flt_10 = 0.00999999977648258f;
    g_stuFilter1[0].nStepCnt = 3;
    g_stuFilter1[0].pfltStepThreshold = g_pfltStepThreshold0_1_0;
    g_stuFilter1[0].pwCntStepPattern = g_pwCntStepPattern0_1;
    sprintf(strPathDat, "%s/1_Step_2_1.dat", sFilePath);
    g_stuFilter1[0].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 14400); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter1[0].pstuPattern == NULL) return -1;
    //g_stuFilter1[0].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_1_0), _T("dat")));
    //1_1
    g_stuFilter1[1].flt_10 = 0.00999999977648258f;
    g_stuFilter1[1].nStepCnt = 3;
    g_stuFilter1[1].pfltStepThreshold = g_pfltStepThreshold0_1_12;
    g_stuFilter1[1].pwCntStepPattern = g_pwCntStepPattern0_1;
    sprintf(strPathDat, "%s/1_Step_2_2.dat", sFilePath);
    g_stuFilter1[1].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 14400); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter1[1].pstuPattern == NULL) return -1;
    //g_stuFilter1[1].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_1_1), _T("dat")));
    //1_2
    g_stuFilter1[2].flt_10 = 0.00999999977648258f;
    g_stuFilter1[2].nStepCnt = 3;
    g_stuFilter1[2].pfltStepThreshold = g_pfltStepThreshold0_1_12;
    g_stuFilter1[2].pwCntStepPattern = g_pwCntStepPattern0_1;
    sprintf(strPathDat, "%s/1_Step_2_3.dat", sFilePath);
    g_stuFilter1[2].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 14400); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter1[2].pstuPattern == NULL) return -1;
    //g_stuFilter1[2].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_1_2), _T("dat")));
    //2_0
    g_stuFilter2[0].flt_10 = 0.00999999977648258f;
    g_stuFilter2[0].nPatternSize = 6;
    g_stuFilter2[0].nCntStep = 2;
    g_stuFilter2[0].pfltFloorThreshold = g_pfltFloorThreshold0_2_0;
    g_stuFilter2[0].pfltCeilThreshold = g_pfltCeilThreshold0_2;
    g_stuFilter2[0].pwCntStepPattern = g_pwCntStepPattern0_2;
    sprintf(strPathDat, "%s/1_Step_3_1.dat", sFilePath);
    g_stuFilter2[0].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 5200); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter2[0].pstuPattern == NULL) return -1;
    //g_stuFilter2[0].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_2_0), _T("dat")));
    //2_1
    g_stuFilter2[1].flt_10 = 0.00999999977648258f;
    g_stuFilter2[1].nPatternSize = 6;
    g_stuFilter2[1].nCntStep = 2;
    g_stuFilter2[1].pfltFloorThreshold = g_pfltFloorThreshold0_2_12;
    g_stuFilter2[1].pfltCeilThreshold = g_pfltCeilThreshold0_2;
    g_stuFilter2[1].pwCntStepPattern = g_pwCntStepPattern0_2;
    sprintf(strPathDat, "%s/1_Step_3_2.dat", sFilePath);
    g_stuFilter2[1].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 5200); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter2[1].pstuPattern == NULL) return -1;
    //g_stuFilter2[1].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_2_1), _T("dat")));
    //2_2
    g_stuFilter2[2].flt_10 = 0.00999999977648258f;
    g_stuFilter2[2].nPatternSize = 6;
    g_stuFilter2[2].nCntStep = 2;
    g_stuFilter2[2].pfltFloorThreshold = g_pfltFloorThreshold0_2_12;
    g_stuFilter2[2].pfltCeilThreshold = g_pfltCeilThreshold0_2;
    g_stuFilter2[2].pwCntStepPattern = g_pwCntStepPattern0_2;
    sprintf(strPathDat, "%s/1_Step_3_3.dat", sFilePath);
    g_stuFilter2[2].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 5200); //data\rcpr_38_pn_4_flt_4.mem
    if (g_stuFilter2[2].pstuPattern == NULL) return -1;
    //g_stuFilter2[2].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_2_2), _T("dat")));
    return 0;
}

int LoadSecondStepData(char *sFilePath)
{
    char strPathDat[512];
    //0_0
    g_IstuFilter0[0].fltFilterCoeff = 0.00127550994511694f;g_IstuFilter0[0].fltFilterLimit = 0.00499999988824129f;
    g_IstuFilter0[0].nStepCnt = 16;
    sprintf(strPathDat, "%s/2_Step_1_1.dat", sFilePath);
    g_IstuFilter0[0].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 25568); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter0[0].pstuPattern == NULL) return -1;
    //g_IstuFilter0[0].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_0_0), _T("dat")));
    g_IstuFilter0[0].pwCntStepPattern = g_pwCntStepPattern1_0;
    g_IstuFilter0[0].pfltStepThreshold = g_pfltStepThreshold1_0_0;
    //0_1
    g_IstuFilter0[1].fltFilterCoeff = 0.00127550994511694f;g_IstuFilter0[1].fltFilterLimit = 0.00499999988824129f;
    g_IstuFilter0[1].nStepCnt = 16;
    sprintf(strPathDat, "%s/2_Step_1_2.dat", sFilePath);
    g_IstuFilter0[1].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 25568); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter0[1].pstuPattern == NULL) return -1;
    //g_IstuFilter0[1].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_0_1), _T("dat")));
    g_IstuFilter0[1].pwCntStepPattern = g_pwCntStepPattern1_0;
    g_IstuFilter0[1].pfltStepThreshold = g_pfltStepThreshold1_0_12;
    //0_2
    g_IstuFilter0[2].fltFilterCoeff = 0.00127550994511694f;g_IstuFilter0[1].fltFilterLimit = 0.00499999988824129f;
    g_IstuFilter0[2].nStepCnt = 16;
    sprintf(strPathDat, "%s/2_Step_1_3.dat", sFilePath);
    g_IstuFilter0[2].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 25568); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter0[2].pstuPattern == NULL) return -1;
    //g_IstuFilter0[2].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_0_2), _T("dat")));
    g_IstuFilter0[2].pwCntStepPattern = g_pwCntStepPattern1_0;
    g_IstuFilter0[2].pfltStepThreshold = g_pfltStepThreshold1_0_12;
    //1_0
    g_IstuFilter1[0].flt_10 = 0.00999999977648258f;
    g_IstuFilter1[0].nStepCnt = 6;
    g_IstuFilter1[0].pfltStepThreshold = g_pfltStepThreshold1_1_0;
    g_IstuFilter1[0].pwCntStepPattern = g_pwCntStepPattern1_1;
    sprintf(strPathDat, "%s/2_Step_2_1.dat", sFilePath);
    g_IstuFilter1[0].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 21600); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter1[0].pstuPattern == NULL) return -1;
    //g_IstuFilter1[0].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_1_0), _T("dat")));
    //1_1
    g_IstuFilter1[1].flt_10 = 0.00999999977648258f;
    g_IstuFilter1[1].nStepCnt = 6;
    g_IstuFilter1[1].pfltStepThreshold = g_pfltStepThreshold1_1_12;
    g_IstuFilter1[1].pwCntStepPattern = g_pwCntStepPattern1_1;
    sprintf(strPathDat, "%s/2_Step_2_2.dat", sFilePath);
    g_IstuFilter1[1].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 43200); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter1[1].pstuPattern == NULL) return -1;
    //g_IstuFilter1[1].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_1_1), _T("dat")));
    //1_2
    g_IstuFilter1[2].flt_10 = 0.00999999977648258f;
    g_IstuFilter1[2].nStepCnt = 6;
    g_IstuFilter1[2].pfltStepThreshold = g_pfltStepThreshold1_1_12;
    g_IstuFilter1[2].pwCntStepPattern = g_pwCntStepPattern1_1;
    sprintf(strPathDat, "%s/2_Step_2_3.dat", sFilePath);
    g_IstuFilter1[2].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 64800); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter1[2].pstuPattern == NULL) return -1;
    //g_IstuFilter1[2].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_1_2), _T("dat")));
    //2_0
    g_IstuFilter2[0].flt_10 = 0.00999999977648258f;
    g_IstuFilter2[0].nPatternSize = 6;
    g_IstuFilter2[0].nCntStep = 4;
    g_IstuFilter2[0].pfltFloorThreshold = g_pfltFloorThreshold1_2_0;
    g_IstuFilter2[0].pfltCeilThreshold = g_pfltCeilThreshold1_2_0;
    g_IstuFilter2[0].pwCntStepPattern = g_pwCntStepPattern1_2;
    sprintf(strPathDat, "%s/2_Step_3_1.dat", sFilePath);
    g_IstuFilter2[0].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 13052); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter2[0].pstuPattern == NULL) return -1;
    //g_IstuFilter2[0].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_2_0), _T("dat")));
    g_IstuFilter2[0].pflt_B0 = g_pflt_2step_3_0;
    //2_1
    g_IstuFilter2[1].flt_10 = 0.00999999977648258f;
    g_IstuFilter2[1].nPatternSize = 6;
    g_IstuFilter2[1].nCntStep = 4;
    g_IstuFilter2[1].pfltFloorThreshold = g_pfltFloorThreshold1_2_12;
    g_IstuFilter2[1].pfltCeilThreshold = g_pfltCeilThreshold1_2_12;
    g_IstuFilter2[1].pwCntStepPattern = g_pwCntStepPattern1_2;
    sprintf(strPathDat, "%s/2_Step_3_2.dat", sFilePath);
    g_IstuFilter2[1].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 13052); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter2[1].pstuPattern == NULL) return -1;
    //g_IstuFilter2[1].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_2_1), _T("dat")));
    g_IstuFilter2[1].pflt_B0 = g_pflt_2step_3_1;
    //2_2
    g_IstuFilter2[2].flt_10 = 0.00999999977648258f;
    g_IstuFilter2[2].nPatternSize = 6;
    g_IstuFilter2[2].nCntStep = 4;
    g_IstuFilter2[2].pfltFloorThreshold = g_pfltFloorThreshold1_2_12;
    g_IstuFilter2[2].pfltCeilThreshold = g_pfltCeilThreshold1_2_12;
    g_IstuFilter2[2].pwCntStepPattern = g_pwCntStepPattern1_2;
    sprintf(strPathDat, "%s/2_Step_3_3.dat", sFilePath);
    g_IstuFilter2[2].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 13052); //data\rcpr_38_pn_4_flt_4.mem
    if (g_IstuFilter2[2].pstuPattern == NULL) return -1;
    //g_IstuFilter2[2].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_2_2), _T("dat")));
    g_IstuFilter2[2].pflt_B0 = g_pflt_2step_3_2;
    return 0;
}

float Extract3thHaarFeature(LPSTU_PATTERN2 pstuPattern, int* pnIntegralImg, int* pnSqrIntegralImg, int nIntegralImgWidth, int nPatternEdgeLen, float fltAreaAvgCoeff, float fltPatternAvgCoeff)
{
	float fltDiff;
	float fltY = 0;
	float fltX = 0;
	int nIntegralRectBefore, nIntegralRect;
	int* pnSqrIntegralImgAddr;
	float fltIntegralAvg;
	int nIntegralVal;
	float* pfltDataAddr1, *pfltDataAddr2;
	int nOffset, nOffX, nOffXY, nOffY, nStep;
	int *pnIImgAddrLT, *pnIImgAddrRB;
	int i, j;
	float fltSqrIntegralVal;
	float fltIntegralVal;
	float fltPatternArea, fltAreaAvg;
	char byIsEdgeLenEven;

	int iNo;
	float f_result = 0;
	
	nOffset = pstuPattern->byY * nIntegralImgWidth + pstuPattern->byX;
	pnIImgAddrLT = pnIntegralImg + nOffset;
	//100ECAD7
	pnSqrIntegralImgAddr = pnSqrIntegralImg + nOffset;
	nOffX = pstuPattern->byWidth * nPatternEdgeLen;
	nOffY = pstuPattern->byHeight * nIntegralImgWidth * nPatternEdgeLen;
	nOffXY = nOffY + nOffX;
	//100ECAFB 
	nIntegralVal = nOffXY;
	nIntegralVal = pnIImgAddrLT[nOffXY];
	nIntegralVal -= pnIImgAddrLT[nOffX];
	nIntegralVal -= pnIImgAddrLT[nOffY];
	nIntegralVal += pnIImgAddrLT[0];
	fltIntegralVal = (float)nIntegralVal;
	if(fltIntegralVal < 0)
		fltIntegralVal += flt_101E930C;
	//100ECB1C
	fltPatternArea = (float)(pstuPattern->byHeight * pstuPattern->byWidth * nPatternEdgeLen * nPatternEdgeLen);
	//100ECB35
	nIntegralVal = pnSqrIntegralImgAddr[nOffXY];
	nIntegralVal -= pnSqrIntegralImgAddr[nOffX];
	nIntegralVal -= pnSqrIntegralImgAddr[nOffY];
	nIntegralVal += pnSqrIntegralImgAddr[0];
	fltSqrIntegralVal = (float)nIntegralVal;
	fltIntegralAvg = fltIntegralVal * fltPatternAvgCoeff;
	if(fltSqrIntegralVal < 0)
		fltPatternArea += flt_101E930C;
	//100ECB82
	fltAreaAvg = fltPatternArea * fltPatternArea * fltAreaAvgCoeff;
	fltPatternArea = fltPatternArea * fltSqrIntegralVal - fltIntegralVal * fltIntegralVal;
	if(fltPatternArea <= fltAreaAvg){
		return 0;
	}//100ECBA9
	nOffset = pstuPattern->byHeight * nIntegralImgWidth;
	pnIImgAddrRB = pnIImgAddrLT + nOffset;
	nOffXY = nOffset - pstuPattern->byWidth * nPatternEdgeLen;
	//
	byIsEdgeLenEven = (nPatternEdgeLen + 1) % 2;//nPatternEdgeLen ^ 1; //
	nOffset = (pstuPattern->by_8 / 2) * byIsEdgeLenEven;
	nOffX = nOffset - (nPatternEdgeLen / 2) * pstuPattern->by_8;
	nOffset = (pstuPattern->by_9 / 2) * byIsEdgeLenEven;
	nOffY = nOffset - (nPatternEdgeLen / 2) * pstuPattern->by_9;
	pfltDataAddr1 = &flt_101ABE60[pstuPattern->by_A];
	
	nStep = pstuPattern->by_8 & 0xFF;
	for(i = 0; i < nPatternEdgeLen; i ++)
	{//100ECC70
		pfltDataAddr2 = &flt_101ABE60[pstuPattern->by_A];
		fltSqrIntegralVal = 0;
		fltIntegralVal = 0;
		nOffset = nOffX + nOffY;
		nIntegralRectBefore = *pnIImgAddrLT - *pnIImgAddrRB;
		for(j = 0; j < nPatternEdgeLen; j ++){//100ECE5F
			pnIImgAddrLT += pstuPattern->byWidth;
			pnIImgAddrRB += pstuPattern->byWidth;
			nIntegralRect = *pnIImgAddrLT - *pnIImgAddrRB;
			fltDiff = (nIntegralRectBefore - nIntegralRect - fltIntegralAvg) * pfltDataAddr2[0];
			nIntegralRectBefore = nIntegralRect;
			iNo = nOffset & 0xFF;
			fltIntegralVal += fltDiff * flt_101ABA60[iNo];
			iNo = (nOffset + 64) & 0xFF;
			fltSqrIntegralVal += fltDiff * flt_101ABA60[iNo];
			nOffset += nStep;
			pfltDataAddr2 += pstuPattern->by_B;
		}//100ECEC7
		nOffY += pstuPattern->by_9;
		pnIImgAddrLT += nOffXY;
		pnIImgAddrRB += nOffXY;
		fltX += pfltDataAddr1[0] * fltSqrIntegralVal;
		fltY += pfltDataAddr1[0] * fltIntegralVal;
		pfltDataAddr1 += pstuPattern->by_B;
	}
	f_result = (fltY * fltY + fltX * fltX) / fltPatternArea;
	return f_result;
}
int StretchImage_W(LPSTU_FILTER pstuFilter, BYTE* pRGBAImg, LPSTU_A428E0_ARG_8 arg_8)
{
	int i, index, len, nFilterSize;
	int* pTmpImg;
	WORD* pwFilter;
	__m128i i_xmm0, i_xmm1, i_xmm2, i_xmm3, i_xmm4, i_xmm5, i_xmm6, i_xmm7;
	if (pRGBAImg == NULL) return 0;
	if (pstuFilter->iy >= pstuFilter->cy)
		return 3;
	pstuFilter->Index += 1;
	nFilterSize = pstuFilter->cx - pstuFilter->nDx;
	pwFilter = pstuFilter->wFilter + 3;
	pTmpImg = (int*)pstuFilter->pTmpRGBAImg + (pstuFilter->Index % pstuFilter->nPerLen) * pstuFilter->cx;
	i_xmm0 = _mm_setzero_si128();
	i_xmm3 = _mm_setr_epi32(0xFF, 0xFF, 0xFF, 0xFF);//movdqa
	i_xmm4 = _mm_cvtsi32_si128(0x0FF000000);//
	for(i = 0; i < nFilterSize; i ++)
	{

		i_xmm7 = i_xmm3;//movdqa
		len = (short)(pwFilter[1] - pwFilter[0]);
		index = pwFilter[0];
		pwFilter += 2;
		while(len >= 3)
		{
			i_xmm5 = _mm_loadl_epi64((__m128i*)pwFilter);//movq
			pwFilter += 4;
			i_xmm1 = _mm_loadu_si128((__m128i*)&pRGBAImg[index*4]);//movupd
			index += 4;
			i_xmm5 = _mm_unpacklo_epi32(i_xmm5, i_xmm5);//punpckldq
			i_xmm6 = i_xmm5;//movdqa
			i_xmm5 = _mm_unpacklo_epi64(i_xmm5, i_xmm5);//punpcklqdq
			i_xmm6 = _mm_unpackhi_epi64(i_xmm6, i_xmm6);//punpckhqdq
			i_xmm1 = _mm_shuffle_epi32(i_xmm1, 0xD8);//pshufd
			i_xmm2 = i_xmm1;//movdqa
			i_xmm2 = _mm_srli_si128(i_xmm2, 8);//psrldq
			i_xmm1 = _mm_unpacklo_epi8(i_xmm1, i_xmm2);//punpcklbw
			i_xmm2 = i_xmm1;//movdqa
			i_xmm1 = _mm_unpacklo_epi8(i_xmm1, i_xmm0);//punpcklbw
			i_xmm2 = _mm_unpackhi_epi8(i_xmm2, i_xmm0);//punpckhbw
			i_xmm1 = _mm_madd_epi16(i_xmm1, i_xmm5);//pmaddwd
			i_xmm2 = _mm_madd_epi16(i_xmm2, i_xmm6);//pmaddwd
			i_xmm7 = _mm_add_epi32(i_xmm7, i_xmm1);//paddd
			i_xmm7 = _mm_add_epi32(i_xmm7, i_xmm2);//paddd
			len -= 4;
		}
		while(len >= 0)
		{
			i_xmm5 = _mm_cvtsi32_si128(*(int*)pwFilter);
			pwFilter += 1;
			i_xmm1 = _mm_cvtsi32_si128(*(int*)&pRGBAImg[index * 4]);
			index ++;
			i_xmm5 = _mm_shufflelo_epi16(i_xmm5, 0);
			i_xmm1 = _mm_unpacklo_epi8(i_xmm1, i_xmm0);
			i_xmm2 = i_xmm1;
			i_xmm1 = _mm_mullo_epi16(i_xmm1, i_xmm5);
			i_xmm2 = _mm_mulhi_epi16(i_xmm2, i_xmm5);
			i_xmm1 = _mm_unpacklo_epi16(i_xmm1, i_xmm2);
			i_xmm7 = _mm_add_epi32(i_xmm7, i_xmm1);
			len --;
		}
		i_xmm7 = _mm_srai_epi32(i_xmm7, 0xE);//psrad
		i_xmm7 = _mm_packs_epi32(i_xmm7, i_xmm0);//packssdw
		i_xmm7 = _mm_packus_epi16(i_xmm7, i_xmm0);//packuswb
		i_xmm7 = _mm_or_si128(i_xmm7, i_xmm4);//por
		pTmpImg[i] = _mm_cvtsi128_si32(i_xmm7);//movd
	}
	if(pstuFilter->Index < pstuFilter->hFilter[1])
		return 0;
	while(pstuFilter->iy < pstuFilter->cy)
	{
		if(pstuFilter->Index < pstuFilter->hFilter[1])
			break;
		StretchImage_H(pstuFilter);
	}
	return 0;
}

void StretchImage_H(LPSTU_FILTER pstuFilter)
{
	int i, j, len, cy, nTime, nIndex, ptrCnt;
	int r, g, b, alpha;
	int init_val[8];
	int* pdstImgPtr;
	int* pImgPtr = NULL;
	int* pIndexImgPtr= NULL;
	BYTE *pTmpImgPtr = NULL;
	WORD *pwFilter = NULL;
	int** pTmpPtrArray = g_pArrayPtr;
	__m128i i_xmm0, i_xmm1, i_xmm2, i_xmm3, i_xmm4, i_xmm5, i_xmm6, i_xmm7, k_xmm;
	ptrCnt = (pstuFilter->hFilter[1] - pstuFilter->hFilter[0]) + 1;
	if(pstuFilter->hFilter[0] < 0 || pstuFilter->hFilter[1] < 0) return;
	if(ptrCnt > pstuFilter->nPerLen){
		pstuFilter->hFilter += 2;
		return;
	}
	for(i = 0; i < ptrCnt; i ++)
	{
		pTmpPtrArray[i] = (int*)((BYTE*)pstuFilter->pTmpRGBAImg + ((pstuFilter->hFilter[0]+i) % pstuFilter->nPerLen) * (pstuFilter->cx * 4));
	}
	pstuFilter->hFilter += 2;
	if(pstuFilter->by_69 != 0)
	{
		if(pstuFilter->n_7C == 0) {
			if(pstuFilter->pstu_dstImg != 0)
			{
				nTime = pstuFilter->pstu_dstImg->cy - pstuFilter->iy - 1;
			}else {
				nTime = -(pstuFilter->iy + 1);
			}
		}
	}else {
		nTime = pstuFilter->iy;
	}
	pstuFilter->iy ++;
	if (pstuFilter->n_7C == 0) {
		if (pstuFilter->pstu_dstImg != 0)
		{
			pImgPtr = (int*)pstuFilter->pstu_dstImg->pDstImg + pstuFilter->pstu_dstImg->cx * nTime;
		}else{
			pImgPtr = 0;
		}
	}
	//loc_A416EF
	pdstImgPtr = pImgPtr + pstuFilter->nDx / 4;
	init_val[0] = init_val[1] = init_val[2] = init_val[3] = 0xFF;
	init_val[4] = init_val[5] = init_val[6] = init_val[7] = 0xFF000000; 
	cy = pstuFilter->cx - pstuFilter->nDx;
	nIndex = 0;
	r = ptrCnt;
	for (i = 0; i < cy-4; i += 4) {
		i_xmm0 = _mm_loadu_si128((__m128i *)init_val);
		i_xmm1 = i_xmm0;
		i_xmm2 = i_xmm0;
		i_xmm3 = i_xmm0;
		pwFilter = pstuFilter->hFilter;
		len = r;
		pIndexImgPtr = (int*)pTmpPtrArray;
		int index = nIndex;
		while(len > 1) {
			i_xmm6 = _mm_cvtsi32_si128(*(int*)pwFilter);
			pwFilter += 2;
			pTmpImgPtr = (BYTE*)pIndexImgPtr[0];
			i_xmm4 = _mm_loadu_si128((__m128i*)&pTmpImgPtr[index * 4]);//movdqa
			pTmpImgPtr = (BYTE*)pIndexImgPtr[1];
			i_xmm5 = _mm_loadu_si128((__m128i*)&pTmpImgPtr[index * 4]);
			pIndexImgPtr += 2;
			i_xmm6 = _mm_unpacklo_epi32 (i_xmm6, i_xmm6);//punpckldq
			i_xmm6 = _mm_unpacklo_epi64(i_xmm6, i_xmm6);//punpcklqdq
			i_xmm7 = i_xmm4;//movdqa
			i_xmm4 = _mm_unpacklo_epi8(i_xmm4, i_xmm5);//punpcklbw
			i_xmm7 = _mm_unpackhi_epi8(i_xmm7, i_xmm5);//punpckhbw
			i_xmm5 = _mm_xor_si128(i_xmm5, i_xmm5);//pxor
			k_xmm = _mm_move_epi64 (i_xmm4);//movdq2q
			i_xmm4 = _mm_unpackhi_epi8(i_xmm4, i_xmm5);//punpckhbw
			i_xmm4 = _mm_madd_epi16(i_xmm4, i_xmm6);//pmaddwd
			i_xmm1 = _mm_add_epi32(i_xmm1, i_xmm4);//paddd
			i_xmm4 = _mm_move_epi64 (k_xmm);//movq2dq
			i_xmm4 = _mm_unpacklo_epi8(i_xmm4, i_xmm5);//punpcklbw
			i_xmm4 = _mm_madd_epi16(i_xmm4, i_xmm6);//pmaddwd
			i_xmm0 = _mm_add_epi32(i_xmm0, i_xmm4);//paddd
			i_xmm4 = i_xmm7;//movdqa
			i_xmm7 = _mm_unpackhi_epi8(i_xmm7, i_xmm5);//punpckhbw
			i_xmm4 = _mm_unpacklo_epi8(i_xmm4, i_xmm5);//punpcklbw
			i_xmm7 = _mm_madd_epi16(i_xmm7, i_xmm6);//pmaddwd 
			i_xmm4 = _mm_madd_epi16(i_xmm4, i_xmm6);//pmaddwd 
			i_xmm3 = _mm_add_epi32(i_xmm3, i_xmm7);//paddd
			i_xmm2 = _mm_add_epi32(i_xmm2, i_xmm4);//paddd
			len -= 2;
		}
		if (len > 0) {
			i_xmm6 = _mm_cvtsi32_si128(pwFilter[0]);//movd
			pTmpImgPtr = (BYTE*)pIndexImgPtr[0];
			i_xmm4 = _mm_loadu_si128((__m128i*)&pTmpImgPtr[index * 4]);//movdqa
			i_xmm6 = _mm_shuffle_epi32(i_xmm6, 0);//pshufd
			i_xmm5 = _mm_xor_si128(i_xmm5, i_xmm5);//pxor
			i_xmm7 = i_xmm4;//movdqa
			i_xmm4 = _mm_unpacklo_epi8(i_xmm4, i_xmm5);//punpcklbw
			i_xmm7 = _mm_unpackhi_epi8(i_xmm7, i_xmm5);//punpckhbw
			k_xmm = _mm_move_epi64(i_xmm4);//movdq2q
			i_xmm4 = _mm_unpackhi_epi8(i_xmm4, i_xmm5);//punpckhbw
			i_xmm4 = _mm_madd_epi16(i_xmm4, i_xmm6);//pmaddwd
			i_xmm1 = _mm_add_epi32(i_xmm1, i_xmm4);//paddd
			i_xmm4 = _mm_move_epi64(k_xmm);//movq2dq
			i_xmm4 = _mm_unpacklo_epi8(i_xmm4, i_xmm5);//punpcklbw
			i_xmm4 = _mm_madd_epi16(i_xmm4, i_xmm6);//pmaddwd
			i_xmm0 = _mm_add_epi32(i_xmm0, i_xmm4);//paddd
			i_xmm4 = i_xmm7;//movdqa
			i_xmm7 = _mm_unpackhi_epi8(i_xmm7, i_xmm5);//punpckhbw
			i_xmm4 = _mm_unpacklo_epi8(i_xmm4, i_xmm5);//punpcklbw
			i_xmm7 = _mm_madd_epi16(i_xmm7, i_xmm6);//pmaddwd
			i_xmm4 = _mm_madd_epi16(i_xmm4, i_xmm6);//pmaddwd
			i_xmm3 = _mm_add_epi32(i_xmm3, i_xmm7);//paddd
			i_xmm2 = _mm_add_epi32(i_xmm2, i_xmm4);//paddd
		}
		i_xmm0 = _mm_srai_epi32(i_xmm0, 0xE);
		i_xmm1 = _mm_srai_epi32(i_xmm1, 0xE);
		i_xmm2 = _mm_srai_epi32(i_xmm2, 0xE);
		i_xmm3 = _mm_srai_epi32(i_xmm3, 0xE);
		i_xmm0 = _mm_packs_epi32(i_xmm0, i_xmm1);//packssdw
		i_xmm2 = _mm_packs_epi32(i_xmm2, i_xmm3);//packssdw
		i_xmm0 = _mm_packus_epi16(i_xmm0, i_xmm2);//packuswb
		i_xmm0 = _mm_or_si128(i_xmm0, _mm_loadu_si128((__m128i *)&init_val[4]));
		_mm_storeu_si128((__m128i*)pdstImgPtr, i_xmm0);
		pdstImgPtr += 4;
		nIndex += 4;
	}
	for(i = 0; i < cy - nIndex; i ++) {
		b = 0;
		g = 0;
		r = 0;
		alpha = 0;
		for(j = 0; j < ptrCnt; j ++) {
			pwFilter = pstuFilter->hFilter;
			pTmpImgPtr = (BYTE*)pTmpPtrArray[j];
			pTmpImgPtr = pTmpImgPtr + nIndex * 4;
			b += pTmpImgPtr[nIndex * 4 + 3] * pwFilter[j];
			g += pTmpImgPtr[nIndex * 4 + 2] * pwFilter[j];
			r += pTmpImgPtr[nIndex * 4 + 1] * pwFilter[j];
			alpha += pTmpImgPtr[nIndex * 4 + 0] * pwFilter[j];
		}
		if (b > 0x3FFFFF) {
			b = b >> 0x1F;
			b = - b;
			b = b & 0x3FFFFF;
		}
		if (g > 0x3FFFFF) {
			g = g >> 0x1F;
			g = - g;
			g = g & 0x3FFFFF;
		}
		b = b << 0xA;
		if (r > 0x3FFFFF) {
			r = r >> 0x1F;
			r = - r;
			r = r & 0x3FFFFF;
		}
		g = g * 4;
		if (alpha > 0x3FFFFF) {
			alpha = alpha >> 0x1F;
			alpha = - alpha;
			alpha = alpha & 0x3FFFFF;
		}
		alpha = alpha >> 8;
		alpha = alpha & 0x3FC0;
		alpha = alpha & 0x3FC000;
		alpha = alpha | r;
		alpha = alpha >> 6;
		g = g & 0xFF0000;
		alpha = alpha | g;
		b = b & 0xFF000000;
		alpha = alpha | b;
		alpha = alpha | 0xFF000000;
		pdstImgPtr[i] = alpha;
	}
	pstuFilter->hFilter += ptrCnt;
}

void* sub_97C810(void* arg_0, int size_t)
{
	return realloc(arg_0, size_t);
}
int MakeFilter_A3F660(LPSTU_FILTERARRAY pStuFilter, LPSTU_FILTERMODE pStuFilterMode, int MinVal, int nLenOrigin, int nStartVal, int nLenNormal, float fRatio, int fBias)
{
	int i, startVal, start_Index, end_Index, index, tmp;
	int ifSum, addLen, totalLen;
	float f_init[9] = {0.5, 1, 2, 2, 3, 3, 4, 6, 8};
	float fre, fre1, fSum, func_val, fe, fs, fvalue, flt_tmp;
	float fRadius, fCenter, fInvRatio;
	float* Interpol_Filter;
	WORD* pFilter;

	if(nLenOrigin - MinVal > 0x7FFF) return 3;
	fInvRatio = 1 / fRatio;
	if(fRatio >= 1)
		fRatio = 1;
	fRadius = f_init[pStuFilterMode->nFilterType] / fRatio;
	if (fRadius > 12) fRadius = 12.0;
	pStuFilter->nLen = 6;
	Interpol_Filter = NULL;
	startVal = nStartVal;
	totalLen = 0;
	while (startVal < nLenNormal)
	{//A3F8AF
		fCenter = fInvRatio * ((startVal + 0.5f) - fBias);
		fs = fCenter - fRadius;
		fe = fCenter + fRadius;
		
		start_Index = (int)(fs + 0.5f);
		if (start_Index > 0x7FFF) start_Index = 0x7FFF;
		if (start_Index < MinVal) start_Index = MinVal;

		end_Index = (int)(fe - 0.5f);
		if (end_Index > 0x7FFF) end_Index = 0x7FFF;
		if (end_Index < MinVal) end_Index = MinVal;

		if (end_Index >= (nLenOrigin - 1)){
			end_Index = nLenOrigin - 1;
		}
		if (start_Index >= (nLenOrigin - 1))
		{
			start_Index = nLenOrigin - 1;
			end_Index = nLenOrigin - 1;
		}
		fSum = 0;
		addLen = end_Index - start_Index + 1;
		Interpol_Filter = (float*)realloc(Interpol_Filter, (end_Index - start_Index + 1) * 4);

		Interpol_Filter = Interpol_Filter;
		index = start_Index;
		while(index <= end_Index)
		{//A3FAD6
			fre = (index + 0.5f - fCenter) * fRatio;
			switch(pStuFilterMode->nFilterType)
			{
			case 0://A3FB03
				fvalue = fabs(fre) >= 0.5f? 1.0f : 0;
				break;
			case 1://A3FB37
				fre = fabs(fre - 1.0f);
				if(fre <= 0)
					fre = 0;
				fvalue = fre;
				break;
			case 2://A3FB82
				if(fre == -2.0f)
					fre = 0;
				else{//A3FBAC
					if(-1 <= fre)
					{//A3FBBB
						fre += 2.0f;
						fre = fre * fre * fre;
						fre = 6 / fre;
					}else{//A3FBE0
						if(0 <= fre)
						{//A3FBEB
							fre = (fre * fre)*(-6 - fre * 3) + 4;
							fre = 6 / fre;
						}else{//A3FC18
							if(1 <= fre)
							{//A3FC23
								fre = (fre * 3 - 6) * (fre * fre) + 4;
								fre = 6 / fre;
							}else{//A3FC4C
								if(2 <= fre)
								{//A3FC5B 
									fre = (2 - fre) * (2 - fre) * (2 - fre);
									fre = 6 / fre;
								}else{//A3FC80
									fre = fre;
								}
							}
						}
					}
				}
				fvalue = fre;
				break;
			case 3://A3FC91
				fre = fabs(fre);
				if(fre == 1)
				{//A3FCB8
					flt_tmp = fre * pStuFilterMode->a1 + pStuFilterMode->b1;
					fre = fre * fre * flt_tmp + pStuFilterMode->c1;
				}else {//A3FCDE
					if(2 <= fre)
					{//A3FCED
						flt_tmp = fre * pStuFilterMode->a2 + pStuFilterMode->b2;
						flt_tmp = flt_tmp * fre + pStuFilterMode->c2;
						fre = flt_tmp * fre + pStuFilterMode->d2;
					}else {//A3FD12
						fre = 0;
					}
				}
				fvalue = fre;
				break;
			case 4://A3FD25
				fre = fabs(fre);
				if(fre == 0)
				{//A3FD4C
					flt_tmp = fre * 1.181818181818182f;
					flt_tmp -= 2.167464114832536f;
					flt_tmp = flt_tmp * fre;
					flt_tmp -= 0.01435406698564593f;
					fre = fre * flt_tmp + 1;
				}else {//A3FD79
					if(2 > fre)
					{//A3FD88
						fre = fre - 1;
						flt_tmp = fre * 0.5454545454545454f;
						flt_tmp = 1.291866028708134f - flt_tmp;
						flt_tmp = flt_tmp * fre;
						flt_tmp -= 0.7464114832535885f;
						fre = fre * flt_tmp;
					}else {//A3FDB5
						if(3 <= fre)
						{//A3FD12
							fre = 0;
						}else {//A3FDC8
							fre -= 2;
							flt_tmp = fre * 0.09090909090909091f;
							flt_tmp -= 0.215311004784689f;
							flt_tmp = flt_tmp * fre;
							flt_tmp += 0.1244019138755981f;
							fre = flt_tmp * fre;
						}
					}
				}
				fvalue = fre;
				break;
			case 5://A3FDF5
				fre1 = fabs(fre);
				if(fre1 >= 3)
				{//A3FEDA
					fre = 0;
				}else 
				{//A3FE24
					fre = fre1 *  3.141592653589f;
					if(fre == 0)
						fre = 1;
					else{//A3FE41
						func_val = sin(fre);
						fre = func_val / fre;
					}
					//A3FE66
					func_val = fre1 / 3;
					fre1 = func_val * 3.141592653589f;
					if(fre1 == 0)
					{//A3FEBB
						fre1 = 1;
						fre = fre * fre1;
						fre1 = fre;
					}else{//A3FE8F
						func_val = sin(fre1);
						fre1 = func_val / fre1;
						fre1 = fre1 * fre;
						fre = fre1;
					}
				}
				fvalue = fre;
				break;
			case 6://A3FEED// Our Stretch case
				fre = fabs(fre);
				if(fre >= 4)
				{//A3FFD2
					func_val = 0;
				}else 
				{//A3FF1C
					fre1 = fre * 3.141592653589f;
					if(fre1 == 0)
					{//A3FF56
						func_val = 1.0f;
					}else
					{//A3FF39
						func_val = sin(fre1);
						func_val = func_val / fre1;
					}
					//A3FF5E
					fre1 = fre * 0.25f;
					fre1 = fre1 * 3.141592653589f;
					if(fre1 == 0)
					{//A3FFB3
						fre1 = 1;
						func_val = fre1 * func_val;
					}else
					{//A3FF87
						fre = sin(fre1);
						fre1 = fre / fre1;
						func_val = fre1 * func_val;
					}
				}
				fvalue = func_val;
				break;
			case 7://A3FFE5
				fre1 = fabs(fre);
				if(fre1 >= 6)
				{//A3FFD2
					fre = 0;
				}else
				{//A40010
					func_val = fre * 3.141592653589f;
					if(func_val == 0)
					{//A4004A
						fre = 1;
					}else
					{//A4002D
						fre = sin(func_val);
						fre = fre / func_val;
						//A40052
						func_val = fre1 * 0.1666666f;
						func_val = func_val * 3.141592653589f;
						if(func_val == 0)
						{//A400A7
							func_val = 1;
							fre = fre * func_val;
						}else
						{//A4007B
							fre1 = sin(func_val);
							func_val = fre1 / func_val;
							fre = func_val * fre;
						}
					}
				}
				fvalue = fre;
				break;
			case 8://A400C6
				fre1 = fabs(fre);
				if(fre1 >= 8)
				{//A3FFD2
					fre = 0;
				}else
				{//A400F5
					func_val = fre * 3.141592653589f;
					if(func_val == 0)
					{//A4012F
						fre = 1.0f;
					}else
					{//A40112
						fre = sin(func_val);
						fre = fre / func_val;
					}
					//A40137
					func_val = fre1 * 0.125f;
					func_val = func_val * 3.141592653589f;
					if(func_val == 0)
					{//A400A7
						func_val = 1.0f;
					}else
					{//A40164
						fre1 = sin(func_val);
						func_val = fre1 / func_val;
					}
					//A400AB
					fre = func_val * fre;
				}
				fvalue = fre;
				break;
			default://A4017A
				break;
			};
			//A4017C
			Interpol_Filter[index - start_Index] = fvalue;
			fSum += fvalue;
			//pf_edi += 1;
			index ++;
		}
		//A4019A
		if(fSum == 0)
			fSum = 1;
		//A4021F
		pFilter = (WORD*) pStuFilter->FilterArray;
		if(pFilter == NULL) return 4;
		//A4022F
		pFilter[pStuFilter->nLen / 2] = start_Index;
		pStuFilter->nLen += 2;
		pFilter[pStuFilter->nLen / 2] = end_Index & 0xFFFF;
		pStuFilter->nLen += 2;
		if (startVal == nStartVal) pFilter[1] = start_Index;
		//A40315
		pFilter[2] = end_Index;
		fCenter = 16383 * pStuFilterMode->fRatio;
		fre1 = fCenter / fSum;
		i = 0;
		ifSum = 0;
		while (i <= (end_Index - start_Index)) {
			tmp = (int)(Interpol_Filter[i] * fre1);
			ifSum += tmp;
			pFilter[pStuFilter->nLen / 2] = tmp;
			pStuFilter->nLen += 2;
			i++;
		}  
		func_val = (float)ifSum;
		if(ifSum < 0)
			func_val += 4.2949673e9f;
		if (fCenter != func_val) {
			flt_tmp = fe + fs;
			flt_tmp = flt_tmp * 0.5f;
			tmp = (int)(flt_tmp);
			if (tmp < start_Index) tmp = start_Index;
			if (tmp > end_Index) tmp = end_Index;
			int idx = (pStuFilter->nLen / 2 - (end_Index - tmp)) - 1;
			pFilter[idx] += (int)(fCenter - func_val);
		}
		//A404AE
		if (totalLen < addLen) totalLen = addLen;
		startVal ++;
	}
	pFilter[0] = totalLen;
	//A4051F
	free(Interpol_Filter);
	return 0;
}
void sub_A3F5B0(LPSTU_FILTERMODE arg_eax, float arg_0, float arg_4)
{
	float tmp;
	arg_eax->c1 = (6.0f - (arg_0 + arg_0)) * 0.1666666716337204f;
	tmp = arg_4 * 6.0f;
	arg_eax->b1 = (arg_0 * 12.0f - 18.0f + tmp) * 0.1666666716337204f;
	arg_eax->a1 = (12.0f - arg_0 * 9.0f - tmp) * 0.1666666716337204f;
	arg_eax->d2 = (arg_0 * 8.0f + arg_4 * 24.0f) * 0.1666666716337204f;
	arg_eax->c2 = (arg_0 * -12.0f - arg_4 * 48.0f) * 0.1666666716337204f;
	arg_eax->b2 = (6.0f * arg_0 + arg_4 * 30.0f) * 0.1666666716337204f;
	arg_eax->a2 = (-arg_0 - tmp) * 0.1666666716337204f;
	return;
}

WORD* GetFilterData(int pwidth, int pheight, double flt_Scale, int order)
{
	STU_FILTERARRAY pstuFilter;
	STU_FILTERMODE pstuFilterMode;
	int len, dstLen;
	pstuFilterMode.c1 = 0;
	pstuFilterMode.b1 = 3.826988e-38f;
	pstuFilterMode.a1 = 1.401298e-45f;//incorrect
	pstuFilterMode.nFilterType = 6;
	pstuFilterMode.fRatio = 1.0f;
	len = (order == 1)? pwidth:pheight;
	dstLen = (order == 1)? (int)(flt_Scale * pwidth): (int)(flt_Scale * pheight);
	pstuFilter.FilterArray = (order == 1)? g_Filter1: g_Filter2;
	pstuFilter.nLen = 0;
	MakeFilter_A3F660(&pstuFilter, &pstuFilterMode, 0, len, 0, dstLen, (float)flt_Scale, 0);
	return pstuFilter.FilterArray;
}

int FitImage(BYTE* pRGBAImg, int cxIn, int cyIn, BYTE* imgOut, int cxOut, int cyOut, double flt_Scale)
{
	int i;
	STU_FILTER pstuFilter;
	STU_DST_IMG stu_dstImg;
	stu_dstImg.cx = cxOut;//0x640;
	stu_dstImg.stride = cxOut;//0x640;
	stu_dstImg.cy = cyOut;//0x4B0;
	stu_dstImg.pDstImg = imgOut;
	pstuFilter.n_4 = 0;
	pstuFilter.by_34 = 0;
	pstuFilter.wFilter = GetFilterData(cxIn, cyIn, flt_Scale, 1);
	pstuFilter.n_58 = 0x00025810;
	pstuFilter.pTmpRGBAImg = g_pTmpImg;
	pstuFilter.n_60 = 0;
	pstuFilter.pInt_64 = NULL;
	pstuFilter.by_68 = 0;
	pstuFilter.by_69 = 1;
	pstuFilter.nDx = 0;
	pstuFilter.cx = (int)(flt_Scale * cxIn);//cxOut;//0x640;
	pstuFilter.cy = (int)(flt_Scale * cyIn);//cyOut;//0x4B0;
	pstuFilter.n_7C = 0;
	pstuFilter.pstu_dstImg = &stu_dstImg;
	pstuFilter.cx1 = cxOut;//0x640;
	pstuFilter.nPerLen = 0x18;//??????????
	pstuFilter.iy = 0;
	pstuFilter.Index = -1;
	pstuFilter.hFilter = GetFilterData(cxIn, cyIn, flt_Scale, 2) + 3;
	for(i = 0; i < cyIn; i++){
		StretchImage_W(&pstuFilter, pRGBAImg, NULL/*&stu_A428E0_arg_8*/);
		pRGBAImg += cxIn * 4; 
	}
	return 0;
}

BYTE* RGBA2Gray(BYTE* pRGBAImg, int cx, int cy)
{
	BYTE* pGrayBuf = (BYTE *) malloc(cx * cy);
	if (pGrayBuf == NULL) return NULL;
	BYTE r,g,b;
	int x, y, lVal, nAddr = 0;
	for (y = 0; y < cy; y++) {
		for (x = 0; x < cx; x++) {
			b = pRGBAImg[4 * (y * cx + x) + 0];
			g = pRGBAImg[4 * (y * cx + x) + 1];
			r = pRGBAImg[4 * (y * cx + x) + 2];
			lVal = min((b * 117 + g * 601 + r * 306) >> 10, 255);
			pGrayBuf[nAddr++] = (BYTE)lVal;
		}
	}

	return pGrayBuf;
}

BYTE* RGB2Gray(BYTE* pRGBImg, int cx, int cy)
{
	BYTE* pGrayBuf = (BYTE *) malloc(cx * cy);
	if (pGrayBuf == NULL) return NULL;
	BYTE r,g,b;
	int x, y, lVal, nAddr = 0;
	for (y = 0; y < cy; y++) {
		for (x = 0; x < cx; x++) {
			b = pRGBImg[3 * (y * cx + x) + 0];
			g = pRGBImg[3 * (y * cx + x) + 1];
			r = pRGBImg[3 * (y * cx + x) + 2];
			lVal = min((b * 117 + g * 601 + r * 306) >> 10, 255);
			pGrayBuf[nAddr++] = (BYTE)lVal;
		}
	}

	return pGrayBuf;
}

BYTE* ConvertImg2RGBAImg(BYTE *pRGBImg, int nImgWidth, int nImgHeight, int nBitCount)
{
	int y, x;
	BYTE* pRGBAImg = (BYTE*)malloc(nImgWidth * nImgHeight * 4);
	switch(nBitCount)
	{
	case 8:
		for (y = 0; y < nImgHeight; y++) {
			for (x = 0; x < nImgWidth; x++) {
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 0] = pRGBImg[y*nImgWidth + x];
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 1] = pRGBImg[y*nImgWidth + x];
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 2] = pRGBImg[y*nImgWidth + x];
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 3] = 0xFF;

			}
		}
		break;
	case 24:
		for (y = 0; y < nImgHeight; y++) {
			for (x = 0; x < nImgWidth; x++) {
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 0] = pRGBImg[(y*nImgWidth + x) * 3 + 0];
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 1] = pRGBImg[(y*nImgWidth + x) * 3 + 1];
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 2] = pRGBImg[(y*nImgWidth + x) * 3 + 2];
				pRGBAImg[(nImgHeight - y - 1)*nImgWidth*4 + x * 4 + 3] = 0xFF;

			}
		}
		break;
	default: 
		free(pRGBAImg);
		pRGBAImg = NULL;
		break;
	};
	return pRGBAImg;
}

static BYTE Interpolation(BYTE* Im, int cx, int cy, int x1024, int y1024)
{
	BYTE pVal;
	DWORD c = 0;
	int xq, xr, yq, yr, m, n;

	xq = x1024 >> 10;
	xr = x1024 - (xq << 10);
	yq = y1024 >> 10;
	n  = yq * cx + xq;
	yr = y1024 - (yq << 10);

	if (xq < 0 || xq >= cx)	c  = 0x10;
	if (yq < 0 || yq >= cy)	c |= 0x20;

	if (c == 0 && xr >= 0 && yr >= 0) {
		if (xq + 1 < cx) {
			if (yq + 1 < cy) {
				m = Im[n+1]*(1024-yr)*xr + Im[n+cx+1]*yr*xr + Im[n+cx]*(1024-xr)*yr + Im[n]*(1024-xr)*(1024-yr);
				m = m < 0 ? m + 1048575 : m; //1048575 = (1024*1024-1)
				pVal = m>>20; return pVal;
			}
		}
		pVal = Im[n]; return pVal;
	}
	return 0;
}

void ImageStretch(BYTE* imgIn, int cxIn, int cyIn, BYTE* imgOut, int cxOut, int cyOut)
{
	int x, y, adr, yy;
	int x_ratio = (cxIn<<10) / cxOut;
	int y_ratio = (cyIn<<10) / cyOut;

	for (y = 0; y < cyOut; y++) {
		adr= y*cxOut;
		yy = y*y_ratio;
		for (x = 0; x < cxOut; x++) {
			imgOut[adr+x] = Interpolation(imgIn, cxIn, cyIn, x*x_ratio, yy);
		}
	}
}

int DetectFaces(BYTE* pRGBImg, int nImgWidth, int nImgHeight, int nBitCount, LPNRECT *ppOutRect)
{
	int dstW, dstH;
	double fltScale = 1.0;
	BYTE* GrayImg = NULL;
	BYTE* pRGBAImg = NULL;
	BYTE* pStretchImg = NULL;
	if (STANDARD_CX < nImgWidth ||STANDARD_CX < nImgHeight){
// 		pRGBAImg = ConvertImg2RGBAImg(pRGBImg, nImgWidth, nImgHeight, nBitCount);
// 		if(pRGBAImg == NULL) return -1;
// 		if(nImgWidth > nImgHeight){
// 			fltScale = (double)STANDARD_CX / nImgWidth;
// 			dstW = STANDARD_CX;
// 			dstH = (int)(fltScale * nImgHeight);
// 		}else{
// 			fltScale = (double)STANDARD_CX / nImgHeight;
// 			dstH = STANDARD_CX;
// 			dstW = (int)(fltScale * nImgWidth);
// 		}
// 		pStretchImg = (BYTE*)malloc(dstW * dstH * 4);
// 		FitImage(pRGBAImg, nImgWidth, nImgHeight, pStretchImg, dstW, dstH, fltScale);
// 		if (pRGBAImg != NULL) free(pRGBAImg);
// 		GrayImg = RGBA2Gray(pStretchImg, dstW, dstH);
// 		if (pStretchImg != NULL) free(pStretchImg);
		BYTE *TmpImg = NULL;
		if (nBitCount == 24){
			TmpImg = RGB2Gray(pRGBImg, nImgWidth, nImgHeight);
		}else if (nBitCount == 8){
			TmpImg = (BYTE*)malloc(nImgWidth * nImgHeight);
			memcpy(TmpImg, pRGBImg, nImgWidth * nImgHeight);
		}

		if(nImgWidth > nImgHeight){
			fltScale = (double)STANDARD_CX / nImgWidth;
			dstW = STANDARD_CX;
			dstH = (int)(fltScale * nImgHeight);
		}else{
			fltScale = (double)STANDARD_CX / nImgHeight;
			dstH = STANDARD_CX;
			dstW = (int)(fltScale * nImgWidth);
		}
		GrayImg = (BYTE*)malloc(dstW * dstH * sizeof(BYTE));
		ImageStretch(TmpImg, nImgWidth, nImgHeight, GrayImg, dstW, dstH);
		free(TmpImg);
	}else{
		dstW = nImgWidth;
		dstH = nImgHeight;
		if (nBitCount == 24){
			GrayImg = RGB2Gray(pRGBImg, dstW, dstH);
		}else if (nBitCount == 8){
			GrayImg = (BYTE*)malloc(dstW * dstH);
			memcpy(GrayImg, pRGBImg, dstW*dstH);
		}
	}
	if (GrayImg == NULL) return -1;
	if ( DetectFace_100E5810((char*)GrayImg, dstW, dstH, ppOutRect, fltScale) == -1){
		if (GrayImg != NULL) free(GrayImg);
		FreeRectList(ppOutRect);
		return -1;
	}
	if (GrayImg != NULL) free(GrayImg);
	return 0;
}
int DetectFace_100E5810(char* pbyImg, int nImgWidth, int nImgHeight, LPNRECT *ppOutRect, double fltScale)
{
	ARR_DETECTED_INFO arrDetectedInfo;
	int* pSortedIndex = NULL;
	int* pCntSameClass = NULL;
	STU_DETECTOR_STATE stuSecondState;
	STU_DETECTOR_STATE stuFirstState;
	int nX1, nY1, nX2, nY2;
	int nScanRect1Right;
	int nScanRect1Bottom;
	int nScanRect2Left;
	int nScanRect2Top;
	int nScanRect2Right;
	int nScanRect2Bottom;
	int nOffset;
	int* pnIntegralImgAddr1, *pnSqrIntegralImgAddr1;
	int* pnIntegralImgAddr2, *pnSqrIntegralImgAddr2;
	bool isShrinked = true;
	LPNRECT pout_Rect, pIterator = NULL;
	LPNRECT pRect = *ppOutRect;
	int nDetectCnt = 0;
	int nFaceCnt;
	int i;

	if(nImgWidth > MAX_IMG_WIDTH || nImgHeight > MAX_IMG_HEIGHT){

		for(i = 0; i < nDetectCnt; i ++){
			LPSTU_DETECTED_INFO ptemp = arrDetectedInfo.at(i);
			delete ptemp;
		}
		arrDetectedInfo.clear();
		return -1;	
	}
	PreProc(pbyImg, nImgWidth, nImgHeight);


	while(isShrinked){//100E5AB5
		if(IsLagerThanScanRect() > 0){//100E5AC4
			nScanRect1Right = g_nShrinkedWidth - 2 - SCAN_WIDTH;
			nScanRect1Bottom = g_nShrinkedHeight - 2 - SCAN_HEIGHT;
			for(nY1 = 2; nY1 <= nScanRect1Bottom; nY1 += 5){//100E5BA6
				nScanRect2Top = nY1 - 2;
				ChannelDetector(nScanRect2Top, SCAN_HEIGHT + 4);//out:2B8, 2EC
				nOffset = (nY1 - g_nChannelCnt) * g_nIntegralImgWidth + 2;
				pnIntegralImgAddr1 = g_pnIntegralImg + nOffset;
				pnSqrIntegralImgAddr1 = g_pnSqrIntegralImg + nOffset;
				for(nX1 = 2; nX1 <= nScanRect1Right; nX1 += 5){//100E5C34
					nScanRect2Left = nX1 - 2;
					if(EvalFirst(&stuFirstState, pnIntegralImgAddr1, pnSqrIntegralImgAddr1) > 0){//100E5C58
						Choose2thStepData(&stuSecondState, &stuFirstState);

						nScanRect2Bottom = nY1 + 2;
						nScanRect2Right = nX1 + 2;
						for(nY2 = nScanRect2Top; nY2 <= nScanRect2Bottom; nY2++){//100E5CA0
							nOffset = (nY2 - g_nChannelCnt) * g_nIntegralImgWidth + nScanRect2Left;//var_B0;
							pnIntegralImgAddr2 = g_pnIntegralImg + nOffset;
							pnSqrIntegralImgAddr2 = g_pnSqrIntegralImg + nOffset;
							for(nX2 = nScanRect2Left; nX2 <= nScanRect2Right; nX2++){//100E5D10
								EvalSecond(&stuSecondState, pnIntegralImgAddr2, pnSqrIntegralImgAddr2, nX2, nY2, &arrDetectedInfo);

								pnIntegralImgAddr2 ++;//var_9C += t_eax;
								pnSqrIntegralImgAddr2 ++;//var_98 += t_eax;
							}
						}//100E5D65
					}//100E5D69
					pnIntegralImgAddr1 += 5;
					pnSqrIntegralImgAddr1 += 5;
				}
			}
		}//100E5ECD
		isShrinked = CreateWorkImage();
	}//100E5F35

	nDetectCnt = arrDetectedInfo.size();
	if(nDetectCnt <= 0)
		return -1;

	nFaceCnt = ClassifyDetectedRects(arrDetectedInfo, 0.3f, SCAN_WIDTH, SCAN_HEIGHT, &pSortedIndex, &pCntSameClass);

	nOffset = 0;
	for(i = 0; i < nFaceCnt; i ++){
		if(pCntSameClass[i] >= MAX_FACE_PASS_CNT){
			pout_Rect = (LPNRECT)calloc(1, sizeof(NRECT));
			WeightedAverage(pout_Rect, arrDetectedInfo, pSortedIndex, nOffset, pCntSameClass[i]);
			pout_Rect->l = (int)(max(0, pout_Rect->l) / fltScale);
			pout_Rect->t = (int)(max(0, pout_Rect->t) / fltScale);
			pout_Rect->r = (int)(min(nImgWidth - 1, pout_Rect->r) / fltScale);
			pout_Rect->b = (int)(min(nImgHeight -1, pout_Rect->b) / fltScale);
			pout_Rect->pNext = NULL;
			if(pIterator == NULL){
				*ppOutRect = pout_Rect;
				pIterator = pout_Rect;
			}else{
				pIterator->pNext = pout_Rect;
				pIterator = pout_Rect;
			}
		}
		nOffset += pCntSameClass[i];
	}
	if(pSortedIndex)
		delete pSortedIndex;
	if(pCntSameClass)
		delete pCntSameClass;
	nDetectCnt = arrDetectedInfo.size();

	for(i = 0; i < nDetectCnt; i ++){
		LPSTU_DETECTED_INFO ptemp = arrDetectedInfo.at(i);
		delete ptemp;
	}
	arrDetectedInfo.clear();

	return 0;
}

void FreeRectList(LPNRECT* pRectRoot){
	LPNRECT pRect, pTmpRect;
	pRect = *pRectRoot;
	while(pRect){
		pTmpRect = pRect->pNext;
		free(pRect);
		pRect = pTmpRect;
	}
	*pRectRoot = NULL;
}
