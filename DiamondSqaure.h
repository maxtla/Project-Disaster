#pragma once
#ifndef DIAMONDSQUARE_H
#define DIAMONDSQUARE_H
#include <vector>
using namespace std;
class DiamondSqaure
{
public:
	DiamondSqaure();
	~DiamondSqaure();

	vector<float> createDiamondSquare(int mapSize, int stepSize, float noise);

private:
	int mapSize;
	vector<float> diamondSquare;

	float fRand();
	float getValue(int x, int z);
	void setValue(int x, int z, float value);
	void diamondStep(int x, int z, int stepSize, float noise);
	void squareStep(int x, int z, int stepSize, float noise);
	void diamondSquareAlgorithm(int stepSize, float noise);
};
#endif // !DIAMONDSQUARE_H

