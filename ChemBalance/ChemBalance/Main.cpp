#include <iostream>
#include <string>
#include "ChemBalance.h"

using namespace std;

int main(void)
{
	while (true)
	{
		cout << "> ";
		string eqString;
		getline(cin, eqString);

		chem_equation eq(eqString);
		cout << eq << endl;

		//chem_equation balanced = eq.balance();

		//cout << balanced << endl;

	}
	getchar();

}