#include "SpringDamper.h"

SpringDamper::SpringDamper(Particle* Par1, Particle* Par2, float kSpring, float kDamper) {
	P1 = Par1;
	P2 = Par2;
	kS = kSpring;
	kD = kDamper;
	rL = glm::distance(P1->r, P2->r);
	//std::cout << rL <<std::endl;
}

void SpringDamper::ApplyForce() {	

	//length and unit vector e
	glm::vec3 e = P2->r - P1->r;
	float l = glm::length(e);
	e = e / l;

	//closing velocity
	float vClose = glm::dot((P1->v - P2->v), e);

	//final forces
	float f = -kS*(rL-l) - kD*vClose;
	glm::vec3 f1 = f*e;
	glm::vec3 f2 = -f1;
	P1->f += f1;
	P2->f += f2;
}