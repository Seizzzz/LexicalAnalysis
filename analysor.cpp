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
						case ':': {
							state = 10; break;
						}
						case '/': {
							state = 11; break;
						}
						case '=': {
							state = 0;
							//ret(relop, EQ);
							break;
						}
						case '+':
						case '-': 
						case '*':
						case '(':
						case ')':
						case ';':
						case '\\': {
							state = 0;
							//ret(C, -);
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
				if (letter() || digit()) state = 1;
				else {
					retract();
					state = 0;
					iskey = reserve();
					if (iskey != -1) ret("iskey ID %d", iskey); //如果是关键字 返回记号
					else {
						int identry = table_insert();
						ret("ID %d %s", reserve(), identry); //不是关键字 新增记号
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
				else if (C == 'E') {
					state = 5;
				}
				else {
					retract();
					state = 0;
					ret("NUM %d", SToI());
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
				else if (C == 'E') state = 5;
				else {
					retract();
					state = 0;
					ret("NUM %lf", SToF());
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
					ret("NUM %lf", SToF());
				}
				break;
			}
			case 8: {
				cat();
				get_char();
				if (C == '=') { //<=
					cat();
					state = 0;
					ret("relop LE");
				}
				else if (C == '>') { //<>
					cat();
					state = 0;
					ret("relop RE");
				}
				else { //<
					retract();
					state = 0;
					ret("relop LT");
				}
				break;
			}
			case 9: {
				cat();
				get_char();
				if (C == '=') { //>=
					cat();
					state = 0;
					ret("relop GE");
				}
				else { //>
					retract();
					state = 0;
					ret("relop GT");
				}
				break;
			}
			case 10: {
				cat();
				get_char();
				if (C == '=') {
					cat();
					state = 0;
					ret("assign_op");
				}
				else {
					retract();
					state = 0;
					ret(":");
				}
				break;
			}
			case 11: {
				cat();
				get_char();
				if (C == '*') state = 12;
				else {
					retract();
					state = 0;
					ret("/");
				}
				break;
			}
			case 12: {
				get_char();
				while (C != '*') get_char();
				get_char();
				if (C == '/') state = 0;
				else state = 12;
				break;
			}
			case 13: {
				error();
				state = 0;
				break;
			}
		}
	} while (C != EOF);
}

void Analysor::get_char()
{
	C = *(forward++);
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
	printf("error!");
	return;
}

void Analysor::ret(const char* format, ...)
{
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
	return distance(table_keyword.begin(), iter);
}

int Analysor::table_insert()
{
	string sv(token);
	table_keyword.push_back(sv);

	return table_keyword.size() - 1;
}

Analysor::Analysor(const char* prgm, const char* key)
{
	FILE* pkey = NULL;
	if (fopen_s(&pkey, key, "r") != 0) {
		ret("Keyword File Open Error");
		return;
	}
	while(fscanf_s(pkey, "%s", buffer)) table_keyword.push_back(string(buffer));
	fclose(pkey);

	FILE* pprgm = NULL;
	if (fopen_s(&pprgm, prgm, "r") != 0) {
		ret("Program File Open Error");
		return;
	}
	fseek(pprgm, 0, SEEK_END);
	int size = ftell(pprgm);
	rewind(pprgm);
	fread(buffer, 1, size, pprgm);
	fclose(pprgm);

	forward = buffer;
	analyse();
}
