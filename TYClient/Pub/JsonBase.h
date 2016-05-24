#pragma once
#include "reader.h"
#include <sstream>
#include <algorithm>
using namespace std;

//#pragma comment(lib,"json/json_vc71_libmtd.lib")

class JsonBase
{
public:
    JsonBase(void);
    ~JsonBase(void);
public:
    bool IsNodeExisted(string jsonObj, string strNode);

    int AppendArrayObject(string& jsonObj, string strNode, string strValue);
    int GetJsonValue(string jsonObj,string strNode,string& strOut);
    int GetJsonChildNodeArray(string jsonObj, string strNode,vector<string>& vecChildNode);
    int SetJsonObejct(string& jsonObj, string strNode, string strValue);
private:
    bool ReadJson(string jObect, Json::Value& root);
    bool GetJsonObject(Json::Value &jsonObj, string strValue);
};

