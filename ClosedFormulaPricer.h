#ifndef CLOSEDFORMULAPRICER_H
#define CLOSEDFORMULAPRICER_H

#ifndef PRICER_H
#include "Pricer.h"
#endif 

 #ifndef HESTONMODEL
#include "HestonModel.h"
#endif

#include <vector>
#include <cmath>
#include <complex>

//using namespace std;
//typedef complex<double> dcomp;

using dcomp = std::complex<double>;
using Vector = std::vector<double>;
using Vector_Pair = std::vector<pair<double, double> >;


class ClosedFormulaPricer : public Pricer
{
public:
	ClosedFormulaPricer(Pair initial_factors,
		const Vector& time_points,
		const HestonModel& modeldouble,
		double discount_rate);

	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER

	//ClosedFormulaPricer(const ClosedFormulaPricer& pricer);

	ClosedFormulaPricer& operator=(const ClosedFormulaPricer& pricer);

	~ClosedFormulaPricer(); // we need to have a virtual destructor : WHY? [interview question]

	double price() const override;

private :

	Pair _initial_factors;					// Each simulated path starts from the same initial value.
	Vector _time_points;		// time interval which is dicretized in points.
	const HestonModel _model;

	dcomp function_logReturnSquared_ti(int i) const;


	dcomp zeroDerivative_D(double w, double dt)  const;

	dcomp zeroDerivative_C(double w, double dt)  const;

	using MagicType = dcomp (ClosedFormulaPricer::*) (double, double) const;

	dcomp derivativeCalculator_0(MagicType, double omega0,int derivationOrder,double eps, double dt) const;


};

#endif

