#ifndef _SPAN_H_
#define _SPAN_H_
#include "core.h"

class Span
{
private:
	static glm::mat4 hermiteBasis;

public:
	float time0, time1;	// time at start and end
	//float pos0, pos1;	// position/value at start and end
	//float tan0, tan1;	// tangent value at start and end
		
	float A,B,C,D;			// cubic coefficients
	//glm::vec4 cubics;
	Span(float t0,float t1, float p0, float p1, float v0,float v1);
	void ComputeCubics(float p0, float p1, float v0, float v1);
	float Evaluate(float time);
	bool HasTime(float time);
};

#endif
