#include "StressTest.h"
#include "Mortgage.h"
#include "CommonFunctions.h"
#include "Waterfall.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include "quazip/JlCompress.h"
#include "BloombergVector.h"
#include "MtgCalculator.h"
#include <QMessageBox>
#include "MtgCashFlow.h"
StressTest::StressTest(QObject* parent)
	: QObject(parent)
	, SequentialComputation(false)
	, ContinueCalculation(false)
	, ProgressForm(nullptr)
	, BaseFlows(nullptr)
	, ShowProgress(true)
	, UseFastVersion(false)
{
	BaseCalculator = new MtgCalculator(this);
	BaseCalculator->SetOverrideAssumptions(true);
	ResetCurrentAssumption();
	connect(this, SIGNAL(CurrentScenarioCalculated()), this, SLOT(GoToNextScenario()),Qt::QueuedConnection);
	connect(this, SIGNAL(ErrorInCalculation()), this, SLOT(HandleError()), Qt::QueuedConnection);
	m_AssumptionsRef[AssCPR] = &m_CPRscenarios;
	m_AssumptionsRef[AssCDR] = &m_CDRscenarios;
	m_AssumptionsRef[AssLS] = &m_LSscenarios;
	m_AssumptionsRef[AssRecLag] = &m_RecLagScenarios;
	m_AssumptionsRef[AssDelinq] = &m_DelinqScenarios;
	m_AssumptionsRef[AssDelinqLag] = &m_DelinqLagScenarios;
}
void StressTest::ResetStressTest() {
	ResetLoans();
	ResetResults();
	if (ProgressForm) {
		ProgressForm->deleteLater();
	}
}
const MtgCalculator& StressTest::GetLoans() const {
	return *BaseCalculator;
}

void StressTest::AddLoan(const Mortgage& a){
	BaseCalculator->AddLoan(a);
}
void StressTest::ResetLoans(){
	BaseCalculator->Reset();
	if (BaseFlows) BaseFlows.reset();
}
void StressTest::RunStressTest() {
	if (ContinueCalculation) return;
	if (BaseFlows) 	BaseFlows.reset();
	BaseCalculator->SetSequentialComputation(SequentialComputation);
	BaseCalculator->SetStartDate(StartDate);
	BaseCalculator->SetCDRass("0");
	BaseCalculator->SetCPRass("0");
	BaseCalculator->SetLSass("0");
	BaseCalculator->SetRecoveryLag("0");
	BaseCalculator->SetDelinquency("0");
	BaseCalculator->SetDelinquencyLag("0");
	QString HardCheks = BaseCalculator->ReadyToCalculate();
	if (!HardCheks.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + HardCheks);
		return;
	}
	HardCheks = Structure.ReadyToCalculate();
	if (!HardCheks.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + HardCheks);
		return;
	}
	if (m_CDRscenarios.isEmpty()) m_CDRscenarios.insert("0");
	if (m_CPRscenarios.isEmpty()) m_CPRscenarios.insert("0");
	if (m_LSscenarios.isEmpty()) m_LSscenarios.insert("0");
	if (m_RecLagScenarios.isEmpty()) m_RecLagScenarios.insert("0");
	if (m_DelinqScenarios.isEmpty()) m_DelinqScenarios.insert("0");
	if (m_DelinqLagScenarios.isEmpty()) m_DelinqLagScenarios.insert("0");
	quint32 CheckScen=RemoveInvalidScenarios();
	if (CheckScen > 0) {
		if (QMessageBox::warning(
			0
			, "Invalid Scenarios"
			, QString("%1 invalid scenarios detected\nWould you want to run the stress test anyway, ignoring those scenarios?").arg(CheckScen)
			, QMessageBox::Yes | QMessageBox::No
			, QMessageBox::Yes
		) != QMessageBox::Yes) return;
	}
	ResetCurrentAssumption();
	ContinueCalculation = true;
