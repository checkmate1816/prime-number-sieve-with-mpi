#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<string.h>
#include<stdlib.h>
using namespace std;

//关键字 
string key[9] = { "begin","end","integer","if","then","else","function","read","write"};
//关键字的种别码
string keyNum[9] = {"01","02","03","04","05","06","07","08","09"};
//运算符和界符 
string symbol[12] = { "=","<>","<=","<",">=",">","-","*",":=","(",")",";"};
//char symbol[12]={'<','>','!=','>=','<=','==',',',';','(',')','{','}'};
//运算符和界符的种别码 
string symbolNum[12] = { "12","13","14","15","16","17","18","19","20","21","22","23" };
//存放文件取出的字符 
string letter[1000];
//将字符转换为单词
string words[1000];
int length; //保存程序中字符的数目 
int num;
int linenumber = 1;
//int token = 0;
string isSymbol(string s) { //判断运算符和界符 
	int i;
	for (i = 0; i < 12; i++) {
		if (s == symbol[i])
			return symbolNum[i];
	}
	return "0";
}

//判断是否为数字 
bool isNum(string s) {
	if (s >= "0" && s <= "9")
		return true;
	return false;
}

//判断是否为字母 
bool isLetter(string s)
{
	if (s >= "a" && s <= "z")
		return true;
	return false;
}

//判断是否为关键字,是返回种别码 
string isKeyWord(string s) {
	int i;
	for (i = 0; i < 9; i++) {
		if (s == key[i])
			return keyNum[i];
	}
	return "0";
}
/*bool isWrong(string s)
{
	if (s >= "A"&&s <= "Z")
		return true;
	return false;
}*/
//返回单个字符的类型 
int typeword(string str) {
	if (str >= "a" && str <= "z") // 字母 
		return 1;

	if (str >= "0" && str <= "9") //数字 
		return 2;

	if (str == "=" || str == "<>" || str == "<=" || str == "<" || str == ">=" || str == ">" || str == "-" || str == "*" || str == ":=" || str == "("
		|| str == ")" || str == ";" ) //判断运算符和界符 
		return 3;
	if (*str.c_str()== '\n')
		return 4;
}

string identifier(string s, int n) {
	int j = n + 1;
	int flag = 1;
	
	while (flag) {
		if (isNum(letter[j]) || isLetter(letter[j]))
		{

			s = (s + letter[j]);
			if (isKeyWord(s) != "0") 
			{

				num = j;
				return s;
			}
			j++;
			num = j;
		   else
			{
				j++;
				num = j;
			}
		}
		/*else if (isWrong(letter[j]))
		{
			token = 1;
			num = j;
			return "NULL";
		}*/
		else {
			
			flag = 0;
			
		}
	}

	num = j-1;
	return s;
}

string symbolStr(string s, int n) {
	int j = n + 1;
	string str = letter[j];
	if (str == ">" || str == "=" || str == "<" || str == "!") {
		s = (s + letter[j]);
		j++;
	}
	num = j-1;
	return s;
}

string Number(string s, int n) {
	int j = n + 1;
	int flag = 1;

	while (flag) {
		if (isNum(letter[j])) {
			s = (s + letter[j]);
			j++;
		}
		else {
			flag = 0;
		}
	}

	num = j-1;
	return s;
}

void print(string s, string n) {
	int count = strlen(s.c_str());
	for (; count <16; count++)
		s = ' ' + s;
	cout << s.c_str() << ' ' << n.c_str() << endl;
}

void TakeWord(FILE *fp) { //取单词 
	int k;
	string error = "LINE:";
	char *linenum = NULL;
	for (num = 0; num < length;num++) {
		string str1, str;
		str = letter[num];
		k = typeword(str);
		switch (k) {
		case 1:
		{
			str1 = identifier(str, num);
			if (str1 == "NULL")
			{
				linenum = (char*)malloc(3 * sizeof(char));
				_itoa(linenumber, linenum, 10);
				error = error + *linenum + "  " + "非法输入";
				fputs(error.c_str(), fp);
			}
			else if (isKeyWord(str1) != "0")
				print(str1, isKeyWord(str1));
			/*else if (str1 == "wrong")
				continue;*/
			else
			{
				if (strlen(str1.c_str()) <= 16)
					print(str1, "10");
				else
				{
					linenum =(char*) malloc(3 * sizeof(char));
					_itoa(linenumber, linenum,10);
					error=error + *linenum +"  "+ "参数过多";
					fputs(error.c_str(), fp);
				}
			}
			break;
		}

		case 2:
		{
			str1 = Number(str, num);
			print(str1, "11");
			break;
		}

		case 3:
		{
			str1 = symbolStr(str, num);
			print(str1, isSymbol(str1));
			break;
		}
		case 4:
		{
			linenumber++;
			print("EOLN", "24");
			break;
		}
		}
	}
}

int main() {
	char w;
	FILE *input;
	FILE *error;
	input = fopen("test.txt", "r");
	error = fopen("error.err", "a");
	//freopen("test.txt", "r", stdin);
	freopen("output.dyd", "w", stdout); //从控制台输出，而不是文本输出
	
	length = 0;
	while ((w=fgetc(input))!=EOF) {
		if (w != ' ') {
			letter[length] = w;
			length++;
		} //去掉程序中的空格
	}
	
	TakeWord(error);
	print("EOF", "25");
	fclose(stdin);//关闭文件 
	fclose(stdout);//关闭文件 
	return 0;
}