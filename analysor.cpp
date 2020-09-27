#include "analysor.h"

void Analysor::analyse()
{
	do
	{
		switch (state)
		{
			case 0: { //³õÊ¼×´Ì¬
				strcpy_s(token, " ");
				get_char();
				get_nbc();
				if (isalpha(C)) {
					state = 1;
				}
				else if (isdigit(C)) {
					state = 2;
				}
				else
				{
					switch (C)
					{
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
			}
		}
	} while (true);
}
