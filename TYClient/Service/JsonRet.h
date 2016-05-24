//---------------------------------------------------------------------------

#ifndef JsonRetH
#define JsonRetH
//---------------------------------------------------------------------------

#include "json.h"

/**************************************************************************

Copyright:软件实施部

Author: 武文斌

Description:用于处理INmlxDeviceControl中DataRequst方法的输入参数JsonReq
			JsonReq为Json格式的字符串

Date:2016-03-22
**************************************************************************/

class TJsonRet{
public:
	UTF8String GetLastErrS() const;
	UTF8String CreatJsonRet(int nRet,UTF8String errMsg,int RowCnt,Json::Value JsonData);

	//---将平台的Json串转为Json对象
	bool JsonStrToJson_pt(UTF8String jsonStr,Json::Value &jv );
	bool RsultAnalysis(UTF8String jsonStr);
private:
	UTF8String _LastErrS;
};

#endif
