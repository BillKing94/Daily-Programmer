#include "Parse.h"
#include "Lex.h"
#include "ChemBalance.h"
#include <iostream>


/*****************************************
* Parsing Rules:
1. The caller prepares the appropriate first token for the callee
2. The callee advances the token past its nonterminal before returning
3. Syntax errors are only thrown when a terminal is consumed or needed to choose a parsing strategy
*****************************************/

struct parser_state
{
	lex_result token;
	chem_lexer lexer;

	parser_state(string equation) : lexer(equation) {}
};

// parsing procs forward declarations
chem_equation _equation(parser_state* state);
t_formula _formula(parser_state* state);
t_formula _formulaTail(parser_state* state);
t_moleculeCount _moleculeCount(parser_state* state);
t_molecule _molecule(parser_state* state);
t_molecule _moleculeTail(parser_state* state);
t_element _element(parser_state* state);

void err_premature_eof()
{
	cerr << "Premature EOF";
	exit(1);
}

void err_syntax()
{
	cerr << "Syntax Error";
	exit(2);
}

inline void advance_token(parser_state* state)
{
	if (!state->lexer.get_next_token(&state->token))
		err_premature_eof();
}

chem_equation
parse_chem_equation(string equation)
{
	parser_state state(equation);
	advance_token(&state);

	chem_equation result = _equation(&state);

	if (state.token.type != END_OF_FILE)
		err_syntax();

	return result;
}

chem_equation
_equation(parser_state* state) // formula ARROW formula
{
	chem_equation result;

	result.left = _formula(state);

	if (state->token.type != ARROW)
		err_syntax();

	advance_token(state);

	result.right = _formula(state);

	return result;
}

t_formula
_formula(parser_state* state) //moleculeCount formulaTail
{
	t_formula result;

	result.push_back(_moleculeCount(state));

	t_formula tail = _formulaTail(state);
	result.insert(result.end(), tail.begin(), tail.end());

	return result;
}

t_formula
_formulaTail(parser_state* state) // eps | PLUS formula
{
	t_formula result;

	switch (state->token.type)
	{
		// Follow Set
		case ARROW:
		case END_OF_FILE:
			break; // return result

		// First Set
		case PLUS:
		{
			advance_token(state);
			t_formula formula = _formula(state);
			result.insert(result.end(), formula.begin(), formula.end());
		} break;
		default:
			err_syntax();
	}

	return result;
}
t_moleculeCount
_moleculeCount(parser_state* state) // NUMBER molecule | molecule
{
	int count;
	t_molecule molecule;

	if (state->token.type == NUMBER)
	{
		count = state->token.num;
		advance_token(state);
	}
	else
		count = 1;

	molecule = _molecule(state);

	t_moleculeCount result = make_tuple(molecule, count);
	return result;
}

t_molecule
_molecule(parser_state* state) // element moleculeTail
{
	t_molecule result;
	result.push_back(_element(state));

	t_molecule tail = _moleculeTail(state);
	result.insert(result.end(), tail.begin(), tail.end());

	return result;
}

t_molecule
_moleculeTail(parser_state* state) // eps | molecule
{
	t_molecule result;

	switch (state->token.type) {
		// follow
		case PLUS:
		case ARROW:
		case END_OF_FILE:
			break; // return result

		// first
		case ELEM:
		{
			result = _molecule(state);
		} break;
		default:
			err_syntax();
	}

	return result;
}

t_element
_element(parser_state* state) // ELEM | ELEM NUMBER
{
	string element;
	int count;

	if (state->token.type != ELEM)
		err_syntax();

	element = *state->token.str;
	advance_token(state);

	switch (state->token.type)
	{
		// follow
		case ELEM:
		case PLUS:
		case ARROW:
		case END_OF_FILE:
			count = 1;
			break;
		case NUMBER:
			count = state->token.num;
			advance_token(state);
			break;
		default:
			err_syntax();
	}

	t_element result = make_tuple(element, count);
	return result;
}