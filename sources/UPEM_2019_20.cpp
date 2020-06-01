#include <iostream>

#ifndef PATHSIMULATOR2D_H
#include "PathSimulator2D.h"
#endif


#ifndef RANDOMNORMALGENERATOR_H
#include "RandomNormalGenerator.h"
#endif


#ifndef MONTECARLOPRICER2D_H
#include "MonteCarloPricer2D.h"
#endif


Vector create_discretization_time_points()
{
	Vector time_points;
	size_t number_time_points = 50;
	double maturity = 2.;

	for (size_t time_index = 0; time_index < number_time_points; ++time_index)
	{
		double time_index_double = (double)time_index;
		double number_time_points_double = (double)number_time_points;
		time_points.push_back(time_index_double * maturity / (number_time_points_double - 1.));
	}

	return time_points;
}



SchemaTG create_pathsimulator_heston_TG()
{
	// Defines initial value of asset
	Pair initial_factors(100., 0.2); // (A0, B0) initial factors

	double drift = 0.06;
	double mean_reversion_speed = 3.0;
	double mean_reversion_level = 0.20;
	double vol_of_vol = 0.2;
	double correlation = -0.5;

	HestonModel model_Heston(correlation, drift, mean_reversion_speed, mean_reversion_level, vol_of_vol);

	// Defines the discretization of time space
	Vector time_points = create_discretization_time_points();

	// Defines the path simulator

	SchemaTG path_simulator_Heston_TG(initial_factors, time_points, model_Heston);
	return path_simulator_Heston_TG;
}

SchemaQE create_pathsimulator_heston_QE()
{
	// Defines initial value of asset
	Pair initial_factors(100., 0.16); // (A0, B0) initial factors

	double drift = 0.06;
	double mean_reversion_speed = 3.0;
	double mean_reversion_level = 0.20;
	double vol_of_vol = 0.2;
	double correlation = -0.5;

	HestonModel model_Heston(correlation, drift, mean_reversion_speed, mean_reversion_level, vol_of_vol);

	// Defines the discretization of time space
	Vector time_points = create_discretization_time_points();

	// Defines the path simulator

	SchemaQE path_simulator_Heston_QE(initial_factors, time_points, model_Heston);
	return path_simulator_Heston_QE;
}



void testing_pricer_2D()
{
	// Defines the pricer
	size_t number_of_simulations = 20000;
	double discount_rate = 0.03;



	///////////////////////// Heston ///////////////////////////
	SchemaTG path_simulator_Heston_TG = create_pathsimulator_heston_TG();
	SchemaQE path_simulator_Heston_QE = create_pathsimulator_heston_QE();

	MonteCarloVarianceSwapPricer2D* pricer_Heston_var_TG = new MonteCarloVarianceSwapPricer2D(path_simulator_Heston_TG, number_of_simulations, discount_rate);
	MonteCarloVarianceSwapPricer2D* pricer_Heston_var_QE = new MonteCarloVarianceSwapPricer2D(path_simulator_Heston_QE, number_of_simulations, discount_rate);
	// Testing convergence ...
	size_t number_of_tests = 10;
	for (int test_index = 0; test_index < number_of_tests; ++test_index)
	{
		double pv_vol_TG = pricer_Heston_var_TG->price();
		double pv_vol_QE = pricer_Heston_var_QE->price();
		std::cout << "Var swap price with Heston model TG for test number " << test_index << " is " << pv_vol_TG << "\n";
		std::cout << "Var swap price with Heston model QE for test number " << test_index << " is " << pv_vol_QE << "\n";
		std::cout<<""<<"\n";
	}

	std::cout << "\n";


}




int main()
{
	// Testing of C++ concepts
	//testing_concepts();
	


	testing_pricer_2D();

	// Homework !
	//testing_pricer_template(); // You can test 1D and 2D cases using only MonteCarloPricerTemplate subclasses
	std::cout << "Hello World!" << std::endl;
	return 0;
}
