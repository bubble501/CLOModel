#include "StressViewer.h"
#include <QCloseEvent>
void StressViewer::closeEvent(QCloseEvent *event){
	//emit Closing();
	event->accept();
}
