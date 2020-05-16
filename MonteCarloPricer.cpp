#include "MonteCarloPricer.h"

#include <algorithm> 

MonteCarloPricer::MonteCarloPricer(const PathSimulator & path_simulator, size_t number_of_simulations, double discount_rate)
	: _path_simulator(new PathSimulator(path_simulator)), _number_of_simulations(number_of_simulations), _discount_rate(discount_rate)
{
}

MonteCarloPricer::MonteCarloPricer(const MonteCarloPricer & pricer)
	:_path_simulator(new PathSimulator(*(pricer._path_simulator))), _number_of_simulations(pricer._number_of_simulations), _discount_rate(pricer._discount_rate)
{
}

MonteCarloPricer & MonteCarloPricer::operator=(const MonteCarloPricer & pricer)
{
	if (this == &pricer)
		return *this;
	else
	{
		delete _path_simulator;												// free the storage pointed to by _model
		_path_simulator = new PathSimulator(*(pricer._path_simulator));		// allocate new memory for the pointer

		// assignment for other fields
		_number_of_simulations = pricer._number_of_simulations;
		_discount_rate = pricer._discount_rate;
	}
	return *this;
}

MonteCarloPricer::~MonteCarloPricer()
{
	delete _path_simulator; // libere l'espace memoire = appel le destructeur de l'objet pointe par le pointeur
}

double MonteCarloPricer::price() const
{
	double price = 0.;

	for (size_t simulation_index = 0; simulation_index < _number_of_simulations; ++simulation_index)
	{
		std::vector<double> path = _path_simulator->path();
		price += path_price(path);
	}
	price /= _number_of_simulations;
	return price;
}


MonteCarloOptionPricer::MonteCarloOptionPricer(const PathSimulator & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call)
	: MonteCarloPricer(path_simulator, number_of_simulations, discount_rate), _strike(strike), _is_call(is_call)
{
}


MonteCarloEuropeanOptionPricer::MonteCarloEuropeanOptionPricer(const PathSimulator & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool isCall)
	:MonteCarloOptionPricer(path_simulator, number_of_simulations, discount_rate, strike, isCall)
{
}

double MonteCarloEuropeanOptionPricer::path_price(const std::vector<double>& path) const
{
	// payoff for this specific path scenario
	double spot_at_maturity = path.at(path.size() - 1);
	double path_payoff = std::max(_is_call ? spot_at_maturity - _strike : _strike - spot_at_maturity, 0.);

	// Discounted payoff = PV
	double maturity = _path_simulator->getTimePoints().at(_path_simulator->getTimePoints().size() - 1);
	double path_price = std::exp(-_discount_rate * maturity) * path_payoff;

	return path_price;
}

MonteCarloAmericanOptionPricer::MonteCarloAmericanOptionPricer(const PathSimulator & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool isCall)
	:MonteCarloOptionPricer(path_simulator, number_of_simulations, discount_rate, strike, isCall)
{
}

double MonteCarloAmericanOptionPricer::path_price(const std::vector<double>& path) const
{
	// payoff for this specific path scenario

	std::vector<double> payoffs;
	for (int time_index = 0; time_index < path.size(); ++time_index)
		payoffs.push_back(std::max(_is_call ? path[time_index] - _strike : _strike - path[time_index], 0.)); // path[time_index] corresponds to S_t

	// "payoff_max_iterator" is a POINTER to the maximum element within the vector "payoffs".
	std::vector<double>::const_iterator payoff_max_iterator = std::max_element<std::vector<double>::const_iterator>(payoffs.begin(), payoffs.end());
	
	// A pointer is an address, and the difference between the address of an element x in a vector and the address of the first element of the vector gives you the index of x.
	int payoff_max_iterator_index = payoff_max_iterator - payoffs.begin();

	// Discounted payoff = PV
	double maturity = _path_simulator->getTimePoints()[payoff_max_iterator_index];
	double path_price = std::exp(-_discount_rate * maturity) * *payoff_max_iterator; // *payoff_max_iterator refers to the value of the max element in "payoffs"
	
	return path_price;
}

MonteCarloAsianEuropeanOptionPricer::MonteCarloAsianEuropeanOptionPricer(const PathSimulator & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call)
	: MonteCarloOptionPricer(path_simulator, number_of_simulations, discount_rate, strike, is_call)
{
}

double MonteCarloAsianEuropeanOptionPricer::path_price(const std::vector<double>& path) const
{
	// compute the average using the Riemann sum : path contains <S0, S1, ..., SM>
	double average = 0.;
	// loop on each time point, compute delta_t, average += delta_t * (S_t + S_{t+delta_t})/2.0 [Trapezes method]

	size_t number_of_points = path.size(); // M points: path[0], ..., path[M-2], path[M-1];
	std::vector<double> time_points = _path_simulator->getTimePoints();
	
	// loop on index going from zero to M-2 ...
	for (size_t index = 0; index <= number_of_points - 2; ++index)
	{
		double mid_point_S_t = 0.5* (path[index] + path[index+1]); // Trapezes method
		double delta_t = time_points[index + 1] - time_points[index];
		average += mid_point_S_t * delta_t;
	}

	double maturity = time_points[number_of_points - 1];
	// A_T = 1/T * \int_0^T {S_t dt}
	average /= maturity;  

	// expression = boolean ? this : that;
	// Same as writing	
	//					if (boolean) :
	//						 expression = this 
	//					else :
	//						 expression = that
	double payoff = _is_call ? std::max(average - _strike, 0.) : std::max(_strike - average, 0.); 
	double pv = std::exp(-_discount_rate * maturity) * payoff;

	return pv;
}
