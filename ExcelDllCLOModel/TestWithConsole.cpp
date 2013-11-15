#include "mvNormSampler.h"
int main(int argc, char* argv[])
{
	mvNormSampler DistrTest(2U);
	std::vector<double> VarMatrix(4);
	VarMatrix[0]=3;
	VarMatrix[1]=1;
	VarMatrix[2]=1;
	VarMatrix[3]=3;
	DistrTest.SetVarCovMatrix(VarMatrix);
	std::vector<double> TempCoords;
	TempCoords.push_back(1.0);
	TempCoords.push_back(0.5);
	double Result=DistrTest.GetDensity(TempCoords);
	return 0;
}

