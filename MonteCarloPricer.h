#ifndef MONTECARLOPRICER_H
#define MONTECARLOPRICER_H

#ifndef PATHSIMULATOR_H
#include "PathSimulator.h"
#endif 


// abstract class because path_payoff() method depends on specific pricer
// Abstract class = at least one pure virtual method.
class MonteCarloPricer
{
public:
	MonteCarloPricer(const PathSimulator& path_simulator, size_t number_of_simulations, double discount_rate);

	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER
	MonteCarloPricer(const MonteCarloPricer& pricer);
	MonteCarloPricer& operator=(const MonteCarloPricer& pricer);

	virtual ~MonteCarloPricer(); // we need to have a virtual destructor : WHY? [interview question]
	
	virtual double path_price(const std::vector<double>& path) const = 0; // pure virtual method: needs to be implemented by all subclasses.
	double price() const;												 // method that all subclasses will inherit.


protected:
	const PathSimulator* _path_simulator; 
	size_t _number_of_simulations;
	double _discount_rate;												// Discounting rate r such as DF(0,T) = exp(-r * T).
};

// abstract as well
class MonteCarloOptionPricer : public MonteCarloPricer
{
public:
	MonteCarloOptionPricer(const PathSimulator& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);
	//virtual double path_price(const std::vector<double> path) const = 0;

protected:
	double _strike;
	bool _is_call;
};

class MonteCarloEuropeanOptionPricer : public MonteCarloOptionPricer
{
public:
	MonteCarloEuropeanOptionPricer(const PathSimulator& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);
	~MonteCarloEuropeanOptionPricer() = default;

	double path_price(const std::vector<double>& path) const override;
													
};

class MonteCarloAmericanOptionPricer : public MonteCarloOptionPricer
{
public:
	MonteCarloAmericanOptionPricer(const PathSimulator& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);

	double path_price(const std::vector<double>& path) const override;
};

class MonteCarloAsianEuropeanOptionPricer : public MonteCarloOptionPricer
{
public:
	MonteCarloAsianEuropeanOptionPricer(const PathSimulator& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);

	double path_price(const std::vector<double>& path) const override;
};

#endif // !MONTECARLOPRICER_H