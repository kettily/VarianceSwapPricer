#ifndef MODEL_H
#define MODEL_H

#include <vector>


// Abstract class = it contains at least one pure virtual method [virtual return_type method_name(arguments) = 0;]
class Model
{
public:
	// Public methods
	// Since our base class does not have any data, we don't need to declare the destructor virtual
	//virtual ~Model() = default;

	// Those methods define the interface that needs to be re-implemented by subclasses (or inherited classes).
	// KESAKO? This is a trick to be able to create copy constructor and assignement operator of the PathSimulator class ...
	virtual Model* clone() const = 0;

	virtual double drift_term(double time, double asset_price) const =0;
	virtual double volatility_term(double time, double asset_price) const =0;

private:
	// no private member variable : no need for virtual destructor, unless we want to make the class abstract and there is no other pure virtual method
};

class BlackScholesModel: public Model
{
	// Building blocks : constructor, destructor
public:
	// since we declare a constructor with parameters, the C++ compiler won't generate a default constructor
	BlackScholesModel(double drift, double volatility);					// Constructor with parameters
	BlackScholesModel(const BlackScholesModel& model);					// Copy constructor
	~BlackScholesModel() = default;										// Destructor : we use the default behavior
	BlackScholesModel& operator=(const BlackScholesModel& model);		// Assignement operator

	// Public methods : the pure virtual methods of Model class HAVE TO BE IMPLEMENTED HERE
	BlackScholesModel* clone() const;
	double drift_term(double time, double asset_price) const override;
	double volatility_term(double time, double asset_price) const override;

private:
	// Private member variables
	double _drift;
	double _volatility;
};

class CIRModel : public Model
{
public:
	CIRModel(double mean_reversion_speed, double mean_reversion_level, double volatility);		// Constructor with parameters
	CIRModel(const CIRModel& model);															// Copy constructor
	~CIRModel() = default;																        // Destructor : we use the default behavior
	CIRModel& operator=(const CIRModel& model);													// Assignement operator

public:
	// Public methods : the pure virtual methods of Model class HAVE TO BE IMPLEMENTED HERE
	CIRModel* clone() const;
	double drift_term(double time, double asset_price) const override;
	double volatility_term(double time, double asset_price) const override;

private:
	// Private members
	double _mean_reversion_speed;
	double _mean_reversion_level;
	double _volatility;
};

#endif