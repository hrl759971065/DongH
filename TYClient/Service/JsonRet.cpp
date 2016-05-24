// ---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "JsonRet.h"
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
UTF8String TJsonRet::GetLastErrS() const {
	return _LastErrS;
}

/*************************************************
 Function:		CreatJsonRet
 Description:	生成Json格式的Json串,用于生成 WebService服务接口的返回值
 Input:          nRet 返回值 errMsg 错误信息 RowCnt数据集行数 JsonData 数据
 Output:         无
 Return:         Json格式的Json串
 Calls:       	无
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-23
 *************************************************/
UTF8String TJsonRet::CreatJsonRet(int nRet, UTF8String errMsg, int RowCnt,
	Json::Value JsonData) {
	Json::Value root;
	root["Result"] = nRet;
	root["ErrMsg"] = errMsg.c_str();
	root["RowNum"] = RowCnt;
	root["JsonData"] = JsonData;

	Json::FastWriter writer;
	return UTF8String(writer.write(root).c_str());

}

/*************************************************
 Function:		CreatJsonRet
 Description:	将平台WebService服务返回的Json串 转成 Json对象
 Input:          jsonStr串
 Output:         jv Json对象
 Return:         true 成功 false失败
 Calls:       	无
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-24
 *************************************************/
bool TJsonRet::JsonStrToJson_pt(UTF8String jsonStr, Json::Value &jv) {
	// ---删除平台的 \, UTF8字符串下应该没有斜杠
	while (jsonStr.Pos("\\") > 0) {
		jsonStr.Delete(jsonStr.Pos("\\"), 1);
	}

	while (jsonStr.Pos("\r") > 0) {
		jsonStr.Delete(jsonStr.Pos("\r"), 1);
	}

	while (jsonStr.Pos("\n") > 0) {
		jsonStr.Delete(jsonStr.Pos("\n"), 1);
	}
	// ---删除中括号的引号
	if (jsonStr.Pos("[") > 1 && jsonStr[jsonStr.Pos("[") - 1] == '\"')
		jsonStr.Delete(jsonStr.Pos("[") - 1, 1);
	if (jsonStr.Pos("]") > 1 && jsonStr[jsonStr.Pos("]") + 1] == '\"')
		jsonStr.Delete(jsonStr.Pos("]") + 1, 1);

	// ---删除花括号的引号
	/* UTF8String s =L"\"{";
	 int n =jsonStr.Pos(s);
	 if(jsonStr.Pos(s) >0 )
	 jsonStr.Delete(jsonStr.Pos("\"{"),1 );
	 if(jsonStr.Pos(L"}\"") >0 )
	 jsonStr.Delete(jsonStr.Pos("}\"")+1,1 ); */	// ---些问题平台已修复

	Json::Reader reader;

	if (!reader.parse(jsonStr.c_str(), jv)) {
		_LastErrS = L"JsonStr_correct_pt:结构错误";
		return false;
	}
	return true;
}

/*************************************************
 Function:		RsultAnalysis
 Description:	分析Json串中的 结果 字段
 Input:          jsonStr串
 Output:
 Return:         true 成功 false失败
 Calls:       	JsonStrToJson_pt
 Table Accessed: 无
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-24
 *************************************************/
bool TJsonRet::RsultAnalysis(UTF8String jsonStr) {
	Json::Value jv_Out;
	if (!JsonStrToJson_pt(jsonStr, jv_Out)) {

		return false;
	}
	if (!jv_Out.isMember("DataType")) {
		_LastErrS = L"返回参数DataType不存在";
		return false;
	}
	UTF8String Result;
	if (jv_Out["DataType"].isString())
		Result = jv_Out["DataType"].asCString();
	if (jv_Out["DataType"].isInt())
		Result = UTF8String(jv_Out["DataType"].asInt());
	else {
		_LastErrS = L"返回参数错误:DataType类型不正确";
		return false;
	}

	if ("0" == Result)
		return true;
	else {
		if (jv_Out.isMember("ErrorMsg") && jv_Out["ErrorMsg"].isString()) {

			_LastErrS = jv_Out["ErrorMsg"].asCString();
		}
		else
			_LastErrS = "未知错误";
		return false;
	}
}
