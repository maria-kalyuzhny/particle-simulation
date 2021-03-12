#include "Particle.h"

Particle::Particle(glm::vec3 pos, float mass, bool fix) {
	r0 = pos;
	r = pos;
	m = mass;
	v = glm::vec3(0);
	a = glm::vec3(0);
	f = glm::vec3(0);
	p = glm::vec3(0);
	n = glm::vec3(0,0,1);
	fixed = fix;
}

void Particle::ApplyUserControls(glm::vec3 offset, glm::vec3 rot) {
	if (!fixed) {
		return;
	}
	r = r0 + offset;
	glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0, 0, 1));
	r = rotZ * rotY * rotX * glm::vec4(r,1);
}

void Particle::ApplyConstraints(float ground, float e, float mD) {
	if (r.y < ground) {
		n = glm::vec3(0,1,0);
		//restitution
		float vClose = glm::dot(v,n);
		glm::vec3 j = -(1+e)*m*vClose*n;
		glm::vec3 deltaV = (1.0f/m)*j;
		glm::vec3 deltaV2 = glm::vec3(0);

		//friction
		glm::vec3 vNorm = glm::dot(v,n)*n;
		glm::vec3 vTan = v - vNorm;
		float magnitude = mD*glm::length(j);
		//glm::vec3 deltaV2;
		if (magnitude >= glm::length(vTan)) {
			deltaV2 = - vTan;
		}
		else {
			deltaV2 = - glm::normalize(vTan) * magnitude;
		}
		
		//apply changes in velocity and correct pos
		v = v + deltaV + deltaV2;
		r.y = ground;
	}
}

void Particle::ForwardIntegrate(float dT) {
	if (fixed) {
		return; //forces don't affect fixed particles
	}
	a = (1/m)*f;
	v = v + a*dT;
	r = r + v*dT;
}

void Particle::PrintPos() {
	std::cout << r.x << " " << r.y << " " << r.z <<std::endl;
}