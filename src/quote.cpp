#include <iostream>
#include <string.h>
#include <stdio.h>  
#include <stdlib.h> 
#include <iconv.h>
#include <unistd.h> //sleep
#include <thread>

#include "quote.h"

using namespace std;

int CQuote::Gb2312ToUtf8(char *sOut, int iMaxOutLen, const char *sIn, int iInLen)     
{     
    char *pIn = (char *)sIn;     
    char *pOut = sOut;     
    size_t ret;     
    size_t iLeftLen=iMaxOutLen;     
    iconv_t cd;     
  
    cd = iconv_open("utf-8", "gb2312");     
    if (cd == (iconv_t) - 1)     
    {     
        return -1;     
    }     
    size_t iSrcLen=iInLen;     
    ret = iconv(cd, &pIn,&iSrcLen, &pOut,&iLeftLen);     
    if (ret == (size_t) - 1)     
    {     
        iconv_close(cd);     
        return -1;     
    }     
  
    iconv_close(cd);     
  
    return (iMaxOutLen - iLeftLen);     
}

void CQuote::ShowMsg(const char* msg)
{
    char pszDst[strlen(msg)+1];     
    Gb2312ToUtf8(pszDst, strlen(msg), msg, strlen(msg)); 
    //printf("%s", pszDst);
    cout << pszDst <<endl;
}

void CQuote::Start(const char *front, const char *broker, const char *investor, const char *pwd)
{
    string user(investor);
    system(("mkdir " + user).c_str());
    api = CThostFtdcMdApi::CreateFtdcMdApi(("./"+user+"/").c_str());
    strcpy(this->broker, broker);
    strcpy(this->investor, investor);
    strcpy(this->pwd, pwd);
    char tmp[256];
    strcpy(tmp, front);
    api->RegisterFront(tmp);
    api->RegisterSpi(this);
    api->Init();
}
void login(CQuote *q)
{
    q->Login();
}
///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CQuote::OnFrontConnected()
{
    cout << "connected" << endl;
    this->logError = -1;
    thread t1(login, this);
    t1.detach();
    // cout << this->logError << endl;
}

void CQuote::Login()
{
    // 无法重连的错误
    while(this->logError < 0 || this->logError > 13)
    {
        CThostFtdcReqUserLoginField f = {0};
        strcpy(f.UserID, this->investor);
        strcpy(f.Password, this->pwd);
        strcpy(f.BrokerID, this->broker);
        strcpy(f.UserProductInfo, "@haifengAT");
        this->api->ReqUserLogin(&f, this->nRequestID++);
        sleep(10);
    }
}

