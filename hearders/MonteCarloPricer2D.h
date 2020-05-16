#ifndef MONTECARLOPRICER2D_H
#define MONTECARLOPRICER2D_H

#ifndef PATHSIMULATOR2D_H
#include "PathSimulator2D.h"
#endif 

#ifndef PRICER_H
#include "Pricer.h"
#endif 


class MonteCarloPricer2D : public Pricer
{
public:
	MonteCarloPricer2D(const PathSimulator2D& path_simulator, size_t number_of_simulations, double discount_rate);

	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER
	MonteCarloPricer2D(const MonteCarloPricer2D& pricer);
	MonteCarloPricer2D& operator=(const MonteCarloPricer2D& pricer);

	virtual ~MonteCarloPricer2D(); // we need to have a virtual destructor : WHY? [interview question]

	virtual double path_price(const Vector& path) const = 0; // pure virtual method: needs to be implemented by all subclasses.
	double price() const override;												 // method that all subclasses will inherit.


protected:
	const PathSimulator2D* _path_simulator;
	size_t _number_of_simulations;
};

// abstract as well
class MonteCarloOptionPricer2D : public MonteCarloPricer2D
{
public:
	MonteCarloOptionPricer2D(const PathSimulator2D& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);
	double path_price(const Vector& path) const override;

	~MonteCarloOptionPricer2D() = default;

protected:
	double _strike;
	bool _is_call;
};



#endif