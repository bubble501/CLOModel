#ifndef COUNTRIESDBBUILDER_H
#define COUNTRIESDBBUILDER_H
#include <QDialog>
class QLineEdit;
class QPushButton;
class QProgressBar;
class CountriesDBbuilder : public QDialog
{
	Q_OBJECT
public:
	CountriesDBbuilder(QWidget *parent = nullptr);
private:
	QLineEdit* m_InputEdit;
	QLineEdit* m_OutputEdit;
	QPushButton* m_InputBrowse;
	QPushButton* m_OutputBrowse;
	QPushButton* m_BuildButton;
	QProgressBar* m_BuildProgress;
	void EnableUserInput();
	void InputError();
private slots:
	void BrowseInput();
	void BrowseOutput();
	void BuildDB();
};

#endif // COUNTRIESDBBUILDER_H
