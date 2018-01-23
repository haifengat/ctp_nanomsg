#include <iostream>
#include <fstream>
#include <unistd.h> //sleep
#include <string.h>
#include <stdio.h>
#include <map>
#include <thread>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "quote.h"

using namespace std;
using namespace rapidjson;



int sock = nn_socket (AF_SP, NN_PUB);
map<string, CQuote*> risks;

void sendmsg(const char*, char*);
void tradeLogin(string host, Value& info);

Document d;
char filepath[256];
char tradingDay[16];

bool read_config(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if(!read_config(argc, argv))
       return -1;

    assert (nn_bind (sock, "ipc:///tmp/pubsub.ipc") >= 0);
    Value& ids = d["use_api_id"];
    Value& trade_info = d["trade_info"];
    for(SizeType i = 0; i < ids.Size(); i++)
    {
        Value& info = trade_info[ids[i].GetString()];
        //thread t1(tradeLogin, ids[i].GetString(), info);

        CQuote* r = new CQuote();
        risks[ids[i].GetString()] = r;
        strcpy(r->api_id, ids[i].GetString());

        Value& query = info["query_id"];
        r->trade_id = query["trade_id"].GetInt();
        r->deposit_id = query["deposit_id"].GetInt();
        r->position_id = query["position_id"].GetInt();
        // 赋值前交易日,相同则继续订阅,不同则从0订阅
        strcpy(tradingDay, d["tradingday"].GetString());

        if(info["is_test"].GetBool())
        {
            string str = d["front_test"].GetString();
            r->Start(str.substr(str.find('|')+1).c_str(), d["broker_test"].GetString(), info["user"].GetString(),info["pwd"].GetString());
        }
        else
        {
            string str = d["front_real"].GetString();
            r->Start(str.substr(str.find('|')+1).c_str(), d["broker_real"].GetString(), info["user"].GetString(),info["pwd"].GetString());
        }
    }

    cout << "please press q to quit." << endl;
    char q;
    scanf("%c", &q);
    while(q != 'Q' && q != 'q')
    {
        cout << "please press q to quit." << endl;
        scanf("%c", &q);
    }
}

bool read_config(int argc, char *argv[])
{
    ifstream conf;
    if(argc == 1)
        strcpy(filepath, "./config.json");
    else
        strcpy(filepath, argv[1]);
    
    conf.open(filepath, ifstream::in);
    if(!conf.is_open())
    {
        cout << "there is not config.json file" << endl;
        getchar();
        return false;
    }
    
    string line;
    string stringFromStream;
    while(getline(conf, line))
    {
        stringFromStream.append(line+"\n");
    }
    conf.close();
    
    //Document d;
    d.Parse(stringFromStream.c_str());
    if(d.HasParseError()){
        rapidjson::ParseErrorCode code = d.GetParseError();
        cout << "read config.json error: " << code << endl;
        return false;
    }
    return true;
}


///登录请求响应
void CQuote::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    this->logError = pRspInfo->ErrorID;
	if(pRspInfo->ErrorID != 0)
	    //cout << "rspuserlogin:" << pRspInfo->ErrorID << ":" << pRspInfo->ErrorMsg << endl;
        {
            char tmp[128];
            sprintf(tmp, "rspuserlogin error: [%d]%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
            this->ShowMsg(tmp);
        }
    else
	{
        //CShfeFtdcInvestorIDRangeField ri = {0};
        //api->ReqQryInvestorDepartment(&ri, 100, requestId++);
        if(strcmp(tradingDay, pRspUserLogin->TradingDay) != 0)
        {
            trade_id = 0;
            position_id = 0;
            deposit_id = 0;
        }
        strcpy(this->tradingDay, pRspUserLogin->TradingDay);
        d["tradingday"].SetString(StringRef(tradingDay));// .SetString(StringRef
        char tmp[1024];
        //sprintf(tmp, "login succeed. %s\t%s@%s", tradingDay, pRspUserLogin->UserID, pRspUserLogin->BrokerID);
        sprintf(tmp, "login succeed. %s\t%s@%s", tradingDay, this->investor, this->broker);
        this->ShowMsg(tmp);
	}
}
