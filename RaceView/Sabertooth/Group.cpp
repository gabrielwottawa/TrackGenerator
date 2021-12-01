#include "Group.h"

Group::Group() {}

Group::~Group() {
	faces.clear();
}

int Group::addFace(Face* face) {
	faces.push_back(face);
	return faces.size() - 1;
}