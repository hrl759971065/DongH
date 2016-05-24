// ---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "Basic.h"
// #include "Globals.h"

#include <comdef.h>
#include <wbemidl.h>
#include "DES.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)

/***************************************************************************
 Function:	HexToASCStr 16进制数组转字符串
 Param：     P_BCD 输入
 length 输入字节长
 Returns:	输出字符串
 ***************************************************************************/
AnsiString TDataSwitch::HexToASCStr(char * P_BCD, int length) {
	AnsiString returnstring = "";
	char high_char, low_char, temp;
	for (int i = 0; i < length; i++) {
		temp = *P_BCD++;
		high_char = (temp & 0xf0) >> 4;
		low_char = temp & 0x0f;
		if ((high_char >= 0x00) && (high_char <= 0x09))
			high_char += 0x30;
		if ((high_char >= 0x0A) && (high_char <= 0x0F))
			high_char += 0x37;

		if ((low_char >= 0x00) && (low_char <= 0x09))
			low_char += 0x30;
		if ((low_char >= 0x0A) && (low_char <= 0x0F))
			low_char += 0x37;

		returnstring += AnsiString(high_char);
		returnstring += AnsiString(low_char);
		// if(i!=(length-1))
		// returnstring += " ";    //00 23 12 这样的格式
	}
	return returnstring;
}

/***************************************************************************
 函数：HexToASC_My
 功能：Hex转ASCII数组
 作者：武文斌通过网络改造
 时间: 2014.3.14
 ***************************************************************************/
void TDataSwitch::HexToASC_My(char * inHex, int length, char * outBuf) {
	// TODO: Add your source code here
	AnsiString returnstring = "";
	char high_char, low_char, temp;
	for (int i = 0; i < length; i++) {
		temp = *inHex++;
		high_char = (temp & 0xf0) >> 4;
		low_char = temp & 0x0f;
		if ((high_char >= 0x00) && (high_char <= 0x09))
			high_char += 0x30;
		if ((high_char >= 0x0A) && (high_char <= 0x0F))
			high_char += 0x37;

		if ((low_char >= 0x00) && (low_char <= 0x09))
			low_char += 0x30;
		if ((low_char >= 0x0A) && (low_char <= 0x0F))
			low_char += 0x37;

		outBuf[i * 2] = high_char;
		outBuf[i * 2 + 1] = low_char;
	}

}

/***************************************************************************
 Function:	strHexToBuff
 Param：     cmd 16进制命令字符串
 out 转换后的命令输出
 Len 输出字节长
 Returns:	无
 ***************************************************************************/
void TDataSwitch::strHexToBuff(AnsiString cmd, unsigned char *out, int &Len) {
	Len = 0;
	for (int i = 0; (i < MaxCmdSize) && (i < cmd.Length() / 2); i++) {

		AnsiString str = "0x";
		str += cmd.SubString(i * 2 + 1, 2);
		out[i] = StrToInt(str);
		Len++;
	}
}

void TDataSwitch::strHexToBuff(AnsiString cmd, unsigned char *out) {
	for (int i = 0; (i < MaxCmdSize) && (i < cmd.Length() / 2); i++) {

		AnsiString str = "0x";
		str += cmd.SubString(i * 2 + 1, 2);
		out[i] = StrToInt(str);
	}
}

char valueToHexCh(const int value) {
	char result = '\0';
	if (value >= 0 && value <= 9) {
		result = (char)(value + 48); // 48为ascii编码的‘0’字符编码值
	}
	else if (value >= 10 && value <= 15) {
		result = (char)(value - 10 + 65);
		// 减去10则找出其在16进制的偏移量，65为ascii的'A'的字符编码值
	}
	else {;
	}

	return result;
}

