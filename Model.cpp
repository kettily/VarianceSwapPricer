#include "Model.h"
#include <cmath> // useful for std::sqrt


////////////////// Black Scholes Model Definitions /////////////////////////

BlackScholesModel::BlackScholesModel(double drift, double volatility)
	: _drift(drift), _volatility(volatility)
{}

BlackScholesModel::BlackScholesModel(const BlackScholesModel& model) // 
	:_drift(model._drift), _volatility(model._volatility)
{
}

BlackScholesModel * BlackScholesModel::clone() const
{
	return new BlackScholesModel(*this); // this is a pointer to my current object
}

double BlackScholesModel::drift_term(double time, double asset_price) const 
{
	return _drift * asset_price;
}

double BlackScholesModel::volatility_term(double time, double asset_price) const
{
	return _volatility * asset_price;
}

// "this" is a pointer to the current object of the class.
// "*this" is then the reference to the current object of the class.
// Since this method return a reference to the current object [return type is BlackScholesModel&], then we need to return *this
BlackScholesModel& BlackScholesModel::operator=(const BlackScholesModel & model)
{
	if (this == &model) // it is the case we do model = model ... there is nothing to do
		return *this;
	else
	{
		_drift = model._drift;
		_volatility = model._volatility;
	}
	return *this;
}


////////////////// Cox-Ingersoll-Ross Model Definitions /////////////////////////

CIRModel::CIRModel(double mean_reversion_speed, double mean_reversion_level, double volatility)
	:_mean_reversion_speed(mean_reversion_speed), _mean_reversion_level(mean_reversion_level), _volatility(volatility)
{
}

CIRModel::CIRModel(const CIRModel & model)
	: _mean_reversion_speed(model._mean_reversion_speed), _mean_reversion_level(model._mean_reversion_level), _volatility(model._volatility)
{
}

CIRModel * CIRModel::clone() const
{
	return new CIRModel(*this);
}

// kappa * (theta - r_t)
double CIRModel::drift_term(double time, double asset_price) const
{
	return _mean_reversion_speed * (_mean_reversion_level - asset_price);
}

// sigma * sqrt(r_t)
double CIRModel::volatility_term(double time, double asset_price) const
{
	return _volatility * std::sqrt(asset_price);
}

// "this" is a pointer to the current object of the class.
// "*this" is then the reference to the current object of the class.
// Since this method return a reference to the current object [return type is CIRModel&], then we need to return *this
CIRModel& CIRModel::operator=(const CIRModel & model)
{
	if (this == &model) // it is the case we do model = model ... there is nothing to do
		return *this;
	else
	{
		_mean_reversion_speed = model._mean_reversion_speed;
		_mean_reversion_level = model._mean_reversion_level;
		_volatility = model._volatility;
	}
	return *this;
}

