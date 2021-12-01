#include "TrackWriter.h"

string OBJECT_PATH = "../../RaceView/Sabertooth/Objects/";

void TrackWriter::GenerateTrackObj(vector<float> points, vector<float> center_array, vector<float> internal_array, vector<float> external_array) {
	
	float GLOBAL_SCALE = 0.05f;
	float HEIGHT_SCALE = 0.05f;

	writeTxt(center_array, points, GLOBAL_SCALE, HEIGHT_SCALE);
	writeMtl();
	wirteObj(internal_array, GLOBAL_SCALE, HEIGHT_SCALE, external_array);
}

void TrackWriter::writeTxt(std::vector<float>& center_array, std::vector<float>& points, float GLOBAL_SCALE, float HEIGHT_SCALE)
{
	ofstream center(OBJECT_PATH + "pista_coords.txt");
	float temp = center_array.size() / ((float)points.size() / 3);
	for (int i = 0; i < center_array.size() - temp; i += 3) {
		center << center_array[i] * GLOBAL_SCALE << " " << center_array[i + 2] * HEIGHT_SCALE << " " << center_array[i + 1] * GLOBAL_SCALE << endl;
	}
	center.close();
}

void TrackWriter::writeMtl()
{
	ofstream trackMtl(OBJECT_PATH + "pista.mtl");
	trackMtl << "newmtl " << "pista" << endl;
	trackMtl << "Ka 0.5 0.5 0.5" << endl;
	trackMtl << "Kd 1.0 1.0 1.0" << endl;
	trackMtl << "Ks 0.0 0.0 0.0" << endl;
	trackMtl << "Ns 80.0" << endl;
	trackMtl << "map_Kd " << "Objects/pista.jpg" << endl;
	trackMtl.close();
}

void TrackWriter::wirteObj(std::vector<float>& internal_array, float GLOBAL_SCALE, float HEIGHT_SCALE, std::vector<float>& external_array)
{
	ofstream trackObj(OBJECT_PATH + "pista.obj");
	trackObj << "mtllib " << OBJECT_PATH << "pista.mtl" << endl;
	trackObj << "g " << "pista" << endl;
	trackObj << "usemtl " << "pista" << endl;

	trackObj << "vn 0.0 1.0 0.0" << endl;
	trackObj << "vt 0.0 0.0" << endl;
	trackObj << "vt 0.0 1.0" << endl;
	trackObj << "vt 1.0 0.0" << endl;
	trackObj << "vt 1.0 1.0" << endl;

	int size = internal_array.size();
	int vertices_size = size / 3;
	for (int i = 0; i < size; i += 3) {
		trackObj << "v " << (internal_array[i] * GLOBAL_SCALE) << " " << internal_array[i + 2] * HEIGHT_SCALE << " " << (internal_array[i + 1] * GLOBAL_SCALE) << endl;
	}
	for (int i = 0; i < size; i += 3) {
		trackObj << "v " << (external_array[i] * GLOBAL_SCALE) << " " << external_array[i + 2] * HEIGHT_SCALE << " " << (external_array[i + 1] * GLOBAL_SCALE) << endl;
	}

	for (int i = 1; i <= size / 3 - 3; i++) {
		trackObj << "f " << i << "/1/1 " << (i + 1) << "/2/1 " << i + vertices_size << "/4/1" << endl;
		trackObj << "f " << i + vertices_size << "/4/1 " << (i + 1) << "/2/1 " << i + 1 + vertices_size << "/3/1" << endl;
	}

	trackObj.close();
}