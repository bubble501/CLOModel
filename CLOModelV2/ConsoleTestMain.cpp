#include <QApplication>
#include "BloombergVector.h"
#include "ConsoleTestObj.h"
#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>
#include "MtgCalculator.h"
#include "CommonFunctions.h"
#include "Waterfall.h"
#include "StressTest.h"
#include "ExcelOutput.h"
#include "BloombergVector.h"
#include "BaseRateVect.h"
#include "BaseRateTable.h"
#include "QBbgRequest.h"
#include "QBbgWorker.h"
#include <QTime>
#include <QTextStream>
#include "DateTrigger.h"
#include "PoolSizeTrigger.h"
int main(int argc, char *argv[]) {
	//QApplication a(argc, argv);
	//ConsoleTestObj a;
	//return a.exec();

	Waterfall TempWtf, TempCallWaterfall;
	QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/.BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return 1;
	}
	{bool Junk; out >> Junk; }
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	file.close();
	TempWtf.SetTrigger(1, QSharedPointer<AbstractTrigger>(new PoolSizeTrigger(75000000.0, PoolSizeTrigger::TriggerSide::SmallerOrEqual, "Test Trigger")));
	TempWtf.CalculateTranchesCashFlows();
	
}
