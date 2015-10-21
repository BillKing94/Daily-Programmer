#include "ChemBalance.h"
#include "Parse.h"
#include <boost/rational.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

using boost::rational;
using namespace boost::numeric::ublas;
using namespace std;

template <typename T>
matrix<T> rref(matrix<T> m)
{
	matrix<T> result = m;

	int lead = 0;
	int rowCount = (int)result.size1();
	int columnCount = (int)result.size2();
	for (int r = 0; r < rowCount; r++)
	{
		if (columnCount <= lead)
			break;

		int i = r;
		while (result(i, lead) == 0)
		{
			i = i + 1;
			if (rowCount == i)
			{
				i = r;
				lead = lead + 1;
				if (columnCount == lead)
					break;
			}
		}

		{
			matrix_row<matrix<T>> rowI(result, i);
			matrix_row<matrix<T>> rowR(result, r);
			rowI.swap(rowR);

			if (result(r, lead) != 0)
				rowR /= result(r, lead);
		}

		for (int i = 0; i < rowCount; i++)
		{
			if (i != r)
			{
				matrix_row<matrix<T>> rowI(result, i);
				matrix_row<matrix<T>> rowR(result, r);

				rowI -= (result(i, lead) * rowR);
			}
		}

		lead = lead + 1;
	}

	return result;
}

template <typename T>
inline bool all_equal(std::vector<T> values)
{
	if (values.size() == 0)
		return true;

	bool allEqual = true;
	int size = (int)values.size();
	T x0 = values[0];
	for (int i = 1; i < size; i++)
	{
		if (values[i] != x0)
		{
			allEqual = false;
			break;
		}
	}

	return allEqual;
}

template <typename T>
inline int idx_of_minimum(std::vector<T> values)
{
	if (values.size() == 0)
		return -1;

	int size = (int)values.size();
	T min = values[0];
	int minIdx = 0;

	for (int i = 1; i < size; i++)
	{
		if (values[i] < min)
		{
			min = values[i];
			minIdx = i;
		}
	}

	return minIdx;
}

template <typename T>
T least_common_multiple(std::vector<T> factors)
{
	if (factors.size() == 0)
		return -1;

	if (factors.size() == 1)
		return factors[0];

	std::vector<T> original = factors;


	while (!all_equal(factors))
	{
		int minIdx = idx_of_minimum(factors);
		factors[minIdx] += original[minIdx];
	}

	return factors[0];
}


chem_equation::chem_equation(string equation)
{
	chem_equation eq = parse_chem_equation(equation);
	this->left = eq.left;
	this->right = eq.right;
}

chem_equation::chem_equation(void) : left(), right() {}

chem_equation::chem_equation(const chem_equation& original)
{
	this->left = original.left;
	this->right = original.right;
}

chem_equation
chem_equation::balance(void)
{
	set<string> elements;

	for (auto iMol = left.begin(); iMol != left.end(); iMol++)
	{
		t_molecule mol = get<0>(*iMol);
		for (auto iEl = mol.begin(); iEl != mol.end(); iEl++)
		{
			elements.insert(get<0>(*iEl));
		}
	}

	for (auto iMol = left.begin(); iMol != left.end(); iMol++)
	{
		t_molecule mol = get<0>(*iMol);
		for (auto iEl = mol.begin(); iEl != mol.end(); iEl++)
		{
			elements.insert(get<0>(*iEl));
		}
	}

	int nElements = (int)elements.size();
	int nMoleculesLeft = (int)left.size();
	int nMoleculesRight = (int)right.size();
	int nMoleculesTotal = nMoleculesLeft + nMoleculesRight;

	matrix<rational<int>> A = zero_matrix<rational<int>>(nElements, nMoleculesTotal);



	for (int molIdx = 0; molIdx < nMoleculesLeft; molIdx++)
	{
		t_molecule mol = get<0>(left[molIdx]);
		for (auto iEl = mol.begin(); iEl != mol.end(); iEl++)
		{
			string elName = get<0>(*iEl);
			int elCount = get<1>(*iEl);

			int elIdx = (int)distance(elements.begin(), elements.find(elName));

			A(elIdx, molIdx) = elCount;
		}
	}

	for (int molIdx = 0; molIdx < nMoleculesRight; molIdx++)
	{
		t_molecule mol = get<0>(right[molIdx]);
		for (auto iEl = mol.begin(); iEl != mol.end(); iEl++)
		{
			string elName = get<0>(*iEl);
			int elCount = get<1>(*iEl);

			int elIdx = (int)distance(elements.begin(), elements.find(elName));

			A(elIdx, molIdx + nMoleculesLeft) = -elCount;
		}
	}

	auto A2 = rref(A);

#if _DEBUG
	cout << A2;
#endif

	std::vector<int> denominators;
	int nRows = (int)A2.size1();
	int nCols = (int)A2.size2();

	int knowns; // # of known coeffs (set to 1)
	int unknowns = 0; // # of unknown coeffs

	for (int rowIdx = 0; rowIdx < nRows; rowIdx++)
	{
		for (int colIdx = 0; colIdx < nCols; colIdx++)
		{
			if (A2(rowIdx, colIdx) != 0)
				goto nonzero;
		}
		//allzero:
		break;
	nonzero:
		unknowns++;
	}
	knowns = nCols - unknowns;

	// Take lcd of values in the first *unknowns* rows and the last *knowns* columns

	for (int colIdx = nCols - knowns; colIdx < nCols; colIdx++)
		for (int rowIdx = 0; rowIdx < unknowns; rowIdx++)
			denominators.push_back(A2(rowIdx, colIdx).denominator());

	int lcm = least_common_multiple(denominators);

	// TODO: multiply coeffs by lcm and build output

	return chem_equation();
}

#pragma region operator<< overloads
ostream& operator<<(std::ostream& os, const t_element& el)
{
	string element = get<0>(el);
	int elCount = get<1>(el);

	os << element;

	if (elCount > 1)
		os << elCount;

	return os;
}

ostream& operator<<(ostream& os, const t_molecule& mol)
{
	for (auto i = mol.begin(); i != mol.end(); i++)
	{
		os << *i;
	}

	return os;
}

ostream& operator<<(ostream& os, const t_moleculeCount& mc)
{
	int count = get<1>(mc);
	if (count > 1)
		os << count;

	os << get<0>(mc);

	return os;
}

ostream& operator<<(ostream& os, const t_formula& f)
{
	for (int i = 0; i < f.size(); i++)
	{
		os << f[i];
		if (i + 1 < f.size())
			os << " + ";
	}

	return os;
}

ostream& operator<<(ostream& os, const chem_equation& eq)
{
	os << eq.left << " -> " << eq.right;
	return os;
}
#pragma endregion