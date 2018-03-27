#include "DiamondSquare.h"
#include <stdlib.h>

float DiamondSquare::fRand()
{
	int min = -1;
	int max = 1;
	float randomNumber = (float)rand() / RAND_MAX;
	return (min + randomNumber * (max - (min)));
}

float DiamondSquare::getValue(int x, int z)
{
	if (x < this->width && x >= 0)
	{
		if (z < this->height && z >= 0)
			return this->diamondSquare[x + (z * this->width)];
		else
			return 0.0f;
	}
	else
		return 0.0f;
}

void DiamondSquare::setValue(int x, int z, float value)
{
	this->diamondSquare[x + (z * this->width)] = value;
}

// x, z coordinates are the center of a square
void DiamondSquare::diamondStep(int x, int z, int stepSize, float noise)
{
	int halfStep = stepSize / 2;

	float corner1 = this->getValue(x - halfStep, z - halfStep);
	float corner2 = this->getValue(x + halfStep, z - halfStep);
	float corner3 = this->getValue(x - halfStep, z + halfStep);
	float corner4 = this->getValue(x + halfStep, z + halfStep);

	float squareValue = ((corner1 + corner2 + corner3 + corner4) / 4) + noise;

	this->setValue(x, z, squareValue);
}

// x, z coordinates are the center of a diamond
void DiamondSquare::squareStep(int x, int z, int stepSize, float noise)
{
	if (x < this->width && z < this->height)
	{
		int halfStep = stepSize / 2;

		float corner1 = this->getValue(x - halfStep, z);
		float corner2 = this->getValue(x + halfStep, z);
		float corner3 = this->getValue(x, z - halfStep);
		float corner4 = this->getValue(x, z + halfStep);

		float diamondValue = ((corner1 + corner2 + corner3 + corner4) / 4) + noise;

		this->setValue(x, z, diamondValue);
	}
}

void DiamondSquare::diamondSquareAlgorithm(int stepSize, float noiseScale)
{
	int halfStep = stepSize / 2;

	for (int z = halfStep; z < this->height; z += stepSize)
		for (int x = halfStep; x < this->width; x += stepSize)
		{
			this->diamondStep(x, z, stepSize, this->fRand() * noiseScale);
		}

	for (int z = 0; z < this->height; z += stepSize)
		for (int x = 0; x < this->width; x += stepSize)
		{
			this->squareStep(x + halfStep, z, stepSize, this->fRand() * noiseScale);
			this->squareStep(x, z + halfStep, stepSize, this->fRand() * noiseScale);
		}
}

DiamondSquare::DiamondSquare()
{
	this->diamondSquare = nullptr;
	this->width = 0;
	this->height = 0;
}

DiamondSquare::~DiamondSquare()
{
	delete[] this->diamondSquare;
}

/*
* Requires srand in main function to work properly
*/
float* DiamondSquare::createDiamondSquare(int width, int height, int initialStepSize, float noiseScaleValue)
{
	this->width = width;
	this->height = height;
	this->diamondSquare = new float[this->width * this->height];
	int stepSize = initialStepSize;
	float noiseScale = noiseScaleValue;

	for (int z = 0; z < this->height; z += stepSize)
		for (int x = 0; x < this->width; x += stepSize)
			this->setValue(x, z, this->fRand());

	while (stepSize > 1)
	{
		this->diamondSquareAlgorithm(stepSize, noiseScale);

		stepSize = stepSize / 2;
		noiseScale = noiseScale / 2;
	}

	return this->diamondSquare;
}