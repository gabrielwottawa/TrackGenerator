#pragma once
#include <vector>
#include <string>
#include "Face.h"
#include <GL\glew.h>

using namespace std;

class Group
{
private:
	GLuint VAO;
	int numVertices;
	string name;
	string material;
	vector<Face*> faces;

public:
	Group();
	~Group();
	int addFace(Face* face);

	vector<Face*> getFaces() {
		return faces;
	}

	GLuint& getVAO() {
		return VAO;
	}

	void setVAO(GLuint* v) {
		VAO = *v;
	}

	int getNumVertices() {
		return numVertices;
	}

	void increaseNumVertices() {
		numVertices++;
	}
	
	void setName(string n) {
		name = n;
	}

	void setMaterial(string n) {
		material = n;
	}

	string getName() {
		return name;
	}

	string getMaterial() {
		return material;
	}
};