int TDataSwitch::strToHex(unsigned char *ch, unsigned char *hex) {
	int high, low;
	int tmp = 0;
	if (ch == NULL || hex == NULL) {
		return -1;
	}

	if (strlen(ch) == 0) {
		return -2;
	}

	while (*ch) {
		tmp = (int) * ch;
		high = tmp >> 4;
		low = tmp & 15;
		*hex++ = valueToHexCh(high); // 先写高字节
		*hex++ = valueToHexCh(low); // 其次写低字节
		ch++;
	}
	*hex = '\0';
	return 0;
}

int TDataSwitch::HexToInt(AnsiString Hex) {
	return StrToInt("$" + Hex);
}

/***************************************************************************
 Function:	StrHexCmp 16进制字符串比较
 Param：     str1 str2 要比对的字符串 ，格式：01030001 16进制字符串
 bFull true 严格一致
 false 遇到** 二个星 认为与任何字符都相同
 Returns:	true 一致 false不一致
 ***************************************************************************/
bool TDataSwitch::StrHexCmp(AnsiString str1, AnsiString str2, bool bFull) {
	if (str1.Length() != str2.Length())
		return false;

	for (int i = 0; i < str1.Length() / 2; i++) {

		if (bFull) {
			if (str1.SubString(i * 2 + 1, 2) != str2.SubString(i * 2 + 1, 2))
				return false;

		}
		else if (str1.SubString(i * 2 + 1, 2) != "**" && str2.SubString
			(i * 2 + 1, 2) != "**") {

			AnsiString tmp1 = str1.SubString(i * 2 + 1, 2);
			AnsiString tmp2 = str2.SubString(i * 2 + 1, 2);
			if (str1.SubString(i * 2 + 1, 2) != str2.SubString(i * 2 + 1, 2))
				return false;
		}
	}

	return true;
}

// 字符在数组中的位置,如果无,返回-1,有则反回下标位置
// nMinPos 位置不小于nMinPos
int TDataSwitch::CharPos(const char *ch, char c, int nMinPos, int nLen) {
	int nPos = -1;
	if (nLen > 0) {

		for (int i = 0; i < nLen; i++) {

			nPos++;
			if (*ch == c && nPos >= nMinPos) {
				return nPos;
			}
			ch++;
		}
	}
	else {

		while (*ch != '\0') {
			nPos++;
			if (*ch == c && nPos >= nMinPos) {
				return nPos;
			}
			ch++;
		}
	}

	return -1;
}

/***************************************************************************
 Function:	IsDigit 是否为数字
 Param：     s 输入字符串
 Len 长度
 Returns:	非 0-9和点 的个数
 ***************************************************************************/

int TDataSwitch::IsDigit(char *s, int Len) {
	int nValid = 0;
	for (int i = 0; i < Len; i++) {

		if ((s[i] < 0x30 || s[i] > 0x39) &&
			(s[i] != 0x2E && s[i] != 0x2B && s[i] != 0x2D && s[i] != 0x20))
			nValid++;
	}
	return nValid;
}

bool TDataSwitch::IsValid_HexStr(AnsiString strHex) {
	if (strHex.Length() != 12)
		return false;

	AnsiString strValid = "0123456789ABCDEF"; // 有效指令字符集

	for (int i = 1; i <= strHex.Length();
	i++) // index of AnsiString starts from 1
	{
		if (strValid.Pos(strHex[i]) == 0) // input data verification
		{
			return false;
		}
	}
	return true;
}

// ====自定义取位置函数        20120626 in pingliang  wuwenbin
int TDataSwitch::myPos(String strtmp, int a, char b) // 字符串中同一字符第N次的位置
{
	int i = 0, j = 0;
	wchar_t *c = strtmp.c_str();
	while (*c != '\0') {
		j++;
		if (*c == b) {
			i++;
			if (i == a) {
				// delete c;
				return j;
			}
		}
		c++;
	}
	// delete c;
	return 0;
}

