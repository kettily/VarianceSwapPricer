#include "MonteCarloPricer2D.h"

#include <algorithm> 

MonteCarloPricer2D::MonteCarloPricer2D(const PathSimulator2D & path_simulator, size_t number_of_simulations, double discount_rate)
	:Pricer(discount_rate), _path_simulator(path_simulator.clone()), _number_of_simulations(number_of_simulations)
{
}

MonteCarloPricer2D::MonteCarloPricer2D(const MonteCarloPricer2D & pricer)
	: Pricer(pricer), _path_simulator( (*(pricer._path_simulator)).clone()), _number_of_simulations(pricer._number_of_simulations)
{
	
}

MonteCarloPricer2D & MonteCarloPricer2D::operator=(const MonteCarloPricer2D & pricer)
{
	if (this == &pricer)
		return *this;
	else
	{
		delete _path_simulator;												// free the storage pointed to by _model
		_path_simulator = (*(pricer._path_simulator)).clone();		// allocate new memory for the pointer

		// assignment for other fields
		_number_of_simulations = pricer._number_of_simulations;
		_discount_rate = pricer._discount_rate;
	}
	return *this;
}

MonteCarloPricer2D::~MonteCarloPricer2D()
{
	delete _path_simulator; 
}

double MonteCarloPricer2D::price() const
{
	double price = 0.;

	for (size_t simulation_index = 0; simulation_index < _number_of_simulations; ++simulation_index)
	{
		Vector path = _path_simulator->path();
		price += path_price(path);
	}
	price /= _number_of_simulations;
	return price;
}


MonteCarloOptionPricer2D::MonteCarloOptionPricer2D(const PathSimulator2D & path_simulator, size_t number_of_simulations, double discount_rate, double strike, bool is_call)
	: MonteCarloPricer2D(path_simulator, number_of_simulations, discount_rate), _strike(strike), _is_call(is_call)
{
}


double MonteCarloOptionPricer2D::path_price(const Vector& path) const
{
	// payoff for this specific path scenario
	double spot_at_maturity = path.at(path.size() - 1);
	double path_payoff = std::max(_is_call ? spot_at_maturity - _strike : _strike - spot_at_maturity, 0.);

	// Discounted payoff = PV
	double maturity = _path_simulator->getTimePoints().at(_path_simulator->getTimePoints().size() - 1);
	double path_price = std::exp(-_discount_rate * maturity) * path_payoff;

	return path_price;
}


MonteCarloVarianceSwapPricer2D::MonteCarloVarianceSwapPricer2D(const PathSimulator2D& path_simulator, size_t number_of_simulations, double discount_rate)
	:MonteCarloPricer2D(path_simulator, number_of_simulations, discount_rate)
{
	}




double MonteCarloVarianceSwapPricer2D::path_price(const Vector& path) const
{
	int nb_time_points = _path_simulator->getTimePoints().size();
	double maturity = _path_simulator->getTimePoints().at(nb_time_points - 1);
	double sigma_2_0_T=0;

	double ln_X = 0;
	double ln_X_0 = 0;
	double t = 0;
	double t_0 = 0;
	for (int i = 0; i < nb_time_points-1; i++) {
		ln_X = path.at(i + 1);
		ln_X_0 = path.at(i);
		t = _path_simulator->getTimePoints().at(i + 1);
		t_0 = _path_simulator->getTimePoints().at(i);
		sigma_2_0_T += pow(log(exp(ln_X - _discount_rate * t) / exp(ln_X_0 - _discount_rate * t_0)), 2);
	}
	sigma_2_0_T = sigma_2_0_T / maturity;
	

	return sigma_2_0_T;
}









