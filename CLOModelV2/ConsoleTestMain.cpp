#include <QApplication>
#include "BloombergVector.h"
#include "ConsoleTestObj.h"
#include <QtDebug>
int main(int argc, char *argv[]) {
	//QApplication a(argc, argv);
	//ConsoleTestObj a;
	//return a.exec();

	double Test = AdjustCoupon(0.05, QDate(2014, 8, 1), QDate(2014, 11, 1), DayCountConvention::ACT365);
}
