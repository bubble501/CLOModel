#include "requirementsinstaller.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QFile>
RequirementsInstaller::RequirementsInstaller(QWidget *parent)
	: QWidget(parent)
{
	setMinimumSize(220,80);
	setWindowTitle("Installing New Files");
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	ProgIndicator=new QProgressBar(this);
	ProgIndicator->setMinimum(0);
	ProgIndicator->setMaximum(5);
	ProgIndicator->setValue(0);
	MainLabel=new QLabel(this);
	MainLabel->setText("Installing Required Files");
	MainLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	QVBoxLayout* MainLay=new QVBoxLayout(this);
	MainLay->addWidget(MainLabel);
	MainLay->addWidget(ProgIndicator);
	connect(this,SIGNAL(ReadyToStrat()),this,SLOT(Install()),Qt::QueuedConnection);
	emit ReadyToStrat();
}
void RequirementsInstaller::Install(){
	if(!QFile::exists("C:\\Windows\\system\\blpapi3_32.dll"))
		QFile::copy(":/Libraries/Bloomberg","C:\\Windows\\system\\blpapi3_32.dll");
	ProgIndicator->setValue(1);
	if(!QFile::exists("C:\\Windows\\system\\QtCore4.dll"))
		QFile::copy(":/Libraries/Core","C:\\Windows\\system\\QtCore4.dll");
	ProgIndicator->setValue(2);
	if(!QFile::exists("C:\\Windows\\system\\QtGui4.dll"))
		QFile::copy(":/Libraries/Gui","C:\\Windows\\system\\QtGui4.dll");
	ProgIndicator->setValue(3);
	if(!QFile::exists("C:\\Windows\\system\\zlib1.dll"))
		QFile::copy(":/Libraries/Zip","C:\\Windows\\system\\zlib1.dll");
	ProgIndicator->setValue(4);
	if(!QFile::exists("C:\\Windows\\system\\kdchart2.dll"))
		QFile::copy(":/Libraries/Charts","C:\\Windows\\system\\kdchart2.dll");
	ProgIndicator->setValue(5);
	close();
}

