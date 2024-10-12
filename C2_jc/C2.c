#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash[110];
char Ans[9];	  // 通过GET操作获取的算式 
char Try[9];	  // 通过搜索尝试可能的算式 
int sure[9];      // 该位置上的字符是否已经确定 
int absent[60];   // 该字符在算式中是否存在
int extAns[60];   // 该字符在答案中是否存在 
int found;		  // 是否已找到一组可能的算式（用于结束搜索） 
int visit[9][60]; // 记录该位置上的某一字符是否已经尝试过 
int numStack[10], numTop, charStack[10], charTop;
//存储数字和运算符的栈，用于计算表达式 

// 将字符映射到数字上，便于枚举 
char ch[15] = {
	'0', '1', '2', '3', '4',
	'5', '6', '7', '8', '9', 
	'+', '-', '*', '/', '='    
};

// 判断是否为运算符号 
#define ischar(x) (((x) < 48) || ((x) == '='))

int Compare(int a, int b){ // 用于比较运算符优先级 
	return (a == '*' || a == '/') && (b == '+' || b == '-' );
}

int Hash(){	// 对字符串hash，避免重复尝试 
	int x = 0;
	for (int i = 0 ; i < 8 ; i ++)
		x = (Try[i] * 13 % 101 + i) % 101;
	return x;	
}

void pop(){ // 出栈操作 
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

int checkValid(){ // 检查算式是否可行 
	int i = 0;
	numTop = charTop = 0;
	
	int cnt = 0; // 统计算式中等号的个数 
	for (int k = 0 ; k < 7 ; k ++){
		if (ischar(Try[k]) && ischar(Try[k + 1])) return 0;
		if (Try[k] == '=') cnt ++;
	}	
	if (cnt != 1) return 0;
	
	// 计算等号左边表达式的值 
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
	
	// 计算等号右边的值 
	int value = Try[++i] - '0';
	
	for ( ; i + 1 < 8 ; i ++)
		value = value * 10 + Try[i + 1] - '0';
	
	if (numStack[1] != value) return 0;
	if (hash[Hash()]) return 0;
	
	// 等号左右相等，且该算式为尝试过，该算式可行 
	hash[Hash()] = 1;
	return 1;
}

int checkAns(){ // 与答案相比对 
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

void guess(int i){ // 通过搜索寻找可行的算式 
	if (found) return;
	if (i > 7) { // 如果Try已经填完 
		if (checkValid()) found = 1;
		return;
	}
	
	if (sure[i]) { // 如果该位置上的字符已确定 
		if (ischar(Try[i - 1]) && ischar(Try[i])) return;
		Try[i] = Ans[i];
		guess(i + 1);
		return;
	}
	
	for (int j = 0 ; j < 15 ; j ++){ // 枚举可能的字符 
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

void print(){ // 输出响应信息 
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
	
	// 先尝试两个固定的算式 
	strcpy(Try, "0+18/3=6");
	
	if (checkAns()) goto success;
	print();
	
	strcpy(Try, "4*9-7=29");

	if (checkAns()) goto success;
	print();
	
	// 尝试可能的算式 
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
