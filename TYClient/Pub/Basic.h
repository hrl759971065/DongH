//---------------------------------------------------------------------------

#ifndef BasicH
#define BasicH

#include <Classes.hpp>
#include <mmsystem.h>
//#include "Globals.h"
#include "Ping.h"
#include "AES.h"
#include "Base64.h"
#include "voBaseInfo.h"
#include <string>
//---------------------------------------------------------------------------
using namespace std;

typedef char * (__stdcall *fp_GetCPUInfomation)(int);
typedef char * (__stdcall *fp_GetBiosSN)();

class TDataSwitch{

public:
	AnsiString 			HexToASCStr(char * P_BCD, int length);
	void 				HexToASC_My(char * inHex, int length,char * outBuf);
	void 				strHexToBuff(AnsiString cmd,unsigned char *out,int &Len);	//ʮ�������ַ���ת����,�ַ�����:01030001DB
	void 				strHexToBuff(AnsiString cmd,unsigned char *out);
	int 				strToHex(unsigned char *ch, unsigned char *hex);
	int 				HexToInt(AnsiString Hex);
	bool 				StrHexCmp(AnsiString str1,AnsiString str2,bool bFull);
	int 				CharPos(const char *ch,char c,int nMinPos,int nLen);
	int 				IsDigit(char *s,int Len);
	bool 				IsValid_HexStr(AnsiString strHex);
	int 				myPos(String strtmp,int a,char b);
	AnsiString 			CharToBinStr(const char ch);
	WORD 				CheckSum(WORD *buffer,int nSize);
	unsigned char  		CheckSum(unsigned char *uBuff, unsigned char uBuffLen);
	UCHAR 				LRC_Chk(UCHAR *data,UCHAR length);
	unsigned char 		CheckSum_BCC(unsigned char *uBuff, unsigned char uBuffLen);
	void 				Makecode(char *in,char *out,int inLen);
	void 				Makecode(char *inout);
	void 				Cutecode(char *in,char *out,int inLen);
	void 				Cutecode(char *inout);
	BYTE* 				AESEncode(BYTE *input, int inLen);
	BYTE*				AESDecode(BYTE *input, int inLen);
	AnsiString 			StrEncode(AnsiString Code);
	AnsiString 			StrDecode(AnsiString Code);
	string 				DesEncode(string strPlain,string key);
	string 				DesDecode(string strCipher,string key);
private:
	char MakecodeChar(char c,int key);
	char CutcodeChar(char c,int key);

	CBase64 _base64;
	//AES _AES;
};

class TFile{

public:
	void 		ReadFileVersion(AnsiString InFileName,TVersionInfo *VerInfo);
	void 		PlayWav(String wavName,DWORD dwSound=SND_FILENAME|SND_ASYNC);	//---Ĭ�ϲ��ŷ�ʽ �ļ���|�첽
	int 		CreateFile(String fileName);    //�����ļ�:����Ϊ�ļ�ȫ��
	int 		WriteFiles(String fileName,  //�ļ�ȫ��
						   const void *Buffer,   //��д������緳�
						   ULONG clength,    //Buffer�ֽ���
						   int Origin,   //λ��: 0 ��ͷ,1��ǰλ�� 2ĩβ
						   int Offset    //ƫ����:��λ�ֽ�
						 ); //д���ļ�
	void 		WriteLog(AnsiString strLog);
	void  		WriteLog_ErrS(AnsiString strLog);
};

class TCommand{

public:

	virtual AnsiString	GetLocalIP();
	virtual bool		GetDevSN(THostInfo *HostInfo);
	bool 				GetWmiInfo(WideString wsClass,WideString wsSub,int iEnumIdx,char *wsVal);
	String 				GetSysVolumeID();
	int					Ping(char *IP);

	String				_LastErrS;

private:
	CPing 				_Ping;
};

#endif