#ifndef PRICER_H
#define PRICER_H

class Pricer
{
public:
	Pricer(double discount_rate);

	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER
	Pricer(const Pricer& pricer);
	Pricer& operator=(const Pricer& pricer);

	virtual ~Pricer(); // we need to have a virtual destructor : WHY? [interview question]

	virtual double price() const = 0;				// pure virtual method: needs to be implemented by all subclasses. And it will make the difference between the 2 pricers.


protected:

	double _discount_rate;
};

#endif