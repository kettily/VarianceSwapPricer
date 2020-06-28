#ifndef PATHSIMULATOR2D_H
#define PATHSIMULATOR2D_H

#ifndef HESTONMODEL_H
#include "HestonModel.h"
#endif

#include <vector>
#include <cmath>

using Vector = std::vector<double>;
using Vector_Pair = std::vector<pair<double, double> >;

class PathSimulator2D 
{
public:
	PathSimulator2D(Pair initial_factors, 
                    const Vector& time_points, 
                    const HestonModel& model);
	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER 
	PathSimulator2D(const PathSimulator2D& path_simulator);
	PathSimulator2D& operator=(const PathSimulator2D& path_simulator);
	virtual ~PathSimulator2D();

	// We simulate the vector of pairs, then return only the vector of first terms of the pairs  
	void path(Vector* path) const;
	Vector getTimePoints() const;
	virtual PathSimulator2D*  clone() const= 0;
	
protected:
	// This method is internal to the class, not needed outside it, so we set it as being private
	virtual Pair nextStep(int current_index, Pair current_factors)  const = 0;
	Pair _initial_factors;					// Each simulated path starts from the same initial value.
	Vector _time_points;		// time interval which is discretized in points.
	const HestonModel _model;

};


class SchemaQE: public PathSimulator2D
{
public:
	SchemaQE(Pair initial_factors,
		const Vector& time_points,
		const HestonModel& model);
	Pair nextStep(int current_index, Pair current_factors) const override;

	SchemaQE* clone() const override;

};

class SchemaTG : public PathSimulator2D
{
public:
	
	SchemaTG(Pair initial_factors,
		const Vector& time_points,
		const HestonModel& model);

	Pair nextStep(int current_index, Pair current_factors) const override;
	static double rootSearch(double psi, double r_0, double eps, int maxIter);

	SchemaTG* clone() const override;

};



#endif
