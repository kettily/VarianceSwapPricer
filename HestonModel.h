#ifndef HESTONMODEL_H
#define HESTONMODEL_H

#include <vector>


using namespace std;
using Pair = std::pair<double, double>;


class HestonModel
{
	public:
	
		HestonModel(double correlation,
                            double drift,
                            double mean_reversion_speed,
                            double mean_reversion_level,
                            double vol_of_vol);

		~HestonModel() = default;


		Pair drift_term(double time, Pair factors) const;
		Pair volatility_term(double time, Pair factors) const;

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
