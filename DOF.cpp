#include "DOF.h"

DOF::DOF() {
	val=0.0;
	// by default, allow full circle of rotation
	minVal=-M_PI; 
	maxVal=M_PI;
}

DOF::DOF(float v, float lo, float hi) {
	val = v;
	minVal = lo;
	maxVal = hi;
}

void DOF::SetValue(float v) {
	//value = v;
	val = std::max(minVal, std::min(v, maxVal));
}

void DOF::SetMin(float lo) {
	minVal = lo;
}
void DOF::SetMax(float hi) {
	maxVal = hi;
}