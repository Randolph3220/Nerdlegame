#pragma once
#include <openssl/ssl.h>

enum REQ_TYPE {
	GET_STRING = 0,
	POST_STRING,
	GET_FILE,
};

int GETRequest(const char* strSvrIP, int iSvrPort,
	const char* strURI, char* strResp);

int tcpConn(const char* pSvrIP, int iSvrPort, int* socket);

int generateGETReqData(const char* strSvrIP, int iSvrPort,
	const char* strURI, char* strReqData);

int readRespToString(SSL* ssl, char* strRespData);

int sendDataToSvr(enum REQ_TYPE reqType, const char* strSvrIP, int iSvrPort,
	const char* strURI, const char* strBody, const char* strFilePath, char* strResp);

