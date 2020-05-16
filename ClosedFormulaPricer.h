#ifndef CLOSEDFORMULAPRICER_H
#define CLOSEDFORMULAPRICER_H

#ifndef PRICER_H
#include "Pricer.h"
#endif 


class ClosedFormulaPricer : public Pricer
{
public:
	ClosedFormulaPricer(double discount_rate);

	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER
	ClosedFormulaPricer(const ClosedFormulaPricer& pricer);
	ClosedFormulaPricer& operator=(const ClosedFormulaPricer& pricer);

	virtual ~ClosedFormulaPricer(); // we need to have a virtual destructor : WHY? [interview question]

	double price() const override;

protected:

	
};

#endif
