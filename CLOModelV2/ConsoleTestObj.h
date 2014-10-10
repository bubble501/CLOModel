#ifndef CONSOLETESTOBJ_H
#define CONSOLETESTOBJ_H
#include <QFile>
#include <QObject>
#include "CentralUnit.h"
class ConsoleTestObj : public QObject
{
	Q_OBJECT

public:
	ConsoleTestObj(QObject *parent=0);
	void BBVALTest();
	void SlateTest();
	void TestStressTest();
private slots:
	void PrintOutput();
private:
	CentralUnit TempUnit;
	StressTest* TempStress;
};

#endif // CONSOLETESTOBJ_H
