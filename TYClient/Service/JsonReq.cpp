// ---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "JsonReq.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)

/*************************************************
 Function:		GetLastErrS
 Description:	读取最近一次的错误描述
 Input:          无
 Output:         无
 Return:         返回错误描述
 Calls:       	无
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
UTF8String TJsonReq::GetLastErrS() const {
	return _LastErrS;
}

/*************************************************
 Function:		GetUserPass
 Description:	从Json请求串中读取用户名和密码
 Input:          JsonReq:Json格式请求数字
 Output:         User 用户名,Pass 密码
 Return:         成功true,失败false
 Calls:       	无
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetUserPass(const UTF8String JsonReq, UTF8String &User,
	UTF8String &Pass) {
	try {
		Json::Reader reader;
		Json::Value valObj;
		if (reader.parse(JsonReq.c_str(), valObj)) {

			if (valObj.isMember("LoginName")) {

				if (valObj["LoginName"].isString()) {

					User = valObj["LoginName"].asCString();

				}
				else {
					_LastErrS = L"Json参数错误:LoginName应为字符串";
					return false;
				}
			}
			else {
				_LastErrS = L"Json参数错误:不存在LoginName字段";
				return false;
			}

			if (valObj.isMember("PassWord")) {

				if (valObj["PassWord"].isString()) {

					Pass = valObj["PassWord"].asCString();

				}
				else {
					_LastErrS = L"Json参数错误:PassWord应为字符串";
					return false;
				}
			}
			else {
				_LastErrS = L"Json参数错误:不存在PassWord字段";
				return false;
			}
			return true;
		}
		else {
			_LastErrS = L"Json结构错误";
			return false;
		}
	}
	catch (Exception &e) {
		_LastErrS = L"Json异常:" + e.Message;
		return false;
	}
	catch (...) {
		_LastErrS = L"Json异常:未知错误";
		return false;
	}
}

/*************************************************
 Function:		IsAllowLogin
 Description:	从Json请求串中读取用户名和密码并验证用户名和密码
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         允许登录true,反之false
 Calls:       	GetUserPass
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::IsAllowLogin(const UTF8String JsonReq) {
	UTF8String User, Pass;
	if (GetUserPass(JsonReq, User, Pass)) {

		if (User != L"Hello") {
			_LastErrS = L"用户名错误";
			return false;
		}

		if (Pass != L"World") {
			_LastErrS = L"用户名错误";
			return false;
		}
		return true;
	}
	else
		return false;
}

/*************************************************
 Function:		GetOperID
 Description:	从Json请求串中读取业务ID号
 Input:          JsonReq:Json格式请求数字
 Output:         OperID 业务ID号
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetOperID(const UTF8String JsonReq, int &OperID) {
	try {
		Json::Reader reader;
		Json::Value valObj;
		if (reader.parse(JsonReq.c_str(), valObj)) {

			if (!valObj.isMember("OperID")) {
				_LastErrS = L"Json参数错误:OperID不存在";
				return false;
			}
			if (!valObj["OperID"].isInt()) {
				_LastErrS = L"Json参数错误:OperID应为整形";
				return false;
			}
			OperID = valObj["OperID"].asInt();
			return true;
		}
		else {
			_LastErrS = L"Json结构错误";
			return false;
		}
	}
	catch (Exception &e) {
		_LastErrS = L"Json异常:" + e.Message;
		return false;
	}
	catch (...) {
		_LastErrS = L"Json异常:未知错误";
		return false;
	}
}

/*************************************************
 Function:		GetJsonData
 Description:	从Json请求串中读取业务请求数据
 Input:          JsonReq:Json格式请求数字
 Output:         jsonDataObj 业务数据,格式为Json对象
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetJsonData(const UTF8String JsonReq, Json::Value &jsonDataObj) {
	try {
		Json::Reader reader;
		Json::Value valObj;
		if (reader.parse(JsonReq.c_str(), valObj)) {

			if (!valObj.isMember("JsonData")) {
				_LastErrS = L"Json参数错误:JsonData不存在";
				return false;
			}
			if (valObj["JsonData"].isNull()) {
				_LastErrS = L"Json参数错误:JsonData为null";
				return false;
			}
			if (!valObj["JsonData"].isObject()) {
				_LastErrS = L"Json参数错误:JsonData应为Json对象";
				return false;
			}
			jsonDataObj = valObj["JsonData"];
			return true;
		}
		else {
			_LastErrS = L"Json结构错误";
			return false;
		}
	}
	catch (Exception &e) {
		_LastErrS = L"Json异常:" + e.Message;
		return false;
	}
	catch (...) {
		_LastErrS = L"Json异常:未知错误";
		return false;
	}
}

/*************************************************
 Function:		GetLocation
 Description:	从Json请求串中读取JsonData中的位置或设备ID
 Input:          JsonReq:Json格式请求数字
 Output:         LocationID 位置或设备ID
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetLocation(const UTF8String JsonReq, UTF8String &Location) {
	Json::Value jsonDataObj;
	if (!GetJsonData(JsonReq, jsonDataObj)) {
		return false;
	}

	if (!jsonDataObj.isMember("Location")) {
		_LastErrS = L"Json参数错误:Location不存在";
		return false;
	}

	if (jsonDataObj["Location"].isNull()) {
		_LastErrS = L"Json参数错误:Location不能为null";
		return false;
	}

	if (!jsonDataObj["Location"].isString()) {
		_LastErrS = L"Json参数错误:Location应为字符串";
		return false;
	}

	Location = jsonDataObj["Location"].asCString();
	return true;
}

/*************************************************
 Function:		GetCZYLocation
 Description:	从Json请求串中读取JsonData中的采样和制样的位置或设备ID
 Input:          JsonReq:Json格式请求数字
 Output:         LocationID 位置或设备ID
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetCZYLocation(const UTF8String JsonReq, UTF8String &CYLocation,
	UTF8String &ZYLocation) {
	Json::Value jsonDataObj;
	if (!GetJsonData(JsonReq, jsonDataObj)) {
		return false;
	}

	if (!jsonDataObj.isMember("CYLocation")) {
		_LastErrS = L"Json参数错误:Location不存在";
		return false;
	}

	if (jsonDataObj["CYLocation"].isNull()) {
		_LastErrS = L"Json参数错误:Location不能为null";
		return false;
	}

	if (!jsonDataObj["CYLocation"].isString()) {
		_LastErrS = L"Json参数错误:Location应为字符串";
		return false;
	}

	CYLocation = jsonDataObj["CYLocation"].asCString();

	if (!jsonDataObj.isMember("ZYLocation")) {
		_LastErrS = L"Json参数错误:Location不存在";
		return false;
	}

	if (jsonDataObj["ZYLocation"].isNull()) {
		_LastErrS = L"Json参数错误:Location不能为null";
		return false;
	}

	if (!jsonDataObj["ZYLocation"].isString()) {
		_LastErrS = L"Json参数错误:Location应为字符串";
		return false;
	}

	ZYLocation = jsonDataObj["ZYLocation"].asCString();

	return true;
}

/*************************************************
 Function:		GetLocation
 Description:	从Json请求串中读取JsonData中的控制指令ID
 Input:          JsonReq:Json格式请求数字
 Output:         LocationID 位置或设备ID
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetActionID(const UTF8String JsonReq, int &ActionID) {
	Json::Value jsonDataObj;
	if (!GetJsonData(JsonReq, jsonDataObj)) {
		return false;
	}

	if (!jsonDataObj.isMember("ActionID")) {
		_LastErrS = L"Json参数错误:ActionID不存在";
		return false;
	}

	if (jsonDataObj["ActionID"].isNull()) {
		_LastErrS = L"Json参数错误:ActionID不能为null";
		return false;
	}

	if (!jsonDataObj["ActionID"].isInt()) {
		_LastErrS = L"Json参数错误:ActionID应为整数";
		return false;
	}

	ActionID = jsonDataObj["ActionID"].asInt();
	return true;
}

/*************************************************
 Function:		GetCZYBM
 Description:	从Json请求串中读取JsonData中的采制样编码
 Input:          JsonReq:Json格式请求数字,nType 查询的编码类型 0采 1制
 Output:         strBM 编码
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
bool TJsonReq::GetCZYBM(const UTF8String JsonReq, UTF8String &strBM, int nType)
{
	Json::Value jsonDataObj;
	if (!GetJsonData(JsonReq, jsonDataObj)) {
		return false;
	}

	if (0 == nType) {
		if (!jsonDataObj.isMember("Cybm")) {
			_LastErrS = L"Json参数错误:Cybm不存在";
			return false;
		}

		if (jsonDataObj["Cybm"].isNull()) {
			_LastErrS = L"Json参数错误:Cybm不能为null";
			return false;
		}

		if (!jsonDataObj["Cybm"].isString()) {
			_LastErrS = L"Json参数错误:Cybm应为字符串";
			return false;
		}
		strBM = jsonDataObj["Cybm"].asCString();
	}
	else {
		if (!jsonDataObj.isMember("Zybm")) {
			_LastErrS = L"Json参数错误:Zybm不存在";
			return false;
		}

		if (jsonDataObj["Zybm"].isNull()) {
			_LastErrS = L"Json参数错误:Zybm不能为null";
			return false;
		}

		if (!jsonDataObj["Zybm"].isString()) {
			_LastErrS = L"Json参数错误:Zybm应为字符串";
			return false;
		}
		strBM = jsonDataObj["Zybm"].asCString();
	}
	return true;
}

/*************************************************
 Function:		CreateJsonReq_pt
 Description:	生成 调用平台接口的Json
 Input:          JsonReq:Json格式请求数字,nType 查询的编码类型 0采 1制
 Output:         strBM 编码
 Return:         成功true,反之false
 Calls:          详见函数
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-21
 *************************************************/
UTF8String TJsonReq::CreateJsonReq_pt(UTF8String User, UTF8String Pass,
	UTF8String OperType, Json::Value JsonData) {
	Json::Value root;

	root["LoginName"] = User.c_str();
	root["PassWord"] = Pass.c_str();
	root["operType"] = OperType.c_str();
	root["JsonData"] = JsonData;

	Json::StyledWriter writer;
	// Json::FastWriter writer;
	return writer.write(root).c_str();
}
