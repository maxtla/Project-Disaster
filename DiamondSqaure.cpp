#include "DiamondSqaure.h"



DiamondSqaure::DiamondSqaure()
{
}


DiamondSqaure::~DiamondSqaure()
{
}

vector<float> DiamondSqaure::createDiamondSquare(int mapSize, int initStepSize, float noise)
{
	this->mapSize = mapSize;
	this->diamondSquare.resize(this->mapSize * this->mapSize);
	int stepSize = initStepSize;
	float noiseScale = noise;

	for (int z = 0; z < this->mapSize; z += stepSize)
		for (int x = 0; x < this->mapSize; x += stepSize)
			this->setValue(x, z, this->fRand());

	while (stepSize > 1)
	{
		this->diamondSquareAlgorithm(stepSize, noiseScale);

		stepSize = stepSize / 2;
		noiseScale = noiseScale / 2;
	}

	return this->diamondSquare;
}

float DiamondSqaure::fRand()
{
	int min = -10;
	int max = 15;
	float randomNumber = (float)rand() / RAND_MAX;
	return (min + randomNumber * (max - (min)));
}

float DiamondSqaure::getValue(int x, int z)
{
	if (x < this->mapSize && x >= 0)
	{
		if (z < this->mapSize && z >= 0)
			return this->diamondSquare[x + (z * this->mapSize)];
		else
			return 0.0f;
	}
	else
		return 0.0f;
}

void DiamondSqaure::setValue(int x, int z, float value)
{
	this->diamondSquare[x + (z * this->mapSize)] = value;
}

void DiamondSqaure::diamondStep(int x, int z, int stepSize, float noise)
{
	int halfStep = stepSize / 2;

	float corner1 = this->getValue(x - halfStep, z - halfStep);
	float corner2 = this->getValue(x + halfStep, z - halfStep);
	float corner3 = this->getValue(x - halfStep, z + halfStep);
	float corner4 = this->getValue(x + halfStep, z + halfStep);

	float squareValue = ((corner1 + corner2 + corner3 + corner4) / 4) + noise;

	this->setValue(x, z, squareValue);
}

void DiamondSqaure::squareStep(int x, int z, int stepSize, float noise)
{
	if (x < this->mapSize && z < this->mapSize)
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

void DiamondSqaure::diamondSquareAlgorithm(int stepSize, float noise)
{
	int halfStep = stepSize / 2;

	for (int z = halfStep; z < this->mapSize; z += stepSize)
		for (int x = halfStep; x < this->mapSize; x += stepSize)
		{
			this->diamondStep(x, z, stepSize, this->fRand() * noise);
		}

	for (int z = 0; z < this->mapSize; z += stepSize)
		for (int x = 0; x < this->mapSize; x += stepSize)
		{
			this->squareStep(x + halfStep, z, stepSize, this->fRand() * noise);
			this->squareStep(x, z + halfStep, stepSize, this->fRand() * noise);
		}
}
