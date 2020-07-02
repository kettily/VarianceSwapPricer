#ifndef MONTECARLOVARIANCESWAPPRICER2D_H
#define MONTECARLOVARIANCESWAPPRICER2D_H

#ifndef PATHSIMULATOR2D_H
#include "PathSimulator2D.h"
#endif 

#ifndef PRICER_H
#include "Pricer.h"
#endif 


class MonteCarloVarianceSwapPricer2D : public Pricer
{
	public:
		MonteCarloVarianceSwapPricer2D(const PathSimulator2D& path_simulator, size_t number_of_simulations, double discount_rate);

		// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER
		MonteCarloVarianceSwapPricer2D(const MonteCarloVarianceSwapPricer2D& pricer);
		MonteCarloVarianceSwapPricer2D& operator=(const MonteCarloVarianceSwapPricer2D& pricer);

		~MonteCarloVarianceSwapPricer2D();

		double path_price(Vector* path) const; 
		Pair price() const override;											


	protected:
		const PathSimulator2D* _path_simulator;
		size_t _number_of_simulations;
};
#endif
