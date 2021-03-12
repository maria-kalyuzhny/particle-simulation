#include "Cloth.h"

//const glm::vec3 Cloth::gravity = glm::vec3(0,-9.8f,0);

Cloth::Cloth(int w, int h, float u, float mass, float kS, float kD) {
	// creates a cloth of w by h units in the xy plane,
	// centered at 0,0
	vertices = std::vector<glm::vec3>();
	normals = std::vector<glm::vec3>();
	faces = std::vector<glm::uint>();
	segments = std::vector<glm::uint>();
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	lineColor = glm::vec3(0.1f, 0.1f, 0.1f);

	model = glm::mat4(1.0f);
	world = glm::mat4();
	view = glm::mat4();

	float left = 0 - u*w/2.0f;
	float top = 0 + u*h/2.0f + 1.0f;
	float pMass = mass/(w*h);
	bool fixed;
	// i is row index, j is col index

	int wP = w+1;	//num particles w
	int hP = h+1; 
	// initialize particles & vertices
	for (int i = 0; i < hP; i++) {
		if (i == 0) { fixed = true; }
		else { fixed = false; } 
		for (int j = 0; j < wP; j++) {
			float y_pos = top - i * u;
			float x_pos = left + j * u;
			float z_pos = 0;
			//x_pos += float((rand() % 10)-5) / 1000;
			if (i != 0) {
				z_pos += float((rand() % 10) - 5) / 1000;
			}
			glm::vec3 pos = glm::vec3(x_pos,y_pos, z_pos);
			Particle * particle = new Particle(pos, pMass, fixed);
			particles.push_back(particle);
			vertices.push_back(pos);
			normals.push_back(particle->n); //for now all normals point at viewer
		}
	}

	// spring dampers
	for (int i = 0; i < hP-1; i++) {
		for (int j = 0; j < wP-1; j++) {
			//indices of particles in square
			int i0 = i * wP + j;
			int i1 = i * wP + j + 1;
			int i2 = (i + 1) * wP + j;
			int i3 = (i + 1) * wP + j + 1;
			//four spring dampers in every square
			SpringDamper* top   = new SpringDamper(particles[i0], particles[i1], kS, kD);
			SpringDamper* left  = new SpringDamper(particles[i0], particles[i2], kS, kD);
			SpringDamper* diag1 = new SpringDamper(particles[i0], particles[i3], kS, kD);
			SpringDamper* diag2 = new SpringDamper(particles[i1], particles[i2], kS, kD);
			springDampers.push_back(top);
			segments.push_back(i0);
			segments.push_back(i1);
			springDampers.push_back(left);
			segments.push_back(i0);
			segments.push_back(i2);
			springDampers.push_back(diag1);
			segments.push_back(i0);
			segments.push_back(i3);
			springDampers.push_back(diag2);
			segments.push_back(i1);
			segments.push_back(i2);
			//add rightmost spring damper if end of row
			if (j == wP - 2) {
				SpringDamper* right = new SpringDamper(particles[i1], particles[i3], kS, kD);
				springDampers.push_back(right);
				segments.push_back(i1);
				segments.push_back(i3);
			}
			//add bottom spring damper if end of column
			if (i == hP - 2) {
				SpringDamper* bottom = new SpringDamper(particles[i2], particles[i3], kS, kD);
				springDampers.push_back(bottom);
				segments.push_back(i2);
				segments.push_back(i3);
			}
		}
	}
	std::cout<< "number of spring dampers" << std::endl;
	std::cout << springDampers.size() << std::endl;
	
	// face indices 
	for (int i = 0; i < hP - 1; i++) {
		for (int j = 0; j < wP - 1; j++) {
			// four indices in the order:
			// topleft, topright, bottomleft, bottomright
			glm::uint i0 = i * wP + j;
			glm::uint i1 = i * wP + j+1;
			glm::uint i2 = (i+1) * wP + j;
			glm::uint i3 = (i+1) * wP + j+1;
			// upperleft triangle
			// counterclockwise order: topleft, bottomleft, topright
			faces.push_back(i0);
			faces.push_back(i2);
			faces.push_back(i1);
			Triangle* triangle = new Triangle(particles[i0], particles[i2], particles[i1]);
			triangles.push_back(triangle);
			// bottomright triangle
			// counterclockwise order: topleft, bottomleft, topright
			faces.push_back(i1);
			faces.push_back(i2);
			faces.push_back(i3);
			Triangle* triangle2 = new Triangle(particles[i1], particles[i2], particles[i3]);
			triangles.push_back(triangle2);
		}
	}
	std::cout << "num triangles" << std::endl;
	std::cout << triangles.size() << std::endl;

	InitializeArrays();
	BindBuffers();

}

void Cloth::InitializeArrays() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBOv);
	glGenBuffers(1, &VBOn);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &EBOs);
}

void Cloth::BindBuffers() {
	// Bind the vertices, normals, faces to the VAO/VBOs/EBO

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind vertex VBO to the bound VAO, and store the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass the vertex data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind vertex normals VBO to the VAO, and store normal data
	glBindBuffer(GL_ARRAY_BUFFER, VBOn);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO, and send the index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * faces.size(), faces.data(), GL_STATIC_DRAW);

	//// Generate EBO, bind the EBO to the bound VAO, and send the index data
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * segments.size(), segments.data(), GL_STATIC_DRAW);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cloth::Draw(GLuint shader)
{
	// Fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Actiavte the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&world);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// Draw the points using triangles, indexed with the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * faces.size(), faces.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_LINES, faces.size(), GL_UNSIGNED_INT, 0);

	/*glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &lineColor[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * segments.size(), segments.data(), GL_STATIC_DRAW);
	glDrawElements(GL_LINES, segments.size(), GL_UNSIGNED_INT,0);*/

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void Cloth::Update(const glm::mat4& world_mat, const glm::mat4& view_mat)
{
	world = world_mat;
	view = view_mat;
}

Cloth::~Cloth()
{
	// Delete the VBO/EBO and the VAO.
	glDeleteBuffers(1, &VBOv);
	glDeleteBuffers(1, &VBOn);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void Cloth::ApplyForces(glm::vec3 gravity, glm::vec3 vWind, float density, float drag) {
	//add gravity
	for (auto& particle : particles) {
		particle->f = particle->m*gravity;
	}
	for (auto& sd : springDampers) {
		sd->ApplyForce();
	}
	for (auto& triangle : triangles) {
		triangle->ApplyForce(vWind, density, drag);
	}
}

void Cloth::IntegrateMotion(float delta) {
	for (auto& particle : particles) {
		particle->ForwardIntegrate(delta);
	}
}

void Cloth::ApplyUserControls(glm::vec3 offset, glm::vec3 rot) {
	for (auto& particle : particles) {
		particle->ApplyUserControls(offset, rot);
	}
}

void Cloth::ApplyConstraints(float ground, float e, float mD) {
	for (auto& particle : particles) {
		particle->ApplyConstraints(ground, e, mD);
	}
}

void Cloth::UpdateVertices() {
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = particles[i]->r;
	}
}

void Cloth::UpdateNormals() {
	//loop through particles and zero out normal
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->n = glm::vec3(0);
	}
	//loop through triangles and add the triangle normal
	//to the normal of each of the three particles it connects
	for (int i = 0; i < triangles.size(); i++) {
		triangles[i]->ApplyNormals();
	}
	// normalize normals
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->n = glm::normalize(particles[i]->n);
		normals[i] = particles[i]->n;
	}
}