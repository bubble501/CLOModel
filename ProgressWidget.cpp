#include "ProgressWidget.h"
#include <QProgressBar>
#include <QLabel>
#include <QGridLayout>
#include <QMovie>
#include <QIcon>
#include <QCloseEvent>
ProgressWidget::ProgressWidget(QWidget* parent)
	:QWidget(parent)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("Loading");
	ProgBar=new QProgressBar(this);
	ProgBar->setObjectName("ProgBar");
	TextLabel=new QLabel(this);
	TextLabel->setObjectName("TextLabel");
	TextLabel->setText("Please Wait");
	AnimationFile=new QMovie(":/Icons/Loading.gif");
	AnimLabel=new QLabel(this);
	AnimLabel->setObjectName("AnimLabel");
	AnimLabel->setMovie(AnimationFile);
	AnimationFile->start();
	QGridLayout* mainLay=new QGridLayout(this);
	mainLay->addWidget(AnimLabel,0,0);
	mainLay->addWidget(TextLabel,0,1);
	mainLay->addWidget(ProgBar,1,0,1,2);
}
void ProgressWidget::SetTitle(const QString& a){setWindowTitle(a);}
void ProgressWidget::SetCaption(const QString& a){TextLabel->setText(a);}
void ProgressWidget::ShowAnimation(bool a){if(a) AnimLabel->show(); else AnimLabel->hide();}
void ProgressWidget::ShowPct(bool a){ProgBar->setTextVisible(a);}
void ProgressWidget::SetMax(int a){ProgBar->setMaximum(a);}
void ProgressWidget::SetMin(int a){ProgBar->setMinimum(a);}
void ProgressWidget::SetValue(int a){ProgBar->setValue(a);}
void ProgressWidget::closeEvent(QCloseEvent *event){
	emit Cancelled();
	event->accept();
}