#ifdef PrintExecutionTime
	ExecutionTime.start();
#endif
	if (ShowProgress) {
		ProgressForm = new ProgressWidget;
		connect(ProgressForm, SIGNAL(Cancelled()), this, SLOT(StopCalculation()));
		ProgressForm->SetValue(0);
		ProgressForm->SetTitle("Stress Test");
		ProgressForm->SetMax(10000);
		ProgressForm->show();
	}
	if (UseFastVersion) {
		connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
		if (!BaseCalculator->StartCalculation(false)){ //TODO should be true
			QMessageBox::critical(0, "Invalid Input", "A base rate in the loans is invalid");
			return;
		}
	}
	else
		RunCurrentScenario();
}
void StressTest::BaseForFastCalculated() {
	disconnect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
	if (!ContinueCalculation) return StoppedCalculation();
	BaseFlows.reset(new MtgCashFlow(BaseCalculator->GetResult()));
	RunCurrentScenario();
}
void StressTest::RunCurrentScenario() {
	if (!ContinueCalculation)  return StoppedCalculation();
	if (
		UseFastVersion 
		&& BaseFlows
		&& !IntegerVector(*(m_RecLagScenarios.constBegin() + CurrentAssumption[AssRecLag])).IsEmpty(0,0)
		&& !IntegerVector(*(m_DelinqLagScenarios.constBegin() + CurrentAssumption[AssDelinqLag])).IsEmpty(0, 0)
		&& !BloombergVector(*(m_DelinqScenarios.constBegin() + CurrentAssumption[AssDelinq])).IsEmpty(0.0, 0.0)
	) {
		Structure.ResetMtgFlows();
		Structure.AddMortgagesFlows(BaseFlows->ApplyScenario(
			*(m_CPRscenarios.constBegin() + CurrentAssumption[AssCPR])
			, *(m_CDRscenarios.constBegin() + CurrentAssumption[AssCDR])
			, *(m_LSscenarios.constBegin() + CurrentAssumption[AssLS])
		));
		if (Structure.CalculateTranchesCashFlows()) emit CurrentScenarioCalculated(); 
		else emit ErrorInCalculation();
	}
	else {
		connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(SlowLoansCalculated()), Qt::UniqueConnection);
		BaseCalculator->SetCPRass(*(m_CPRscenarios.constBegin() + CurrentAssumption[AssCPR]));
		BaseCalculator->SetCDRass(*(m_CDRscenarios.constBegin() + CurrentAssumption[AssCDR]));
		BaseCalculator->SetLSass(*(m_LSscenarios.constBegin() + CurrentAssumption[AssLS]));
		BaseCalculator->SetRecoveryLag(*(m_RecLagScenarios.constBegin() + CurrentAssumption[AssRecLag]));
		BaseCalculator->SetDelinquency(*(m_DelinqScenarios.constBegin() + CurrentAssumption[AssDelinq]));
		BaseCalculator->SetDelinquencyLag(*(m_DelinqLagScenarios.constBegin() + CurrentAssumption[AssDelinqLag]));
		if (!BaseCalculator->StartCalculation(false)) {
			emit ErrorInCalculation();
		}
	}

}
void StressTest::SlowLoansCalculated() {
	if (!ContinueCalculation)  return StoppedCalculation();
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(BaseCalculator->GetResult());
	if (Structure.CalculateTranchesCashFlows()) 
		emit CurrentScenarioCalculated();
	else 
		emit ErrorInCalculation();
}
void StressTest::GoToNextScenario() {
	Results.insert(
		AssumptionSet(
			*(m_CPRscenarios.constBegin() + CurrentAssumption[AssCPR])
			, *(m_CDRscenarios.constBegin() + CurrentAssumption[AssCDR])
			, *(m_LSscenarios.constBegin() + CurrentAssumption[AssLS])
			, *(m_RecLagScenarios.constBegin() + CurrentAssumption[AssRecLag])
			, *(m_DelinqScenarios.constBegin() + CurrentAssumption[AssDelinq])
			, *(m_DelinqLagScenarios.constBegin() + CurrentAssumption[AssDelinqLag])
		)
		, QSharedPointer<Waterfall>(new Waterfall(Structure)));
	if (!IncreaseCurrentAssumption()) {
		if (ProgressForm) {
			ProgressForm->deleteLater();
		}
		BaseFlows.reset();
		disconnect(BaseCalculator);
		emit AllFinished();
	}
	double CurrentProgress = static_cast<double>(CountScenariosCalculated()) / static_cast<double>(CountScenarios());
	if (ProgressForm) ProgressForm->SetValue(static_cast<int>(CurrentProgress*10000.0));
	emit ProgressStatus(CurrentProgress);
	RunCurrentScenario();
}

