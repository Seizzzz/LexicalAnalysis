#pragma once

#include <memory>
#include <cctype>

const int MAX_TOKEN_LEN = 10;
const int MAX_BUFFER_LEN = 100;

class Analysor
{
public:
	int state = 0;
	char C;
	char token[MAX_TOKEN_LEN];
	char buffer[MAX_BUFFER_LEN];
	char* forward;
	char* lexemebegin;
	int iskey = -1;

	void analyse();

	void get_char();
	void get_nbc();
	void cat();
	void retract();
	void SToI();
	void SToF();
	void error();
	void ret();

	bool letter();
	bool digit();

	int reserve();

	char* table_insert();


	
	
};