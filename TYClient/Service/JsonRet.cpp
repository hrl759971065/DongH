// ---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "JsonRet.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

/*************************************************
 Function:		GetLastErrS
 Description:	��ȡ���һ�εĴ�������
 Input:          ��
 Output:         ��
 Return:         ���ش�������
 Calls:       	��
 Table Accessed: ��
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-21
 *************************************************/
UTF8String TJsonRet::GetLastErrS() const {
	return _LastErrS;
}

/*************************************************
 Function:		CreatJsonRet
 Description:	����Json��ʽ��Json��,�������� WebService����ӿڵķ���ֵ
 Input:          nRet ����ֵ errMsg ������Ϣ RowCnt���ݼ����� JsonData ����
 Output:         ��
 Return:         Json��ʽ��Json��
 Calls:       	��
 Table Accessed: ��
 Table Updated:  ��
 Others:
 Author: 		���ı�
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
 Description:	��ƽ̨WebService���񷵻ص�Json�� ת�� Json����
 Input:          jsonStr��
 Output:         jv Json����
 Return:         true �ɹ� falseʧ��
 Calls:       	��
 Table Accessed: ��
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-24
 *************************************************/
bool TJsonRet::JsonStrToJson_pt(UTF8String jsonStr, Json::Value &jv) {
	// ---ɾ��ƽ̨�� \, UTF8�ַ�����Ӧ��û��б��
	while (jsonStr.Pos("\\") > 0) {
		jsonStr.Delete(jsonStr.Pos("\\"), 1);
	}

	while (jsonStr.Pos("\r") > 0) {
		jsonStr.Delete(jsonStr.Pos("\r"), 1);
	}

	while (jsonStr.Pos("\n") > 0) {
		jsonStr.Delete(jsonStr.Pos("\n"), 1);
	}
	// ---ɾ�������ŵ�����
	if (jsonStr.Pos("[") > 1 && jsonStr[jsonStr.Pos("[") - 1] == '\"')
		jsonStr.Delete(jsonStr.Pos("[") - 1, 1);
	if (jsonStr.Pos("]") > 1 && jsonStr[jsonStr.Pos("]") + 1] == '\"')
		jsonStr.Delete(jsonStr.Pos("]") + 1, 1);

	// ---ɾ�������ŵ�����
	/* UTF8String s =L"\"{";
	 int n =jsonStr.Pos(s);
	 if(jsonStr.Pos(s) >0 )
	 jsonStr.Delete(jsonStr.Pos("\"{"),1 );
	 if(jsonStr.Pos(L"}\"") >0 )
	 jsonStr.Delete(jsonStr.Pos("}\"")+1,1 ); */	// ---Щ����ƽ̨���޸�

	Json::Reader reader;

	if (!reader.parse(jsonStr.c_str(), jv)) {
		_LastErrS = L"JsonStr_correct_pt:�ṹ����";
		return false;
	}
	return true;
}

/*************************************************
 Function:		RsultAnalysis
 Description:	����Json���е� ��� �ֶ�
 Input:          jsonStr��
 Output:
 Return:         true �ɹ� falseʧ��
 Calls:       	JsonStrToJson_pt
 Table Accessed: ��
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-24
 *************************************************/
bool TJsonRet::RsultAnalysis(UTF8String jsonStr) {
	Json::Value jv_Out;
	if (!JsonStrToJson_pt(jsonStr, jv_Out)) {

		return false;
	}
	if (!jv_Out.isMember("DataType")) {
		_LastErrS = L"���ز���DataType������";
		return false;
	}
	UTF8String Result;
	if (jv_Out["DataType"].isString())
		Result = jv_Out["DataType"].asCString();
	if (jv_Out["DataType"].isInt())
		Result = UTF8String(jv_Out["DataType"].asInt());
	else {
		_LastErrS = L"���ز�������:DataType���Ͳ���ȷ";
		return false;
	}

	if ("0" == Result)
		return true;
	else {
		if (jv_Out.isMember("ErrorMsg") && jv_Out["ErrorMsg"].isString()) {

			_LastErrS = jv_Out["ErrorMsg"].asCString();
		}
		else
			_LastErrS = "δ֪����";
		return false;
	}
}