// ---字符转二进制字符串
AnsiString TDataSwitch::CharToBinStr(const char ch) {
	AnsiString strRet = "";

	for (int i = 7; i >= 0; i--) {

		if (7 == i)
			strRet = IntToStr((ch & (int)pow(2, i)) >> i);
		else
			strRet += IntToStr((ch & (int)pow(2, i)) >> i);
	}
	return strRet;
}

WORD TDataSwitch::CheckSum(WORD *buffer, int nSize) {
	DWORD dwChkSum = 0;
	while (nSize > 1) {
		dwChkSum += *buffer++;
		nSize -= sizeof(WORD);
	}

	if (nSize) {
		dwChkSum += *(UCHAR *)buffer;
	}
	// ---32位转16位
	while (dwChkSum >> 16) {

		dwChkSum = (dwChkSum >> 16) + (dwChkSum & 0xFFFF);
	}
	return (WORD)(~dwChkSum);
}

unsigned char TDataSwitch::CheckSum(unsigned char *uBuff,
	unsigned char uBuffLen) {
	unsigned char i, uSum = 0;
	for (i = 0; i < uBuffLen; i++) {
		uSum = uSum + uBuff[i];
	}
	uSum = (~uSum) + 1;
	return uSum;
}

UCHAR TDataSwitch::LRC_Chk(UCHAR *data, UCHAR length) {
	UCHAR i;
	UINT k;
	UCHAR result;

	UCHAR *lrcdata = new UCHAR[length];
	for (i = 1; i < length + 1; i++) {
		if (data[i] > 0x40)
			lrcdata[i - 1] = data[i] - 0x41 + 10;
		else
			lrcdata[i - 1] = data[i] - 0x30;
	}

	k = 0;
	for (i = 0; i < length / 2; i++) {
		k += (lrcdata[2*i]*16 + lrcdata[2*i + 1]);
	}

	k = k % 256;
	k = 256 - k;
	result = k % 256;

	delete[]lrcdata;
	return result;
}

unsigned char TDataSwitch::CheckSum_BCC(unsigned char *uBuff,
	unsigned char uBuffLen) {
	unsigned char xor = 0;
	for (unsigned char i = 0; i < uBuffLen; i++) {
		xor ^= *uBuff++;
	}
	return xor;
}

// 单个字符异或运算
char TDataSwitch::MakecodeChar(char c, int key) {
	return c = c ^ key;
}

// 单个字符解密
char TDataSwitch::CutcodeChar(char c, int key) {
	return c ^ key;
}

// 加密
void TDataSwitch::Makecode(char *in, char *out, int inLen) {
	int key[] = {6, 2, 2, 4, 0, 5, 1}; // 加密密码
	// int len=strlen(in);//获取长度
	for (int i = 0; i < inLen; i++)
		* (out + i) = MakecodeChar(*(in + i), key[i % 6]);

	char bs64[1024] = {0};
	memcpy(bs64, out, strlen(out)); // ---将加密数据放到bs64中
	// pstr="";
	_base64.EncodeBuffer(bs64, strlen(bs64), out); // ---进行base64加密
}

void TDataSwitch::Makecode(char *inout) {
	int key[] = {1, 2, 3, 4, 5, 6}; // 加密密码
	int len = strlen(inout); // 获取长度
	for (int i = 0; i < len; i++)
		* (inout + i) = MakecodeChar(*(inout + i), key[i % 6]);
}

// 解密
void TDataSwitch::Cutecode(char *in, char *out, int inLen) {
	char bs64_c[1024] = {0};
	char bs64_t[1024] = {0};
	memcpy(bs64_c, in, strlen(in)); // ---将加密数据放到bs64中
	_base64.DecodeBuffer(bs64_c, bs64_t); // ---进行base64解密
	in = bs64_t;

	int key[] = {6, 2, 2, 4, 0, 5, 1}; // 加密密码
	int Len = strlen(in);
	for (int i = 0; i < Len; i++)
		* (out + i) = CutcodeChar(*(in + i), key[i % 6]);
}

