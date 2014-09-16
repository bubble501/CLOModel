#ifndef CONSOLETESTOBJ_H
#define CONSOLETESTOBJ_H

#include <QObject>
#include "CentralUnit.h"
class ConsoleTestObj : public QObject
{
	Q_OBJECT

public:
	ConsoleTestObj(QObject *parent=0);
	void BBVALTest();
private slots:
	void PrintOutput();
private:
	CentralUnit TempUnit;
};

#endif // CONSOLETESTOBJ_H
