#include "StressTest.h"
#include "Mortgage.h"
#include "CommonFunctions.h"
#include "StressThread.h"
#include "Waterfall.h"
#include "ProgressWidget.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include "QuaZip\JlCompress.h"
#include <QMessageBox>
#ifdef Q_WS_WIN
#include <Windows.h>
#endif
#ifdef PrintExecutionTime
#include <QMessageBox>
#endif
StressTest::StressTest(QObject* parent)
	:QObject(parent)
	,SequentialComputation(false)
	,SentBees(0)
	,BeesReturned(0)
	,ContinueCalculation(false)
	,ProgressForm(NULL)
{
	StressDimension[0]=ChangingCDR;
	StressDimension[1]=ChangingLS;
}
StressTest::~StressTest(){
	ResetLoans();
	if(ProgressForm) ProgressForm->deleteLater();
}
const Mortgage* StressTest::GetLoans(int index)const{
	if(index<0 || index>=Loans.size()) return NULL;
	return Loans.at(index);
}
void StressTest::SetXSpann(const QList<QString>& a){
	XSpann.clear();
	AddXSpann(a);
}
void StressTest::AddXSpann(const QList<QString>& a){
	foreach(const QString& b,a){
		AddXSpann(b);
	}
}
void StressTest::AddXSpann(const QString& a){
	if(ValidBloombergVector(a))
		XSpann.append(a);
}
void StressTest::SetYSpann(const QList<QString>& a){
	YSpann.clear();
	AddYSpann(a);
}
void StressTest::AddYSpann(const QList<QString>& a){
	foreach(const QString& b,a){
		AddYSpann(b);
	}
}
void StressTest::AddYSpann(const QString& a){
	if(ValidBloombergVector(a))
		YSpann.append(a);
}
void StressTest::SetConstantPar(const QString& a){
	if(ValidBloombergVector(a))
		ConstantPar=a;
}
void StressTest::AddLoan(const Mortgage& a){
	Loans.append(new Mortgage(a));
}
void StressTest::ResetLoans(){
	for(QList<Mortgage*>::iterator i=Loans.begin();i!=Loans.end();i++){
		delete (*i);
	}
	Loans.clear();
}
void StressTest::RunStressTest(){
#ifdef PrintExecutionTime
	ExecutionTime.start();
#endif
	ProgressForm=new ProgressWidget;
	connect(ProgressForm,SIGNAL(Cancelled()),this,SLOT(StopCalculation()));
	ProgressForm->SetValue(0);
	ProgressForm->SetMax(XSpann.size()*YSpann.size());
	ProgressForm->show();
	ContinueCalculation=true;
	BeesReturned=0;
	if(SequentialComputation){
		for(int i=0;i<XSpann.size();i++){
			for(int j=0;j<YSpann.size();j++){
				if(!ContinueCalculation) return;
				CalculateScenario(i,j);
				ProgressForm->SetValue((i*(j+1))+j+1);
				QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			}
		}
	}
	else{
		for(SentBees=0;SentBees<XSpann.size()*YSpann.size() && SentBees<QThread::idealThreadCount();SentBees++){
			StressThread* WorkingThread=new StressThread(
				SentBees/YSpann.size()
				,SentBees%YSpann.size()
				,XSpann.at(SentBees/YSpann.size())
				,YSpann.at(SentBees%YSpann.size())
				,ConstantPar
				,Loans
				,Structure
				,StartDate
				,StressDimension[0]
				,StressDimension[1]
				,this
			);
			connect(WorkingThread,SIGNAL(ScenarioCalculated(int,int,Waterfall)),this,SLOT(RecievedData(int,int,Waterfall)));
			connect(WorkingThread,SIGNAL(ScenarioCalculated(int,int,Waterfall)),WorkingThread,SLOT(stop()),Qt::QueuedConnection);
			WorkingThread->start();
		}
	}
}
void StressTest::CalculateScenario(int XDim,int YDim){
	MtgCashFlow TotalFlow;
	Waterfall LocalStructure(Structure);
	for(QList<Mortgage*>::iterator i=Loans.begin();i!=Loans.end();i++){
		if(StressDimension[0]==ChangingCDR){
			if(StressDimension[1]==ChangingLS){
				(*i)->CalculateCashFlows(ConstantPar,XSpann.at(XDim),YSpann.at(YDim),StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCDR(XSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetLS(YSpann.at(YDim));
			}
			else {
				(*i)->CalculateCashFlows(YSpann.at(YDim),XSpann.at(XDim),ConstantPar,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(YSpann.at(YDim));
				LocalStructure.GetReinvestmentTest().SetCDR(XSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
			}
			
		}
		else if(StressDimension[0]==ChangingLS){
			if(StressDimension[1]==ChangingCDR){
				(*i)->CalculateCashFlows(ConstantPar,YSpann.at(YDim),XSpann.at(XDim),StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCDR(YSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetLS(XSpann.at(YDim));
			}
			else {
				(*i)->CalculateCashFlows(YSpann.at(YDim),ConstantPar,XSpann.at(XDim),StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(YSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetLS(XSpann.at(YDim));
			}
		}
		else if(StressDimension[0]==ChangingCPR){
			if(StressDimension[1]==ChangingCDR){
				(*i)->CalculateCashFlows(XSpann.at(XDim),YSpann.at(YDim),ConstantPar,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(XSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetCDR(YSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
			}
			else{
				(*i)->CalculateCashFlows(XSpann.at(XDim),ConstantPar,YSpann.at(YDim),StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(XSpann.at(XDim));
				LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetLS(YSpann.at(YDim));
			}
		}
		TotalFlow+=(*i)->GetCashFlow();
	}
	LocalStructure.ResetMtgFlows();
	LocalStructure.AddMortgagesFlows(TotalFlow);
	LocalStructure.CalculateTranchesCashFlows();
	RecievedData(XDim,YDim,LocalStructure);
}
void StressTest::RecievedData(int IDx,int IDy,const Waterfall& Res){
		if (!ContinueCalculation) return;
		Results[XSpann.at(IDx)][YSpann.at(IDy)]=Res;
		BeesReturned++;
		if(!SequentialComputation){
			if(SentBees<XSpann.size()*YSpann.size()){
				StressThread* WorkingThread=new StressThread(
					SentBees/YSpann.size()
					,SentBees%YSpann.size()
					,XSpann.at(SentBees/YSpann.size())
					,YSpann.at(SentBees%YSpann.size())
					,ConstantPar
					,Loans
					,Structure
					,StartDate
					,StressDimension[0]
					,StressDimension[1]
					,this
				);
				connect(WorkingThread,SIGNAL(ScenarioCalculated(int,int,Waterfall)),this,SLOT(RecievedData(int,int,Waterfall)));
				connect(WorkingThread,SIGNAL(ScenarioCalculated(int,int,Waterfall)),WorkingThread,SLOT(stop()));
				WorkingThread->start();
				SentBees++;
			}
		}
		if(ProgressForm) ProgressForm->SetValue(BeesReturned);
		QApplication::processEvents();
		if(BeesReturned==XSpann.size()*YSpann.size()){
#ifdef PrintExecutionTime
			QTime TempTime;
			TempTime.addMSecs(ExecutionTime.elapsed());
			QMessageBox::information(0,"Computation Time",QString("Stress Test Took: %1 Seconds").arg(ExecutionTime.elapsed()/1000));
#endif
			ContinueCalculation=false;
			if(ProgressForm){
				ProgressForm->deleteLater();
				ProgressForm=NULL;
			}
			emit AllFinished();
		}
}
QDataStream& operator<<(QDataStream & stream, const StressTest& flows){
	stream
		<< flows.XSpann
		<< flows.YSpann
		<< flows.ConstantPar
		<< flows.Structure
		<< flows.StartDate
		<< flows.Results
		<< flows.StressDimension[0]
		<< flows.StressDimension[1]
		<< flows.SequentialComputation
		<< flows.Loans.size()
	;
	foreach(Mortgage* SilgleLoan,flows.Loans)
		stream << (*SilgleLoan);
	return stream;
}
QDataStream& operator>>(QDataStream & stream, StressTest& flows){
	int tempint;
	Mortgage TempMtg;
	stream
		>> flows.XSpann
		>> flows.YSpann
		>> flows.ConstantPar
		>> flows.Structure
		>> flows.StartDate
		>> flows.Results
		>> tempint;
	flows.StressDimension[0]=StressTest::StressVariability(tempint);
	stream >> tempint;
	flows.StressDimension[1]=StressTest::StressVariability(tempint);
	stream >> flows.SequentialComputation;
	stream >> tempint;
	flows.ResetLoans();
	for (int i=0;i<tempint;i++){
		stream >> TempMtg;
		flows.AddLoan(TempMtg);
	}
	return stream;
}
void StressTest::StopCalculation(){
	ContinueCalculation=false;
	if(ProgressForm){
		ProgressForm->deleteLater();
		ProgressForm=NULL;
	}
}
void StressTest::SaveResults(const QString& DestPath)const{
	QString DestinationPath(DestPath.trimmed());
	if(DestinationPath.at(DestinationPath.size()-1)!='\\') DestinationPath.append('\\');
	QDir curDir;
	QStringList FileNames;
	QString DestinationFull = DestinationPath+QString("\\.StressResult%1%2.fcsr").arg(StressDimension[0]).arg(StressDimension[1]);
	if (curDir.exists(DestinationFull)) {
		if (!curDir.remove(DestinationFull)) return;
	}
	if(!curDir.exists(DestinationPath+"TempStressResults")) curDir.mkpath(DestinationPath+"TempStressResults");
	FileNames.append(DestinationPath+"TempStressResults\\VersionIdentifier");
	QFile file(FileNames.last());
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_8);
		out << qint32(ModelVersionNumber) << ConstantPar;
		file.close();
	}
	foreach(const QString& SingleX,XSpann){
		foreach(const QString& SingleY,YSpann){
			FileNames.append(DestinationPath+"TempStressResults\\"+SingleX+"#,#"+SingleY+".csw");
			QFile file(FileNames.last());
			if (file.open(QIODevice::WriteOnly)) {
				QDataStream out(&file);
				out.setVersion(QDataStream::Qt_4_8);
				out << Results.value(SingleX).value(SingleY);
				file.close();
			}
		}
	}
	JlCompress::compressFiles(DestinationFull, FileNames);
	removeDir(DestinationPath+"TempStressResults");
#ifdef Q_WS_WIN
	SetFileAttributes(DestinationFull.toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
#endif
}
Waterfall StressTest::GetScenarioFromFile(const QString& DestPath,const QString& XScenario,const QString& YScenario){
	Waterfall Result;
	if(!QFile::exists(DestPath)) return Result;
	QuaZip zip(DestPath);
	if(!zip.open(QuaZip::mdUnzip)) return Result;
	QuaZipFile TargetFile(&zip);
	try{
		if(!zip.setCurrentFile("VersionIdentifier")) throw 1;
		qint32 VesionCheck;
		TargetFile.open(QIODevice::ReadOnly);
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_4_8);
		out >> VesionCheck;
		TargetFile.close();
		if(VesionCheck!=qint32(ModelVersionNumber)) throw 1;
	}
	catch(int ExcCode){
		if(!QApplication::instance()){
			char *argv[] = {"NoArgumnets"};
			int argc = sizeof(argv) / sizeof(char*) - 1;
			QApplication ComputationLoop(argc,argv);
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not comaptible with the current model version\nPlease run ALL the stress tests again");
			QApplication::quit();
		}
		else QMessageBox::critical(0,"Incompatible Version","The stress test data is not comaptible with the current model version\nPlease run ALL the stress tests again");
		return Result;
	}
	if(!zip.setCurrentFile(XScenario+"#,#"+YScenario+".csw")) return Result;
	TargetFile.open(QIODevice::ReadOnly);
	QDataStream out(&TargetFile);
	out.setVersion(QDataStream::Qt_4_8);
	out >> Result;
	TargetFile.close();
	return Result;
}
bool StressTest::LoadResultsFromFile(const QString& DestPath){
	XSpann.clear();
	YSpann.clear();
	if(!QFile::exists(DestPath)) return false;
	QuaZip zip(DestPath);
	if(!zip.open(QuaZip::mdUnzip)) return false;
	QuaZipFile TargetFile(&zip);
	try{
		if(!zip.setCurrentFile("VersionIdentifier")) throw 1;
		qint32 VesionCheck;
		TargetFile.open(QIODevice::ReadOnly);
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_4_8);
		out >> VesionCheck;
		if(VesionCheck!=qint32(ModelVersionNumber)){
			TargetFile.close();
			throw 1;
		}
		out >> ConstantPar;
		TargetFile.close();
	}
	catch(int ExcCode){
		if(!QApplication::instance()){
			char *argv[] = {"NoArgumnets"};
			int argc = sizeof(argv) / sizeof(char*) - 1;
			QApplication ComputationLoop(argc,argv);
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not comaptible with the current model version\nPlease run ALL the stress tests again");
			QApplication::quit();
		}
		else {
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not comaptible with the current model version\nPlease run ALL the stress tests again");
		}
		return false;
	}
	for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
		TargetFile.open(QIODevice::ReadOnly);
		if(TargetFile.getActualFileName()=="VersionIdentifier"){
			TargetFile.close();
			continue;
		}
		QStringList Spanns=TargetFile.getActualFileName().left(TargetFile.getActualFileName().lastIndexOf(".")).split("#,#");
		if(!XSpann.contains(Spanns.at(0))) XSpann.append(Spanns.at(0));
		if(!YSpann.contains(Spanns.at(1))) YSpann.append(Spanns.at(1));
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_4_8);
		out >> (Results[Spanns.at(0)][Spanns.at(1)]);
		TargetFile.close();
	}
	return true;
}