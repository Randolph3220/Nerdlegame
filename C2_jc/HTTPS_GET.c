#define URI "https://nerdlegame.com/words/3a066bda8c96b9478bb0512f0a43028c"
#define SvrIP "nerdlegame.com"
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

char plainText[20] = {
	'*', '+', ' ', '-', ' ',
	'/', '0', '1', '2', '3',
	'4', '5', '6', '7', '8',
	'9', ' ', ' ', ' ', '='
};

int main() {
	FILE* f = fopen("C2.txt", "w");

	WSADATA wsa = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsa);

	char strResp[1000] = {0};
	
	// 从网站GET到加密后的答案 
	if (GETRequest(SvrIP, 443, URI, strResp) == 0) {
		puts("=== GET req suc ===\n");
		printf("Ciphertext : %s\n\n", strResp);
	}
	else puts("=== GET req err ===");
	
	// 将密文解密得答案 
	printf("Plaintext  : ");
	for (int i = 0; i < strlen(strResp); i++) {
		char c = plainText[(int)strResp[i] - 55];
		putchar(c);
		fwrite(&c, 1, 1, f);
	}
	puts("\n");

	fclose(f);
	return 0;
}
