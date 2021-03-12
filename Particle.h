#pragma once
#include "core.h"
class Particle
{
	glm::vec3 r0;

	// may want to add a rendering mechanism, maybe not in this class though?
public:
	glm::vec3 r; //position
	glm::vec3 f; //force
	glm::vec3 v; //velocity
	glm::vec3 a; //acceleration
	glm::vec3 p; //momentum
	glm::vec3 n;
	float m;   //mass
	bool fixed;  //?

	Particle(glm::vec3 pos, float mass, bool fixed);
	void ForwardIntegrate(float dT);
	void ApplyUserControls(glm::vec3 offset, glm::vec3 rot);
	void ApplyConstraints(float ground, float e, float mD);
	void PrintPos();
};

