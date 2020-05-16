#ifndef PATHSIMULATOR_H
#include "PathSimulator.h"
#endif

#ifndef RANDOMNORMALGENERATOR_H
#include "RandomNormalGenerator.h"
#endif

// We cannot use _model(new Model(model))) in the initialization list 
// because we cannot instantiate Model which is an ABSTRACT class.
// Hence we created the pure virtual method clone() that deleguates the construction to the subclasses.
PathSimulator::PathSimulator(double initial_value, const std::vector<double>& time_points, const Model& model)
	: _initial_value(initial_value), _time_points(time_points), _model(model.clone())
{
}

// We cannot use _model(new Model(*(path_simulator._model))) in the initialization list 
// because we cannot instantiate Model which is an ABSTRACT class.
// Hence we created the pure virtual method clone() that deleguates the construction to the subclasses.
PathSimulator::PathSimulator(const PathSimulator & path_simulator)
	: _initial_value(path_simulator._initial_value), 
	_time_points(path_simulator._time_points), 
	_model(path_simulator._model->clone()) //, // clone() is used here
{
}

PathSimulator& PathSimulator::operator=(const PathSimulator & path_simulator)
{
	// check for "self assignment" and do nothing in that case
	if (this == &path_simulator) return *this;
	else {
		delete _model;								// free the storage pointed to by _model
		_model = path_simulator._model->clone();	// allocate new memory for the pointer

		// assignment for other fields
		_initial_value = path_simulator._initial_value;
		_time_points = path_simulator._time_points;
		}

		return *this;								 // return this PathSimulator
}

// The destructor needs to release the memory pointed by its member "POINTER variables".
PathSimulator::~PathSimulator()
{
	delete _model;
}

std::vector<double> PathSimulator::path() const
{
	std::vector<double> path {_initial_value};
	for (int index = 0; index < _time_points.size() - 1; ++index)
		path.push_back(nextStep(index, path[index]));

	return path;
}

std::vector<double> PathSimulator::getTimePoints() const
{
	return _time_points;
}

// S_t to S_{t + Delta t}
double PathSimulator::nextStep(int current_index, double current_asset_price) const
{
	double cur_time = _time_points[current_index];
	double time_gap = _time_points[current_index + 1] - cur_time;

	// mu(t, St) * Delta t
	double drift_increment = _model->drift_term(cur_time, current_asset_price) * time_gap;

	// sigma(t, St) * sqrt(Delta t) * N(0,1)
	double volatility_increment = _model->volatility_term(cur_time, current_asset_price) * std::sqrt(time_gap) * RandomNormalGenerator::normalRandom();  //(*_random_normal_generator)();

	return current_asset_price + drift_increment + volatility_increment;
}
