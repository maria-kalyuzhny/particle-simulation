#include "ParticleSystem.h"
const float ParticleSystem::PI = 3.14159265359;

ParticleSystem::ParticleSystem() {
	//vertices = std::vector<glm::vec3>();
	//faces = std::vector<glm::uint>();
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	lineColor = glm::vec3(0.1f, 0.1f, 0.1f);

	model = glm::mat4(1.0f);
	world = glm::mat4();
	view = glm::mat4();
	
	mass = 0.1f;
	startPos = glm::vec3(0.0f, 1.0f, 0.0f);
	startVel = glm::vec3(0.0f,1.0f,0.0f);
	lifeSpan = 3.0f;
	posVar = 1.0f;
	velVar = 1.0f;
	lifeVar = 2.0f;
	pps = 50;
	spawnPeriod = 1/pps;
	timeToSpawn = spawnPeriod;
	radius = 0.1f;
	numParticles = 10;

	// initialize particles as well as vertices and faces
	for (int i = 0; i < MAX_P; i++) {
		if (i < numParticles) {
			glm::vec3 pos = RandomVec(startPos, posVar);
			//std::cout << pos.x << std::endl;
			glm::vec3 vel = RandomVec(startVel, velVar);
			float life = lifeSpan + lifeVar*RandomOffset();
			particles[i] = new Particle(pos, vel, mass, life, radius);
			vertices[i] = particles[i]->r;
		}
		else {
			particles[i] = NULL;
			vertices[i] = glm::vec3(0);
		}
		faces[i] = i;
	}


	InitializeArrays();
	BindBuffers();
}

void ParticleSystem::Spawn() {
	glm::vec3 pos = RandomVec(startPos, posVar);
	glm::vec3 vel = RandomVec(startVel, velVar);
	float life = lifeSpan + lifeVar * RandomOffset();
	particles[numParticles] = new Particle(pos, vel, mass, life, radius);
	numParticles = numParticles + 1;
}

float ParticleSystem::RandomOffset() {
	// returns a random float between -1 and 1
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	return r*2 - 1;
}

glm::vec3 ParticleSystem::RandomVec(glm::vec3 startVec, float variance) {
	// returns random vector based on start pos & max offset from start pos
	return startVec + RandomOffset() * variance * RandomDirVector();
}

glm::vec3 ParticleSystem::RandomDirVector() {
	float s = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float t = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float u = 2*PI*s;
	float v = glm::sqrt(t*(1-t));
	float dX = 2*v*cos(u);
	float dY = 1 - 2*t;
	float dZ = 2*v*sin(u);
	return glm::vec3(dX, dY, dZ);
}

void ParticleSystem::InitializeArrays() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void ParticleSystem::BindBuffers() {
	// Bind the vertices, normals, faces to the VAO/VBOs/EBO

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind vertex VBO to the bound VAO, and store the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_P, vertices, GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass the vertex data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO, and send the index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * MAX_P, faces, GL_STATIC_DRAW);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleSystem::Draw(GLuint shader)
{
	// Fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Actiavte the shader program 
	glPointSize(100*radius);
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&world);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// Draw the points indexed with the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * MAX_P, faces, GL_STATIC_DRAW);
	glDrawElements(GL_POINTS, MAX_P, GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void ParticleSystem::Update(const glm::mat4& world_mat, const glm::mat4& view_mat)
{
	world = world_mat;
	view = view_mat;
}

ParticleSystem::~ParticleSystem()
{
	// Delete the VBO/EBO and the VAO.
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	// Delete the particles!
	for (auto& particle : particles) {
		if (particle != NULL) {
			delete particle;
		}
	}
	delete particles;
}

void ParticleSystem::ApplyForces(glm::vec3 gravity, float density, float drag) {
	// add gravity
	for (auto& particle : particles) {
		if (particle != NULL) {
			particle->f = particle->m * gravity;
		}
	}
	// add aerodynamic drag
	for (auto& particle : particles) {
		if (particle != NULL) {
			glm::vec3 e = -glm::normalize(particle->v);
			float area = std::pow(particle->rad * PI, 2);
			float v2 = std::pow(glm::length(particle->v), 2);
			glm::vec3 fDrag = 0.5f * density * v2 * drag * area * e;
			particle->f = particle->f + fDrag;
		}
	}
	// add collision forces??
}

void ParticleSystem::Step(float delta) {
	//step all particles
	for (auto& particle : particles) {

		if (particle != NULL) {
			particle->ForwardIntegrate(delta);
			particle->Step(delta);
			// delete particles whos life span ended
			if (particle->life <= 0) {
				delete particle;
				particle = particles[numParticles-1];
				particles[numParticles-1] = NULL;
				numParticles = numParticles - 1;
			}

		}
	}
	// spawn new particles
	timeToSpawn = timeToSpawn - delta;
	if (timeToSpawn <= 0) {
		int numSpawn = 1 - (int)(timeToSpawn / spawnPeriod);
		//std::cout << "num spawn" << numSpawn << std::endl;
		for (int i = 0; i < numSpawn; i++) {
			if (numParticles < MAX_P) {
				Spawn();
			}
		}
		spawnPeriod = 1 / pps;
		timeToSpawn = spawnPeriod;
	}
}

void ParticleSystem::IntegrateMotion(float delta) {
	for (auto& particle : particles) {
		if (particle != NULL) {
			particle->ForwardIntegrate(delta);
		}
	}
}

void ParticleSystem::ApplyConstraints(float ground, float e, float mD) {
	for (auto& particle : particles) {
		if (particle != NULL) {
			particle->ApplyConstraints(ground, e, mD);
		}
	}
}

void ParticleSystem::UpdateVertices() {
	for (int i = 0; i < MAX_P; i++) {
		if (particles[i] != NULL) {
			vertices[i] = particles[i]->r;
		}
		else {
			vertices[i] = glm::vec3(0.0f,-100.0f,0.0f);
		}
	}
}