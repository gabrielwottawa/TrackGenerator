#include "ObjReader.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Mesh* ObjReader::read(string filename)
{
	Mesh* mesh = new Mesh();

	int firstGroup = 1;
	Group* group = new Group();
	group->setName("default");

	ifstream arq(filename);

	if (!arq) {
		exit(EXIT_FAILURE);
	}

	while (!arq.eof()) {
		string line;
		getline(arq, line);

		stringstream sline(line);

		string temp;
		sline >> temp;

		if (temp.empty() || temp == "#") {
			continue;
		}

		if (temp == "mtllib") {
			material(mesh, sline);
		} else if (temp == "v") {
			vertice(mesh, sline);
		}
		else if (temp == "vn") {
			normal(mesh, sline);
		}
		else if (temp == "vt") {
			texture(mesh, sline);
		}
		else if (temp == "f") {
			face(group, sline);
		}
		else if (temp == "g") {
			if (firstGroup == 1) {
				firstGroup = 0;
			}
			else {
				mesh->addGroup(group);
				group = new Group();
			}

			string name;
			sline >> name;
			group->setName(name);
		}
		else if (temp == "usemtl") {
			usemtl(group, sline);
		}
	}

	arq.close();

	mesh->addGroup(group);
	return mesh;
}

void ObjReader::material(Mesh* mesh, stringstream& sline) {
	string path;
	sline >> path;
	readMtl(mesh, path);
}

void ObjReader::vertice(Mesh* mesh, stringstream &sline) {
	float x, y, z;
	sline >> x >> y >> z;
	mesh->addVertice(new glm::vec3(x, y, z));
}

void ObjReader::normal(Mesh* mesh, stringstream& sline) {
	float x, y, z;
	sline >> x >> y >> z;
	mesh->addNormal(new glm::vec3(x, y, z));
}

void ObjReader::texture(Mesh* mesh, stringstream& sline) {
	float x, y;
	sline >> x >> y;
	mesh->addTexture(new glm::vec2(x, y));
}

int getNextTokenValue(stringstream& stoken) {
	string temp;
	getline(stoken, temp, '/');
	return temp.empty() ? 0 : stoi(temp);
}

void ObjReader::face(Group* group, stringstream& sline) {
	Face* face = new Face();

	string token;
	while (getline(sline, token, ' ')) {

		if (token.empty()) {
			continue;
		}

		stringstream stoken(token);

		int v = getNextTokenValue(stoken);
		int t = getNextTokenValue(stoken);
		int n = getNextTokenValue(stoken);

		face->push(v, n, t);
	}

	group->addFace(face);
}

void ObjReader::usemtl(Group* group, stringstream& sline) {
	string name;
	sline >> name;
	group->setMaterial(name);
}

//Mtl reader
void ObjReader::readMtl(Mesh* mesh, string filename)
{
	int firstMtl = 1;
	Material* material = new Material();

	ifstream arq(filename);

	if (!arq) {
		exit(EXIT_FAILURE);
	}

	while (!arq.eof()) {
		string line;
		getline(arq, line);

		stringstream sline(line);

		string temp;
		sline >> temp;

		if (temp.empty() || temp == "#") {
			continue;
		}

		if (temp == "newmtl") {
			if (firstMtl == 1) {
				firstMtl = 0;
			}
			else {
				mesh->addMaterial(material->getID(), material);
				material = new Material();
			}

			id(material, sline);
		}
		else if (temp == "Ka") {
			ambient(material, sline);
		}
		else if (temp == "Kd") {
			diffuse(material, sline);
		}
		else if (temp == "Ks") {
			specular(material, sline);
		}
		else if (temp == "Ns") {
			shininess(material, sline);
		}
		else if (temp == "map_Kd") {
			textureMtl(material, sline);
		}
	}

	arq.close();

	mesh->addMaterial(material->getID(), material);
}

void ObjReader::id(Material* material, stringstream& sline) {
	string id;
	sline >> id;
	material->setID(id);
}

void ObjReader::ambient(Material* material, stringstream& sline) {
	float x, y, z;
	sline >> x >> y >> z;
	material->setAmbient(new glm::vec3(x, y, z));
}

void ObjReader::diffuse(Material* material, stringstream& sline) {
	float x, y, z;
	sline >> x >> y >> z;
	material->setDiffuse(new glm::vec3(x, y, z));
}

void ObjReader::specular(Material* material, stringstream& sline) {
	float x, y, z;
	sline >> x >> y >> z;
	material->setSpecular(new glm::vec3(x, y, z));
}

void ObjReader::shininess(Material* material, stringstream& sline) {
	float shininess;
	sline >> shininess;
	material->setShininess(shininess);
}

void ObjReader::textureMtl(Material* material, stringstream& sline) {
	string texture;
	sline >> texture;
	material->setTexture(texture);
}