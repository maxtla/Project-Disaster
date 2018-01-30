#ifndef DIAMONDSQUARE_H
#define DIAMONDSQUARE_H


class DiamondSquare
{
private:
	float* diamondSquare;
	int width;
	int height;

	float fRand();
	float getValue(int x, int y);
	void setValue(int x, int y, float value);
	void diamondStep(int x, int y, int stepSize, float noise);
	void squareStep(int x, int y, int stepSize, float noise);
	void diamondSquareAlgorithm(int stepSize, float noiseScale);
public:
	DiamondSquare();
	virtual ~DiamondSquare();
	float* createDiamondSquare(int width, int height, int initialStepSize, float noiseScaleValue);
};

#endif