#include "PhasedProgressWidget.h"
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QGroupBox>
#include <QIcon>
PhasedProgressWidget::PhasedProgressWidget(QWidget *parent)
	: QDialog(parent)
	, m_PhaseEqual(false)
	, m_CurrentPhase(-1)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	QGroupBox *TotalGroup=new QGroupBox(this);
	TotalGroup->setTitle(tr("Total"));
	QVBoxLayout* TotalLay = new QVBoxLayout(TotalGroup);
	QGroupBox *PhaseGroup = new QGroupBox(this);
	PhaseGroup->setTitle(tr("Current"));
	QVBoxLayout* PhaseLay = new QVBoxLayout(PhaseGroup);

	
	m_TotalLabel = new QLabel(this);
	m_TotalLabel->setText(tr("Total Progress"));
	TotalLay->addWidget(m_TotalLabel);
	m_TotalProgress = new QProgressBar(this);
	m_TotalProgress->setRange(0,100);
	m_TotalProgress->reset();
	TotalLay->addWidget(m_TotalProgress);
	m_PhaseLabel = new QLabel(this);
	m_PhaseLabel->setText("");
	PhaseLay->addWidget(m_PhaseLabel);
	m_PhaseProgress = new QProgressBar(this);
	m_PhaseProgress->setRange(0, 100);
	m_PhaseProgress->reset();
	PhaseLay->addWidget(m_PhaseProgress);
	QHBoxLayout* BottomLay = new QHBoxLayout;
	QSpacerItem* BottomSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	BottomLay->addItem(BottomSpace);
	m_CancelButton = new QPushButton(this);
	m_CancelButton->setText(tr("Cancel"));
	m_CancelButton->setDefault(false);
	BottomLay->addWidget(m_CancelButton);
	
	setWindowTitle(tr("Please Wait"));
	QVBoxLayout* MainLay = new QVBoxLayout(this);
	MainLay->addWidget(TotalGroup);
	MainLay->addWidget(PhaseGroup);
	MainLay->addLayout(BottomLay);
	connect(m_PhaseProgress, SIGNAL(valueChanged(int)), this, SIGNAL(NeedRefresh()));
	connect(m_CancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(this, SIGNAL(NeedRefresh()), this, SLOT(UpdateTotalProgress()));
}
void PhasedProgressWidget::closeEvent(QCloseEvent *event) {
	emit Cancelled();
	event->accept();
}
void PhasedProgressWidget::SetTotalProgressLabel(const QString& val) {
	m_TotalLabel->setText(val);
}

bool PhasedProgressWidget::NextPhase() {
	if (m_CurrentPhase + 1 >= m_PhasNames.size()) return false; 
	m_PhaseLabel->setText(m_PhasNames.at(++m_CurrentPhase));
	m_PhaseProgress->setRange(m_Mins.at(m_CurrentPhase), m_Maxs.at(m_CurrentPhase));
	m_PhaseProgress->setValue(m_Mins.at(m_CurrentPhase));
	NeedRefresh();
	return true;
}

void PhasedProgressWidget::SetPhaseProgress(int val) {
	if (m_CurrentPhase < 0) return;
	m_PhaseProgress->setValue(val);
}

void PhasedProgressWidget::SetPhaseProgress(double val) {
	if (val<0.0 || val>1.0) return;
	SetPhaseProgress(static_cast<int>(
		val * static_cast<double>(m_PhaseProgress->maximum() - m_PhaseProgress->minimum())
		));
}

void PhasedProgressWidget::SetTotalTextVisible(bool vis) {
	m_TotalProgress->setTextVisible(vis);
}
void PhasedProgressWidget::SetPhaseTextVisible(bool vis) {
	m_PhaseProgress->setTextVisible(vis);
}

void PhasedProgressWidget::AddPhase(const QString& name, int minV, int maxV) {
	if (minV > maxV) {
		int temp = minV;
		minV = maxV;
		maxV = temp;
	}
	m_PhasNames.append(name);
	m_Mins.append(minV);
	m_Maxs.append(maxV);
	if (m_CurrentPhase < 0) NextPhase();
}

