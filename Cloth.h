#pragma once
#include "core.h"
#include "Particle.h"
#include "Triangle.h"
#include "SpringDamper.h"

class Cloth{
	//rendering
	GLuint VAO;
	GLuint VBOv, VBOn, EBO, EBOs;

	glm::vec3 color;
	glm::vec3 lineColor;
	glm::mat4 world;
	glm::mat4 model;
	glm::mat4 view;


	void InitializeArrays();


	
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::uint> faces;
	std::vector<glm::uint> segments;
	//physics logic make private later
	std::vector<Particle*> particles;
	std::vector<Triangle*> triangles;
	std::vector<SpringDamper*> springDampers;
	Cloth(int w, int h, float unitSize, float mass, float kS, float kD);
	~Cloth();
	void Draw(GLuint shader);
	void Update(const glm::mat4& world, const glm::mat4& view);
	void ApplyForces(glm::vec3 gravity, glm::vec3 vWind, float density, float drag);
	void IntegrateMotion(float delta);
	void ApplyUserControls(glm::vec3 offset, glm::vec3 rot);
	void ApplyConstraints(float ground, float e, float mD);
	void UpdateVertices();
	void UpdateNormals();
	void BindBuffers();
};

