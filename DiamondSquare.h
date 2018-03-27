#ifndef DIAMONDSQUARE_H
#define DIAMONDSQUARE_H


class DiamondSquare
{
private:
	float* diamondSquare;
	int width;
	int height;

	float fRand();
	float getValue(int x, int z);
	void setValue(int x, int z, float value);
	void diamondStep(int x, int z, int stepSize, float noise);
	void squareStep(int x, int z, int stepSize, float noise);
	void diamondSquareAlgorithm(int stepSize, float noiseScale);
public:
	DiamondSquare();
	virtual ~DiamondSquare();
	float* createDiamondSquare(int width, int height, int initialStepSize, float noiseScaleValue);
};

#endif