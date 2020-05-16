#include "PathSimulator2D.h"
#include "RandomNormalGenerator.h"

PathSimulator2D::PathSimulator2D(Pair initial_factors, 
                const Vector& time_points, 
                const Model2D& model):
    _initial_factors(initial_factors), _time_points(time_points), _model(model.clone())
{
}

PathSimulator2D::PathSimulator2D(const PathSimulator2D& path_simulator) :
    _initial_factors(path_simulator._initial_factors), _time_points(path_simulator._time_points),
    _model(path_simulator._model->clone())
{}

// P2 = P1 equivalent to P2.operator=(P1)
PathSimulator2D& PathSimulator2D::operator=(const PathSimulator2D& path_simulator){
    // check for "self assignment" and do nothing in that case
	if (!(this == &path_simulator)){
		delete _model;								// free the storage pointed to by _model
		_model = path_simulator._model->clone();	// allocate new memory for the pointer

		// assignment for other fields
		_initial_factors = path_simulator._initial_factors;
		_time_points = path_simulator._time_points;
    }
    return *this;								 // return this PathSimulator2D
}

PathSimulator2D::~PathSimulator2D(){
    delete _model;
}

Vector PathSimulator2D::path() const
{
	Vector_Pair path2D{ _initial_factors };
	Vector path{ _initial_factors.first };

	for (int index = 0; index < _time_points.size() - 1; ++index)
	{
		path2D.push_back(nextStep(index, path2D[index]));
		path.push_back(path2D[index + 1].first);
	}

	return path;
}

Vector PathSimulator2D::getTimePoints() const
{
	return _time_points;
}

SchemaQE::SchemaQE(Pair initial_factors,  const Vector& time_points, const Model2D& model):PathSimulator2D(initial_factors, time_points, model){}

Pair SchemaQE::nextStep(int current_index, Pair current_factors) const
{
    double cur_time = _time_points[current_index];
    double time_gap = _time_points[current_index + 1] - cur_time;

    Pair nextStep;

    //We have two independent normal random variables N(0,1)
    double n1 = RandomNormalGenerator::normalRandom();
    double n2 = RandomNormalGenerator::normalRandom();

    double z1 = n1;
    double rho = _model->get_correlation();
    double z2 = rho * n1 + std::sqrt(1 - rho * rho) * n2;

    nextStep.first = current_factors.first + _model->drift_term(cur_time, current_factors).first * time_gap
        + _model->volatility_term(cur_time, current_factors).first * std::sqrt(time_gap) * z1;
    nextStep.second = current_factors.second + _model->drift_term(cur_time, current_factors).second * time_gap
        + _model->volatility_term(cur_time, current_factors).second * sqrt(time_gap) * z2;


    return nextStep;
}


SchemaQE* SchemaQE::clone() const
{
    return  new SchemaQE(*this); //Il est entrain de cloner: il se clone lui meme donc l'objet qu'il est entrain de copier c'est lui meme donc si c'est lui mneme c'est la reference sur lui meme donc *this
}


SchemaTG::SchemaTG(Pair initial_factors, const Vector& time_points, const Model2D& model) :PathSimulator2D(initial_factors, time_points, model) {}


Pair SchemaTG::nextStep(int current_index, Pair current_factors) const
{
    double cur_time = _time_points[current_index];
    double time_gap = _time_points[current_index + 1] - cur_time;

    Pair nextStep;

    //We have two independent normal random variables N(0,1)
    double n1 = RandomNormalGenerator::normalRandom();
    double n2 = RandomNormalGenerator::normalRandom();

    double z1 = n1;
    double rho = _model->get_correlation();
    double z2 = rho * n1 + std::sqrt(1 - rho * rho) * n2;

    nextStep.first = current_factors.first + _model->drift_term(cur_time, current_factors).first * time_gap
        + _model->volatility_term(cur_time, current_factors).first * std::sqrt(time_gap) * z1;
    nextStep.second = current_factors.second + _model->drift_term(cur_time, current_factors).second * time_gap
        + _model->volatility_term(cur_time, current_factors).second * sqrt(time_gap) * z2;


    return nextStep;
}

SchemaTG* SchemaTG::clone() const
{
    return  new SchemaTG(*this); //Il est entrain de cloner: il se clone lui meme donc l'objet qu'il est entrain de copier c'est lui meme donc si c'est lui mneme c'est la reference sur lui meme donc *this
}
