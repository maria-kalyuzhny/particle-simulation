#pragma once
#include "core.h"
class Particle
{
	glm::vec3 r0;

	// may want to add a rendering mechanism, maybe not in this class though?
public:
	float life;
	glm::vec3 r; //position
	glm::vec3 f; //force
	glm::vec3 v; //velocity
	glm::vec3 a; //acceleration
	float rad;   //radius
	float m;	 //mass

	Particle(glm::vec3 pos, glm::vec3 vel, float mass, float lifeSpan, float radius);
	void ForwardIntegrate(float dT);
	void Step(float dT);
	void ApplyUserControls(glm::vec3 offset, glm::vec3 rot);
	void ApplyConstraints(float ground, float e, float mD);
	void PrintVec(glm::vec3 v) {
		std::cout << v.x << " " << v.y << " " << v.z << std::endl;
	}
};

