#pragma once
#include <string>
#include <vector>
#include <memory>
#include <regex>
using namespace std;

enum symbol_type
{
	UNKNOWN,
	SPACE,
	ELEM,
	NUMBER,
	PLUS,
	ARROW,
	END_OF_FILE
};

struct lex_result
{
	symbol_type type;

	union
	{
		string* str;
		int num;
	};

	lex_result()
	{
		str = NULL;
	}

	lex_result(const lex_result& original)
	{
		type = original.type;
		switch (type)
		{
			case ELEM:
				str = new string(*original.str);
				break;
			case NUMBER:
				num = original.num;
				break;
		}
	}

	~lex_result()
	{
		if (type == ELEM)
			delete str;
	}
};

class chem_lexer
{
private:
	const static regex rx_space;
	const static regex rx_chars;
	const static regex rx_number;
	const static regex rx_plus;
	const static regex rx_arrow;

	string equation;
	string equationRemaining;
	bool returnedEOF;

public:
	chem_lexer(string equation);
	bool get_next_token(lex_result* resultPtr);
};