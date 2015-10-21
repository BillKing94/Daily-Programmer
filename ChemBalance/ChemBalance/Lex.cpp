#include "Lex.h"
#include <iostream>

const regex chem_lexer::rx_space("^[ \r\n]+");
const regex chem_lexer::rx_chars("^[A-Z][a-z]*");
const regex chem_lexer::rx_number("^[0-9]+");
const regex chem_lexer::rx_plus("^\\+");
const regex chem_lexer::rx_arrow("^->");

chem_lexer::chem_lexer(string equation)
{
	this->equation = equation;
}

bool
chem_lexer::get_next_token(lex_result* resultPtr)
{
	static const char* eq = equation.c_str();
	static bool returnedEOF = false;

	if (*eq == 0) // end of string
	{
		if (returnedEOF)
			return false;
		else
		{
			resultPtr->type = END_OF_FILE;
			returnedEOF = true;
			return true;
		}
	}

	cmatch matchResults;

	do
	{
		if (regex_search(eq, matchResults, rx_space))
		{
			resultPtr->type = SPACE;
		}
		else if (regex_search(eq, matchResults, rx_chars))
		{
			resultPtr->type = ELEM;
			resultPtr->str = new string(matchResults[0].str());
		}
		else if (regex_search(eq, matchResults, rx_number))
		{
			resultPtr->type = NUMBER;
			resultPtr->num = stoi(matchResults[0].str());
		}
		else if (regex_search(eq, matchResults, rx_plus))
		{
			resultPtr->type = PLUS;
		}
		else if (regex_search(eq, matchResults, rx_arrow))
		{
			resultPtr->type = ARROW;
		}

		if (resultPtr->type != UNKNOWN)
		{
			eq += matchResults[0].length();
		}
		else
		{
			cerr << "Unknown Symbol" << endl;
			eq += 1;
		}
	} while (resultPtr->type == SPACE && *eq != 0);

	return true;
}