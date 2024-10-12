#define CODE_SUCCESS 0
#define CODE_FAILURE -1
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define printError(Code, errorType) \
int sslErrCode = SSL_get_error(ssl, Code);\
printf("%s", errorType);\
printf(", openssl errCode = %d\n", sslErrCode);

#include "HTTPS.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "ws2_32.lib")

// 实现对https的GET
int GETRequest(const char* strSvrIP, int iSvrPort, const char* strURI, char* strResp) {
	return sendDataToSvr(GET_STRING, strSvrIP, iSvrPort, strURI, "", "", strResp);
}

// 建立tcp连接 
int tcpConn(const char* pSvrIP, int iSvrPort, int* sock) {

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*sock == -1) return CODE_FAILURE;
	
	struct sockaddr_in sa;
	sa.sin_addr.s_addr = inet_addr(pSvrIP);
	sa.sin_port = htons(iSvrPort);
	sa.sin_family = AF_INET;

	int retCode = connect(*sock, (struct sockaddr*)&sa, sizeof(sa));
	if (retCode == -1) return CODE_FAILURE;

	return CODE_SUCCESS;
}

// 生成GET请求所需发送的信息 
int generateGETReqData(const char* strSvrIP, int iSvrPort,
	const char* strURI, char* strReqData) {

	char pLine[256] = { 0 };
	sprintf_s(pLine, sizeof(pLine), "GET %s HTTP/1.1\r\n", strURI);
	strcat(strReqData, pLine);

	memset(pLine, 0, sizeof(pLine));
	sprintf_s(pLine, sizeof(pLine), "Host: %s:%d\r\n", strSvrIP, iSvrPort);
	strcat(strReqData, pLine);

	memset(pLine, 0, sizeof(pLine));
	strcat(strReqData, "Accept: */*\r\n");
	strcat(strReqData, "Connection: close\r\n\r\n");

	return CODE_SUCCESS;
}

int readRespToString(SSL* ssl, char* strRespData) {
	// 每次读取1K 
	char pBody[1024 + 1] = { 0 };
	int readSize = sizeof(pBody) - 1;
	int readLen = 0;
	while ((readLen = SSL_read(ssl, pBody, readSize)) > 0) {
		strcat(strRespData, pBody);
		memset(pBody, 0, sizeof(pBody));
	}
	
	if (readLen < 0) {
		printError(readLen, "SSL_read");
		return CODE_FAILURE;
	}
	strcat(strRespData, pBody);

	return CODE_SUCCESS;
}

// 向服务器发送数据 
int sendDataToSvr(enum REQ_TYPE reqType, const char* strSvrIP, int iSvrPort,
	const char* strURI, const char* strBody, const char* strFilePath, char* strResp) {
	//strBody     : request body when reqType = 1(POST_STRING)
	//strFilePath : storage path of downloaded files when reqType = 2(GET_FILE)
	//strResp     : response string when reqType = 0/1(GET_STRING/POST_STRING)
	//				error description information when an error occurs
	
	int iRet = CODE_FAILURE;
	int socketFd = 0;
	SSL_CTX* ctx = 0;
	SSL* ssl = 0;
	
	do {
		char* pSvrIP = NULL;
		struct hostent* pHostent = NULL;
		pHostent = gethostbyname(strSvrIP);

		if (pHostent == NULL) {
			printf("WSAGetLastError = %d\n", WSAGetLastError());
			break;
		}
		pSvrIP = inet_ntoa(*(struct in_addr*)pHostent->h_addr_list[0]);

		// 建立tcp连接 
		if (tcpConn(pSvrIP, iSvrPort, &socketFd) != CODE_SUCCESS) break;

		// 初始化SSL并连接接口 
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
		ctx = SSL_CTX_new(SSLv23_client_method());
		if (ctx == NULL) break;
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, socketFd);
		
		// 对于超过一个主机名的服务器，需设定主机名 
		SSL_set_tlsext_host_name(ssl, strSvrIP);

		int retCode = SSL_connect(ssl);
		
		if (retCode != 1) {
			printError(retCode, "SSL_connect");
			break;
		}
		
		char strReqData[1000] = { 0 };

		generateGETReqData(strSvrIP, iSvrPort, strURI, strReqData);

		// 通过SSL发送数据
		int writeLen = SSL_write(ssl, strReqData, strlen(strReqData));
		if (writeLen <= 0) {
			int sslErrCode = SSL_get_error(ssl, writeLen);
			printf("SSL_write error, openssl errCode = %d\n", sslErrCode);
			break;
		}
		
		// 读取响应数据 
		int readLen = 0, i = 0;
		char pHeader[1] = { 0 };
		while ((readLen = SSL_read(ssl, pHeader, 1)) == 1) {
			if (i < 4) {
				if (pHeader[0] == '\r' || pHeader[0] == '\n') {
					i++;
					if (i >= 4) break;
				}
				else i = 0;
			}
		}
		
		if (readLen < 0) {
			printError(readLen, "SSL_read");
			break;
		}
		
		iRet = readRespToString(ssl, strResp);
	} while (0);

	// 关闭接口，断开连接 
	if (socket) closesocket(socketFd);
	if (ctx) SSL_CTX_free(ctx);
	if (ssl) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}

	return iRet;
}
