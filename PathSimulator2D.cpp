#include "PathSimulator2D.h"
#include "RandomNormalGenerator.h"
#include <algorithm>
#include <iostream>

# define M_PI     3.14159265358979323846  /* pi */
# define M_SQRT1_2  0.707106781186547524401 /*sqrt(1/2)*/

PathSimulator2D::PathSimulator2D(Pair initial_factors,
    const Vector& time_points,
    const HestonModel& model) :
    _initial_factors(initial_factors), _time_points(time_points), _model(model)
{
}

PathSimulator2D::PathSimulator2D(const PathSimulator2D& path_simulator) :
    _initial_factors(path_simulator._initial_factors), _time_points(path_simulator._time_points),
    _model(path_simulator._model)
{
}


PathSimulator2D& PathSimulator2D::operator=(const PathSimulator2D& path_simulator) {
    // check for "self assignment" and do nothing in that case
    if (!(this == &path_simulator)) {
        // free the storage pointed to by _model
	
	// assignment for other fields
        _initial_factors = path_simulator._initial_factors;
        _time_points = path_simulator._time_points;
    }
    return *this;					
}

PathSimulator2D::~PathSimulator2D() {

}

/*
 * generator of random trajectories
 */
void PathSimulator2D::path(Vector* path) const
{

    int size = _time_points.size();
    path->at(0) = _initial_factors.first;

    Pair nextStep_i = _initial_factors;

    for (size_t index = 0; index < size - 1; ++index)
    {
        nextStep_i = nextStep(index, nextStep_i);
        path->at(index + 1) = nextStep_i.first;

    }

}


Vector PathSimulator2D::getTimePoints() const
{
    return _time_points;
}

/*
 * inverse of cumulative distribution of the volatility process
 */
double cdf_inv(double u, double p, double beta) {
    if (u <= p) {
        return 0.;
    }
    else {
        return log((1 - p) / (1 - u)) / beta;
    }
}


SchemaQE::SchemaQE(Pair initial_factors, const Vector& time_points, const HestonModel& model) :PathSimulator2D(initial_factors, time_points, model)
{
}

/*
 * Quadratic-exponential discretization scheme
 */
