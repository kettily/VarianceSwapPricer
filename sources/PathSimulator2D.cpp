#include "PathSimulator2D.h"
#include "RandomNormalGenerator.h"
#include <algorithm>
#include <iostream>

# define M_PI     3.14159265358979323846  /* pi */
# define M_SQRT1_2  0.707106781186547524401 /*sqrt(1/2)*/

PathSimulator2D::PathSimulator2D(Pair initial_factors, 
                const Vector& time_points, 
                const HestonModel& model):
    _initial_factors(initial_factors), _time_points(time_points), _model(model)
{
}

PathSimulator2D::PathSimulator2D(const PathSimulator2D& path_simulator) :
    _initial_factors(path_simulator._initial_factors), _time_points(path_simulator._time_points),
    _model(path_simulator._model)
{}

// P2 = P1 equivalent to P2.operator=(P1)
PathSimulator2D& PathSimulator2D::operator=(const PathSimulator2D& path_simulator){
    // check for "self assignment" and do nothing in that case
	if (!(this == &path_simulator)){
										// free the storage pointed to by _model


		// assignment for other fields
		_initial_factors = path_simulator._initial_factors;
		_time_points = path_simulator._time_points;
    }
    return *this;								 // return this PathSimulator2D
}

PathSimulator2D::~PathSimulator2D(){

}

Vector PathSimulator2D::path() const
{
	Vector_Pair path2D{ _initial_factors };
	Vector path{ _initial_factors.first };

	for (size_t index = 0; index < _time_points.size() - 1; ++index)
	{
		path2D.push_back(nextStep(index, path2D[index]));
		path.push_back(path2D[index + 1].first);
	}

	return path;
}

Vector PathSimulator2D::getTimePoints() const
{
	return _time_points;
}


double cdf_inv (double u, double p, double beta){
	if (u<=p){
		return 0.;
	}
	else {
		return log((1-p)/(1-u))/beta;
	}
}

SchemaQE::SchemaQE(Pair initial_factors,  const Vector& time_points, const HestonModel& model):PathSimulator2D(initial_factors, time_points, model)
{
	}

Pair SchemaQE::nextStep(int current_index, Pair current_factors) const
{
    double cur_time = _time_points[current_index];
    double time_gap = _time_points[current_index + 1] - cur_time;

    Pair nextStep;

    double rho = _model.get_correlation();
    double vol_of_vol = _model.get_vol_of_vol();

    double z2= RandomNormalGenerator::normalRandom();
    double k = _model.get_mean_reversion_speed();
    double theta = _model.get_mean_reversion_level();

    double m = theta + (current_factors.second - theta) * std::exp(-k * time_gap);

    double s_2 = (current_factors.second * pow(vol_of_vol, 2)*std::exp(-k * time_gap))
            / k*(1 - std::exp(-k * time_gap))
            + (theta * pow(vol_of_vol, 2) / 2 * k) * pow(1 - std::exp(-k * time_gap), 2);



    double psi_critical = 1.5; // appartient à [1,2] valeur critique de Psi en-dessous de laquelle, V est en régime faible, V en régime fort sinon (faible : chi 2, fort:dirac+exponentielle)
    double psi = s_2/pow(m,2);

    if (psi<=psi_critical){

    	double b = sqrt(2/psi-1+sqrt(2/psi)*sqrt(2/psi-1));

    	double a = m/(1+pow(b,2));

    	nextStep.second=a*pow(b+z2,2);
    	double variance = time_gap * (0.5 * current_factors.second + 0.5 * nextStep.second);
        double z1 = sqrt(variance) * RandomNormalGenerator::normalRandom();

    	nextStep.first = current_factors.first + rho / vol_of_vol *
    	        (nextStep.second - current_factors.second -
    	            k * theta * time_gap)
    	        + (k * rho / vol_of_vol - 0.5) * variance + sqrt(1 - pow(rho, 2)) * z1;
    }

    else{

    	double beta =2/(m*(psi+1)) ;
    	double p = (psi-1)/(psi+1);
    	double u = RandomNormalGenerator::uniformRandom();

    	nextStep.second=cdf_inv(u,p,beta);
    	double variance = time_gap * (0.5 * current_factors.second + 0.5 * nextStep.second);
    	double z1 = sqrt(variance) * RandomNormalGenerator::normalRandom();

    	nextStep.first = current_factors.first + rho / vol_of_vol *
    	    	        (nextStep.second - current_factors.second -
    	    	            k * theta * time_gap)
    	    	        + (k * rho / vol_of_vol - 0.5) * variance + sqrt(1 - pow(rho, 2)) * z1;


    }




    return nextStep;
}


SchemaQE* SchemaQE::clone() const
{
    return  new SchemaQE(*this); //Il est entrain de cloner: il se clone lui meme donc l'objet qu'il est entrain de copier c'est lui meme donc si c'est lui mneme c'est la reference sur lui meme donc *this
}

//double Norm( double x){ if( x >= 0 ){ static const double p = 0.2316419 ; static const double b1 = 0.319381530 ; static const double b2 = -0.356563782 ; static const double b3 = 1.781477937 ; static const double b4 = -1.821255978 ; static const double b5 = 1.330274429 ; static const double two_pi = 2.0*3.14159265358979323846; float t = 1.0 / ( 1.0 + p*x ) ; float temp1 = 1.0 / ( sqrt( two_pi ) ) ; float temp2 = exp(-x*x*0.5 ) ; float temp3 = temp1 * temp2 * ( b1 * pow( t , 1.0 ) + b2*pow( t , 2.0 ) + b3*pow( t , 3.0 ) + b4*pow( t , 4.0 ) + b5*pow( t , 5.0 ) ) ; float temp4 = 1.0 - temp3 ; return temp4 ; } else{ return( 1.0 - Norm(-x) ) ; } }