bool StressTest::IncreaseCurrentAssumption(int level) {
	if (level >= NumStressDimentsions || level < 0) return false;
	if (CurrentAssumption[level] == m_AssumptionsRef[level]->size() - 1) {
		CurrentAssumption[level] = 0;
		return IncreaseCurrentAssumption(level - 1);
	}
	CurrentAssumption[level]++;
	return true;
}

qint32 StressTest::CountScenariosCalculated(int level) {
	if (level >= NumStressDimentsions || level < 0) return 0;
	qint32 Result = CurrentAssumption[level];
	for (int i = level + 1; i < NumStressDimentsions && Result>0; ++i) Result *= m_AssumptionsRef[i]->size();
	return Result + CountScenariosCalculated(level - 1);
}

/*
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
}*/
/*
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
}*/

quint32 StressTest::RemoveInvalidScenarios() {
	quint32 InvalidScen = 0;
	for (auto i = m_CDRscenarios.begin(); i != m_CDRscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)){
			i = m_CDRscenarios.erase(i); ++InvalidScen;
			if (i == m_CDRscenarios.end()) break;
		}
	}
	for (auto i = m_CPRscenarios.begin(); i != m_CPRscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
			i = m_CPRscenarios.erase(i); ++InvalidScen;
			if (i == m_CPRscenarios.end()) break;
		}
	}
	for (auto i = m_LSscenarios.begin(); i != m_LSscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
			i = m_LSscenarios.erase(i); ++InvalidScen;
			if (i == m_LSscenarios.end()) break;
		}
	}
	for (auto i = m_DelinqScenarios.begin(); i != m_DelinqScenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
			i = m_DelinqScenarios.erase(i); ++InvalidScen;
			if (i == m_DelinqScenarios.end()) break;
		}
	}
	for (auto i = m_RecLagScenarios.begin(); i != m_RecLagScenarios.end(); ++i) {
		while (IntegerVector(*i).IsEmpty(0)) {
			i = m_RecLagScenarios.erase(i); ++InvalidScen;
			if (i == m_RecLagScenarios.end()) break;
		}
	}
	for (auto i = m_DelinqLagScenarios.begin(); i != m_DelinqLagScenarios.end(); ++i) {
		while (IntegerVector(*i).IsEmpty(0)) {
			i = m_DelinqLagScenarios.erase(i); ++InvalidScen;
			if (i == m_DelinqLagScenarios.end()) break;
		}
	}
	return InvalidScen;
}

quint32 StressTest::CountScenarios() const {
	quint32 Result=1;
	Result *= m_CDRscenarios.size();
	Result *= m_CPRscenarios.size();
	Result *= m_LSscenarios.size();
	Result *= m_RecLagScenarios.size();
	Result *= m_DelinqScenarios.size();
	Result *= m_DelinqLagScenarios.size();
	return Result;
}

void StressTest::StopCalculation() {
	ContinueCalculation = false;
	if (ProgressForm) {
		ProgressForm->deleteLater();
	}
}