void TDataSwitch::Cutecode(char *inout) {
	int key[] = {1, 2, 3, 4, 5, 6}; // 加密密码
	int len = strlen(inout);
	for (int i = 0; i < len; i++)
		* (inout + i) = CutcodeChar(*(inout + i), key[i % 6]);
}

BYTE* TDataSwitch::AESEncode(BYTE *input, int inLen) {
	// _AES.Cipher(input,inLen);
	return NULL;
}

BYTE* TDataSwitch::AESDecode(BYTE *input, int inLen) {
	// _AES.InvCipher(input,inLen);
	return NULL;
}

AnsiString TDataSwitch::StrEncode(AnsiString Code) {
	/* HINSTANCE Hinst=LoadDll( (ExtractFilePath(Application->ExeName)+"Plugin\\Terutnsc.dll").c_str() );
	 TeEncodeMth2 api_TeEncodeMth2=(TeEncodeMth2)GetProcAddress(Hinst,"TeEncodeMth2");
	 int OutBytes;
	 char Buf[1024]={0};
	 strcpy(Buf,Code.c_str());
	 api_TeEncodeMth2(Buf,Code.Trim().Length(),&OutBytes);
	 FreeDll(Hinst);
	 return AnsiString(Buf); */

	char Buf[1024] = {0};
	char outBuf[1024] = {0};
	strcpy(Buf, Code.c_str());
	// _DS.Makecode(Buf);
	// return AnsiString(Buf);
	Makecode(Buf, outBuf, strlen(Buf));
	return AnsiString(outBuf);
}

AnsiString TDataSwitch::StrDecode(AnsiString Code) {
	/* HINSTANCE Hinst=LoadDll( (ExtractFilePath(Application->ExeName)+"Plugin\\Terutnsc.dll").c_str() );
	 TeDecodeMth2 api_TeDecodeMth2=(TeDecodeMth2)GetProcAddress(Hinst,"TeDecodeMth2");
	 int OutBytes;
	 char Buf[1024]={0};
	 strcpy(Buf,Code.c_str());
	 api_TeDecodeMth2(Buf,&OutBytes);
	 FreeDll(Hinst);
	 return AnsiString(Buf); */

	char Buf[1024] = {0};
	char outBuf[1024] = {0};
	strcpy(Buf, Code.c_str());
	// _DS.Cutecode(Buf);
	// return AnsiString(Buf);
	Cutecode(Buf, outBuf, strlen(Buf));
	return AnsiString(outBuf);
}

string TDataSwitch::DesEncode(string strPlain, string key) {
	CDES Des;
	char szPlaintextData[81920] = {0};
	strncpy(szPlaintextData, strPlain.c_str(), 81920);
	unsigned char szKey[8] = {0};
	strncpy(szKey, key.c_str(), 8);
	Des.InitializeKey(szKey, 0);
	Des.EncryptAnyLength(szPlaintextData, strlen(szPlaintextData), 0);

	int nBitsLen = strlen(szPlaintextData) % 8 == 0 ?
		strlen(szPlaintextData) << 3 :
		((strlen(szPlaintextData) >> 3) + 1) << 6;
	char bitsCiphertextAnyLength[327680] = {0};
	char hexCiphertextAnyLength[163840] = {0};
	Des.Bytes2Bits(Des.GetCiphertextAnyLength(), bitsCiphertextAnyLength,
		nBitsLen);
	Des.Bits2Hex(hexCiphertextAnyLength, bitsCiphertextAnyLength, nBitsLen);
	return hexCiphertextAnyLength;
}