double g_psi(double r, double psi) {
    double phi_r = (1 / sqrt(2 * M_PI)) * exp(-pow(r, 2) / 2);
    double Phi_r = 0.5 * erfc(-r * M_SQRT1_2);

    return r * phi_r + Phi_r * (1 + pow(r, 2)) - (1 + psi) * pow(phi_r + r * Phi_r, 2);
}

double g_psi_d(double r, double psi) {
    double phi_r = (1 / sqrt(2 * M_PI)) * exp(-pow(r, 2) / 2);
    double Phi_r = 0.5 * erfc(-r * M_SQRT1_2);
    return 2 * phi_r + 2 * r * Phi_r - 2 * (1 + psi) * Phi_r * (phi_r + r * Phi_r);
}

double SchemaTG::rootSearch(double psi, double r_0, double eps, int maxIter) {
    int numIter = 0;
    double r_1, r_2;
    r_1 = r_0;
    do {
        numIter++;
        r_2 = r_1 - (g_psi(r_1, psi) / g_psi_d(r_1, psi));
        r_1 = r_2;

    } while (abs(g_psi(r_1, psi) )> eps && numIter < maxIter);
    //std::cout<<numIter<<endl;
    return r_1;
}

SchemaTG::SchemaTG(Pair initial_factors, const Vector& time_points, const HestonModel& model) 
    :PathSimulator2D(initial_factors, time_points, model) {}

// Psi has to belong to the range ]0, 2]


Pair SchemaTG::nextStep(int current_index, Pair current_factors) const
{
    double cur_time = _time_points[current_index];
    double time_gap = _time_points[current_index + 1] - cur_time;

    Pair nextStep;
    double rho = _model.get_correlation();
    double vol_of_vol = _model.get_vol_of_vol();

    double z2 = RandomNormalGenerator::normalRandom();
    // calcul nextStep Second = partie posititve de (mu+sigma*N(0,1))
    double k = _model.get_mean_reversion_speed();
    double theta = _model.get_mean_reversion_level();
    double m = theta + (current_factors.second - theta) * std::exp(-k * time_gap);

    double s_2 = (current_factors.second * pow(vol_of_vol, 2)*std::exp(-k * time_gap))
        / k*(1 - std::exp(-k * time_gap))
        + (theta * pow(vol_of_vol, 2) / 2 * k) * pow(1 - std::exp(-k * time_gap), 2);
    double psi = s_2 / pow(m, 2);

    //std::cout<<"psi="<<psi<<endl;


    // root search
    double r_0 = 0.5; // find the best r_0, eps, maxIter !!!
    double eps = 10e-10;
    int maxIter = (int) pow(10, 2);
    double r_psi = SchemaTG::rootSearch(psi, r_0, eps, maxIter);
    //std::cout<<"r_psi"<<r_psi<<endl;


    // calculating sigma and mu 
    double phi_r = (1 / sqrt(2 * M_PI)) * exp(-pow(r_psi, 2) / 2);
    double Phi_r = 0.5 * erfc(-r_psi * sqrt(0.5));
    double mu = m / (Phi_r + phi_r / r_psi);
    double sigma = mu / r_psi;
    nextStep.second = std::max(mu + sigma * z2, (double)0);

    double variance = time_gap * (0.5 * current_factors.second + 0.5 * nextStep.second);
    double z1 = sqrt(variance) * RandomNormalGenerator::normalRandom();// normal ransom variable (m =0 ; var = time_gap*(0.5*current_factors.second+0.5.nextStep.second)
    // nextStep.first refers to ln(X) 
    nextStep.first = current_factors.first + rho / vol_of_vol *
        (nextStep.second - current_factors.second -
            k * theta * time_gap)
        + (k * rho / vol_of_vol - 0.5) * variance + sqrt(1 - pow(rho, 2)) * z1;

    return nextStep;
}
/*
Pair SchemaTG::nextStep(int current_index, Pair current_factors) const
{
    double cur_time = _time_points[current_index];
    double time_gap = _time_points[current_index + 1] - cur_time;

    Pair nextStep;

    //We have two independent normal random variables N(0,1)
    double n1 = RandomNormalGenerator::normalRandom();
    double n2 = RandomNormalGenerator::normalRandom();

    double z1 = n1;
    double rho = _model->get_correlation();
    double z2 = rho * n1 + std::sqrt(1 - rho * rho) * n2;

    nextStep.first = current_factors.first + _model->drift_term(cur_time, current_factors).first * time_gap
        + _model->volatility_term(cur_time, current_factors).first * std::sqrt(time_gap) * z1;
    nextStep.second = current_factors.second + _model->drift_term(cur_time, current_factors).second * time_gap
        + _model->volatility_term(cur_time, current_factors).second * sqrt(time_gap) * z2;


    return nextStep;
}*/

SchemaTG* SchemaTG::clone() const
{
    return  new SchemaTG(*this); //Il est entrain de cloner: il se clone lui meme donc l'objet qu'il est entrain de copier c'est lui meme donc si c'est lui mneme c'est la reference sur lui meme donc *this
}



