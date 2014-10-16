#ifndef CONSOLETESTOBJ_H
#define CONSOLETESTOBJ_H
#include <QFile>
#include <QObject>
#include <QDebug>
#include "CentralUnit.h"
class ConsoleTestObj : public QObject
{
	Q_OBJECT

public:
	ConsoleTestObj(QObject *parent=0);
	void BBVALTest();
	void SlateTest();
	void TestStressTest();
	void TestApplier();
private slots:
	void PrintOutput();
	void ConsoleProgress(double pr);
private:
	CentralUnit TempUnit;
	StressTest* TempStress;
	ScenarioApplier* Tested;
};

#endif // CONSOLETESTOBJ_H
