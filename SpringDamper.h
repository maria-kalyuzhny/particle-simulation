#pragma once
#include "core.h"
#include "Particle.h"

class SpringDamper
{
public:
	float rL;  //rest length
	float kS; //spring const
	float kD; //damping const
	Particle *P1, *P2;
	//public:
	SpringDamper(Particle* P1, Particle* P2, float kS, float kD);
	void ApplyForce();
};

