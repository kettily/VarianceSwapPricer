#include "ClosedFormulaPricer.h"

ClosedFormulaPricer::ClosedFormulaPricer(double discount_rate):Pricer(discount_rate)
{
}

ClosedFormulaPricer::ClosedFormulaPricer(const ClosedFormulaPricer& pricer):Pricer(pricer)
{
}

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
	return 0.0;
}
