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
						case '=': {
							state = 10; break;
						}
						case '/': {
							state = 11; break;
						}
						case ',':
						case '+':
						case '-': 
						case '*':
						case '(':
						case ')':
						case '{':
						case '}':
						case ';':
						case '\\': {
							state = 0;
							ret("symbol:\t\t%c\n", C);
							break;
						}
						case '"': {
							state = 14;
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
					if (iskey != -1) ret("keyword:\t%s(ID:%d)\n", token.c_str(), iskey); //如果是关键字 返回记号
					else {
						int identry = table_insert();
						ret("keyword:\t%s(new)\n", token.c_str()); //不是关键字 新增记号
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
					ret("number:\t\t%d\n", SToI());
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
					ret("number:\t%lf\n", SToF());
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
					ret("number:\t%lf\n", SToF());
				}
				break;
			}
			case 8: { // '<'
				cat();
				get_char();
				if (C == '=') { //<=
					cat();
					state = 0;
					ret("symbol:\t\tLE\n");
				}
				else if (C == '>') { //<>
					cat();
					state = 0;
					ret("symbol:\t\t<>\n");
				}
				else { //<
					retract();
					state = 0;
					ret("symbol:\t\tLS\n");
				}
				break;
			}
			case 9: { // '>'
				cat();
				get_char();
				if (C == '=') { //>=
					cat();
					state = 0;
					ret("symbol:\t\tGE\n");
				}
				else { //>
					retract();
					state = 0;
					ret("symbol:\t\tGT\n");
				}
				break;
			}
			case 10: { // '='
				cat();
				get_char();
				if (C == '=') { //==
					cat();
					state = 0;
					ret("symbol:\t\tEqual Opt\n");
				}
				else { //=
					retract();
					state = 0;
					ret("symbol:\t\tAssign Opt\n");
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
				else {
					retract();
					state = 0;
					ret("symbol:\t\t/\n");
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
			case 14: { // '"'
				cat();
				get_char();
				if(C == '\\') state = 15;
				else if (C != '"') state = 14;
				else {
					cat();
					state = 0;
					ret("constchar:\t%s\n", token.c_str()); //不是关键字 新增记号
				}
				break;
			}
			case 15: { // '"'...'\'
				get_char();
				if (C != '"') {
					token.append(1, '\\');
					state = 14;
				}
				else {
					state = 14;
				}
				break;
			}
		}
	} while (forward - buffer < buffer_size); //C != EOF
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
	printf("(line:%d,column:%d)", line, col-strlen(token.c_str()));
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
