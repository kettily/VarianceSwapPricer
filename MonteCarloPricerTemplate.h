#ifndef MONTECARLOPRICERTEMPLATE_H
#define MONTECARLOPRICERTEMPLATE_H

#include <vector>
using Vector = std::vector<double>;

template<class PathSim> // PathSim can take any type it wants: "PathSim = PathSimulator" or PathSim = PathSimulator2D 
class MonteCarloPricerTemplate
{
	public:
	MonteCarloPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate);

	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER
	MonteCarloPricerTemplate(const MonteCarloPricerTemplate& pricer);
	MonteCarloPricerTemplate& operator=(const MonteCarloPricerTemplate& pricer);

	virtual ~MonteCarloPricerTemplate(); // we need to have a virtual destructor : WHY? [interview question]

	virtual double path_price(const Vector& path) const = 0; // pure virtual method: needs to be implemented by all subclasses.
	double price() const;												 // method that all subclasses will inherit.

protected:
	const PathSim* _path_simulator;
	size_t _number_of_simulations;
	double _discount_rate;
};

template<class PathSim>
class MonteCarloOptionPricerTemplate : public MonteCarloPricerTemplate<PathSim>
{
public:
	MonteCarloOptionPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);

protected:
	double _strike;
	bool _is_call;
};

template<class PathSim>
class MonteCarloEuropeanOptionPricerTemplate : public MonteCarloOptionPricerTemplate<PathSim>
{
public:
	MonteCarloEuropeanOptionPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);
	~MonteCarloEuropeanOptionPricerTemplate() = default;

	double path_price(const Vector& path) const override;
};

template<class PathSim>
class MonteCarloAmericanOptionPricerTemplate : public MonteCarloOptionPricerTemplate<PathSim>
{
public:
	MonteCarloAmericanOptionPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);

	double path_price(const Vector& path) const override;
};

template<class PathSim>
class MonteCarloAsianEuropeanPricerTemplate : public MonteCarloOptionPricerTemplate<PathSim>
{
public:
	MonteCarloAsianEuropeanPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call);

	double path_price(const Vector& path) const override;
};



#endif