void PhasedProgressWidget::AddPhase(int minV , int maxV) {
	AddPhase(tr("Phase %1").arg(m_PhasNames.size()), minV, maxV);
}

void PhasedProgressWidget::Clear() {
	m_Mins.clear(); 
	m_PhasNames.clear(); 
	m_Maxs.clear();
	m_CurrentPhase = -1;
	m_TotalProgress->reset();
	m_PhaseProgress->reset();
	m_PhaseLabel->setText("");
}

void PhasedProgressWidget::SetEqualLenghtPhases(bool val) {
	m_PhaseEqual = val; 
	emit NeedRefresh();
}

void PhasedProgressWidget::UpdateTotalProgress() {
	double TotalRes;
	if (m_PhaseEqual) {
		TotalRes = 100.0*(
			static_cast<double>(m_CurrentPhase)+(
				(m_PhaseProgress->value()-m_PhaseProgress->minimum())
				/
				(m_PhaseProgress->maximum() - m_PhaseProgress->minimum())
			))/ static_cast<double>(m_PhasNames.size());
	}
	else {
		double CurrentSum = 0.0;
		double TotalSum = 0.0;
		for (int i = 0; i < m_Mins.size(); ++i) {
			TotalSum += static_cast<double>(m_Maxs.at(i) - m_Mins.at(i));
			if (i<m_CurrentPhase) CurrentSum += static_cast<double>(m_Maxs.at(i) - m_Mins.at(i));
			else if (i == m_CurrentPhase) CurrentSum += static_cast<double>(m_PhaseProgress->value() - m_Mins.at(i));
		}
		TotalRes = 100.0*CurrentSum / TotalSum;
	}
	m_TotalProgress->setValue(static_cast<int>(TotalRes));
	emit TotalProgress(static_cast<int>(TotalRes));
}
void PhasedProgressWidget::SetPhaseMin(int idx, int val) {
	if (idx < 0 || idx >= m_PhasNames.size()) return;
	if (m_Mins.at(idx) == val) return;
	m_Mins[idx] = val;
	if (idx == m_CurrentPhase) m_PhaseProgress->setMinimum(val);
	emit NeedRefresh();
}
void PhasedProgressWidget::SetPhaseMax(int idx, int val) {
	if (idx < 0 || idx >= m_PhasNames.size()) return;
	if (m_Maxs.at(idx) == val) return;
	m_Maxs[idx] = val;
	if (idx == m_CurrentPhase) m_PhaseProgress->setMaximum(val);
	emit NeedRefresh();
}
void PhasedProgressWidget::SetPhaseRange(int idx, int MinVal, int MaxVal) {
	if (idx < 0 || idx >= m_PhasNames.size()) return;
	if (m_Mins.at(idx) == MinVal) return;
	if (m_Maxs.at(idx) == MaxVal) return;
	m_Mins[idx] = MinVal;
	m_Maxs[idx] = MaxVal;
	if (idx == m_CurrentPhase) {
		m_PhaseProgress->setMinimum(MinVal);
		m_PhaseProgress->setMaximum(MaxVal);
	}
	emit NeedRefresh();
}
int PhasedProgressWidget::GetPhaseMin(int idx) const {
	if (idx < 0 || idx >= m_PhasNames.size()) return 0;
	return m_Mins.at(idx);
}
int PhasedProgressWidget::GetPhaseMax(int idx) const {
	if (idx < 0 || idx >= m_PhasNames.size()) return 0;
	return m_Maxs.at(idx);
}

int PhasedProgressWidget::GetPhaseProgress() const {
	return m_PhaseProgress->value();
}

int PhasedProgressWidget::GetTotalProgress() const {
	return m_TotalProgress->value();
}

void PhasedProgressWidget::show() {
	QDialog::show();
	if (minimumHeight()>0)
		setFixedSize(minimumHeight() * 2, minimumHeight());
}


