#include "ChemBalance.h"
#include "Parse.h"
#include <boost/rational.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

using boost::rational;
using boost::numeric::ublas::vector;
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
					return result;
			}
		}

		{
			matrix_row<matrix<T>> rowI(result, i);
			matrix_row<matrix<T>> rowR(result, r);
			rowI.swap(rowR);

			if (result(r, lead) != 0)
			{
				rational<int> divisor = result(r, lead);
				rowR /= divisor;
			}
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

		lead++;

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

	// Check for alchemy (i.e. if the equation is impossible)
	for (auto iMol = right.begin(); iMol != right.end(); iMol++)
	{
		t_molecule mol = get<0>(*iMol);
		for (auto iEl = mol.begin(); iEl != mol.end(); iEl++)
		{
			if (elements.find(get<0>(*iEl)) == elements.end())
			{
				// TODO: return some sort of flag
				cerr << "Alchemy detected." << endl;
				return *this;
			}
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

			A(elIdx, molIdx) += elCount;
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

#if _DEBUG
	cout << "A:" << endl << A << endl << endl;
#endif

	auto A2 = rref(A);


#if _DEBUG
	cout << "A2:" << endl << A2 << endl << endl;
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

	if (knowns == 0)
	{
		cerr << "Impossible!" << endl;
		// TODO: return error flag
		return *this;
	}

	matrix_range<matrix<rational<int>>> unknownCoeffs(A2, range(0, unknowns), range(nCols - knowns, nCols));

	// Take lcd of values in the first *unknowns* rows and the last *knowns* columns

	for (int rowIdx = 0; rowIdx < unknownCoeffs.size1(); rowIdx++)
		for (int colIdx = 0; colIdx < unknownCoeffs.size2(); colIdx++)
			denominators.push_back(unknownCoeffs(rowIdx, colIdx).denominator());

	int lcm = least_common_multiple(denominators);

	A2 *= lcm;

	boost::numeric::ublas::vector<int> coeffs(nMoleculesTotal);

	for (int i = knowns; i < nMoleculesTotal; i++)
		coeffs(i) = lcm;

#if _DEBUG
	cout << "unknownCoeffs:" << endl << unknownCoeffs << endl << endl;
#endif

	matrix<rational<int>> unknownValues = prod(-1 * unknownCoeffs, matrix<rational<int>>(knowns, 1, 1));

#if _DEBUG
	cout << "unknownValues:" << endl << unknownValues << endl << endl;
#endif

	for (int i = 0; i < unknowns; i++)
		coeffs(i) = unknownValues(i, 0).numerator();

#if _DEBUG
	cout << "coeffs:" << endl << coeffs << endl << endl;
#endif

	chem_equation result = *this;

	for (int i = 0; i < result.left.size(); i++)
		get<1>(result.left[i]) = coeffs[i];

	for (int i = 0; i < result.right.size(); i++)
		get<1>(result.right[i]) = coeffs[result.left.size() + i];

	return result;
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