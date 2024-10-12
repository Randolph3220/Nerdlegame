#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash[110];
char Ans[9];	  // ͨ��GET������ȡ����ʽ 
char Try[9];	  // ͨ���������Կ��ܵ���ʽ 
int sure[9];      // ��λ���ϵ��ַ��Ƿ��Ѿ�ȷ�� 
int absent[60];   // ���ַ�����ʽ���Ƿ����
int extAns[60];   // ���ַ��ڴ����Ƿ���� 
int found;		  // �Ƿ����ҵ�һ����ܵ���ʽ�����ڽ��������� 
int visit[9][60]; // ��¼��λ���ϵ�ĳһ�ַ��Ƿ��Ѿ����Թ� 
int numStack[10], numTop, charStack[10], charTop;
//�洢���ֺ��������ջ�����ڼ�����ʽ 

// ���ַ�ӳ�䵽�����ϣ�����ö�� 
char ch[15] = {
	'0', '1', '2', '3', '4',
	'5', '6', '7', '8', '9', 
	'+', '-', '*', '/', '='    
};

// �ж��Ƿ�Ϊ������� 
#define ischar(x) (((x) < 48) || ((x) == '='))

int Compare(int a, int b){ // ���ڱȽ���������ȼ� 
	return (a == '*' || a == '/') && (b == '+' || b == '-' );
}

int Hash(){	// ���ַ���hash�������ظ����� 
	int x = 0;
	for (int i = 0 ; i < 8 ; i ++)
		x = (Try[i] * 13 % 101 + i) % 101;
	return x;	
}

void pop(){ // ��ջ���� 
	int y = numStack[numTop--];
	int x = numStack[numTop--];
	switch(charStack[charTop]){
		case 42 : numStack[++numTop] = x * y; break;
		case 43 : numStack[++numTop] = x + y; break;
		case 45 : numStack[++numTop] = x - y; break;
		case 47 : numStack[++numTop] = x / y; break;
	}
	charTop--;
}

int checkValid(){ // �����ʽ�Ƿ���� 
	int i = 0;
	numTop = charTop = 0;
	
	int cnt = 0; // ͳ����ʽ�еȺŵĸ��� 
	for (int k = 0 ; k < 7 ; k ++){
		if (ischar(Try[k]) && ischar(Try[k + 1])) return 0;
		if (Try[k] == '=') cnt ++;
	}	
	if (cnt != 1) return 0;
	
	// ����Ⱥ���߱��ʽ��ֵ 
	for ( ; i < 8 ; i ++){
		if (Try[i] == '=') break;
		if (Try[i] > 47) {
			int x = Try[i] - '0';
			for ( ; (i + 1 < 8) && (Try[i + 1] > 47) && (Try[i + 1] != '=') ; i ++)
				x = x * 10 + Try[i + 1] - '0';
			numStack[++numTop] = x;
		}
		else {
			if (!charTop) charStack[++charTop] = Try[i];
			else {
				if (Compare(Try[numTop], charStack[charTop]))
					charStack[++charTop] = Try[i];
				else {
					while(charTop != 0 && (!Compare(Try[i], charStack[charTop]))) pop();
					charStack[++charTop] = Try[i];
				}
			}
		}
	}
	while(charTop) pop();
	
	// ����Ⱥ��ұߵ�ֵ 
	int value = Try[++i] - '0';
	
	for ( ; i + 1 < 8 ; i ++)
		value = value * 10 + Try[i + 1] - '0';
	
	if (numStack[1] != value) return 0;
	if (hash[Hash()]) return 0;
	
	// �Ⱥ�������ȣ��Ҹ���ʽΪ���Թ�������ʽ���� 
	hash[Hash()] = 1;
	return 1;
}

int checkAns(){ // �����ȶ� 
	int suc = 1;
	
	for (int i = 0 ; i < 8 ; i ++){
		visit[i][Try[i]] = 1;
		if (Try[i] == Ans[i]) sure[i] = 1;
			else {
				if (!extAns[Try[i]]) absent[Try[i]] = 1;
				suc = 0;
			}
	}
	return suc;
}

void guess(int i){ // ͨ������Ѱ�ҿ��е���ʽ 
	if (found) return;
	if (i > 7) { // ���Try�Ѿ����� 
		if (checkValid()) found = 1;
		return;
	}
	
	if (sure[i]) { // �����λ���ϵ��ַ���ȷ�� 
		if (ischar(Try[i - 1]) && ischar(Try[i])) return;
		Try[i] = Ans[i];
		guess(i + 1);
		return;
	}
	
	for (int j = 0 ; j < 15 ; j ++){ // ö�ٿ��ܵ��ַ� 
		if (absent[ch[j]] || visit[i][ch[j]]) continue;
		if (i == 0 && j > 9) break;
		if (i == 7 && j > 9) break;
		if (i > 0){
			if (Try[i - 1] == '0' && j < 10) continue;
			if (ischar(Try[i - 1]) && j > 9) break;
		}
		Try[i] = ch[j];
		guess(i + 1);
		if (found) return;
	}
}

void print(){ // �����Ӧ��Ϣ 
	puts("===== Response =====");
	puts("R : Right Postion");
	puts("W : Wrong Postion");
	puts("N : Not Existing\n");
	for (int i = 0 ; i < 8 ; i ++) printf(" %c", Try[i]);
	puts("");
	for (int i = 0 ; i < 8 ; i ++) 
		printf(sure[i]   ? " R" : 
			   absent[Try[i]] ? " N" : " W");
	puts("\n\n");
}

int main(){
	FILE* f = fopen("C2.txt", "r");
	fgets(Ans, 9, f);

	for (int i = 0 ; i < 8 ; i ++)
		extAns[Ans[i]] = 1;
	
	// �ȳ��������̶�����ʽ 
	strcpy(Try, "0+18/3=6");
	
	if (checkAns()) goto success;
	print();
	
	strcpy(Try, "4*9-7=29");

	if (checkAns()) goto success;
	print();
	
	// ���Կ��ܵ���ʽ 
	while(1){
		found = 0;
		guess(0);
		int res = checkAns();
		print();
		if (res) goto success; 
	}
	
	success : 
	puts("===== Right Answer! =====");
	return 0;
}
