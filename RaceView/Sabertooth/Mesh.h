#pragma once
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "Group.h"
#include "Material.h"

using namespace std;
using namespace glm;

class Mesh	
{

private:
	vector<Group*> groups;
	vector<vec3*> vertices;
	vector<vec3*> normais;
	vector<vec2*> textures;
	map<string, Material*> materials;

public:
	Mesh();
	~Mesh();
	vec3* vertice(int id);
	vec3* normal(int id);
	vec2* texture(int id);
	int addVertice(vec3*);
	int addNormal(vec3*);
	int addTexture(vec2*);
	int addGroup(Group*);
	int Mesh::addMaterial(string id, Material* material);
	
	vector<Group*> getGroups() {
		return groups;
	}

	Material* getMaterial(string id) {
		return materials[id];
	}
};