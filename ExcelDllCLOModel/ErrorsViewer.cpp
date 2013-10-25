#include "ErrorsViewer.h"
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QCloseEvent>
ErrorsViewer::ErrorsViewer(QWidget* parent/* =0 */)
	:QWidget(parent)
{
	ErrorsText=new QTextEdit(this);
	ErrorsText->setObjectName("ErrorsText");
	ErrorsText->setReadOnly(true);
	MainLabel=new QLabel(this);
	MainLabel->setObjectName("MainLabel");
	MainLabel->setText("Errors Occurred:");
	QVBoxLayout *MainLay=new QVBoxLayout(this);
	MainLay->addWidget(MainLabel);
	MainLay->addWidget(ErrorsText);
}
void ErrorsViewer::SetText(QString a){
	a.replace('\n',"<br>");
	ErrorsText->setText(a);
}
void ErrorsViewer::closeEvent(QCloseEvent* event){
	emit Closing();
	event->accept();
}