string TDataSwitch::DesDecode(string strCipher, string key) {
	CDES Des;

	char szCiphertextData[81920] = {0};
	char hexCiphertextAnyLength[163840] = {0};
	char bitsCiphertextAnyLength[327680] = {0};
	unsigned char szKey[8] = {0};
	strncpy(szKey, key.c_str(), 8);

	strncpy(szCiphertextData, strCipher.c_str(), 81920);

	Des.InitializeKey(szKey, 0);

	int nLen = ((strlen(szCiphertextData) >> 2) +
		(strlen(szCiphertextData) % 4 == 0 ? 0 : 1)) << 4;
	memcpy(hexCiphertextAnyLength, szCiphertextData, strlen(szCiphertextData));
	Des.Hex2Bits(hexCiphertextAnyLength, bitsCiphertextAnyLength, nLen);
	Des.Bits2Bytes(szCiphertextData, bitsCiphertextAnyLength, nLen);

	nLen >> 3;

	Des.DecryptAnyLength(szCiphertextData, nLen, 0);
	return Des.GetPlaintextAnyLength();

}

// ---文件类
void TFile::ReadFileVersion(AnsiString InFileName, TVersionInfo *VerInfo) {
	VerInfo->FilePathName = String(InFileName);
	String Version = NULL;
	// 首先获得版本信息资源的长度
	DWORD dwHandle, InfoSize;
	InfoSize = GetFileVersionInfoSizeA(InFileName.c_str(), &dwHandle);
	// 将版本信息资源读入缓冲区
	char *InfoBuf = new char[InfoSize];
	GetFileVersionInfoA(InFileName.c_str(), 0, InfoSize, InfoBuf);
	// 获得生成文件使用的代码页及字符集信息
	char *pInfoVal;
	unsigned int dwInfoValSize;
	VerQueryValueA(InfoBuf, "\\VarFileInfo\\Translation", &((void *)pInfoVal),
		&dwInfoValSize);

	AnsiString V = "\\StringFileInfo\\" +
		IntToHex(*((unsigned short int *) pInfoVal), 4) +
		IntToHex(*((unsigned short int *)&pInfoVal[2]), 4) + "\\FileVersion";
	// 获得具体的版本号
	VerQueryValueA(InfoBuf, V.c_str(), &((void *)pInfoVal), &dwInfoValSize);
	Version = AnsiString(pInfoVal).SetLength(dwInfoValSize - 1);
	VerInfo->FileVersion = Version;
	// 获得文件的说明 FileDescription
	VerQueryValueA(InfoBuf, "\\VarFileInfo\\Translation", &((void *)pInfoVal),
		&dwInfoValSize);
	AnsiString N = "\\StringFileInfo\\" +
		IntToHex(*((unsigned short int *) pInfoVal), 4) +
		IntToHex(*((unsigned short int *)&pInfoVal[2]), 4) +
		"\\FileDescription";
	VerQueryValueA(InfoBuf, N.c_str(), &((void *)pInfoVal), &dwInfoValSize);
	// wcscpy(VerInfo->FileDescription,String(pInfoVal).SetLength(dwInfoValSize-1).c_str());
	VerInfo->FileDescription = String(pInfoVal);

	delete[]InfoBuf;
}

void TFile::PlayWav(String wavName, DWORD dwSound) {
	String WavP;
	WavP = ExtractFilePath(Application->ExeName) + "wav\\" + wavName;
	PlaySound(WavP.c_str(), NULL, dwSound);
}

int TFile::CreateFile(String fileName) {
	int File;
	String fPath = ExtractFilePath(fileName); // 文件夹
	if (!DirectoryExists(fPath)) // 如果文件夹不存在,创建
	{
		if (!ForceDirectories(fPath)) {
			return 3; // 文件夹创建失败
		}
	}
	if (!FileExists(fileName)) // 判断该文件是否存在，如果不存在，创建
	{
		File = FileCreate(fileName);
		if (File != -1) {
			FileClose(File);
			return 0; // 成功
		}
		else {
			return 1; // 失败
		}
	}
	else {
		return 2; // 已存在
	}
}

