#pragma once
#include <string>
#include "Mesh.h"
#include "Group.h"
#include "Material.h"

using namespace std;

class ObjReader
{
public:
	// Obj
	static Mesh* read(string filename);

	static void material(Mesh* mesh, stringstream& sline);
	static void vertice(Mesh* mesh, stringstream& sline);
	static void normal(Mesh* mesh, stringstream& sline);
	static void texture(Mesh* mesh, stringstream& sline);
	static void face(Group* group, stringstream& sline);
	static void usemtl(Group* group, stringstream& sline);

	// Mtl
	static void readMtl(Mesh* mesh, string filename);

	static void id(Material* material, stringstream& sline);
	static void ambient(Material* material, stringstream& sline);
	static void diffuse(Material* material, stringstream& sline);
	static void specular(Material* material, stringstream& sline);
	static void shininess(Material* material, stringstream& sline);
	static void textureMtl(Material* material, stringstream& sline);
};