#include "Joint.h"

int Joint::currIndex = 0;
std::vector<Joint*> Joint::joints = std::vector<Joint*>();
std::vector<DOF*> Joint::allDofs = std::vector<DOF*>();

Joint::Joint() {
	world = glm::mat4(1);
	local = glm::mat4(1);
	offset = glm::vec3();
}

bool Joint::Load(Tokenizer& token) {
	joints.push_back(this); // add current joint to ordered list of joints
	char name_ptr[256];
	token.GetToken(name_ptr);
	name = name_ptr;
	if (name == "balljoint") {
		name = "root";
	}

	token.FindToken("{");

	for (int i = 0; i < dofs.size(); i++) {
		dofs[i] = new DOF();
		allDofs.push_back(dofs[i]);
	}

	glm::vec3 boxmin = glm::vec3(-0.2,-0.2,-0.2);
	glm::vec3 boxmax = glm::vec3(0.2,0.2,0.2);
	glm::vec3 pose = glm::vec3(0,0,0);
	//for (int i = 0; i<10; i++){
	while (true) {
		char temp[256];
		token.GetToken(temp);
		if (strcmp(temp, "offset") == 0) {
			offset.x = token.GetFloat();
			offset.y = token.GetFloat();
			offset.z = token.GetFloat();
		}
		else if (strcmp(temp, "boxmin") == 0) {
			boxmin.x = token.GetFloat();
			boxmin.y = token.GetFloat();
			boxmin.z = token.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0) {
			boxmax.x = token.GetFloat();
			boxmax.y = token.GetFloat();
			boxmax.z = token.GetFloat();
		}
		else if (strcmp(temp, "pose") == 0) {
			pose.x = token.GetFloat();
			pose.y = token.GetFloat();
			pose.z = token.GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0) {
			dofs[0]->SetMin(token.GetFloat());
			dofs[0]->SetMax(token.GetFloat());
		}
		else if (strcmp(temp, "rotylimit") == 0) {
			dofs[1]->SetMin(token.GetFloat());
			dofs[1]->SetMax(token.GetFloat());
		}
		else if (strcmp(temp, "rotzlimit") == 0) {
			dofs[2]->SetMin(token.GetFloat());
			dofs[2]->SetMax(token.GetFloat());
		}
		else if (strcmp(temp, "balljoint") == 0) {
			Joint* jnt = new Joint;
			jnt->Load(token);
			AddChild(jnt);
		}
		else if (strcmp(temp, "}") == 0) {
			break;
		}
		else token.SkipLine(); // Unrecognized token
	}

	//set DOF values (after setting limits so clamping happens correctly
	for (int i = 0; i < dofs.size(); i++) {
		dofs[i]->SetValue(pose[i]);
	}

	constructLocal();
	box = new Cube(boxmin, boxmax);
	return true;
}

void Joint::constructLocal() {
	glm::mat4 xRot = glm::rotate(glm::mat4(1), dofs[0]->val, glm::vec3(1, 0, 0));
	glm::mat4 yRot = glm::rotate(glm::mat4(1), dofs[1]->val, glm::vec3(0, 1, 0));
	glm::mat4 zRot = glm::rotate(glm::mat4(1), dofs[2]->val, glm::vec3(0, 0, 1));
	glm::mat4 tran = glm::translate(glm::mat4(1), offset);
	local = tran * zRot * yRot * xRot;
}

void Joint::AddChild(Joint* child) {
	children.push_back(child);
}

void Joint::Update(const glm::mat4& parent, bool updateLocal) {
	if (updateLocal) {
		constructLocal(); // TODO this is kinda bad we dont really wanna do this
	}
	world = parent * local;
	for (auto* child : children) {
		child->Update(world, updateLocal);
	}
	return;
}

void Joint::Draw(GLuint shader, const glm::mat4& view_mat) {
	box->model = world;
	box->draw(view_mat, shader);
	for (auto* child : children) {
		child->Draw(shader, view_mat);
	}
	return;
}

void Joint::MakeImGui() {
	//outputs name and interactive slider for the joint
	//ImGui::SetWindowFontScale(2.0f);
	std::string axes[3] = { "x", "y", "z"};
	ImGui::Text(name.c_str());
	for (int i = 0; i < dofs.size(); i++) {

		if (dofs[i]->minVal != dofs[i]->maxVal) {
			std::string slider_name = name + "_" + axes[i];
			ImGui::SliderFloat(slider_name.c_str(), &dofs[i]->val, dofs[i]->minVal, dofs[i]->maxVal);
		}
	}
	for (auto* child : children) {
		child->MakeImGui();
	}
}

//Joint* Joint::GetJoint(int offset) {
//	if (offset == 0) {
//		return this;
//	}
//	else {
//		for (auto& child : children) {
//			return child->GetJoint(offset-1);
//		}
//	}
//}

//void Joint::DFSUtil(int v, bool visited[])
//{

Joint::~Joint() {
	delete box;
	for (auto* dof : dofs) {
		delete dof;
	}
	for (auto* child : children) {
		delete child;
	}
	return;
}