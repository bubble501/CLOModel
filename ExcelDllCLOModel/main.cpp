#include "mvNormSampler.h"
#include <QtGui>
#include <QtCore>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	mvNormSampler DistrTest(2U);
	std::vector<double> VarMatrix(4);
	VarMatrix[0]=3;
	VarMatrix[1]=1;
	VarMatrix[2]=1;
	VarMatrix[3]=3;
	DistrTest.SetVarCovMatrix(VarMatrix);
	DistrTest.ExtractSample();
	return a.exec();
}
