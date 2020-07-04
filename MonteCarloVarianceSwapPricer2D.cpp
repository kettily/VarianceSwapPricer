#include "MonteCarloVarianceSwapPricer2D.h"
#include <algorithm> 


MonteCarloVarianceSwapPricer2D::MonteCarloVarianceSwapPricer2D(const PathSimulator2D & path_simulator, size_t number_of_simulations, double discount_rate)
	:Pricer(discount_rate), _path_simulator(path_simulator.clone()), _number_of_simulations(number_of_simulations)
{
}

MonteCarloVarianceSwapPricer2D::MonteCarloVarianceSwapPricer2D(const MonteCarloVarianceSwapPricer2D & pricer)
	: Pricer(pricer), _path_simulator( (*(pricer._path_simulator)).clone()), _number_of_simulations(pricer._number_of_simulations)
{
	
}

MonteCarloVarianceSwapPricer2D & MonteCarloVarianceSwapPricer2D::operator=(const MonteCarloVarianceSwapPricer2D & pricer)
{
	if (this == &pricer)
		return *this;
	else
	{
		delete _path_simulator;						// free the storage pointed to by _model
		_path_simulator = (*(pricer._path_simulator)).clone();		// allocate new memory for the pointer

		// assignment for other fields
		_number_of_simulations = pricer._number_of_simulations;
		_discount_rate = pricer._discount_rate;
	}
	return *this;
}

MonteCarloVarianceSwapPricer2D::~MonteCarloVarianceSwapPricer2D()
{
	delete _path_simulator; 
}

/*
 * returns a pair (price, 95% MC confidence interval) 
 */
Pair MonteCarloVarianceSwapPricer2D::price() const
{
	double sum_price = 0.;
        double sum_price_2 = 0.;
        double price = 0.;
        Pair priceConfMC(0.,0.);
        int size = _path_simulator->getTimePoints().size();
        Vector* path = new Vector(size);

        for (size_t simulation_index = 0; simulation_index < _number_of_simulations; ++simulation_index)
        {
                _path_simulator->path(path);
                price = path_price(path);
                sum_price+=price;
                sum_price_2+=price*price;
                
        }
        delete path;
        priceConfMC.first = sum_price / _number_of_simulations;
        priceConfMC.second =1.96/ sqrt(_number_of_simulations-1) *sqrt(sum_price_2/_number_of_simulations -pow(priceConfMC.first,2));
        return priceConfMC;

}


/*
 * returns a double that contains the payoff of underlyings trajectory
 */
double MonteCarloVarianceSwapPricer2D::path_price(Vector* path) const
{
	Vector time_points = _path_simulator->getTimePoints();
	int nb_time_points = time_points.size();
	double maturity = time_points.at(nb_time_points - 1);
	double sigma_2_0_T = 0.0;

	double ln_X_next = path->at(0);
	double ln_X = 0.0;
	double t_next = time_points.at(0);
	double t = 0.0;
	for (int i = 0; i < nb_time_points-1; i++) {
		ln_X = ln_X_next;
		ln_X_next= path->at(i+1);
		t = t_next;
		t_next = time_points.at(i + 1);
		//sigma_2_0_T += pow(log(exp(ln_X_next - _discount_rate * t_next) / exp(ln_X - _discount_rate * t)), 2);
		sigma_2_0_T += pow(log(exp(ln_X_next ) / exp(ln_X)), 2);
	}
	return exp(-_discount_rate*time_points[nb_time_points - 1])*sigma_2_0_T/maturity ;
}









