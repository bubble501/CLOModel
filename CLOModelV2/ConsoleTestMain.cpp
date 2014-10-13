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
#include "StressViewer.h"
int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	StressViewer b;
	b.LoadStress("C:/Temp/.StressResult.fcsr");
	b.show();
	return a.exec();

	/*Waterfall TempWtf, TempCallWaterfall;
	QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/.BaseCase.clo");
	//QFile file("C:/Temp/.SavedInputs.clo");
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
	TempCallWaterfall.CalculateTranchesCashFlows();*/

	/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	RegisterAsMetaType<Waterfall>();
	RegisterAsMetaType<MtgCashFlow>();

	Tranche TeResult = *( StressTest::GetScenarioFromFile(
		"Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/.StressResult.fcsr"
		, "20", "0.5", "0", "0", "0", "0"
		).GetTranche("HARVT 10X A"));
	return a.exec();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
}
