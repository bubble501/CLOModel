#ifndef CHECKANDEDIT_H
#define CHECKANDEDIT_H

#include <QWidget>
class QCheckBox;
class QLineEdit;
class CheckAndEdit : public QWidget
{
	Q_OBJECT
public:
	CheckAndEdit(QWidget *parent=NULL);
	void SetText(const QString& a);
	QString GetText()const;
	void SetChecked(bool a = true);
	bool IsChecked()const;
	void SetReadOnly(bool a);
private:
	QCheckBox* m_check;
	QLineEdit* m_edit;
signals:
	void CheckChanged(int);
	void LineEdited(const QString&);
private slots:
	void ForceCheck(const QString& a);
};

#endif // CHECKANDEDIT_H
