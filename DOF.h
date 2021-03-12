#ifndef DOF_H
#define DOF_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

class DOF
{
public:
	float val;
	float minVal;
	float maxVal;
	DOF();
	DOF(float v, float lo, float hi);
	void SetValue(float v);
	void SetMin(float lo);
	void SetMax(float hi);
};

#endif
