#include "analysor.h"

void Analysor::analyse()
{
	do
	{
		switch (state)
		{
			case 0: {
				token = "";
				get_char();
				get_nbc();
				if (forward - buffer >= buffer_size) break;
				if (isalpha(C)) {
					state = 1;
				}
				else if (isdigit(C)) {
					state = 2;
				}
				else {
					switch (C) {
						case '<': {
							state = 8; break;
						}
						case '>': {
							state = 9; break;
						}
						case '=': {
							state = 10; break;
						}
						case '/': {
							state = 11; break;
						}
						case '+':
						case '-':
						case '*':
						case '%': {
							cat();
							state = 18;
							break;
						}
						case ',':
						case '(':
						case ')':
						case '{':
						case '}':
						case ';':
						case '\\': {
							state = 0;
							ret("<symbol, %d, %c>\n", line, C);
							break;
						}
						case '"': {
							state = 14;
							break;
						}
						case '&': {
							state = 16;
							break;
						}
						case '|': {
							state = 17;
							break;
						}
						default: {
							state = 13;
							break;
						}
					}
				}
				break;
			}
			case 1: {
				cat();
				get_char();
				if (letter() || digit()) state = 1; // || C == '.'
				else {
					retract();
					state = 0;
					iskey = reserve();
					if (iskey != -1) ret("<ID=%d, %d, %s>\n", iskey, line, token.c_str()); //是关键字 返回
					else {
						int identry = table_insert();
						ret("<ID=%d(new), %d, %s>\n", identry, line, token.c_str()); //不是关键字 新增
					}
				}
				break;
			}
			case 2: {
				cat();
				get_char();

				if (isdigit(C)) {
					state = 2;
				}
				else if (C == '.') {
					state = 3;
				}
				else if (C == 'E' || C == 'e') {
					state = 5;
				}
				else {
					retract();
					state = 0;
					ret("<num, %d, %d>\n", line, SToI());
				}

				break;
			}
			case 3: {
				cat();
				get_char();
				if (digit()) state = 4;
				else {
					error();
					state = 0;
				}
				break;
			}
			case 4: {
				cat();
				get_char();

				if (isdigit(C)) state = 4;
				else if (C == 'E' || C == 'e') state = 5;
				else {
					retract();
					state = 0;
					ret("<num, %d, %lf>\n", line, SToF());
				}
				break;
			}
			case 5: {
				cat();
				get_char();
				if (isdigit(C)) state = 7;
				else if (C == '+' || C == '-') state = 6;
				else {
					retract();
					error();
					state = 0;
				}
				break;
			}
			case 6: {
				cat();
				get_char();
				if (digit()) state = 7;
				else {
					retract();
					error();
					state = 0;
				}
				break;
			}
			case 7: {
				cat();
				get_char();
				if (digit()) state = 7;
				else {
					retract();
					state = 0;
					ret("<num, %d, %lf>\n", line, SToF());
				}
				break;
			}
			case 8: { // '<'
				cat();
				get_char();
				if (C == '=') { //<=
					cat();
					state = 0;
					ret("<symbol, %d, LE>\n", line);
				}
				else { //<
					retract();
					state = 0;
					ret("<symbol, %d, LS>\n", line);
				}
				break;
			}
			case 9: { // '>'
				cat();
				get_char();
				if (C == '=') { //>=
					cat();
					state = 0;
					ret("<symbol, %d, GE>\n", line);
				}
				else { //>
					retract();
					state = 0;
					ret("<symbol, %d, GT>\n", line);
				}
				break;
			}
			case 10: { // '='
				cat();
				get_char();
				if (C == '=') { //==
					cat();
					state = 0;
					ret("<symbol, %d, EQ>\n", line);
				}
				else { //=
					retract();
					state = 0;
					ret("<symbol, %d, ASSIGN>\n", line);
				}
				break;
			}
			case 11: { // '/'
				cat();
				get_char();
				if (C == '*') state = 12;
				else if (C == '/') {
					while (C != '\n') get_char();
					get_char();
					state = 0;
				}
				else if (C == '=') {
					ret("<symbol, %d, /=>\n", line);
					state = 0;
				}
				else {
					retract();
					ret("<symbol, %d, />\n", line);
					state = 0;
				}
				break;
			}
			case 12: { // '/'
				get_char();
				while (C != '*') get_char();
				get_char();
				if (C == '/') state = 0;
				else state = 12;
				break;
			}
			case 13: { //error
				error();
				state = 0;
				break;
			}
			case 14: { // '"'
				cat();
				get_char();
				if(C == '\\') state = 15;
				else if (C != '"') state = 14;
				else {
					cat();
					state = 0;
					ret("<array, %d, %s>\n", line, token.c_str()); //常量字符数组
				}
				break;
			}
			case 15: { // '"'...'\'
				get_char();
				if (C != '"') token.append(1, '\\');
				state = 14;
				break;
			}
			case 16: { // '&'
				get_char();
				if (C == '&') ret("<symbol, %d, &&>\n", line);
				else {
					retract();
					ret("<symbol, %d, &>\n", line);
				}
				state = 0;
				break;
			}
			case 17: { // '|'
				get_char();
				if (C == '|') ret("<symbol, %d, ||>\n", line);
				else {
					retract();
					ret("<symbol, %d, |>\n", line);
				}
				state = 0;
				break;
			}
			case 18: { // '+ - * %'
				get_char(); 
				if(C == '=') ret("<symbol, %d, %s=>\n", line, token.c_str());
				else {
					retract();
					ret("<symbol, %d, %s>\n", line, token.c_str());
				}
				state = 0;
				break;
			}
		}
	} while (forward - buffer < buffer_size); //C != EOF

	statistic();
}

