#include "BSplineTensorProductSurface.h"

#include <iostream>
#include <ostream>

#include "../Core/Helpers.h"
#include "../Core/World.h"

BSplineTensorProductSurface_::BSplineTensorProductSurface_(const glm::vec3 InPos, const glm::vec3 InSize) : WorldObject(InPos, InSize, {0,0,0}, "BSplineSurface (" + std::to_string(GameWorld.CurrentValidID + 1) + ")")
{
	ClassID = BSplineTensorProductSurfaceClassID;
}

float BSplineTensorProductSurface_::BasisFunction(const int i, const int k, const float u, std::vector<float> Knots)
{
	//check if the degree is 0
	if (k == 0)
	{
		//check if the parameter is within the range of the knot
		if (Knots[i] <= u && u < Knots[i + 1])
		{
			//return 1
			return 1;
		}

		//return 0
		return 0;
	}

	//calculate the basis function
	float Denom1 = Knots[i + k] - Knots[i];
	float Denom2 = Knots[i + k + 1] - Knots[i + 1];

	float FirstPart = 0;
	float SecondPart = 0;

	//check if the denominator is 0
	if (Denom1 != 0)
	{
		FirstPart = ((u - Knots[i]) / Denom1) * BasisFunction(i, k - 1, u, Knots);
	}

	//check if the denominator is 0
	if (Denom2 != 0)
	{
		SecondPart = ((Knots[i + k + 1] - u) / Denom2) * BasisFunction(i + 1, k - 1, u, Knots);
	}

	//return the basis function
	return FirstPart + SecondPart;
}

glm::vec3 BSplineTensorProductSurface_::GetTensorProduct(float u, float v, std::vector<std::vector<glm::vec3>> ControlPoints, std::vector<float> UKnots, std::vector<float> VKnots)
{
	//storage for the tensor product
	glm::vec3 TensorProduct = { 0,0,0 };

	//iterate through each control point
	for (int i = 0; i < ControlPoints.size(); i++)
	{
		for (int j = 0; j < ControlPoints[i].size(); j++)
		{
			//calculate the basis functions
			float BasisU = BasisFunction(i, 2, u, UKnots);
			float BasisV = BasisFunction(j, 2, v, VKnots);

			//add the tensor product
			TensorProduct += ControlPoints[i][j] * BasisU * BasisV;
		}
	}

	//return the tensor product
	return TensorProduct;
}

std::vector<Vertex> BSplineTensorProductSurface_::CreateVertices()
{
	//storage for the vertices
	std::vector<Vertex> ReturnVertices;

	//storage for the control points
	const std::vector<std::vector<glm::vec3>> ControlPoints = {
		{ {0, 0, 0}, {1, 1, 0}, {2, 0, 0} },
		{ {0, 1, 0}, {1, 2, 1}, {2, 1, 1} },
		{ {0, 0, 2}, {1, 1, 2}, {2, 0, 2} }
	};


	//storage for the knots
	const std::vector<float> UKnots = {0, 0, 0, 1, 2, 2, 2};
	const std::vector<float> VKnots = {0, 0, 0, 1, 2, 2, 2};

	//iterate through the u values
	for (float u = 0; u <= 1; u += 0.1)
	{
		//iterate through the v values
		for (float v = 0; v <= 1; v += 0.1)
		{
			//the multiplication factor
			constexpr float Multiplier = 10;

			//get the tensor product
			const glm::vec3 TensorProduct = GetTensorProduct(u, v, ControlPoints, UKnots, VKnots);

			//add the vertex
			ReturnVertices.emplace_back(glm::vec3(TensorProduct.x * Multiplier, TensorProduct.y * Multiplier, TensorProduct.z * Multiplier));
		}
	}

	//return the vertices
	return ReturnVertices;
}

std::vector<unsigned int> BSplineTensorProductSurface_::CreateIndices()
{
	//get the vertices
	const std::vector<Vertex> Vertices = CreateVertices();

	//create a vector to store the indices
	std::vector<unsigned int> Indices;
	Indices.reserve(Vertices.size());

	//iterate through the vertices in a grid 10 at a time because we are using a 10x10 grid
	for (int i = 0; i < 90; i += 10)
	{
		//iterate through the vertices in a row
		for (int j = 0; j < 10; j++)
		{
			//add the indices for the first triangle
			Indices.push_back(i + j);
			Indices.push_back(i + j + 1);
			Indices.push_back(i + j + 10);

			//add the indices for the second triangle
			Indices.push_back(i + j + 1);
			Indices.push_back(i + j + 11);
			Indices.push_back(i + j + 10);
		}
	}

	return Indices;
}


