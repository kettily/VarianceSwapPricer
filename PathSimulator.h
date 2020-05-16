#ifndef PATHSIMULATOR_H
#define PATHSIMULATOR_H

#ifndef MODEL_H
#include "Model.h"
#endif

#include <vector>

class PathSimulator final
{
public:
	PathSimulator(double initial_value, const std::vector<double>& time_points, const Model& model);
	// Copy constructor, Assignement operator and Destructor are NEEDED because one of the member variable is a POINTER 
	PathSimulator(const PathSimulator& path_simulator);
	PathSimulator& operator=(const PathSimulator& path_simulator);
	~PathSimulator();

	// Main method : computes an entire path from valuation date using 
	std::vector<double> path() const;
	std::vector<double> getTimePoints() const;

private:
	// This method is internal to the class, not needed outside it, so we set it as being private
	double nextStep(int current_index, double current_asset_price) const; 

	double _initial_value;					// Each simulated path starts from the same initial value.
	std::vector<double> _time_points;		// time interval which is dicretized in points.
	const Model* _model;
						 // Model _model; // we cannot instanciate an abstract class ...
						 // but we can POINT to an object of ABSTRACT type Model.	
						 // Please note we have a pointer to a "const Model", the pointer itself is not const.
};

#endif