void Analysor::get_char()
{
	C = *(forward++);
	if (C == '\n') {
		++line; col = 0;
	}
	++col;
}

void Analysor::get_nbc()
{
	while (C == ' ' || C == '\n' || C == '\t' || C == '\0') get_char();
}

void Analysor::cat()
{
	token.append(1, C);
}

void Analysor::retract()
{
	--forward;
	if (*forward == '\n') --line;
	--col;
}

int Analysor::SToI()
{
	return atoi(token.c_str());
}

double Analysor::SToF()
{
	return atof(token.c_str());
}

void Analysor::error()
{
	printf("error<line:%d, column:%d>\n", line, col);
	return;
}

void Analysor::ret(const char* format, ...)
{
	++cnttoken;
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

bool Analysor::letter()
{
	return isalpha(C);
}

bool Analysor::digit()
{
	return isdigit(C);
}

int Analysor::reserve()
{
	auto iter = find(table_keyword.begin(), table_keyword.end(), string(token));
	if(iter != table_keyword.end()) return distance(table_keyword.begin(), iter);
	return -1;
}

int Analysor::table_insert()
{
	string sv(token);
	table_keyword.push_back(sv);

	return table_keyword.size() - 1;
}

void Analysor::statistic()
{
	cout << "字符总数" << buffer_size << endl;
	cout << "语句行数：" << line << endl;
	cout << "识别出的符号数：" << cnttoken << endl;
}

Analysor::Analysor(const char* prgm, const char* key)
{
	FILE* pkey;
	if (fopen_s(&pkey, key, "r") != 0) {
		ret("Keyword File Open Error");
		return;
	}

	while (fscanf_s(pkey, "%s", buffer, MAX_TOKEN_LEN) != EOF) table_keyword.push_back(string(buffer));
	fclose(pkey);

	FILE* pprgm = NULL;
	if (fopen_s(&pprgm, prgm, "r") != 0) {
		ret("Program File Open Error");
		return;
	}
	fseek(pprgm, 0, SEEK_END);
	buffer_size = ftell(pprgm);
	rewind(pprgm);
	fread(buffer, 1, buffer_size, pprgm);
	fclose(pprgm);

	forward = buffer;

	//init
	line = 1;
	col = 0;

	analyse();
}
