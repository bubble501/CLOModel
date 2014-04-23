#include "StandaloneViewer.h"
#include "SummaryView.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QCloseEvent>
#include "CommonFunctions.h"
#include "StressViewer.h"
StandaloneViewer::StandaloneViewer(QWidget *parent)
	:QMainWindow(parent)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("CLO Model Viewer");
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	if(!QMetaType::isRegistered(qMetaTypeId<MtgCashFlow>()))
		qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
	TheViewer=new SummaryView(this);
	setCentralWidget(TheViewer);
	StressWindow=new StressViewer;
	connect(StressWindow,SIGNAL(Closing()),this,SLOT(closeFile()));
	connect(StressWindow,SIGNAL(StressLevelChanged(Waterfall)),this,SLOT(HandleStressChange(Waterfall)));
	StressWindow->hide();
	createActions();
	createMenus();
}
//void StandaloneViewer::AdjustTableSizes(){TheViewer->AdjustTableSizes();}
void StandaloneViewer::createActions(){
	openAction=new QAction("&Open",this);
	openAction->setIcon(QIcon::fromTheme("document-open"));
	openAction->setStatusTip("Open Base Case Scenario");
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	openStressAction=new QAction("Open &Stress",this);
	openStressAction->setIcon(QIcon::fromTheme("folder-new"));
	openStressAction->setStatusTip("Open Stress Test");
	connect(openStressAction, SIGNAL(triggered()), this, SLOT(openStress()));

	exitAction=new QAction("&Exit",this);
	exitAction->setIcon(QIcon::fromTheme("application-exit"));
	exitAction->setStatusTip(tr("Quit Application"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	CloseFileAction=new QAction("&Close",this);
	CloseFileAction->setIcon(QIcon::fromTheme("edit-clear"));
	CloseFileAction->setStatusTip(tr("Close The current File"));
	connect(CloseFileAction, SIGNAL(triggered()), this, SLOT(closeFile()));
	

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);
		recentFileActions[i]->setIcon(QIcon::fromTheme("document-open-recent"));
		connect(recentFileActions[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile()));
	}
}
void StandaloneViewer::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAction);
	fileMenu->addAction(openStressAction);
	separatorAction = fileMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		fileMenu->addAction(recentFileActions[i]);
	fileMenu->addSeparator();
	fileMenu->addAction(CloseFileAction);
	fileMenu->addAction(exitAction);
}
void StandaloneViewer::open(){
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Base Case"), "./.Basecase.clo",
		tr("CLO Model Base Case (*.clo)"));
	if (fileName.isEmpty()) return;
	LoadFile(fileName);
}
void StandaloneViewer::openStress(){
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Stress Test"), "./.StressResult01.fcsr",
		tr("CLO Model Stress Test (*.fcsr)"));
	if (fileName.isEmpty()) return;
	LoadFile(fileName);
}
void StandaloneViewer::openRecentFile(){
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		LoadFile(action->data().toString());
}
void StandaloneViewer::updateRecentFileActions()
{
	QMutableStringListIterator i(recentFiles);
	while (i.hasNext()) {
		if (!QFile::exists(i.next()))
			i.remove();
	}
	for (int j = 0; j < MaxRecentFiles; ++j) {
		if (j < recentFiles.count()) {
			QString text = tr("&%1 %2")
				.arg(j + 1)
				.arg(recentNames.at(j));
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles[j]);
			recentFileActions[j]->setVisible(true);
		} else {
			recentFileActions[j]->setVisible(false);
		}
	}
	separatorAction->setVisible(!recentFiles.isEmpty());
}

void StandaloneViewer::LoadFile(const QString& fileName){
	closeFile();
	QFile file(fileName);
	if(!file.exists()) return;
	if(QFileInfo(file).suffix().toLower()=="clo"){
		StressWindow->hide();
		Waterfall TempWaterfall;
		Waterfall TempCallWaterfall;
		QFile file(fileName);
		file.open(QIODevice::ReadOnly);
		qint32 VersionChecker;
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_8);
		out >> VersionChecker;
		if(VersionChecker<qint32(MinimumSupportedVersion)){
			QMessageBox::critical(this,"Invalid File Format","The selected file version is not compatible with this viewer");
			return closeFile();
		}
		TempWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempWaterfall;
		TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempCallWaterfall;
		file.close();
		TheViewer->SetStructure(TempWaterfall,TempCallWaterfall);
		TheViewer->ShowCallStructure();
		QString TempName=TempWaterfall.GetTranche(0)->GetTrancheName();
		recentNames.removeAll(TempName);
		recentNames.prepend(TempName);
		recentFiles.removeAll(fileName);
		recentFiles.prepend(fileName);
		updateRecentFileActions();
	}
	else if (QFileInfo(file).suffix().toLower()=="fcsr"){
		 if(!StressWindow->LoadStress(fileName)) return closeFile();
		 StressWindow->show();
		 QString TempName=StressWindow->GetFirstName();
		 recentNames.removeAll("Stress " +TempName);
		 recentNames.prepend("Stress " +TempName);
		 recentFiles.removeAll(fileName);
		 recentFiles.prepend(fileName);
		 updateRecentFileActions();
	}
	else {
		QMessageBox::critical(this,"Invalid File Extension","The selected file extension is not supported");
		return closeFile();
	}
}
void StandaloneViewer::closeFile(){
	StressWindow->hide();
	TheViewer->SetStructure(Waterfall());
}
void StandaloneViewer::HandleStressChange(Waterfall a){
	TheViewer->SetStructure(a);
	TheViewer->ShowCallStructure(false);
}
void StandaloneViewer::closeEvent(QCloseEvent *event){
	StressWindow->close();
	event->accept();
}