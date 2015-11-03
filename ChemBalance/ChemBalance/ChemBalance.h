#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <ostream>

struct chem_component
{
	//virtual std::map<std::string, int> elementCounts() = 0;
	virtual void print(std::ostream& stream) const = 0;
};

struct chem_componentCount : public chem_component
{
	std::shared_ptr<chem_component> component;
	int count;

	void print(std::ostream& stream) const;
};

struct chem_element : public chem_component
{
	std::string symbol;
	//std::map<std::string, int> elementCounts();

	void print(std::ostream& stream) const;
};

struct chem_molecule : public chem_component
{
	std::vector<chem_componentCount> components;
	//std::map<std::string, int> elementCounts();

	void print(std::ostream& stream) const;
};

struct chem_formula : public chem_component
{
	std::vector<std::tuple<chem_molecule, int>> molecules;
	//std::map<std::string, int> elementCounts();

	void print(std::ostream& stream) const;
};

struct chem_equation : public chem_component
{
	chem_formula left;
	chem_formula right;

	chem_equation(void);
	chem_equation(std::string equation);
	chem_equation(const chem_equation& original);

	//chem_equation balance(void);

	void print(std::ostream& stream) const;
};

std::ostream& operator<<(std::ostream& os, const chem_component& el);