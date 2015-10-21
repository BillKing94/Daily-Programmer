#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <set>
#include <ostream>

typedef std::tuple<std::string, int> t_element;
typedef std::vector<t_element> t_molecule;
typedef std::tuple<t_molecule, int> t_moleculeCount;
typedef std::vector<t_moleculeCount> t_formula;


struct chem_equation
{
	t_formula left;
	t_formula right;

	chem_equation(void);
	chem_equation(std::string equation);
	chem_equation(const chem_equation& original);

	chem_equation balance(void);

	friend std::ostream& operator<<(std::ostream& os, const chem_equation& eq);
};

std::ostream& operator<<(std::ostream& os, const chem_equation& eq);
std::ostream& operator<<(std::ostream& os, const t_element& el);
std::ostream& operator<<(std::ostream& os, const t_molecule& mol);
std::ostream& operator<<(std::ostream& os, const t_moleculeCount& mc);
std::ostream& operator<<(std::ostream& os, const t_formula& f);