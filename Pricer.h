#ifndef PRICER_H
#define PRICER_H
#include <vector>


using namespace std;
using Pair = std::pair<double, double>;

class Pricer
{
	public:
		Pricer(double discount_rate);
		Pricer(const Pricer& pricer);
		Pricer& operator=(const Pricer& pricer);

		virtual ~Pricer(); 

		virtual Pair price() const = 0;	


	protected:

		double _discount_rate;
};
#endif
