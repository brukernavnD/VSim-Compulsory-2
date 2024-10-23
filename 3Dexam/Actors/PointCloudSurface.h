#pragma once
#include "../Core/WorldObject.h"
#include "../Core/Helpers.h"

//the class id for the point cloud surface
constexpr int PointCloudSurfaceClassID = 1;

//the delimiter used in the file to separate the x, y, and z values
static char PointCloudDelimiter = '	';

//the file path to the file containing the point cloud data
static std::string PointCloudSurfaceFilePath = "C:/School/2024 2/Visualisering og Simulering/2SPIM Compulsory 2/3Dexam/Assets/vsim_las.txt";

class PointCloudSurface_ : public WorldObject
{
public:

	//constructor(s)
	PointCloudSurface_(glm::vec3 InPos, glm::vec3 InSize);

	static std::vector<Vertex> CreateVertices();
	static std::vector<unsigned int> CreateIndices();

};
