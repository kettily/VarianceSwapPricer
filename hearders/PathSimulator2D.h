#ifndef PATHSIMULATOR2D_H
#define PATHSIMULATOR2D_H

#ifndef MODEL2D_H
#include "Model2D.h"
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
                    const Model2D& model);
	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER 
	PathSimulator2D(const PathSimulator2D& path_simulator);
	PathSimulator2D& operator=(const PathSimulator2D& path_simulator);
	~PathSimulator2D();

	// We simulate the vector of pairs, then return only the vector of first terms of the pairs  
	Vector path() const;
	Vector getTimePoints() const;
	virtual PathSimulator2D*  clone() const= 0;

protected:
	// This method is internal to the class, not needed outside it, so we set it as being private
	virtual Pair nextStep(int current_index, Pair current_factors)  const = 0;


	Pair _initial_factors;					// Each simulated path starts from the same initial value.
	Vector _time_points;		// time interval which is dicretized in points.
	const Model2D* _model;
						 // Model _model; // we cannot instanciate an abstract class ...
						 // but we can POINT to an object of ABSTRACT type Model.	
						 // Please note we have a pointer to a "const Model", the pointer itself is not const.
};


class SchemaQE: public PathSimulator2D
{
	SchemaQE(Pair initial_factors,
		const Vector& time_points,
		const Model2D& model);
	Pair nextStep(int current_index, Pair current_factors) const override;

	SchemaQE* clone() const override;



};

class SchemaTG : public PathSimulator2D
{
	SchemaTG(Pair initial_factors,
		const Vector& time_points,
		const Model2D& model);
	Pair nextStep(int current_index, Pair current_factors) const override;

	SchemaTG* clone() const override;

};



#endif