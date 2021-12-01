#pragma once
#include <vector>

using namespace std;

class Face
{
private:
	vector<int> vertices;
	vector<int> normais;
	vector<int> textures;

public:
	Face();
	~Face();
	int addVertice(int v);
	void push(int v, int n, int t);

	vector<int> getVertices() {
		return vertices;
	}

	vector<int> getNormais() {
		return normais;
	}

	vector<int> getTextures() {
		return textures;
	}
};