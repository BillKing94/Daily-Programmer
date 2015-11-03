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
chem_formula _formula(parser_state* state);
chem_formula _formulaTail(parser_state* state);
tuple<chem_molecule, int> _moleculeCount(parser_state* state);
chem_molecule _molecule(parser_state* state);
shared_ptr<chem_component> _component(parser_state* state);
chem_componentCount _componentCount(parser_state* state);
chem_molecule _moleculeTail(parser_state* state);
chem_element _element(parser_state* state);


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

chem_formula _formula(parser_state* state) // moleculeCount formulaTail
{
	chem_formula result;
	result.molecules.push_back(_moleculeCount(state));
	chem_formula tail = _formulaTail(state);
	result.molecules.insert(result.molecules.end(), tail.molecules.begin(), tail.molecules.end());

	return result;
}

chem_formula _formulaTail(parser_state* state) // e | PLUS FORMULA
{
	chem_formula result;

	switch (state->token.type)
	{
		// Follow:
		case ARROW:
		case END_OF_FILE:
			return result;

			// First:
		case PLUS:
			break;

		default:
			err_syntax();
	}

	advance_token(state);

	result = _formula(state);

	return result;
}

tuple<chem_molecule, int> _moleculeCount(parser_state* state) // NUMBER molecule | molecule
{
	int count = 1;

	if (state->token.type == NUMBER)
	{
		count = state->token.num;
		advance_token(state);
	}

	chem_molecule molecule = _molecule(state);

	return make_tuple(molecule, count);
}

chem_molecule _molecule(parser_state* state) // componentCount _moleculeTail
{
	chem_molecule result;
	result.components.push_back(_componentCount(state));

	chem_molecule tail = _moleculeTail(state);
	result.components.insert(result.components.end(), tail.components.begin(), tail.components.end());

	return result;
}

shared_ptr<chem_component> _component(parser_state* state) // ELEM | LPAREN molecule RPAREN
{
	switch (state->token.type)
	{
		case ELEM:
			return make_shared<chem_element>(_element(state));
		case LPAREN:
		{
			advance_token(state);
			auto result = make_shared<chem_molecule>(_molecule(state));
			if (state->token.type == RPAREN)
				advance_token(state);
			else
				err_syntax();
			return result;
		}
		default:
			err_syntax();
			return shared_ptr<chem_element>(); // to avoid compiler warning
	}
}

chem_componentCount _componentCount(parser_state* state) // component | component NUMBER
{
	chem_componentCount result;
	result.component = _component(state);
	result.count = 1;

	if (state->token.type == NUMBER)
	{
		result.count = state->token.num;
		advance_token(state);
	}

	return result;
}

chem_molecule _moleculeTail(parser_state* state) // e | molecule
{
	switch (state->token.type)
	{
		// First:
		case ELEM:
		case LPAREN:
			return _molecule(state);
		
		// Follow:
		case PLUS:
		case ARROW:
		case RPAREN:
		case END_OF_FILE:
			return chem_molecule();

		default:
			err_syntax();
			return chem_molecule(); // to avoid compiler warning
	}
}

chem_element _element(parser_state* state) // ELEM
{
	chem_element result;

	if (state->token.type != ELEM)
		err_syntax();

	result.symbol = *state->token.str;

	return result;
}