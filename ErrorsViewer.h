#ifndef ErrorsViewer_h__
#define ErrorsViewer_h__

#include <QWidget>
class QTextEdit;
class QLabel;
class ErrorsViewer :public QWidget{
	Q_OBJECT
public:
	ErrorsViewer(QWidget* parent=0);
	void SetText(QString a);
private:
	QTextEdit* ErrorsText;
	QLabel* MainLabel;
signals:
	void Closing();
protected:
	void closeEvent(QCloseEvent* event);
};
#endif // ErrorsViewer_h__