#pragma once

#include <memory>
#include <cctype>
#include <cstdarg>
#include <vector>
#include <string>
#include <algorithm>
using std::find;
using std::distance;
using std::string;
using std::vector;

const int MAX_BUFFER_LEN = 500;
const int MAX_TOKEN_LEN = 50;

class Analysor
{
public:
	int state = 0;
	char C;
	string token;
	char buffer[MAX_BUFFER_LEN];
	int buffer_size;
	const char* forward;
	int iskey = -1;

	vector<string> table_keyword;

	void analyse();

	void get_char();
	void get_nbc();
	void cat();
	void retract();
	int SToI();
	double SToF();
	void error();
	void ret(const char*, ...);

	bool letter();
	bool digit();

	int reserve();
	int table_insert();
	
	Analysor(const char*, const char*); //³ÌÐò¡¢±£Áô×Ö

private:
	int line, col;
};