int TFile::WriteFiles(String fileName, // 文件全名
	const void *Buffer, // 待写入数据绶冲
	ULONG clength, // Buffer字节数
	int Origin, // 位置: 0 开头,1当前位置 2末尾
	int Offset // 偏移量:单位字节
	) // 写入文本
{
	CreateFile(fileName); // 先创建文件

	int File = FileOpen(fileName, fmOpenWrite); // 以写的方式打开文件
	if (-1 == File) {
		return 1; // 打开失败
	}
	else {
		FileSeek(File, Offset, Origin); // 参数：文件句柄，相对参数3的偏移量
		// 2表示文件尾部
		FileWrite(File, Buffer, clength);
		FileClose(File);
		if (-1 == File) {
			return 2; // 写入失败
		}
		else {
			return 0;
		}
	}
}

void TFile::WriteLog(AnsiString strLog) {
	// 写日志
	String logName = ExtractFilePath(Application->ExeName) + "Log\\" +
		Now().FormatString("yyyymmdd") + "系统日志.log";
	AnsiString logStr = "时间:" + Now().DateTimeString() + ",事件:" +
		strLog + "\r\n";
	WriteFiles(logName, logStr.c_str(), logStr.Length(), 2, 0);
}

void TFile::WriteLog_ErrS(AnsiString strLog) {
	// 写错误日志
	String logName = ExtractFilePath(Application->ExeName) + "Log\\" +
		Now().FormatString("yyyymmdd") + "错误日志.log";
	AnsiString logStr = "时间:" + Now().DateTimeString() + ",事件:" +
		strLog + "\r\n";
	WriteFiles(logName, logStr.c_str(), logStr.Length(), 2, 0);
}

// ---常用命令类

AnsiString TCommand::GetLocalIP() {
	AnsiString IP = "";
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData); // 初始化WINSOCK调用
	char HostName[80]; // 存放本主机名
	LPHOSTENT lpHostEnt;
	gethostname(HostName, sizeof(HostName)); // 利用得到的主机名去获得主机结构

	lpHostEnt = gethostbyname(HostName); // 利用主机名去取主机结构

	for (int i = 0; lpHostEnt->h_addr_list[i] != 0; i++) {
		struct in_addr *p = (struct in_addr*)(lpHostEnt->h_addr_list[i]);
		IP = inet_ntoa(*p);
		// Memo1->Lines->Add(IP);
	}
	WSACleanup(); // 释放WINSOCK调用
	return IP;
}

