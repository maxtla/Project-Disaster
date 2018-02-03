#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "DiamondSquare.h"
#include <iostream>

int main()
{
	srand(time(NULL));
	float* heightMap = nullptr;
	int width = 17;
	DiamondSquare dS;
	heightMap = dS.createDiamondSquare(17, 17, 16, 1.0f);

	for (int y = 0; y < width; y++)
	{
		std::cout << "-------------------------------------------------------------------------------------" << std::endl;
		for (int x = 0; x < width; x++)
		{
			std::cout << "| " << heightMap[x + (y * width)] << " | ";
		}
		std::cout << std::endl;
	}
	system("pause");
	return(0);
}