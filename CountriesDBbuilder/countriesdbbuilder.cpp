#include "countriesdbbuilder.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <simstring.h>
#include <QFile>
#include <QMessageBox>
#include <QProgressBar>
#include <QApplication>
#include <QStringList>
CountriesDBbuilder::CountriesDBbuilder(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("ISO 3166-1 Database Builder"));
	QGroupBox* InputGroup = new QGroupBox(this);
	InputGroup->setTitle(tr("Input"));
	QHBoxLayout* InputLay = new QHBoxLayout(InputGroup);
	m_InputEdit = new QLineEdit(this);
	m_InputBrowse = new QPushButton(this);
	m_InputBrowse->setText(tr("Browse"));
	connect(m_InputBrowse, &QPushButton::clicked, this, &CountriesDBbuilder::BrowseInput);
	InputLay->addWidget(m_InputEdit);
	InputLay->addWidget(m_InputBrowse);

	QGroupBox* OutputGroup = new QGroupBox(this);
	OutputGroup->setTitle(tr("Output"));
	QHBoxLayout* OutputLay = new QHBoxLayout(OutputGroup);
	m_OutputEdit = new QLineEdit(this);
	m_OutputBrowse = new QPushButton(this);
	m_OutputBrowse->setText(tr("Browse"));
	connect(m_OutputBrowse, &QPushButton::clicked, this, &CountriesDBbuilder::BrowseOutput);
	OutputLay->addWidget(m_OutputEdit);
	OutputLay->addWidget(m_OutputBrowse);


	m_BuildButton = new QPushButton(this);
	m_BuildButton->setText(tr("Build DB"));
	connect(m_BuildButton, &QPushButton::clicked, this, &CountriesDBbuilder::BuildDB);
	m_BuildProgress = new QProgressBar(this);
	m_BuildProgress->hide();

	QGridLayout* mainLay = new QGridLayout(this);
	mainLay->addWidget(InputGroup,0,0,1,2);
	mainLay->addWidget(OutputGroup, 1, 0, 1, 2);
	mainLay->addWidget(m_BuildProgress, 2, 0, 1, 2);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred),3,0);
	mainLay->addWidget(m_BuildButton, 3, 1);
	
}

void CountriesDBbuilder::BrowseInput() {
	QString Res = QFileDialog::getOpenFileName(this, tr("Open Input XML"), QString(), tr("eXtensible Markup Language (*.xml)"));
	if (!Res.isEmpty()) m_InputEdit->setText(Res);
}

void CountriesDBbuilder::BrowseOutput() {
	QString Res = QFileDialog::getSaveFileName(this, tr("Save Output"), QString(), tr("Simstring Database (*.ssdb)"));
	if (!Res.isEmpty()) m_OutputEdit->setText(Res);
}

void CountriesDBbuilder::BuildDB() {
	m_InputEdit->setEnabled(false);
	m_OutputEdit->setEnabled(false);
	m_InputBrowse->setEnabled(false);
	m_OutputBrowse->setEnabled(false);
	m_BuildButton->setEnabled(false);

	if (!QFile::exists(m_InputEdit->text())) return EnableUserInput();
	if (QFile::exists(m_OutputEdit->text())) {
		/*if (
			QMessageBox::question(this, tr("Are you sure?"), tr("This action will overwrite the existing DB\nAre you sure you want to continue?"))
			!= QMessageBox::Yes) return EnableUserInput();*/
		QFileInfo FiletoDelete(m_OutputEdit->text());
		QDir ParDir = FiletoDelete.absoluteDir();
		ParDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		ParDir.setNameFilters(QStringList() << "*.ssdb.*.cdb");
		auto FilesInFolder = ParDir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		for (auto fifi = FilesInFolder.constBegin(); fifi != FilesInFolder.constEnd(); ++fifi) {
			if (FiletoDelete.baseName() == fifi->baseName()) {
				QFile::remove(fifi->absoluteFilePath());
			}
		}
		QFile::remove(FiletoDelete.absoluteFilePath());
	}
	int CountCountries = 0;
	{
		QFile InputFile(m_InputEdit->text());
		InputFile.open(QIODevice::ReadOnly);
		QXmlStreamReader xml(&InputFile);
		while (!xml.atEnd() && !xml.hasError()) {
			xml.readNext();
			if (xml.isStartElement()) {
				if (xml.name() == "Country") ++CountCountries;
			}
		}
		if (xml.hasError()) return InputError();
		InputFile.close();
	}
	m_BuildProgress->setRange(0, CountCountries);
	m_BuildProgress->setValue(0);
	m_BuildProgress->show();


	simstring::ngram_generator gen(3, false);
	simstring::writer_base<std::wstring> dbw(gen, m_OutputEdit->text().toStdString());
	QStringList NameList;
	{
		QFile InputFile(m_InputEdit->text());
		InputFile.open(QIODevice::ReadOnly);
		bool CountryFound = false;
		bool NameFound = false;
		QXmlStreamReader xml(&InputFile);
		while (!xml.atEnd() && !xml.hasError()) {
			xml.readNext();
			if (xml.isStartElement()) {
				if (xml.name() == "Country") {
					if (CountryFound) { dbw.close(); return InputError(); }
					CountryFound = true;
				}
				else if (CountryFound && xml.name() == "Name") {
					if (NameFound) { dbw.close(); return InputError(); }
					NameFound = true;
				}
			}
			else if (xml.isEndElement()) {
				if (xml.name() == "Country") {
					if (!CountryFound || NameFound) { dbw.close(); return InputError(); }
					CountryFound = false;
				}
				else if (xml.name() == "Name") {
					if (!NameFound) { dbw.close(); return InputError(); }
					NameFound = false;
				}
			}
			else if (xml.isCharacters() && NameFound) {
				NameList = xml.text().toString().split("#,#",QString::SkipEmptyParts);
				for (auto nli = NameList.constBegin(); nli != NameList.constEnd(); ++nli) dbw.insert(nli->trimmed().toLower().toStdWString());
				m_BuildProgress->setValue(m_BuildProgress->value() + 1);
				QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			}
		}
		if (xml.hasError()) { dbw.close(); return InputError(); }
		InputFile.close();
	}
	QMessageBox::information(this, tr("Completed"), tr("The database was built correctly"));
	dbw.close();
	return EnableUserInput();
}

void CountriesDBbuilder::EnableUserInput() {
	m_InputEdit->setEnabled(true);
	m_OutputEdit->setEnabled(true);
	m_InputBrowse->setEnabled(true);
	m_OutputBrowse->setEnabled(true);
	m_BuildButton->setEnabled(true);
	m_BuildProgress->hide();
}

void CountriesDBbuilder::InputError() {
	QFile::remove(m_OutputEdit->text());
	QMessageBox::critical(this, tr("Error"), tr("An error occurred while building the database.\nCheck the input."));
	return EnableUserInput();
}

