#include "Pricer.h"


Pricer::Pricer(double discount_rate):_discount_rate(discount_rate)
{
}

Pricer::Pricer(const Pricer& pricer):_discount_rate(pricer._discount_rate)
{
}

Pricer& Pricer::operator=(const Pricer& pricer)
{
	// TODO: insérer une instruction return ici
	_discount_rate = pricer._discount_rate;
	return *this;
}

Pricer::~Pricer()
{
}