bool TCommand::GetWmiInfo(WideString wsClass, WideString wsSub, int iEnumIdx,
	char *wsVal) {
	bool bRet = false;
	static bool bFirst = true;
	CoInitialize(NULL);
	if (bFirst) {
		if (CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0) != S_OK) {
			_LastErrS = "无法获取权限!";
		}
		else
			bFirst = false;
	}

	IWbemLocator *pWbemLocator = NULL;
	if (CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL,
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown,
		(void**)&pWbemLocator) == S_OK) {

		IWbemServices *pWbemServices = NULL;
		WideString wsNamespace = (L"root\\cimv2");
		if (pWbemLocator->ConnectServer(wsNamespace.c_bstr(), NULL, NULL, NULL,
			0, NULL, NULL, &pWbemServices) == S_OK) {

			IEnumWbemClassObject *pEnumClassObject = NULL;
			WideString wsWQL = L"WQL", wsQuery = WideString(L"Select * from ") +
				wsClass;
			if (pWbemServices->ExecQuery(wsWQL.c_bstr(), wsQuery.c_bstr(),
				WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumClassObject) == S_OK)
			{

				IWbemClassObject *pClassObject = NULL;
				ULONG uCount = 1, uReturned;
				if (pEnumClassObject->Reset() == S_OK) {

					bool bOK = true;
					for (int i = 0; i < (iEnumIdx + 1); i++) {

						if (pEnumClassObject->Next(WBEM_INFINITE, uCount,
							&pClassObject, &uReturned) != S_OK) {

							bOK = false;
							break;
						}
					}

					if (bOK) {
						SAFEARRAY *pvNames = NULL;
						if (pClassObject->GetNames(NULL,
							WBEM_FLAG_ALWAYS | WBEM_MASK_CONDITION_ORIGIN, NULL,
							&pvNames) == S_OK) {
							long vbl, vbu;
							SafeArrayGetLBound(pvNames, 1, &vbl);
							SafeArrayGetUBound(pvNames, 1, &vbu);

							for (long idx = vbl; idx <= vbu; idx++) {
								long aidx = idx;
								wchar_t *wsName = 0;
								VARIANT vValue;
								VariantInit(&vValue);
								SafeArrayGetElement(pvNames, &aidx, &wsName);

								BSTR bs = SysAllocString(wsName);
								HRESULT hRes =
									pClassObject->Get(bs, 0, &vValue, NULL, 0);
								SysFreeString(bs);

								if (hRes == S_OK) {
									AnsiString s;
									Variant v = *(Variant*)&vValue;
									if (v.IsArray()) {

										for (int i = v.ArrayLowBound();
										i <= v.ArrayHighBound(); i++) {
										Variant a = v.GetElement(i);
										if (!s.IsEmpty())
										s += ", ";
										s += VarToStr(a);
										}
									}
									else {
										s = VarToStr(v);
									}
									if (WideString(wsName) == wsSub) {

										strcpy(wsVal, s.Trim().c_str());
										bRet = true;
									}
								}

								VariantClear(&vValue);
								SysFreeString(wsName);
							}
						}
						if (pvNames)
							SafeArrayDestroy(pvNames);

					}
					else {
						wsVal[0] = '\0';
						_LastErrS = "空值";
					}
				}
				if (pClassObject)
					pClassObject->Release();
			}
			if (pEnumClassObject)
				pEnumClassObject->Release();
		}
		if (pWbemServices)
			pWbemServices->Release();
	}
	if (pWbemLocator)
		pWbemLocator->Release();
	CoUninitialize();
	return bRet;
}

bool TCommand::GetDevSN(THostInfo *HostInfo) {
	char BaseBoardID[128] = {0};
	char ProcessorID[128] = {0};
	char HardDiskID[128] = {0};
	if (!GetWmiInfo("Win32_BaseBoard", "SerialNumber", 0, BaseBoardID))
		return false;

	GetWmiInfo("Win32_Processor", "ProcessorId", 0, ProcessorID);
	GetWmiInfo("Win32_PhysicalMedia", "SerialNumber", 0, HardDiskID);

	HostInfo->BaseBoardID = String(BaseBoardID);
	HostInfo->ProcessorID = String(ProcessorID);
	HostInfo->HardDiskID = String(HardDiskID);
	return true;
}

/***************************************************************************
 Function:	GetSysVolumeID 取磁盘系统卷标ID
 Param：     s 输入字符串
 Len 长度
 Returns:	非 0-9和点 的个数
 ***************************************************************************/
String TCommand::GetSysVolumeID() {
	String Serial = "";
	wchar_t str[MAX_PATH] = {0};
	GetSystemDirectory(str, MAX_PATH); // 读取操作系统目录
	String SysDir = String(str);
	wchar_t Volume[256] = {0}; // 卷标名
	wchar_t FileSysName[256] = {0};
	DWORD SerialNum; // 序列号
	DWORD FileNameLength;
	DWORD FileSysFlag;

	::GetVolumeInformation(L"D:\\", Volume, 256, &SerialNum, &FileNameLength,
		&FileSysFlag, FileSysName, 256);

	Serial = Serial.Format("1%04x", ARRAYOFCONST((SerialNum ^ 0x34795814)));
	return Serial;
}

// ---反回ping值,-1表示不通,大于2000ms
int TCommand::Ping(char *IP) {
	int nReply = -1;
	PingReply pingRep = {0};
	_Ping;
	if (_Ping.Ping(IP, &pingRep)) {
		nReply = pingRep.m_dwRoundTripTime;
	}
	return nReply;
}
