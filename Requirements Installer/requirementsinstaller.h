#ifndef REQUIREMENTSINSTALLER_H
#define REQUIREMENTSINSTALLER_H

#include <QWidget>
class QProgressBar;
class QLabel;
class RequirementsInstaller : public QWidget
{
	Q_OBJECT

public:
	RequirementsInstaller(QWidget *parent = 0);
signals:
	void ReadyToStrat();
private slots:
	void Install();
private:
	QProgressBar* ProgIndicator;
	QLabel* MainLabel;
};

#endif // REQUIREMENTSINSTALLER_H
