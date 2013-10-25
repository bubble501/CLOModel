#ifndef ProgressWidget_h__
#define ProgressWidget_h__

#include <QWidget>
class QLabel;
class QProgressBar;
class QMovie;
class QPushButton;
class ProgressWidget : public QWidget{
	Q_OBJECT
public:
	ProgressWidget(QWidget* parent=0);
	void SetTitle(const QString& a);
	void SetCaption(const QString& a);
	void ShowPct(bool a=true);
	void ShowAnimation(bool a=true);
	void SetMin(int a);
	void SetMax(int a);
	void SetValue(int a);
signals:
	void Cancelled();
private:
	QProgressBar* ProgBar;
	QLabel* TextLabel;
	QLabel* AnimLabel;
	QMovie *AnimationFile;
protected:
	void closeEvent(QCloseEvent *event);
};
#endif // ProgressWidget_h__