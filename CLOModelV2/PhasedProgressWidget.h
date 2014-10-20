#ifndef PHASEDPROGRESSWIDGET_H
#define PHASEDPROGRESSWIDGET_H
#include <QString>
#include <QDialog>
#include <QList>
class QProgressBar;
class QPushButton;
class QLabel;
class PhasedProgressWidget : public QDialog
{
	Q_OBJECT
public:
	PhasedProgressWidget(QWidget *parent=0);
	void AddPhase(const QString& name, int minV = 0, int maxV = 100);
	void AddPhase(int minV = 0, int maxV = 100);
	void SetTotalProgressLabel(const QString& val);
	void SetTotalTextVisible(bool vis);
	void SetPhaseTextVisible(bool vis);
	void SetEqualLenghtPhases(bool val);
	void SetPhaseMin(int idx,int val);
	void SetPhaseMax(int idx, int val);
	void SetPhaseRange(int idx, int MinVal, int MaxVal);
	int GetPhaseMin(int idx)const;
	int GetPhaseMax(int idx)const;
	void Clear();
	int NumPhases() const { return m_PhasNames.size(); }
	int GetPhaseProgress() const;
	int GetTotalProgress() const;
public slots:
	bool NextPhase();
	void SetPhaseProgress(int val);
	void SetPhaseProgress(double val);
private slots:
	void UpdateTotalProgress();
signals:
	void NeedRefresh();
	void TotalProgress(int);
	void Cancelled();
protected:
	void closeEvent(QCloseEvent *event);
	bool m_PhaseEqual;
	int m_CurrentPhase;
	QList<QString> m_PhasNames;
	QList<int> m_Mins;
	QList<int> m_Maxs;
	QProgressBar* m_TotalProgress;
	QProgressBar* m_PhaseProgress;
	QLabel* m_TotalLabel;
	QLabel* m_PhaseLabel;
	QPushButton* m_CancelButton;
};

#endif // PHASEDPROGRESSWIDGET_H
