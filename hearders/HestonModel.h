#ifndef HESTONMODEL_H
#define HESTONMODEL_H

#include <vector>
//#include <utility>

using namespace std;
using Pair = std::pair<double, double>;


class HestonModel
{
public:
	// Public methods

	~HestonModel() = default;


	Pair drift_term(double time, Pair factors) const;
	Pair volatility_term(double time, Pair factors) const;


    HestonModel(double correlation,
                    double drift,
                    double mean_reversion_speed,
                    double mean_reversion_level,
                    double vol_of_vol);


    double get_correlation() const;
    double get_vol_of_vol() const;
    double get_mean_reversion_speed() const;
    double get_mean_reversion_level() const;
    double get_drift() const;

private:

    double _correlation;
    double _drift;
    double _mean_reversion_speed;
    double _mean_reversion_level;
    double _vol_of_vol;
};




#endif