Pair SchemaQE::nextStep(int current_index, Pair current_factors) const
{
    double time_gap = _time_points[current_index + 1] - _time_points[current_index];

    Pair nextStep;

    double rho = _model.get_correlation();
    double vol_of_vol = _model.get_vol_of_vol();

    double z2 = RandomNormalGenerator::normalRandom();
    double k = _model.get_mean_reversion_speed();
    double theta = _model.get_mean_reversion_level();

    double m = theta + (current_factors.second - theta) * std::exp(-k * time_gap);

    double s_2 = (current_factors.second * pow(vol_of_vol, 2) * std::exp(-k * time_gap))
        / k * (1 - std::exp(-k * time_gap))
        + (theta * pow(vol_of_vol, 2) / 2 * k) * pow(1 - std::exp(-k * time_gap), 2);



    double psi_critical = 1.5; // belongs to  [1,2] critical value of Psi under which  V is in a weak regime, else  V follows a strong regime (weak : chi 2, strong : dirac + exponential)
    double psi = s_2 / pow(m, 2);

    if (psi <= psi_critical) {

        double b = sqrt(2 / psi - 1 + sqrt(2 / psi) * sqrt(2 / psi - 1));

        double a = m / (1 + pow(b, 2));

        nextStep.second = a * pow(b + z2, 2);
        double variance = time_gap * (0.5 * current_factors.second + 0.5 * nextStep.second);
        double z1 = sqrt(variance) * RandomNormalGenerator::normalRandom();

        nextStep.first = current_factors.first + rho / vol_of_vol *
            (nextStep.second - current_factors.second -
                k * theta * time_gap)
            + (k * rho / vol_of_vol - 0.5) * variance + sqrt(1 - pow(rho, 2)) * z1;
    }

    else {

        double beta = 2 / (m * (psi + 1));
        double p = (psi - 1) / (psi + 1);
        double u = RandomNormalGenerator::uniformRandom();
 

        nextStep.second = cdf_inv(u, p, beta);
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
    return  new SchemaQE(*this);
}



/*
 * Newton algorithm to find a zero (simple gradient descent)
 */
double SchemaTG::rootSearch(double psi, double r_0, double eps, int maxIter) {
    int numIter = 0;
    double r_1, r_2, g_psi, g_psi_d, r_1_square;
    r_1 = r_0;
    do {
        numIter++;
        r_1_square = r_1 * r_1;
        double phi_r = (1 / sqrt(2 * M_PI)) * exp(-r_1_square / 2);
        double Phi_r = 0.5 * erfc(-r_1 * M_SQRT1_2);
        // Computation of g_psi
        g_psi = r_1 * phi_r + Phi_r * (1 + r_1_square) - (1 + psi) * pow(phi_r + r_1 * Phi_r, 2);
        // Computation of g_psi_d (derivative of g_psi)
        g_psi_d = 2 * phi_r + 2 * r_1 * Phi_r - 2 * (1 + psi) * Phi_r * (phi_r + r_1 * Phi_r);
        r_2 = r_1 - g_psi / g_psi_d;
        r_1 = r_2;

    } while (abs(g_psi) > eps && numIter < maxIter);
    
    return r_1;
}

SchemaTG::SchemaTG(Pair initial_factors, const Vector& time_points, const HestonModel& model)
    :PathSimulator2D(initial_factors, time_points, model) 
{
    
}

// Psi has to belong to the range ]0, 2]

/*
 * Truncated Gaussian discretzation scheme
 */
Pair SchemaTG::nextStep(int current_index, Pair current_factors) const
{
    double time_gap = _time_points[current_index + 1] - _time_points[current_index];

    Pair nextStep;
    double rho = _model.get_correlation();
    double vol_of_vol = _model.get_vol_of_vol();

    double z2 = RandomNormalGenerator::normalRandom();
    // computation of  nextStep Second = positive part of (mu+sigma*N(0,1))
    double k = _model.get_mean_reversion_speed();
    double theta = _model.get_mean_reversion_level();
    double m = theta + (current_factors.second - theta) * std::exp(-k * time_gap);

    double s_2 = (current_factors.second * pow(vol_of_vol, 2) * std::exp(-k * time_gap))
        / k * (1 - std::exp(-k * time_gap))
        + (theta * pow(vol_of_vol, 2) / 2 * k) * pow(1 - std::exp(-k * time_gap), 2);
    double psi = s_2 / pow(m, 2);

 
    // root search
    double r_0 = 10.0; // be careful to find the best r_0, eps, maxIter !!!
    double eps = 10e-15;
    int maxIter = 100;//(int)pow(10, 2);
    double r_psi = SchemaTG::rootSearch(psi, r_0, eps, maxIter);
 


    // calculating sigma and mu 
    double phi_r = (1 / sqrt(2 * M_PI)) * exp(-pow(r_psi, 2) / 2);
    double Phi_r = 0.5 * erfc(-r_psi * sqrt(0.5));
    double mu = m / (Phi_r + phi_r / r_psi);
    double sigma = mu / r_psi;
    nextStep.second = std::max(mu + sigma * z2, (double)0);

    double variance = time_gap * (0.5 * current_factors.second + 0.5 * nextStep.second);
    double z1 = sqrt(variance) * RandomNormalGenerator::normalRandom();// normal random variable (m =0 ; var = time_gap*(0.5*current_factors.second+0.5.nextStep.second)
    // nextStep.first refers to ln(X) 
    nextStep.first = current_factors.first + rho / vol_of_vol *
        (nextStep.second - current_factors.second -
            k * theta * time_gap)
        + (k * rho / vol_of_vol - 0.5) * variance + sqrt(1 - pow(rho, 2)) * z1;

    return nextStep;
}


SchemaTG* SchemaTG::clone() const
{
    return  new SchemaTG(*this); 
}
