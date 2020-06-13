#include "ClosedFormulaPricer.h"
#include <iostream>


ClosedFormulaPricer::ClosedFormulaPricer(Pair initial_factors, const Vector& time_points, const HestonModel& modeldouble, double discount_rate) : Pricer(discount_rate), _initial_factors(initial_factors), _time_points(time_points), _model(modeldouble)
{

}

/*
ClosedFormulaPricer::ClosedFormulaPricer(const ClosedFormulaPricer& pricer) : Pricer(pricer)
{
}
*/

ClosedFormulaPricer& ClosedFormulaPricer::operator=(const ClosedFormulaPricer& pricer)
{
	_discount_rate = pricer._discount_rate;


	// TODO: insérer une instruction return ici
	return *this;
}

ClosedFormulaPricer::~ClosedFormulaPricer()
{
}

double ClosedFormulaPricer::price() const
{
	int N = _time_points.size();
	double annualizedVariance = 0.0;
	double maturity = _time_points[N - 1];
	for (int i = 1; i <= N; i++) {
		dcomp logReturnSquared_ti = function_logReturnSquared_ti(i);
		double test = logReturnSquared_ti.real();
		annualizedVariance = annualizedVariance + test;
	}

	return annualizedVariance / maturity;
}

dcomp ClosedFormulaPricer::function_logReturnSquared_ti(int i) const
{
	double eps0 = 0.0;
	double eps = pow(10, -4);
	double dt = _time_points[1] - _time_points[0];

	dcomp fd_D0 = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_D, eps0, 1, eps, dt);
	dcomp sd_D0 = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_D, eps0, 2, eps, dt);
	dcomp fd_C0 = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_C, eps0, 1, eps, dt);
	dcomp sd_C0 = derivativeCalculator_0(&ClosedFormulaPricer::zeroDerivative_C, eps0, 2, eps, dt);

	dcomp fd_D = (zeroDerivative_D(eps0 + eps, dt) - zeroDerivative_D(eps0, dt)) / eps;
	dcomp sd_D = (zeroDerivative_D(eps0 + eps, dt) - 2.0 * zeroDerivative_D(eps0, dt) + zeroDerivative_D(eps0 - eps, dt)) / (eps * eps);
	dcomp fd_C = (zeroDerivative_C(eps0 + eps, dt) - zeroDerivative_C(eps0, dt)) / eps;
	dcomp sd_C = (zeroDerivative_C(eps0 + eps, dt) - 2.0 * zeroDerivative_C(eps0, dt) + zeroDerivative_C(eps0 - eps, dt)) / (eps * eps);



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

dcomp ClosedFormulaPricer::zeroDerivative_D(double w, double dt) const
{
	dcomp a = _model.get_mean_reversion_speed()
		- _model.get_correlation() * _model.get_vol_of_vol() * w * dcomp(0, 1);
	dcomp delta = pow(a, 2) + pow(_model.get_vol_of_vol(), 2) * (pow(w, 2) + w * dcomp(0, 1));
	dcomp b = -sqrt(delta);
	dcomp g = (a + b) / (a - b);

	dcomp val;
	if (w == 0.0) {
		val = 0.0;
	}
	else {
		val = ((a + b) * (1.0 - exp(b * dt))) / (pow(_model.get_vol_of_vol(), 2) * (1.0 - g * exp(b * dt)));
	}

	return val;
}



dcomp ClosedFormulaPricer::zeroDerivative_C(double omega, double dt) const
{

	dcomp a = _model.get_mean_reversion_speed()
		- _model.get_correlation() * _model.get_vol_of_vol() * omega * dcomp(0, 1);
	dcomp delta = pow(a, 2) + pow(_model.get_vol_of_vol(), 2) * (pow(omega, 2) + omega * dcomp(0, 1));
	dcomp b = -sqrt(delta);
	dcomp g = (a + b) / (a - b);

	dcomp val;

	if (omega == 0.0) {
		val = -_discount_rate * dt;
	}
	else {
		val = _discount_rate * (omega * dcomp(0, 1) - 1.0) * dt
			+ _model.get_mean_reversion_speed() * _model.get_mean_reversion_level() * ((a + b) * dt - 2.0 * log((1.0 - g * exp(b * dt)) / (1.0 - g))) / pow(_model.get_vol_of_vol(), 2);
	}


	return val;
}



dcomp ClosedFormulaPricer::derivativeCalculator_0(MagicType func, double omega0, int derivationOrder, double dw, double dt) const
{
	int order = 9;
	Vector weights(order);

	if (derivationOrder == 1) {
		//weights = { -1.0/2.0, 0.0, 1.0/2.0 };
		weights = { 3.0 / 840.0, -32.0 / 840.0, 168.0 / 840.0, -672.0 / 840.0, 0.0, 672.0 / 840.0, -168.0 / 840.0, 32.0 / 840.0, -3.0 / 840.0 };
	}
	else {
		//weights = { 1.0 , -2.0, 1.0 };
		weights = { -9,128.0 / 5040.0,-1008.0 / 5040.0,8064.0 / 5040.0,-14350.0 / 5040.0,8064.0 / 5040.0,-1008.0 / 5040.0,128.0 / 5040.0,-9.0 / 5040.0 };

	}

	dcomp val = 0.0;
	for (int k = 0; k < order; k++) {
		val += weights[k] * ((this->*func)(omega0 + (k - 4) * dw, dt));

	}

	return val / pow(dw, derivationOrder);

}