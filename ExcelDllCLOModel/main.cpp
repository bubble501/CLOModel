#include "StressTest.h"
#include <QtGui>
#include <QtCore>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTableWidget Tabella;
	QStringList Xhead;
	QStringList Yhead;
		StressTest w;
		w.LoadResultsFromFile("Z:/24AM/ABS Trade Folders/CLO/MERCT I-X/.StressResult01.fcsr");
		
		Xhead=w.GetXSpann();
		Yhead=w.GetYSpann();
		const QHash<QString,QHash<QString,Waterfall> >& Risultati=w.GetResults();
	Tabella.setRowCount(Xhead.size());
	Tabella.setColumnCount(Yhead.size());
	Tabella.setHorizontalHeaderLabels(Yhead);
	Tabella.setVerticalHeaderLabels(Xhead);
	int countX=0,  countY=0;
	for(QStringList::const_iterator i=Xhead.begin();i!=Xhead.end();i++){
		for(QStringList::const_iterator j=Yhead.begin();j!=Yhead.end();j++){
			Tabella.setItem(countX,countY,new QTableWidgetItem(QString::number(
				Risultati.value(*i).value(*j).GetTranche("MERCT I-X B2")->GetLossRate()
				*100.0,'f',2)));
			countY++;
		}
		countY=0;
		countX++;
	}
	Tabella.show();
	return a.exec();
}
