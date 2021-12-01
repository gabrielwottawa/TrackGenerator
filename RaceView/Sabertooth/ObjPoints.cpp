#include "ObjPoints.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

vector<vec3*> ObjPoints::read(string filename)
{
	vector<vec3*> points;

	ifstream arq(filename);

	if (!arq) {
		exit(EXIT_FAILURE);
	}

	while (!arq.eof()) {
		string line;
		getline(arq, line);

		stringstream sline(line);

		float x, y, z;
		sline >> x >> y >> z;
		points.push_back(new glm::vec3(x, y, z));
	}

	arq.close();

	return points;
}