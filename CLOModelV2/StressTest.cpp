#include "StressTest.h"
#include "Mortgage.h"
#include "CommonFunctions.h"
#include "StressThread.h"
#include "Waterfall.h"
#include "ProgressWidget.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include "quazip/JlCompress.h"
#include "BloombergVector.h"
#include "MtgCalculator.h"
#include <QMessageBox>
#ifdef Q_WS_WIN
#include <Windows.h>
#endif
StressTest::StressTest(QObject* parent)
	:QObject(parent)
	,SequentialComputation(false)
	,SentBees(0)
	,BeesReturned(0)
	,ContinueCalculation(false)
	,ProgressForm(NULL)
	,ShowProgress(true)
	, UseFastVersion(false)
{
	BaseCalculator = new MtgCalculator(this);
	StressDimension[0]=ChangingCDR;
	StressDimension[1]=ChangingLS;
	connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(StartStresses()));
}
StressTest::~StressTest(){
	StressThread* CurrentRunning;
	for (QHash<quint64, StressThread*>::iterator j = ThreadsStack.begin(); j != ThreadsStack.end(); j++) {
		CurrentRunning = j.value();
		if (CurrentRunning) {
			if (CurrentRunning->isRunning()) {
				CurrentRunning->exit();
				CurrentRunning->wait();
			}
		}
	}
	ResetLoans();
	if(ProgressForm) ProgressForm->deleteLater();
}
const QHash<qint32, Mortgage*>& StressTest::GetLoans()const {
	return BaseCalculator->GetLoans();
}
const Mortgage* StressTest::GetLoans(int index)const{
	auto TempLoans = BaseCalculator->GetLoans();
	if (index<0 || index >= TempLoans.size()) return NULL;
	return *(TempLoans.constBegin()+index);
}
Mortgage* StressTest::GetLoans(int index){
	auto TempLoans = BaseCalculator->GetLoans();
	if (index<0 || index >= TempLoans.size()) return NULL;
	return *(TempLoans.begin() + index);
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
	if(!BloombergVector(a).IsEmpty())
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
	if(!BloombergVector(a).IsEmpty())
		YSpann.append(a);
}
void StressTest::SetConstantPar(const QString& a){
	if(!BloombergVector(a).IsEmpty())
		ConstantPar=a;
}
void StressTest::AddLoan(const Mortgage& a){
	BaseCalculator->AddLoan(a);
}
void StressTest::ResetLoans(){
	BaseCalculator->Reset();
}
void StressTest::RunStressTest() {
#ifdef PrintExecutionTime
	ExecutionTime.start();
#endif
	if (ShowProgress) {
		ProgressForm = new ProgressWidget;
		connect(ProgressForm, SIGNAL(Cancelled()), this, SLOT(StopCalculation()));
		ProgressForm->SetValue(0);
		ProgressForm->SetTitle("Stress Test");
		ProgressForm->SetMax((XSpann.size()*YSpann.size()) + (UseFastVersion ? 1 : 0));
		ProgressForm->show();
	}
	//QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	ContinueCalculation = true;
	BeesReturned = 0;
	BaseCalculator->SetSequentialComputation(SequentialComputation);
	BaseCalculator->SetStartDate(StartDate);
	BaseCalculator->SetCDRass("0");
	BaseCalculator->SetCPRass("0");
	BaseCalculator->SetLSass("0");
	BaseCalculator->SetRecoveryLag("0");
	BaseCalculator->SetDelinquency("0");
	BaseCalculator->SetDelinquencyLag("0");
	if (UseFastVersion) {
		QString CheckLoans = BaseCalculator->ReadyToCalculate();
		if (CheckLoans.isEmpty()) {
			if(!BaseCalculator->StartCalculation(false))
				QMessageBox::critical(0, "Invalid Input", "A base rate in the loans is invalid");
		}
		else QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + CheckLoans);
	}
	else
		StartStresses();
}
void StressTest::StartStresses() {
	//Check that CPR and CDR are Valid
	if (StressDimension[0] == ChangingCDR || StressDimension[0] == ChangingCPR) {
		foreach(const QString& SingleScen, XSpann) {
			if (BloombergVector(SingleScen).IsEmpty(0.0, 1.0)) return;
		}
	}
	if (StressDimension[1] == ChangingCDR || StressDimension[1] == ChangingCPR) {
		foreach(const QString& SingleScen, YSpann) {
			if (BloombergVector(SingleScen).IsEmpty(0.0, 1.0)) return;
		}
	}
	if ((StressDimension[0] | StressDimension[1]) != (ChangingCDR | ChangingCPR)) {
		if (BloombergVector(ConstantPar).IsEmpty(0.0, 1.0)) return;
	}


	if (ProgressForm && ShowProgress && UseFastVersion) ProgressForm->SetValue(1);
	int NumberOfThreads = QThread::idealThreadCount();
	if(SequentialComputation || NumberOfThreads<1) NumberOfThreads=1;
	for (SentBees = 0; SentBees < XSpann.size()*YSpann.size() && SentBees < NumberOfThreads; SentBees++) {
		if (UseFastVersion) {
			ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())] = new StressThread(
				SentBees / YSpann.size()
				, SentBees%YSpann.size()
				, XSpann.at(SentBees / YSpann.size())
				, YSpann.at(SentBees%YSpann.size())
				, ConstantPar
				, BaseCalculator->GetResult()
				, Structure
				, StartDate
				, StressDimension[0]
				, StressDimension[1]
				, this
				);
		}
		else {
			ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())] = new StressThread(
				SentBees / YSpann.size()
				, SentBees%YSpann.size()
				, XSpann.at(SentBees / YSpann.size())
				, YSpann.at(SentBees%YSpann.size())
				, ConstantPar
				, BaseCalculator->GetLoans()
				, Structure
				, StartDate
				, StressDimension[0]
				, StressDimension[1]
				, this
				);
		}
		connect(ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())], SIGNAL(ScenarioCalculated(int, int, Waterfall)), this, SLOT(RecievedData(int, int, Waterfall)));
		connect(ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())], SIGNAL(ScenarioCalculated(int, int, Waterfall)), ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())], SLOT(stop()), Qt::QueuedConnection);
		ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())]->start();
	}
}
void StressTest::RecievedData(int IDx,int IDy,const Waterfall& Res){
		if (!ContinueCalculation) return;
		Results[XSpann.at(IDx)][YSpann.at(IDy)]=Res;
		ThreadsStack.erase(ThreadsStack.find(ConcatenateIDs(IDx, IDy)));
		//ThreadsStack[ConcatenateIDs(IDx,IDy)] = NULL;
		BeesReturned++;
		if(SentBees<XSpann.size()*YSpann.size()){
			if (UseFastVersion) {
				ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(),SentBees%YSpann.size())] = new StressThread(
					SentBees / YSpann.size()
					, SentBees%YSpann.size()
					, XSpann.at(SentBees / YSpann.size())
					, YSpann.at(SentBees%YSpann.size())
					, ConstantPar
					, BaseCalculator->GetResult()
					, Structure
					, StartDate
					, StressDimension[0]
					, StressDimension[1]
					, this
					);
			}
			else {
				ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(),SentBees%YSpann.size())] = new StressThread(
					SentBees / YSpann.size()
					, SentBees%YSpann.size()
					, XSpann.at(SentBees / YSpann.size())
					, YSpann.at(SentBees%YSpann.size())
					, ConstantPar
					, BaseCalculator->GetLoans()
					, Structure
					, StartDate
					, StressDimension[0]
					, StressDimension[1]
					, this
					);
			}
			connect(ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())], SIGNAL(ScenarioCalculated(int, int, Waterfall)), this, SLOT(RecievedData(int, int, Waterfall)));
			connect(ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())], SIGNAL(ScenarioCalculated(int, int, Waterfall)), ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())], SLOT(stop()));
			ThreadsStack[ConcatenateIDs(SentBees / YSpann.size(), SentBees%YSpann.size())]->start();
			SentBees++;
		}
		double Testing = 100.0*static_cast<double>(BeesReturned) / static_cast<double>(XSpann.size()*YSpann.size());
		emit ProgressStatus(100.0*static_cast<double>(BeesReturned)/static_cast<double>(XSpann.size()*YSpann.size()));
		if (ProgressForm && ShowProgress) ProgressForm->SetValue(BeesReturned + (UseFastVersion ? 1 : 0));
		
		//QApplication::processEvents();
		if(BeesReturned==XSpann.size()*YSpann.size()){
#ifdef PrintExecutionTime
			QTime TempTime;
			TempTime.addMSecs(ExecutionTime.elapsed());
			QMessageBox::information(0, "Computation Time", QString("Stress Test Took: %1 Seconds").arg(ExecutionTime.elapsed() / 1000));
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
	//Deprecated, use SaveResults(), it's much faster
	stream
		<< flows.XSpann
		<< flows.YSpann
		<< flows.ConstantPar
		<< flows.UseFastVersion
		<< flows.Structure
		<< flows.StartDate
		<< qint32(flows.StressDimension[0])
		<< qint32(flows.StressDimension[1])
		<< flows.SequentialComputation
		<< (*(flows.BaseCalculator))
	;
	const QList<QString> XKeys = flows.Results.keys();
	const QList<QString> YKeys = flows.Results.begin()->keys();
	stream << XKeys;
	stream << YKeys;
	for (QList<QString>::const_iterator i = XKeys.begin(); i != XKeys.end();i++) {
		for (QList<QString>::const_iterator j = YKeys.begin(); j != YKeys.end(); j++) {
			stream << flows.Results.value(*i).value(*j);
		}
	}
	return stream;
}
QDataStream& StressTest::LoadOldVersion(QDataStream& stream) {
	qint32 tempint;
	stream
		>> XSpann
		>> YSpann
		>> ConstantPar
		>> UseFastVersion
		;
	Structure.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> Structure;
	stream >> StartDate;
	stream >> tempint;
	StressDimension[0] = StressTest::StressVariability(tempint);
	stream >> tempint;
	StressDimension[1] = StressTest::StressVariability(tempint);
	stream >> SequentialComputation;
	BaseCalculator->SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> (*BaseCalculator);
	QList<QString> XKeys;
	QList<QString> YKeys;
	stream >> XKeys;
	stream >> YKeys;
	Waterfall TempWaterfall;
	for (QList<QString>::const_iterator i = XKeys.begin(); i != XKeys.end(); i++) {
		for (QList<QString>::const_iterator j = YKeys.begin(); j != YKeys.end(); j++) {
			TempWaterfall.SetLoadProtocolVersion(m_LoadProtocolVersion);
			stream >> TempWaterfall;
			Results[*i][*j]=TempWaterfall;
		}
	}
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, StressTest& flows) {
	//Deprecated, use LoadResultsFromFile(), it's much faster
	return flows.LoadOldVersion(stream);
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
	QString DestinationFull = DestinationPath + QString("\\.StressResult%1%2.fcsr").arg(StressDimension[0] >> 1).arg(StressDimension[1] >> 1);
	if (curDir.exists(DestinationFull)) {
		if (!curDir.remove(DestinationFull)) return;
	}
	if(!curDir.exists(DestinationPath+"TempStressResults")) curDir.mkpath(DestinationPath+"TempStressResults");
	FileNames.append(DestinationPath+"TempStressResults\\VersionIdentifier");
	QFile file(FileNames.last());
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out << qint32(ModelVersionNumber) << ConstantPar << (qint32(StressDimension[0]) >> 1) << (qint32(StressDimension[1]) >> 1) << UseFastVersion;
		file.close();
	}
	foreach(const QString& SingleX,XSpann){
		foreach(const QString& SingleY,YSpann){
			FileNames.append(DestinationPath+"TempStressResults\\"+SingleX+"#,#"+SingleY+".csw");
			QFile file(FileNames.last());
			if (file.open(QIODevice::WriteOnly)) {
				QDataStream out(&file);
				out.setVersion(QDataStream::Qt_5_3);
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
	qint32 VesionCheck;
	if(!QFile::exists(DestPath)) return Result;
	QuaZip zip(DestPath);
	if(!zip.open(QuaZip::mdUnzip)) return Result;
	QuaZipFile TargetFile(&zip);
	try{
		if(!zip.setCurrentFile("VersionIdentifier")) throw 1;
		TargetFile.open(QIODevice::ReadOnly);
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_5_3);
		out >> VesionCheck;
		TargetFile.close();
		if(VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) throw 1;
	}
	catch(int ExcCode){
		if(!QApplication::instance()){
			char *argv[] = {"NoArgumnets"};
			int argc = sizeof(argv) / sizeof(char*) - 1;
			QApplication ComputationLoop(argc,argv);
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not compatible with the current model version\nPlease run ALL the stress tests again");
			QApplication::quit();
		}
		else QMessageBox::critical(0,"Incompatible Version","The stress test data is not compatible with the current model version\nPlease run ALL the stress tests again");
		return Result;
	}
	if(!zip.setCurrentFile(XScenario+"#,#"+YScenario+".csw")) return Result;
	TargetFile.open(QIODevice::ReadOnly);
	QDataStream out(&TargetFile);
	out.setVersion(QDataStream::Qt_5_3);
	Result.SetLoadProtocolVersion(VesionCheck);
	out >> Result;
	TargetFile.close();
	return Result;
}
bool StressTest::LoadResultsFromFile(const QString& DestPath){
	XSpann.clear();
	YSpann.clear();
	qint32 VesionCheck,Temp32;
	if(!QFile::exists(DestPath)) return false;
	QuaZip zip(DestPath);
	if(!zip.open(QuaZip::mdUnzip)) return false;
	QuaZipFile TargetFile(&zip);
	try{
		if(!zip.setCurrentFile("VersionIdentifier")) throw 1;
		TargetFile.open(QIODevice::ReadOnly);
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_5_3);
		out >> VesionCheck;
		if(VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)){
			TargetFile.close();
			throw 1;
		}
		out >> ConstantPar;
		if(!out.atEnd()){
			out >> Temp32;
			StressDimension[0]=StressVariability(Temp32);
			out >> Temp32;
			StressDimension[1]=StressVariability(Temp32);
			out >> UseFastVersion;
		}
		
		TargetFile.close();
	}
	catch(int ExcCode){
		if(!QApplication::instance()){
			char *argv[] = {"NoArgumnets"};
			int argc = sizeof(argv) / sizeof(char*) - 1;
			QApplication ComputationLoop(argc,argv);
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not compatible with the current model version\nPlease run ALL the stress tests again");
			QApplication::quit();
		}
		else {
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not compatible with the current model version\nPlease run ALL the stress tests again");
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
		out.setVersion(QDataStream::Qt_5_3);
		(Results[Spanns.at(0)][Spanns.at(1)]).SetLoadProtocolVersion(VesionCheck);
		out >> (Results[Spanns.at(0)][Spanns.at(1)]);
		TargetFile.close();
	}
	return true;
}