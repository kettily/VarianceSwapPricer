#include "ClosedFormulaPricer.h"
#include <iostream>


ClosedFormulaPricer::ClosedFormulaPricer(Pair initial_factors, const Vector& time_points, const HestonModel& modeldouble, double discount_rate) : Pricer(discount_rate), _initial_factors(initial_factors), _time_points(time_points), _model(modeldouble)
{

}


ClosedFormulaPricer& ClosedFormulaPricer::operator=(const ClosedFormulaPricer& pricer)
{
	_discount_rate = pricer._discount_rate;

	return *this;
}

ClosedFormulaPricer::~ClosedFormulaPricer()
{
}

/*
 * returns a pair (price, zero because it's a derministic method so no confidence interval)
 */
Pair ClosedFormulaPricer::price() const
{

	int N = _time_points.size();
        double annualizedVariance = 0.0;
        double maturity = _time_points[N - 1];
        Pair price(0.,0.);
        for (int i = 1; i <= N; i++) {
                dcomp logReturnSquared_ti = function_logReturnSquared_ti(i);
                annualizedVariance = annualizedVariance + logReturnSquared_ti.real();
        }
        price.first = annualizedVariance / maturity * exp(-_discount_rate * _time_points[N-1]);

        return price ;

}

/*
 * calculate log return squared with combinaison of first and second derivatives of C and D
 */
dcomp ClosedFormulaPricer::function_logReturnSquared_ti(int i) const
{
	
	double eps0 = 0.0;
	double eps = pow(10, -4);
	double dt = _time_points[1] - _time_points[0];

	dcomp fd_D = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_D, eps0, 1, eps, dt);
	dcomp sd_D = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_D, eps0, 2, eps, dt);
	dcomp fd_C = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_C, eps0, 1, eps, dt);
	dcomp sd_C = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_C, eps0, 2, eps, dt);


	double kappa = _model.get_mean_reversion_speed();
	double sigma = _model.get_vol_of_vol();
	double theta = _model.get_mean_reversion_level();

	double v_0 = _initial_factors.second;

	if (i == 1) {

		return std::pow(fd_D * v_0, 2)
			+ (dcomp(2, 0) * fd_C * fd_D - sd_D) * v_0
			+ std::pow(fd_C, 2)
			- sd_C;
	}
	else {

		double c_i = 2 * kappa / (std::pow(sigma, 2) * (1 - exp(-kappa * _time_points[i - 1])));

		double W_i = c_i * v_0 * exp(-kappa * _time_points[i - 1]);

		double q_tilda = 2 * kappa * theta / std::pow(sigma, 2);

		return std::pow(fd_D, 2) * ((q_tilda + 2 * W_i) + std::pow(q_tilda + W_i, 2)) / std::pow(c_i, 2)
			+ (dcomp(2, 0) * fd_C * fd_D - sd_D) * (q_tilda + W_i) / c_i
			+ (std::pow(fd_C, 2) - sd_C);
	}
}


/*
 * calculate D(w, dt) modified to be continuous in zero
 */
dcomp ClosedFormulaPricer::zeroDerivative_D(double w, double dt) const
{
	
	dcomp a = _model.get_mean_reversion_speed()
		- _model.get_correlation() * _model.get_vol_of_vol() * w * dcomp(0, 1);
	dcomp delta = pow(a, 2) + pow(_model.get_vol_of_vol(), 2) * (pow(w, 2) + w * dcomp(0, 1));
	dcomp b = sqrt(delta); 
	dcomp g = (a - b) / (a + b);
	dcomp val = (a - b) * (1.0 - exp(-b * dt)) / (pow(_model.get_vol_of_vol(), 2) * (1.0 - g * exp(-b * dt))) ;

	return val;
}


/*
 * calculate C(w, dt) modified to be continuous in zero
 */
dcomp ClosedFormulaPricer::zeroDerivative_C(double omega, double dt) const
{
	
	dcomp a = _model.get_mean_reversion_speed()
		- _model.get_correlation() * _model.get_vol_of_vol() * omega * dcomp(0, 1);
	dcomp delta = pow(a, 2) + pow(_model.get_vol_of_vol(), 2) * (pow(omega, 2) + omega * dcomp(0, 1));
	dcomp b = sqrt(delta); 
	dcomp g = (a - b) / (a + b); 
	dcomp val = (_model.get_drift() * omega * dcomp(0, 1) - _discount_rate) * dt 
		+ _model.get_mean_reversion_speed() * _model.get_mean_reversion_level() * ((a - b) * dt - 2.0 * log((1.0 - g * exp(-b * dt)) / (1.0 - g))) / pow(_model.get_vol_of_vol(), 2);
	
	return val;
}

/*
 * Implementation of numerical derivation based on Finite differences for order 1 and 2
 */
dcomp ClosedFormulaPricer::derivativeCalculator_0(MagicType func, double omega0, int derivationOrder, double dw, double dt) const
{

	Vector weights(3);

	if (derivationOrder == 1) {
		weights = { 0.0, -1.0, 1.0 };
	}
	else {
		weights = { 1.0 , -2.0, 1.0 };
	}

	dcomp val = 0.0;
	for (int k = 0; k < 3; k++) {
		val += weights[k] * ((this->*func)(omega0 + (k - 1) * dw, dt));
	}

	return val / pow(dw, derivationOrder);

}
