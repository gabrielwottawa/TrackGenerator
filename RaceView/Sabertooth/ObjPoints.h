#pragma once
#include <string>
#include <vector>
#include <glm\glm.hpp>

using namespace std;
using namespace glm;

class ObjPoints
{
public:
	static vector<vec3*> read(string filename);
};