#include <iostream>
#include <string>
#include "ChemBalance.h"

using namespace std;

int main(void)
{
	chem_equation eq("Al + Fe2O4 -> Fe + Al2O3");
	cout << eq << endl;

	chem_equation balanced = eq.balance();

	getchar();
}