#include "Span.h"
glm::mat4 Span::hermiteBasis =
glm::mat4(2, -3, 0, 1,
	-2, 3, 0, 0,
	1, -2, 1, 0,
	1, -1, 0, 0);

Span::Span(float t0, float t1, float p0, float p1, float v0, float v1){
	time0 = t0;
	time1 = t1;
	ComputeCubics( p0, p1, v0, v1);
}

void Span::ComputeCubics(float p0, float p1, float v0, float v1) {
	glm::vec4 cubics = hermiteBasis * glm::vec4(p0, p1, v0, v1);
	A = cubics[0];
	B = cubics[1];
	C = cubics[2];
	D = cubics[3];
}

float Span::Evaluate(float time) {
	float u = (time - time0)/(time1-time0);
	float x = D + u*(C + u * (B + u * A));
	return x;
}

bool Span::HasTime(float time) {
	if (time >= time0 && time <= time1) {
		return true;
	}
	return false;
}