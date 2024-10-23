#pragma once
#include "../Core/WorldObject.h"
#include "../Core/Helpers.h"

//the class id for the bspline surface class (used for rendering)
constexpr int BSplineTensorProductSurfaceClassID = 2;

//the bspline surface class
class BSplineTensorProductSurface_ : public WorldObject
{
public:

	//constructor(s)
	BSplineTensorProductSurface_(glm::vec3 InPos, glm::vec3 InSize);

private:

	//helper function for the basis function of the bspline surface
	static float BasisFunction(int i, int k, float u, std::vector<float> Knots);

	//helper function to get the tensor product at a given u, v
	static glm::vec3 GetTensorProduct(float u, float v, std::vector<std::vector<glm::vec3>> ControlPoints, std::vector<float> UKnots, std::vector<float> VKnots);

public:

	static std::vector<Vertex> CreateVertices();
	static std::vector<unsigned int> CreateIndices();

};
