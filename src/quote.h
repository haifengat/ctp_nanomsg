
#pragma once
#ifdef _WINDOWS //64位系统没有预定义 WIN32
#ifdef __cplusplus
#define DLL_EXPORT_C_DECL extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT_DECL __declspec(dllexport)
#endif
#else
#ifdef __cplusplus
#define DLL_EXPORT_C_DECL extern "C"
#else
#define DLL_EXPORT_DECL extern
#endif
#endif

#ifdef _WINDOWS
#define WINAPI __stdcall
#define WIN32_LEAN_AND_MEAN23 //  从 Windows 头文件中排除极少使用的信息
#include "stddef.h"
#pragma comment(lib, "../ctp_20171123/thostmduserapi.lib")
#else
#define WINAPI
#endif

#include <string.h>
#include "../ctp_20171123/ThostFtdcMdApi.h"

class CQuote : public CThostFtdcMdSpi
{
public:
    CQuote() = default;
    CQuote(CQuote &&) = default;
    ~CQuote() = default;
    
    char* api_id = new char[256];
    int trade_id, position_id, deposit_id;

    void Start(const char *front, const char *broker, const char *investor, const char *pwd);
    void Login();
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected();
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
    CThostFtdcMdApi *api = NULL;
    int logError = 999;
    char broker[8];
    char investor[16];
    char pwd[16];
    char tradingDay[9];
    int nRequestID = 0;

    void ShowMsg(const char* msg);
    int Gb2312ToUtf8(char *sOut, int iMaxOutLen, const char *sIn, int iInLen);
};
