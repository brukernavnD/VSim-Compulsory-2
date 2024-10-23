#include "PointCloudSurface.h"

#include <fstream>
#include <iostream>
#include <ostream>


#include "../Core/World.h"

PointCloudSurface_::PointCloudSurface_(glm::vec3 InPos, glm::vec3 InSize) : WorldObject(InPos, InSize, {0,0,0}, "PointCloudSurface (" + std::to_string(GameWorld.CurrentValidID + 1) + ")")
{
	ClassID = PointCloudSurfaceClassID;
}

std::vector<Vertex> PointCloudSurface_::CreateVertices()
{
	//open our point cloud file
	std::ifstream FileReader(PointCloudSurfaceFilePath);

	// Check if the file is successfully opened
    if (!FileReader.is_open()) {
	    std::cerr << "Error opening the file!";
		return {};
    }

	//storage for each line of the file
	std::string LineString;

	//storage for the vertices
	std::vector<Vertex> Vertices;

	//storage for the line number we're on
	int LineNumber = 0;

	//iterate through each line of the file
    while (getline(FileReader, LineString))
    {
		//check if this is the first line
		if (LineNumber == 0)
		{
			//increment the line number early
			LineNumber++;

			//convert the first line to an integer
			int NumVertices = std::stoi(LineString);

			//reserve space for the vertices
			Vertices.reserve(NumVertices);

			//skip further processing of this line
			continue;
		}

		//storage for the part of the line we're on
		std::vector<std::string> LineParts;

		//add the first part of the line
		LineParts.emplace_back("");
		
		//iterate through each character in the line
		for (const char CurrentChar : LineString)
		{
			//check if the character is the delimiter
			if (CurrentChar == PointCloudDelimiter)
			{
				//add a new part to the line
				LineParts.emplace_back("");
			}
			else
			{
				//add the character to the current part
				LineParts[std::max(int(LineParts.size() - 1), 0)] += CurrentChar;
			}
		}

		//create the new vertex
		Vertex NewVertex = Vertex(glm::vec3(std::stof(LineParts[0]), std::stof(LineParts[2]), std::stof(LineParts[1])));

		//add the new vertex to the vertices
		Vertices.emplace_back(NewVertex);

		//increment the line number
		LineNumber++;
    }

	//storage for the point closest to the origin
	glm::vec3 ClosestPoint = glm::vec3(FLT_MAX);

	//iterate through the vertices and find the closest point to the origin
	for (const Vertex& CurrentVertex : Vertices)
	{
		//check if the current vertex is closer to the origin than the current closest point
		if (glm::length(CurrentVertex.Position) < glm::length(ClosestPoint))
		{
			//set the closest point to the current vertex
			ClosestPoint = CurrentVertex.Position;
		}
	}

	//iterate through the vertices and move them so the closest point is at the origin
	for (Vertex& CurrentVertex : Vertices)
	{
		//move the vertex
		CurrentVertex.Position -= ClosestPoint;
	}
 
    // Close the file
    FileReader.close();
    return Vertices;
}

std::vector<unsigned int> PointCloudSurface_::CreateIndices()
{
	//get the vertices
	const std::vector<Vertex> Vertices = CreateVertices();

	//create a vector to store the indices
	std::vector<unsigned int> Indices;
	Indices.reserve(Vertices.size());

	//loop through the vertices and add the index to the indices
	for (unsigned int Index = 0; Index < Vertices.size(); Index++)
	{
		//add the index to the indices
		Indices.emplace_back(Index);
	}

	return Indices;
}


