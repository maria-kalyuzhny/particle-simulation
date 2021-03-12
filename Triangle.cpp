#include "Triangle.h"
#include <cmath>

Triangle::Triangle(Particle* Par1, Particle* Par2, Particle* Par3) {
	P1 = Par1;
	P2 = Par2;
	P3 = Par3;
	glm::vec3 n0 = glm::cross(P2->r - P1->r, P3->r - P1->r);
	n = glm::normalize(n0);
}

//aerodynamic force
void Triangle::ApplyForce(glm::vec3 vWind, float density, float drag) {
	glm::vec3 vSurf = (P1->v + P2->v + P3->v)/3.0f;
	if (vWind == glm::vec3(0) && vSurf == glm::vec3(0)) {
		return;
	}
	glm::vec3 v = vSurf - vWind;
	glm::vec3 n0 = glm::cross(P2->r - P1->r, P3->r - P1->r);
	n = glm::normalize(n0);
	float a0 = 0.5f * glm::length(glm::cross(P2->r - P1->r, P3->r - P1->r));
	float a = glm::dot(glm::normalize(v),n);
	glm::vec3 fAero = -0.5f * density * float(std::pow(glm::length(v),2)) * drag * a * n;
	glm::vec3 fAeroP = fAero/3.0f;
	//std::cout << fAeroP.x << " " << fAeroP.y << " " << fAeroP.z << std::endl;
	P1->f += fAeroP;
	P2->f += fAeroP;
	P3->f += fAeroP;
}

void Triangle::ApplyNormals() {
	P1->n += n;
	P2->n += n;
	P3->n += n;
}