#pragma once
#include "core.h"
#include "Particle.h"
class Triangle
{
public:
	Particle* P1;
	Particle* P2;
	Particle* P3;
	glm::vec3 n;
	Triangle(Particle* Par1, Particle* Par2, Particle* Par3);
	void ApplyForce(glm::vec3 vWind, float density, float drag);
	void ApplyNormals();
};

