#include "CheckAndEdit.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QHBoxLayout>
CheckAndEdit::CheckAndEdit(QWidget *parent)
	: QWidget(parent)
{
	m_check = new QCheckBox(this);
	m_check->setText(QString());
	m_check->setChecked(false);
	m_edit = new QLineEdit(this);
	
	QHBoxLayout* mainLay = new QHBoxLayout(this);
	mainLay->setMargin(1);
	mainLay->addWidget(m_check);
	mainLay->addWidget(m_edit);

	connect(m_edit, SIGNAL(textEdited(QString)), this, SIGNAL(LineEdited(QString)));
	connect(m_check, SIGNAL(stateChanged(int)), this, SIGNAL(CheckChanged(int)));
	connect(m_edit, SIGNAL(textEdited(const QString&)), this, SLOT(ForceCheck(const QString&)));
}

void CheckAndEdit::SetText(const QString& a) {
	m_edit->setText(a);
}

QString CheckAndEdit::GetText() const {
	return m_edit->text();
}

void CheckAndEdit::SetChecked(bool a /*= true*/) {
	m_check->setChecked(a);
}

bool CheckAndEdit::IsChecked() const {
	return m_check->isChecked();
}

void CheckAndEdit::SetReadOnly(bool a) {
	m_edit->setReadOnly(a);
}

void CheckAndEdit::ForceCheck(const QString& a) {
	m_check->setChecked(!a.isEmpty());
}

Qt::CheckState CheckAndEdit::CheckState() const {
	return m_check->checkState();
}

void CheckAndEdit::SetCheckState(Qt::CheckState a) {
	m_check->setCheckState(a);
}
