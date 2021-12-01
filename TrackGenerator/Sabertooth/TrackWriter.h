#include <fstream>
#include <vector>

using namespace std;

#pragma once
class TrackWriter
{
public:
	void GenerateTrackObj(vector<float> points, vector<float> center_array, vector<float> internal_array, vector<float> external_array);

private:
	void writeMtl();
	void writeTxt(std::vector<float>& center_array, std::vector<float>& points, float GLOBAL_SCALE, float HEIGHT_SCALE);
	void wirteObj(std::vector<float>& internal_array, float GLOBAL_SCALE, float HEIGHT_SCALE, std::vector<float>& external_array);
};