#include "RandomNormalGenerator.h"
#include <cmath>

// BOX_MULLER method
double RandomNormalGenerator::normalRandom()
{
	double u1 = uniformRandom();
	double u2 = uniformRandom();
	return cos(8. * atan(1.) * u2) * sqrt(-2. * log(u1));
}

// Generator that relies on pseudo random generator
double RandomNormalGenerator::uniformRandom()
{
	return ((double)(rand()) + 1.) / ((double)(RAND_MAX)+2.);
}
