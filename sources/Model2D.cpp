#include "Model2D.h"

Model2D::Model2D(double correlation): _correlation(correlation) {}

double Model2D::get_correlation() const{
    return _correlation;
}

HestonModel::HestonModel(double correlation,
                double drift, 
                double mean_reversion_speed, 
                double mean_reversion_level,
                double vol_of_vol): 
    Model2D(correlation), _drift(drift), _mean_reversion_speed(mean_reversion_speed), 
    _mean_reversion_level(mean_reversion_level), _vol_of_vol(vol_of_vol)
{}

HestonModel* HestonModel::clone() const{
    return new HestonModel(*this);
}

Pair HestonModel::drift_term(double time, Pair factors) const{
    Pair drift_pair;
    drift_pair.first = _drift*factors.first;
    drift_pair.second = _mean_reversion_speed*(_mean_reversion_level-factors.second);
    return drift_pair;
}

Pair HestonModel::volatility_term(double time, Pair factors) const{
	Pair volatility_pair;
    volatility_pair.first = sqrt(factors.second)*factors.first;
    volatility_pair.second = _vol_of_vol*sqrt(factors.second);
    return volatility_pair;
}

SabrModel::SabrModel(double correlation,
	double beta,
	double alpha) :
	Model2D(correlation), _beta(beta), _alpha(alpha)
{}

SabrModel* SabrModel::clone() const {
	return new SabrModel(*this);
}

Pair SabrModel::drift_term(double time, Pair factors) const {
	Pair drift_pair(0., 0.);
	return drift_pair;
}

Pair SabrModel::volatility_term(double time, Pair factors) const {
	Pair volatility_pair;
	volatility_pair.first = factors.second * pow(factors.first, _beta);
	volatility_pair.second = _alpha * factors.second;
	return volatility_pair;
}