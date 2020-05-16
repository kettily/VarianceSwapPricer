#include "MonteCarloPricerTemplate.h"

#include <algorithm> 

template<class PathSim>
MonteCarloPricerTemplate<PathSim>::MonteCarloPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate)
	:_path_simulator(new PathSim(path_simulator)), _number_of_simulations(number_of_simulations), _discount_rate(discount_rate)
{
}

template<class PathSim>
MonteCarloPricerTemplate<PathSim>::MonteCarloPricerTemplate(const MonteCarloPricerTemplate& pricer)
	: _path_simulator(new PathSim(*(pricer._path_simulator))), _number_of_simulations(pricer._number_of_simulations), _discount_rate(pricer._discount_rate)
{
}

template<class PathSim>
MonteCarloPricerTemplate<PathSim>& MonteCarloPricerTemplate<PathSim>::operator=(const MonteCarloPricerTemplate<PathSim> & pricer)
{
	if (this == &pricer)
		return *this;
	else
	{
		delete _path_simulator;												// free the storage pointed to by _model
		_path_simulator = new PathSim(*(pricer._path_simulator));			// allocate new memory for the pointer

		// assignment for other fields
		_number_of_simulations = pricer._number_of_simulations;
		_discount_rate = pricer._discount_rate;
	}
	return *this;
}

template<class PathSim>
MonteCarloPricerTemplate<PathSim>::~MonteCarloPricerTemplate()
{
	delete _path_simulator;
}

template<class PathSim>
double MonteCarloPricerTemplate<PathSim>::price() const
{
	double price = 0.;

	for (size_t simulation_index = 0; simulation_index < _number_of_simulations; ++simulation_index)
	{
		Vector path = _path_simulator->path(); // PathSim generic type needs to have a path() method;
		price += path_price(path);
	}
	price /= _number_of_simulations;
	return price;
}

template<class PathSim>
MonteCarloOptionPricerTemplate<PathSim>::MonteCarloOptionPricerTemplate(const PathSim & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call)
	: MonteCarloPricerTemplate<PathSim>(path_simulator, number_of_simulations, discount_rate), _strike(strike), _is_call(is_call)
{
}

template<class PathSim>
MonteCarloEuropeanOptionPricerTemplate<PathSim>::MonteCarloEuropeanOptionPricerTemplate(const PathSim & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool isCall)
	: MonteCarloOptionPricerTemplate<PathSim>(path_simulator, number_of_simulations, discount_rate, strike, isCall)
{
}

template<class PathSim>
double MonteCarloEuropeanOptionPricerTemplate<PathSim>::path_price(const Vector& path) const
{
	// payoff for this specific path scenario
	double spot_at_maturity = path.at(path.size() - 1);
	double path_payoff = std::max(this->_is_call ? spot_at_maturity - this->_strike : this->_strike - spot_at_maturity, 0.);

	// Discounted payoff = PV
	double maturity = this->_path_simulator->getTimePoints().at(this->_path_simulator->getTimePoints().size() - 1);
	double path_price = std::exp(-this->_discount_rate * maturity) * path_payoff;

	return path_price;
}

template<class PathSim>
MonteCarloAmericanOptionPricerTemplate<PathSim>::MonteCarloAmericanOptionPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool isCall)
	:MonteCarloOptionPricerTemplate<PathSim>(path_simulator, number_of_simulations, discount_rate, strike, isCall)
{
}

template<class PathSim>
double MonteCarloAmericanOptionPricerTemplate<PathSim>::path_price(const Vector& path) const
{
	// payoff for this specific path scenario

	Vector payoffs;
	for (int time_index = 0; time_index < path.size(); ++time_index)
		payoffs.push_back(std::max(this->_is_call ? path[time_index] - this->_strike : this->_strike - path[time_index], 0.)); // path[time_index] corresponds to S_t

	// "payoff_max_iterator" is a POINTER to the maximum element within the vector "payoffs".
	Vector::const_iterator payoff_max_iterator = std::max_element<Vector::const_iterator>(payoffs.begin(), payoffs.end());

	// A pointer is an address, and the difference between the address of an element x in a vector and the address of the first element of the vector gives you the index of x.
	int payoff_max_iterator_index = payoff_max_iterator - payoffs.begin();

	// Discounted payoff = PV
	double maturity = this->_path_simulator->getTimePoints()[payoff_max_iterator_index];
	double path_price = std::exp(-this->_discount_rate * maturity) * *payoff_max_iterator; // *payoff_max_iterator refers to the value of the max element in "payoffs"

	return path_price;
}

template<class PathSim>
MonteCarloAsianEuropeanPricerTemplate<PathSim>::MonteCarloAsianEuropeanPricerTemplate(const PathSim& path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call)
	: MonteCarloOptionPricerTemplate(path_simulator, number_of_simulations, discount_rate, strike, is_call)
{
}

template<class PathSim>
double MonteCarloAsianEuropeanPricerTemplate<PathSim>::path_price(const Vector& path) const
{
	// compute the average using the Riemann sum : path contains <S0, S1, ..., SM>
	double average = 0.;
	// loop on each time point, compute delta_t, average += delta_t * S_t;

	// How many points M?
	size_t number_of_points = path.size(); // M points: path[0], ..., path[M-2], path[M-1];
	Vector time_points = this->_path_simulator->getTimePoints();
	for (size_t index = 0; index <= number_of_points - 2; ++index)
	{
		double mid_point_S_t = 0.5* (path[index] + path[index + 1]); // Trapezes method
		double delta_t = time_points[index + 1] - time_points[index];
		average += mid_point_S_t * delta_t;
	}

	double maturity = time_points[number_of_points - 1];
	average /= maturity;

	// expression = boolean ? this : that;
	// Same as writing	
	//					if (boolean) :
	//						 expression = this 
	//					else :
	//						 expression = that

	double payoff = this->_is_call ? std::max(average - this->_strike, 0.) : std::max(this->_strike - average, 0.);
	double maturity = time_points[number_of_points - 1];
	double pv = std::exp(-this->_discount_rate * maturity) * payoff;

	return pv;
}
