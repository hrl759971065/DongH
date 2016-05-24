//---------------------------------------------------------------------------

#ifndef JsonReqH
#define JsonReqH

#include "json.h"
//---------------------------------------------------------------------------

/**************************************************************************

Copyright:软件实施部

Author: 武文斌

Description:用于处理INmlxDeviceControl中DataRequst方法的输入参数JsonReq
			JsonReq为Json格式的字符串

Date:2016-03-22
**************************************************************************/

class TJsonReq{
public:
	UTF8String GetLastErrS() const;
	bool GetUserPass(const UTF8String JsonReq,UTF8String &User,UTF8String &Pass);
	bool IsAllowLogin(const UTF8String JsonReq);
	bool GetOperID(const UTF8String JsonReq,int &OperID);
	bool GetJsonData(const UTF8String JsonReq,Json::Value &jsonDataObj);
	bool GetLocation(const UTF8String JsonReq,UTF8String &Location);
	bool GetCZYLocation(const UTF8String JsonReq,UTF8String &CYLocation,UTF8String &ZYLocation);
	bool GetActionID(const UTF8String JsonReq,int &ActionID);
	bool GetCZYBM(const UTF8String JsonReq,UTF8String &strBM,int nType);

	//---生成 调用平台接口的Json
	UTF8String CreateJsonReq_pt(UTF8String User,UTF8String Pass,UTF8String OperType,Json::Value JsonData);
private:
	UTF8String _LastErrS;
};

#endif
