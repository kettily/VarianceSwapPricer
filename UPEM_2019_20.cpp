#include <iostream>

#ifndef PATHSIMULATOR2D_H
#include "PathSimulator2D.h"
#endif


#ifndef RANDOMNORMALGENERATOR_H
#include "RandomNormalGenerator.h"
#endif


#ifndef MONTECARLOVARIANCESWAPPRICER2D_H
#include "MonteCarloVarianceSwapPricer2D.h"
#endif

#ifndef CLOSEDFORMULAPRICER_H	
#include "ClosedFormulaPricer.h"
#endif




Vector create_discretization_time_points(size_t number_time_points, double maturity)
{
	Vector time_points;


	for (size_t time_index = 0; time_index < number_time_points; ++time_index)
	{
		double time_index_double = (double)time_index;
		double number_time_points_double = (double)number_time_points;
		time_points.push_back(time_index_double * maturity / (number_time_points_double - 1.));
	}

	return time_points;
}



SchemaTG create_pathsimulator_heston_TG(Vector time_points, Pair initial_factors, HestonModel model_Heston)
{
	// Defines the path simulator

	SchemaTG path_simulator_Heston_TG(initial_factors, time_points, model_Heston);
	return path_simulator_Heston_TG;
}

SchemaQE create_pathsimulator_heston_QE(Vector time_points, Pair initial_factors, HestonModel model_Heston)
{
	// Defines the path simulator

	SchemaQE path_simulator_Heston_QE(initial_factors, time_points, model_Heston);
	return path_simulator_Heston_QE;
}



void testing_MC_pricer_2D(Vector time_points,
	size_t number_of_simulations,
	double discount_rate,
	Pair initial_factors,
	HestonModel model_Heston)
{

	///////////////////////// Heston ///////////////////////////
	SchemaTG path_simulator_Heston_TG = create_pathsimulator_heston_TG(time_points, initial_factors, model_Heston);
	SchemaQE path_simulator_Heston_QE = create_pathsimulator_heston_QE(time_points, initial_factors, model_Heston);

	MonteCarloVarianceSwapPricer2D* pricer_Heston_var_TG = new MonteCarloVarianceSwapPricer2D(path_simulator_Heston_TG, number_of_simulations, discount_rate);
	MonteCarloVarianceSwapPricer2D* pricer_Heston_var_QE = new MonteCarloVarianceSwapPricer2D(path_simulator_Heston_QE, number_of_simulations, discount_rate);
	// Testing convergence ...
	size_t number_of_tests = 1;
	for (int test_index = 0; test_index < number_of_tests; ++test_index)
	{
		Pair pv_vol_TG = pricer_Heston_var_TG->price();
                Pair pv_vol_QE = pricer_Heston_var_QE->price();
                std::cout << "Var swap price with Heston model TG with 95% MC confidence interval for test number " << test_index << " is " << pv_vol_TG.first <<" +- "<<pv_vol_TG.second<< "\n";
                std::cout << "Var swap price with Heston model QE with 95% MC confidence interval for test number " << test_index << " is " << pv_vol_QE.first <<" +- "<<pv_vol_QE.second<< "\n";
                std::cout << "" << "\n";

	}

	std::cout << "\n";
}


void testing_CF_pricer(Vector time_points,
	double discount_rate,
	Pair initial_factors,
	HestonModel model_Heston)
{
	ClosedFormulaPricer* pricer_Heston_var = new ClosedFormulaPricer(initial_factors,
		time_points,
		model_Heston,
		discount_rate);
	// Testing convergence ...
	size_t number_of_tests = 1;
	for (int test_index = 0; test_index < number_of_tests; ++test_index)
	{
		Pair pv_vol = pricer_Heston_var->price();

                std::cout << "Var swap closed formula price with Heston model for test number " << test_index << " is " << pv_vol.first << "\n";
                std::cout << "" << "\n";
        }
        std::cout << "\n";


}

int main()
{

	size_t number_time_points = 1000;
	double maturity = 2.0; //1.0

	// Defines the discretization of time space
	Vector time_points = create_discretization_time_points(number_time_points, maturity);

	size_t number_of_simulations = 100000; //5k
	double discount_rate = 0.0; //0.03;


	// Defines initial value of asset
	Pair initial_factors(100., 0.04); // (A0, B0) initial factors

	double drift = 0.0;//0.06;
	double mean_reversion_speed = 0.5;
	double mean_reversion_level = 0.04;
	double vol_of_vol = 1.0;
	double correlation = 0.5;

	HestonModel model_Heston(correlation, drift, mean_reversion_speed, mean_reversion_level, vol_of_vol);

	
	testing_MC_pricer_2D(time_points,
						 number_of_simulations,
						 discount_rate, initial_factors, model_Heston);
	

	testing_CF_pricer(time_points,
		discount_rate,
		initial_factors,
		model_Heston);

	// Homework !

	std::cout << "Hello World!" << std::endl;
	system("pause");
	return 0;
}
