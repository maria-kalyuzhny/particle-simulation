#ifndef _JOINT_H_
#define _JOINT_H_
#include "core.h"
#include "Tokenizer.h"
#include "Cube.h"
#include "DOF.h"
#include <array>

class Joint
{
public:
	static int currIndex;
	static std::vector<Joint*> joints; // = std::vector<Joint*>();
	static std::vector<DOF*> allDofs;
	glm::mat4 local;
	glm::mat4 world;
	Cube* box;
	std::string name;
	glm::vec3 offset;
	std::array<DOF*,3> dofs;
	std::vector<Joint*> children;
	bool visited = false;
	Joint();
	~Joint();
	bool Load(Tokenizer& t);
	void constructLocal();
	void AddChild(Joint * child);
	void Update(const glm::mat4& parent, bool updateLocal);
	void Draw(GLuint shader, const glm::mat4& view_mat);
	void MakeImGui(); 
	//Joint * GetJoint(int offset);
};

#endif
