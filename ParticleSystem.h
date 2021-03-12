#pragma once
#include "core.h"
#include "Particle.h"

class ParticleSystem
{
	static const int MAX_P = 500;
	static const float PI;
	GLuint VAO;
	GLuint VBO, EBO;
	glm::vec3 color;
	glm::vec3 lineColor;
	float spawnPeriod;
	glm::mat4 world;
	glm::mat4 model;
	glm::mat4 view;
	float timeToSpawn;
	void InitializeArrays();
	float ParticleSystem::RandomOffset();
	glm::vec3 RandomVec(glm::vec3 startVec, float variance);
	glm::vec3 RandomDirVector();

public:
	//std::vector<glm::vec3> vertices;
	glm::vec3 vertices[MAX_P];
	//std::vector<glm::uint> faces;
	glm::uint faces[MAX_P];
	Particle* particles[MAX_P];

	glm::vec3 startPos;
	glm::vec3 startVel;
	float lifeSpan;
	float posVar;
	float velVar;
	float lifeVar;
	float pps;
	float mass;
	float radius;
	int numParticles;

	ParticleSystem();
	~ParticleSystem();
	void Draw(GLuint shader);
	void Update(const glm::mat4& world, const glm::mat4& view);
	void ApplyForces(glm::vec3 gravity, float density, float drag);
	void Step(float delta);
	void Spawn();
	void IntegrateMotion(float delta);
	void ApplyConstraints(float ground, float e, float mD);
	void UpdateVertices();
	void UpdateNormals();
	void BindBuffers();
	void PrintVec(glm::vec3 v) {
		std::cout << v.x << " " << v.y << " " << v.z << std::endl;
	}
};

