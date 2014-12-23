#include "LoanAssumptionsEditor.h"
#include "CommonFunctions.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#endif
#include <QStandardItemModel>
#include <QListView>
#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QCompleter>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QScopedPointer>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QTableView>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QHeaderView>
#include <QSplitter>
#include "AssumptionsComboDelegate.h"
#include "LoanAssumptionDelegate.h"
#include "RichTextDelegate.h"
LoanAssumptionsEditor::LoanAssumptionsEditor(QWidget *parent)
	: QWidget(parent)
	, ActiveAssumption(nullptr)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle(tr("Loan Scenarios Editor"));

	m_ScenariosModel = new QStandardItemModel(this);
	m_ScenariosModel->setRowCount(0);
	m_ScenariosModel->setColumnCount(1);
	m_ScenariosModel->setHeaderData(0, Qt::Horizontal, "Scenario");

	m_SortScenarios = new QSortFilterProxyModel(this);
	m_SortScenarios->setSourceModel(m_ScenariosModel);

	

	m_AliasesModel = new QStandardItemModel(this);
	m_AliasesModel->setColumnCount(1);
	m_AliasesModel->setRowCount(0);
	m_AliasesModel->setHeaderData(0, Qt::Horizontal, "Alias");

	m_AliasesList = new QListView(this);
	m_AliasesList->setModel(m_AliasesModel);
	m_AliasesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_AliasesList->setSelectionMode(QAbstractItemView::SingleSelection);
	
	QLabel* SelectScenarioLabel = new QLabel(this);
	SelectScenarioLabel->setText(tr("Select Scenario"));
	QCompleter* ScenariosCompleter = new QCompleter(m_SortScenarios, this);
	ScenariosCompleter->setCompletionColumn(0);
	ScenariosCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	ScenariosCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
	m_ScenariosCombo = new QComboBox(this);
	m_ScenariosCombo->setModel(m_SortScenarios);
	m_ScenariosCombo->setEditable(true);
	m_ScenariosCombo->setModelColumn(0);
	m_ScenariosCombo->setCompleter(ScenariosCompleter);
	m_ScenariosCombo->setInsertPolicy(QComboBox::NoInsert);
	AddScenarioButton = new QPushButton(this);
	AddScenarioButton->setText(tr("Add"));
	RemoveScenarioButton = new QPushButton(this);
	RemoveScenarioButton->setText(tr("Remove"));
	RemoveScenarioButton->setEnabled(false);

	m_ScenarioList = new QListView(this);
	m_ScenarioList->setModel(m_SortScenarios);
	m_ScenarioList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_ScenarioList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ScenarioList->setItemDelegate(new RichTextDelegate(m_ScenarioList));


	QLabel* AliasLabel = new QLabel(this);
	AliasLabel->setText(tr("Aliases"));
	m_AliasLineEdit = new QLineEdit(this);
	m_AliasLineEdit->setMaxLength(50);
	AddAliasButton = new QPushButton(this);
	AddAliasButton->setText("+");
	AddAliasButton->setEnabled(false);
	RemoveAliasButton = new QPushButton(this);
	RemoveAliasButton->setText("-");
	RemoveAliasButton->setEnabled(false);
	ReplaceAliasButton = new QPushButton(this);
	ReplaceAliasButton->setText("\342\207\205"); //Unicode for up and down arrow
	ReplaceAliasButton->setEnabled(false);
	
	

	QGroupBox* NamesGroup = new QGroupBox(this);
	NamesGroup->setStyleSheet(
		"QPushButton {"
		"min-width: 20px;"
		"font-size: 16px;"
		"border: 1px solid #555;"
		"border-radius: 5px;"
		"background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #fff, stop: 1 #ccc);"
		"}"
		"QPushButton:pressed {"
		"background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #ccc, stop: 1 #fff);"
		"}"
		"QLabel { qproperty-alignment: 'AlignRight | AlignVCenter'; }"
	);
	NamesGroup->setTitle(tr("Identifiers"));
	QLabel* ScenarioNameLabel = new QLabel(this);
	ScenarioNameLabel->setText(tr("Scenario Name"));
	m_ScenarioNameEdit = new QLineEdit(this);
	QGridLayout* NamesLay = new QGridLayout(NamesGroup);
	NamesLay->addWidget(ScenarioNameLabel, 0, 0);
	NamesLay->addWidget(m_ScenarioNameEdit, 0, 1,1,4);
	NamesLay->addWidget(AliasLabel, 1, 0);
	NamesLay->addWidget(m_AliasLineEdit, 1, 1);
	NamesLay->addWidget(AddAliasButton, 1, 2);
	NamesLay->addWidget(RemoveAliasButton, 1, 3);
	NamesLay->addWidget(ReplaceAliasButton, 1, 4);
	NamesLay->addWidget(m_AliasesList, 2, 1, 1, 4);
	
	m_SeniorAsumptionsModel = new QStandardItemModel(this);
	m_SeniorAsumptionsModel->setColumnCount(2);
	m_SeniorAsumptionsModel->setRowCount(1);
	m_SeniorAsumptionsModel->setHorizontalHeaderLabels(QStringList() << tr("Assumption Type") << tr("Assumption value"));
	m_MezzAsumptionsModel = new QStandardItemModel(this);
	m_MezzAsumptionsModel->setColumnCount(2);
	m_MezzAsumptionsModel->setRowCount(1);
	m_MezzAsumptionsModel->setHorizontalHeaderLabels(QStringList() << tr("Assumption Type") << tr("Assumption value"));

	m_SeniorTable = new QTableView(this);
	m_SeniorTable->setModel(m_SeniorAsumptionsModel);
	m_SeniorTable->setItemDelegateForColumn(0, new AssumptionsComboDelegate(this));
	m_SeniorTable->setItemDelegateForColumn(1, new LoanAssumptionDelegate(this));
	m_SeniorTable->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_SeniorTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_SeniorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_SeniorTable->horizontalHeader()->setMinimumSectionSize(130);
	m_SeniorTable->horizontalHeader()->setStretchLastSection(true);
	m_SeniorTable->verticalHeader()->hide();
	QGroupBox* SeniorGroup = new QGroupBox(this);
	SeniorGroup->setTitle(tr("Assumption for Senior Obligations"));
	QGridLayout* SeniorLay = new QGridLayout(SeniorGroup);
	m_seniorDateCheck = new QCheckBox(this);
	m_seniorDateCheck->setText(tr("Scenario Reference Date"));
	m_seniorDateCheck->setLayoutDirection(Qt::RightToLeft);
	m_SeniorDateEdit = new QDateEdit(this);
	m_SeniorDateEdit->setCalendarPopup(true);
	m_SeniorDateEdit->setDate(QDate::currentDate());
	m_SeniorDateEdit->setEnabled(false);
	AddSeniorAssumptionButton = new QPushButton(this);
	AddSeniorAssumptionButton->setText(tr("Add"));
	AddSeniorAssumptionButton->setEnabled(false);
	RemoveSeniorAssumptionButton = new QPushButton(this);
	RemoveSeniorAssumptionButton->setText(tr("Remove"));
	RemoveSeniorAssumptionButton->setEnabled(false);

	SeniorLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 0);
	SeniorLay->addWidget(m_seniorDateCheck, 0, 1);
	SeniorLay->addWidget(m_SeniorDateEdit, 0, 2);
	SeniorLay->addWidget(m_SeniorTable, 1, 0, 1, 3);
	QHBoxLayout* SeniorLowerlay = new QHBoxLayout();
	SeniorLowerlay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));
	SeniorLowerlay->addWidget(AddSeniorAssumptionButton);
	SeniorLowerlay->addWidget(RemoveSeniorAssumptionButton);
	SeniorLay->addLayout(SeniorLowerlay, 2, 0, 1, 3);

	m_MezzTable = new QTableView(this);
	m_MezzTable->setModel(m_MezzAsumptionsModel);
	m_MezzTable->setItemDelegateForColumn(0, new AssumptionsComboDelegate(this));
	m_MezzTable->setItemDelegateForColumn(1, new LoanAssumptionDelegate(this));
	m_MezzTable->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_MezzTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_MezzTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_MezzTable->horizontalHeader()->setMinimumSectionSize(130);
	m_MezzTable->horizontalHeader()->setStretchLastSection(true);
	m_MezzTable->verticalHeader()->hide();
	QGroupBox* MezzGroup = new QGroupBox(this);
	MezzGroup->setTitle(tr("Assumption for Mezzanine Obligations"));
	QGridLayout* MezzLay = new QGridLayout(MezzGroup);
	m_MezzDateCheck = new QCheckBox(this);
	m_MezzDateCheck->setText(tr("Scenario Reference Date"));
	m_MezzDateCheck->setLayoutDirection(Qt::RightToLeft);
	m_MezzDateEdit = new QDateEdit(this);
	m_MezzDateEdit->setCalendarPopup(true);
	m_MezzDateEdit->setDate(QDate::currentDate());
	m_MezzDateEdit->setEnabled(false);
	AddMezzAssumptionButton = new QPushButton(this);
	AddMezzAssumptionButton->setText(tr("Add"));
	AddMezzAssumptionButton->setEnabled(false);
	RemoveMezzAssumptionButton = new QPushButton(this);
	RemoveMezzAssumptionButton->setText(tr("Remove"));
	RemoveMezzAssumptionButton->setEnabled(false);

	MezzLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 0);
	MezzLay->addWidget(m_MezzDateCheck, 0, 1);
	MezzLay->addWidget(m_MezzDateEdit, 0, 2);
	MezzLay->addWidget(m_MezzTable, 1, 0, 1, 3);
	QHBoxLayout* MezzLowerlay = new QHBoxLayout();
	MezzLowerlay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));
	MezzLowerlay->addWidget(AddMezzAssumptionButton);
	MezzLowerlay->addWidget(RemoveMezzAssumptionButton);
	MezzLay->addLayout(MezzLowerlay, 2, 0, 1, 3);

	QWidget* RightSection = new QWidget(this);
	QVBoxLayout* RightLay = new QVBoxLayout(RightSection);
	RightLay->addWidget(NamesGroup);
	RightLay->addWidget(SeniorGroup);
	RightLay->addWidget(MezzGroup);
	RightSection->setMinimumWidth(400);

	QWidget* LeftSection = new QWidget(this);
	QGridLayout* LeftLay = new QGridLayout(LeftSection);
	LeftLay->addWidget(SelectScenarioLabel, 0, 0, 1, 2);
	LeftLay->addWidget(m_ScenariosCombo, 1, 0, 1, 2);
	LeftLay->addWidget(m_ScenarioList, 2, 0,1,2);
	LeftLay->addWidget(AddScenarioButton, 3, 0);
	LeftLay->addWidget(RemoveScenarioButton, 3, 1);
	LeftSection->setMinimumWidth(150);

	QSplitter* MainSplit = new QSplitter(this);
	MainSplit->setOrientation(Qt::Horizontal);
	MainSplit->addWidget(LeftSection);
	MainSplit->addWidget(RightSection);
	MainSplit->setChildrenCollapsible(false);
	MainSplit->setFrameShape(QFrame::Panel);
	MainSplit->setFrameShadow(QFrame::Plain);
	MainSplit->setLineWidth(0);
	MainSplit->setMidLineWidth(3);
	MainSplit->setHandleWidth(3);

#ifdef Q_OS_WIN
	MainSplit->setStyleSheet(
		"QSplitter::handle {"
		"border-image: url(:/Icons/SplitterHandle.png);"
		"}"
		);
#endif
	QFrame* BottomLine = new QFrame(this);
	BottomLine->setFrameStyle(QFrame::HLine);
	BottomLine->setLineWidth(1);
	BottomLine->setMidLineWidth(0);
	BottomLine->setFrameShadow(QFrame::Sunken);
	DiscardAllButton = new QPushButton(this);
	DiscardAllButton->setText(tr("Discard All"));
	DiscardAllButton->setEnabled(false);
	DiscardCurrentButton = new QPushButton(this);
	DiscardCurrentButton->setText(tr("Discard"));
	DiscardCurrentButton->setEnabled(false);
	SaveAllButton = new QPushButton(this);
	SaveAllButton->setText(tr("Save All"));
	SaveAllButton->setEnabled(false);
	SaveCurrentButton = new QPushButton(this);
	SaveCurrentButton->setText(tr("Save"));
	SaveCurrentButton->setEnabled(false);


	QGridLayout* mainLay = new QGridLayout(this);
	/*mainLay->setSpacing(0);
	mainLay->setMargin(0);*/
	mainLay->addWidget(MainSplit,0,0,1,5);
	mainLay->addWidget(BottomLine, 1, 0, 1, 5);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred),2,0);
	mainLay->addWidget(DiscardAllButton, 2, 1);
	mainLay->addWidget(DiscardCurrentButton, 2, 2);
	mainLay->addWidget(SaveAllButton, 2, 3);
	mainLay->addWidget(SaveCurrentButton, 2, 4);

	
	connect(m_ScenariosModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		m_SortScenarios->sort(0);
	});
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, this, &LoanAssumptionsEditor::ChangeScenario);
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.isValid()) m_ScenariosCombo->setCurrentIndex(index.row());
		RemoveAliasButton->setEnabled(index.isValid() && m_AliasesModel->rowCount()>0);
		AddAliasButton->setEnabled(false );
		ReplaceAliasButton->setEnabled(index.isValid() && m_AliasesModel->rowCount()>0);
		AddSeniorAssumptionButton->setEnabled(index.isValid());
		RemoveSeniorAssumptionButton->setEnabled(index.isValid() && m_SeniorAsumptionsModel->rowCount()>0);
		m_SeniorDateEdit->setEnabled(index.isValid());
		m_seniorDateCheck->setEnabled(index.isValid());
		AddMezzAssumptionButton->setEnabled(index.isValid());
		RemoveMezzAssumptionButton->setEnabled(index.isValid() && m_MezzAsumptionsModel->rowCount() > 0);
		m_MezzDateEdit->setEnabled(index.isValid());
		m_MezzDateCheck->setEnabled(index.isValid());
		m_AliasLineEdit->clear();
	});
	connect(m_AliasLineEdit, &QLineEdit::textChanged, [&](const QString& a) {AddAliasButton->setEnabled(!a.isEmpty()); });
	connect(m_SeniorTable->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		RemoveSeniorAssumptionButton->setEnabled(index.isValid());
	});
	connect(m_MezzTable->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		RemoveMezzAssumptionButton->setEnabled(index.isValid());
	});
	connect(m_ScenariosCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int row) {
		m_ScenarioList->setCurrentIndex(m_SortScenarios->index(row, 0));
	});
	connect(m_AliasesList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.isValid())
			m_AliasLineEdit->setText(m_AliasesModel->data(index).toString());
	});
	connect(AddAliasButton, &QPushButton::clicked, [&](bool) {
		if (CheckAliasInput()) {
			m_AliasesModel->insertRow(m_AliasesModel->rowCount());
			m_AliasesModel->setData(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), m_AliasLineEdit->text());
			m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
		}
	});
	connect(RemoveAliasButton, &QPushButton::clicked, [&](bool) {
		if (m_AliasesList->selectionModel()->currentIndex().isValid()) 
			m_AliasesModel->removeRow(m_AliasesList->selectionModel()->currentIndex().row());
		RemoveAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
		ReplaceAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
	});
	connect(ReplaceAliasButton, &QPushButton::clicked, [&](bool) {
		if (m_AliasesList->selectionModel()->currentIndex().isValid()){
			if (CheckAliasInput())
				m_AliasesModel->setData(m_AliasesModel->index(m_AliasesList->selectionModel()->currentIndex().row(), 0), m_AliasLineEdit->text());
		}
	});
	connect(m_seniorDateCheck, &QCheckBox::clicked, m_SeniorDateEdit, &QDateEdit::setEnabled);
	connect(m_SeniorAsumptionsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(SeniorScenarioChanged(const QModelIndex&)));
	connect(m_MezzDateCheck, &QCheckBox::clicked, m_MezzDateEdit, &QDateEdit::setEnabled);
	connect(m_MezzAsumptionsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(MezzScenarioChanged(const QModelIndex&)));
	connect(m_AliasesModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		ReplaceAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
		RemoveAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
	});
	connect(m_AliasesModel, &QStandardItemModel::modelReset, [&]() {
		ReplaceAliasButton->setEnabled(m_AliasesModel->rowCount() > 0);
		RemoveAliasButton->setEnabled(m_AliasesModel->rowCount() > 0);
	});
	connect(m_SeniorAsumptionsModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
	connect(m_SeniorAsumptionsModel, &QStandardItemModel::modelReset, [&]() {
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
	connect(AddSeniorAssumptionButton, &QPushButton::clicked, [&](bool) {
		m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
		m_SeniorTable->selectionModel()->setCurrentIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	});
	connect(RemoveSeniorAssumptionButton, &QPushButton::clicked, [&](bool) {
		if (m_SeniorTable->selectionModel()->currentIndex().isValid()) {
			m_SeniorAsumptionsModel->removeRow(m_SeniorTable->selectionModel()->currentIndex().row());
		}
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
	connect(m_MezzAsumptionsModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		RemoveMezzAssumptionButton->setEnabled(m_MezzAsumptionsModel->rowCount() > 0);
	});
	connect(m_MezzAsumptionsModel, &QStandardItemModel::modelReset, [&]() {
		RemoveMezzAssumptionButton->setEnabled(m_MezzAsumptionsModel->rowCount() > 0);
	});
	connect(AddMezzAssumptionButton, &QPushButton::clicked, [&](bool) {
		m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
		m_MezzTable->selectionModel()->setCurrentIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	});
	connect(RemoveMezzAssumptionButton, &QPushButton::clicked, [&](bool) {
		if (m_MezzTable->selectionModel()->currentIndex().isValid()) {
			m_MezzAsumptionsModel->removeRow(m_MezzTable->selectionModel()->currentIndex().row());
		}
		RemoveMezzAssumptionButton->setEnabled(m_MezzAsumptionsModel->rowCount() > 0);
	});
	connect(AddScenarioButton, &QPushButton::clicked, [&](bool) {
		QString NewScenName = "Scenario %1";
		{quint64 i = 1; for (; ScenExist(NewScenName.arg(i)); ++i); NewScenName = NewScenName.arg(i); }
		m_Assumptions.insert(NewScenName, QSharedPointer<LoanAssumption>(nullptr));
		m_DirtyAssumptions.insert(NewScenName, QSharedPointer<LoanAssumption>(new LoanAssumption(NewScenName)));
		m_ScenariosModel->insertRow(m_ScenariosModel->rowCount());
		m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), NewScenName, Qt::EditRole);
		m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), RichTextDelegate::Added, Qt::UserRole);
		m_ScenarioList->selectionModel()->setCurrentIndex(m_SortScenarios->mapFromSource(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0)), QItemSelectionModel::ClearAndSelect);
	});
	connect(RemoveScenarioButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
			if (m_ScenariosModel->data(currIdx, Qt::UserRole) == RichTextDelegate::Added) {
				m_DirtyAssumptions.remove(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
				m_ScenariosModel->removeRow(currIdx.row());
			}
			else {
				m_ScenariosModel->setData(currIdx, RichTextDelegate::Erased, Qt::UserRole);
				m_DirtyAssumptions[m_ScenariosModel->data(currIdx, Qt::EditRole).toString()] = QSharedPointer<LoanAssumption>(nullptr);
			}
		}
		RemoveScenarioButton->setEnabled(m_ScenariosModel->rowCount() > 0);
	});
	connect(m_ScenariosModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		RemoveScenarioButton->setEnabled(m_ScenariosModel->rowCount() > 0);
	});
	connect(m_ScenariosModel, &QStandardItemModel::modelReset, [&]() {
		RemoveScenarioButton->setEnabled(m_ScenariosModel->rowCount() > 0);
	});
	connect(DiscardCurrentButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
			if (YesNoDialog(DiscardCurrentButton->text(), tr("Are you sure you want to discard changes to %1").arg(m_ScenariosModel->data(currIdx, Qt::EditRole).toString()))) {
				DiscardScenario(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
			}
		}
	});
	connect(DiscardAllButton, &QPushButton::clicked, [&](bool) {
		if (YesNoDialog(DiscardAllButton->text(), tr("Are you sure you want to discard changes to %n scenario(s)", "", m_DirtyAssumptions.size()))){
			auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
				DiscardScenario(k);
			}
		}
		CheckAllDirty();
	});
	connect(SaveCurrentButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioNameEdit->text().isEmpty()) {
			QMessageBox::critical(this, "Error", "Scenario Name can't be empty"); 
			return;
		}
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
			if (YesNoDialog(SaveCurrentButton->text(), tr("Are you sure you want to save changes to %1").arg(m_ScenariosModel->data(currIdx, Qt::EditRole).toString()))) {
				SaveScenario(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
			} 
		}
	});
	connect(SaveAllButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioNameEdit->text().isEmpty()) {
			QMessageBox::critical(this, "Error", "Scenario Name can't be empty");
			return;
		}
		if (YesNoDialog(SaveAllButton->text(), tr("Are you sure you want to save changes to %n scenario(s)", "", m_DirtyAssumptions.size()))) {
			auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
				SaveScenario(k);
			}
		}
		CheckAllDirty();
	});


	connect(RemoveSeniorAssumptionButton, SIGNAL(clicked()), this,SLOT(CheckCurrentDirty()));
	connect(AddSeniorAssumptionButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(RemoveMezzAssumptionButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(AddMezzAssumptionButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(AddAliasButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(RemoveAliasButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(ReplaceAliasButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(m_ScenarioNameEdit, &QLineEdit::textEdited, [&](const QString& a) {
		if (!a.isEmpty()) CheckCurrentDirty();
	});
	connect(m_seniorDateCheck, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(m_SeniorDateEdit, SIGNAL(dateChanged(const QDate&)), this, SLOT(SeniorDateChanged(const QDate&)), Qt::QueuedConnection);
	connect(m_SeniorTable->itemDelegateForColumn(0), SIGNAL(Edited()), this, SLOT(CheckCurrentDirty()));
	connect(m_SeniorTable->itemDelegateForColumn(1), SIGNAL(Edited()), this, SLOT(CheckCurrentDirty()));
	connect(m_MezzDateCheck, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(m_MezzDateEdit, SIGNAL(dateChanged(const QDate&)), this, SLOT(MezzDateChanged(const QDate&)), Qt::QueuedConnection);
	connect(m_MezzTable->itemDelegateForColumn(0), SIGNAL(Edited()), this, SLOT(CheckCurrentDirty()));
	connect(m_MezzTable->itemDelegateForColumn(1), SIGNAL(Edited()), this, SLOT(CheckCurrentDirty()));
	connect(m_ScenariosModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		bool CurrentDirty, Anydirty ;
		Anydirty = CurrentDirty = m_ScenariosModel->data(index, Qt::UserRole).toInt() != RichTextDelegate::NotDirty;
		for (int i = 0; i < m_ScenariosModel->rowCount() && !Anydirty; ++i) {
			Anydirty = m_ScenariosModel->data(m_ScenariosModel->index(i,0), Qt::UserRole).toInt() != RichTextDelegate::NotDirty;
		}
		SaveCurrentButton->setEnabled(CurrentDirty);
		DiscardCurrentButton->setEnabled(CurrentDirty);
		SaveAllButton->setEnabled(Anydirty);
		DiscardAllButton->setEnabled(Anydirty);
	});
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		bool Result = false;
		if (index.isValid()) {
			Result = m_ScenariosModel->data(m_SortScenarios->mapToSource(index), Qt::UserRole).toInt() != RichTextDelegate::NotDirty;
		}
		SaveCurrentButton->setEnabled(Result);
		DiscardCurrentButton->setEnabled(Result);
	});
}
#ifndef NO_DATABASE
void LoanAssumptionsEditor::FillFromQuery() {
	m_Assumptions.clear();
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
			+ "}; "
			+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress; Initial Catalog = ABSDB; Integrated Security = SSPI; Trusted_Connection = Yes;)")
			);
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
	if (DbOpen) {
		QSqlQuery PrepayAssQuerry(db);
		PrepayAssQuerry.setForwardOnly(true);
		PrepayAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetAllLoanAssumptionStoredProc", "getAllLoanAssumptions") + "}");
		if (PrepayAssQuerry.exec()) {
			bool CurrentSenior;
			int FieldCount;
			while (PrepayAssQuerry.next()) {
				FieldCount = 0;
				QString CurrentScenario = PrepayAssQuerry.value(FieldCount).toString(); ++FieldCount;
				QSharedPointer<LoanAssumption> CurrentAss(nullptr);
				for (auto i = m_Assumptions.begin(); i != m_Assumptions.end(); ++i) {
					if (i.key().compare(CurrentScenario, Qt::CaseInsensitive) == 0) {
						CurrentAss = i.value();
						break;
					}
				}
				if (!CurrentAss) {
					CurrentAss = QSharedPointer<LoanAssumption>(new LoanAssumption(CurrentScenario));
					if (!PrepayAssQuerry.isNull(FieldCount)) CurrentAss->SetAliases(PrepayAssQuerry.value(FieldCount).toString());
					m_Assumptions.insert(CurrentAss->GetScenarioName(), CurrentAss);
				}++FieldCount;
				CurrentSenior = PrepayAssQuerry.value(FieldCount).toBool(); ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorMaturityExtension(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzMaturityExtension(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorInitialHaircut(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzInitialHaircut(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrepaymentFee(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzPrepaymentFee(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDayCount(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzDayCount(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorCPR(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzCPR(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorCDR(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzCDR(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLS(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzLS(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorRecoveryLag(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzRecoveryLag(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDelinquency(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzDelinquency(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDelinquencyLag(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzDelinquencyLag(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrice(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzPrice(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorHaircut(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzHaircut(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrepayMultiplier(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzPrepayMultiplier(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLossMultiplier(PrepayAssQuerry.value(FieldCount).toString());
					else CurrentAss->SetMezzLossMultiplier(PrepayAssQuerry.value(FieldCount).toString());
				} ++FieldCount;
				if (!PrepayAssQuerry.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLastUpdate(PrepayAssQuerry.value(FieldCount).toDate());
					else CurrentAss->SetMezzLastUpdate(PrepayAssQuerry.value(FieldCount).toDate());
				} ++FieldCount;

			}
		}
	}
	FillScenariosModel();
}

#endif

void LoanAssumptionsEditor::FillScenariosModel() {
	m_ScenariosModel->setRowCount(m_Assumptions.size());
	int RawIndex = 0;
	for (auto i = m_Assumptions.constBegin(); i != m_Assumptions.constEnd(); ++i, ++RawIndex) {
		m_ScenariosModel->setData(m_ScenariosModel->index(RawIndex, 0), i.value()->GetScenarioName(), Qt::EditRole);
	}
	m_ScenarioList->setCurrentIndex(QModelIndex());
	m_ScenariosCombo->setCurrentIndex(-1);
}

void LoanAssumptionsEditor::ChangeScenario(const QModelIndex& curr, const QModelIndex& prev) {
	ActiveAssumption = m_DirtyAssumptions.value(m_ScenariosModel->data(m_SortScenarios->mapToSource(curr)).toString(),
		m_Assumptions.value(m_ScenariosModel->data(m_SortScenarios->mapToSource(curr)).toString(), QSharedPointer<LoanAssumption>(nullptr)));
	m_ScenarioNameEdit->setText(ActiveAssumption ? ActiveAssumption->GetScenarioName():QString());
	m_SeniorAsumptionsModel->setRowCount(0);
	m_MezzAsumptionsModel->setRowCount(0);
	if (ActiveAssumption){
		m_AliasesModel->setRowCount(ActiveAssumption->GetAliases().count());
		int RowIter = 0;
		for (auto i = ActiveAssumption->GetAliases().constBegin(); i != ActiveAssumption->GetAliases().constEnd(); ++i,++RowIter) {
			m_AliasesModel->setData(m_AliasesModel->index(RowIter, 0), *i);
		}
		if (ActiveAssumption->GetSeniorLastUpdate().isNull()) {
			m_seniorDateCheck->setChecked(false);
			m_SeniorDateEdit->setDate(QDate::currentDate());
			m_SeniorDateEdit->setEnabled(false);
		}
		else {
			m_seniorDateCheck->setChecked(true);
			m_SeniorDateEdit->setEnabled(true);
			m_SeniorDateEdit->setDate(ActiveAssumption->GetSeniorLastUpdate());
		}
		if (ActiveAssumption->GetMezzLastUpdate().isNull()) {
			m_MezzDateCheck->setChecked(false);
			m_MezzDateEdit->setDate(QDate::currentDate());
			m_MezzDateEdit->setEnabled(false);
		}
		else {
			m_MezzDateCheck->setChecked(true);
			m_MezzDateEdit->setEnabled(true);
			m_MezzDateEdit->setDate(ActiveAssumption->GetMezzLastUpdate());
		}
		//Senior Assumptions
		int AssIndex = 0;
		if (!ActiveAssumption->GetRawSeniorMaturityExtension().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				,Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1), 
				ActiveAssumption->GetRawSeniorMaturityExtension().toInt()
			, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorInitialHaircut().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorInitialHaircut().toDouble()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorPrepaymentFee().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorPrepaymentFee()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorDayCount().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorDayCount()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorCPR().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorCPR()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorCDR().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorCDR()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorLS().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorLS()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorRecoveryLag().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorRecoveryLag()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorDelinquency().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorDelinquency()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorDelinquencyLag().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorDelinquencyLag()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorPrice().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorPrice().toDouble()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorHaircut().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorHaircut()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorPrepayMultiplier().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorPrepayMultiplier()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawSeniorLossMultiplier().isEmpty()) {
			m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_SeniorAsumptionsModel->setData(
				m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_SeniorTable->itemDelegate(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawSeniorLossMultiplier()
				, Qt::EditRole);
		}AssIndex++;
		//Mezz Assumptions
		AssIndex = 0;
		if (!ActiveAssumption->GetRawMezzMaturityExtension().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzMaturityExtension().toInt()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzInitialHaircut().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzInitialHaircut().toDouble()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzPrepaymentFee().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzPrepaymentFee()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzDayCount().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzDayCount()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzCPR().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzCPR()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzCDR().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzCDR()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzLS().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzLS()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzRecoveryLag().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzRecoveryLag()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzDelinquency().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzDelinquency()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzDelinquencyLag().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzDelinquencyLag()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzPrice().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzPrice().toDouble()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzHaircut().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzHaircut()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzPrepayMultiplier().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzPrepayMultiplier()
				, Qt::EditRole);
		}AssIndex++;
		if (!ActiveAssumption->GetRawMezzLossMultiplier().isEmpty()) {
			m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
			m_MezzAsumptionsModel->setData(
				m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)
				, qobject_cast<AssumptionsComboDelegate*>(m_MezzTable->itemDelegate(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 1),
				ActiveAssumption->GetRawMezzLossMultiplier()
				, Qt::EditRole);
		}AssIndex++;
	}
	else {
		m_seniorDateCheck->setChecked(false);
		m_SeniorDateEdit->setEnabled(false);
		m_SeniorDateEdit->setDate(QDate::currentDate());
		m_MezzDateCheck->setChecked(false);
		m_MezzDateEdit->setEnabled(false);
		m_MezzDateEdit->setDate(QDate::currentDate());
		m_AliasesModel->setRowCount(0);
	}
	

}

void LoanAssumptionsEditor::SeniorScenarioChanged(const QModelIndex& index) {
	if (index.column() == 0) {
		m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::EditRole);
		auto AssumptionType = m_SeniorAsumptionsModel->data(index, Qt::UserRole).toInt();
		for (int i = 0; i < m_SeniorAsumptionsModel->rowCount() && AssumptionType>=0; ++i) {
			if (i == index.row())continue;
			if (m_SeniorAsumptionsModel->data(m_SeniorAsumptionsModel->index(i, 0), Qt::UserRole) == AssumptionType) {
				m_SeniorAsumptionsModel->setData(index, -1, Qt::UserRole);
				m_SeniorAsumptionsModel->setData(index, QVariant(), Qt::EditRole);
				m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::UserRole);
				return;
			}
		}
		switch (AssumptionType) {
		case LoanAssumption::MaturityExtension:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::IntegerAssumption), Qt::UserRole);
			break;
		case LoanAssumption::InitialHaircut:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::DoubleAssumption0To100), Qt::UserRole);
			break;
		case LoanAssumption::DayCount:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::DayCountVectorAssumption), Qt::UserRole);
			break;
		case LoanAssumption::PrepaymentFee:
		case LoanAssumption::CPR:
		case LoanAssumption::CDR:
		case LoanAssumption::LS:
		case LoanAssumption::Delinquency:
		case LoanAssumption::Haircut:
		case LoanAssumption::PrepayMultiplier:
		case LoanAssumption::LossMultiplier:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::BloombergVectorAssumption), Qt::UserRole);
			break;
		case LoanAssumption::RecoveryLag:
		case LoanAssumption::DelinquencyLag:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::IntegerVectorAssumption), Qt::UserRole);
			break;
		case LoanAssumption::Price:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::DoubleAssumption), Qt::UserRole);
			break;
		}
		
	}
	
}
void LoanAssumptionsEditor::MezzScenarioChanged(const QModelIndex& index) {
	if (index.column() == 0) {
		m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), QVariant(), Qt::EditRole);
		auto AssumptionType = m_MezzAsumptionsModel->data(index, Qt::UserRole).toInt();
		for (int i = 0; i < m_MezzAsumptionsModel->rowCount() && AssumptionType >= 0; ++i) {
			if (i == index.row())continue;
			if (m_MezzAsumptionsModel->data(m_MezzAsumptionsModel->index(i, 0), Qt::UserRole) == AssumptionType) {
				m_MezzAsumptionsModel->setData(index, -1, Qt::UserRole);
				m_MezzAsumptionsModel->setData(index, QVariant(), Qt::EditRole);
				m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), QVariant(), Qt::UserRole);
				return;
			}
		}
		switch (AssumptionType) {
		case LoanAssumption::MaturityExtension:
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::IntegerAssumption), Qt::UserRole);
			break;
		case LoanAssumption::InitialHaircut:
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::DoubleAssumption0To100), Qt::UserRole);
			break;
		case LoanAssumption::DayCount:
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::DayCountVectorAssumption), Qt::UserRole);
			break;
		case LoanAssumption::PrepaymentFee:
		case LoanAssumption::CPR:
		case LoanAssumption::CDR:
		case LoanAssumption::LS:
		case LoanAssumption::Delinquency:
		case LoanAssumption::Haircut:
		case LoanAssumption::PrepayMultiplier:
		case LoanAssumption::LossMultiplier:
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::BloombergVectorAssumption), Qt::UserRole);
			break;
		case LoanAssumption::RecoveryLag:
		case LoanAssumption::DelinquencyLag:
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::IntegerVectorAssumption), Qt::UserRole);
			break;
		case LoanAssumption::Price:
			m_MezzAsumptionsModel->setData(m_MezzAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::DoubleAssumption), Qt::UserRole);
			break;
		}

	}

}
bool LoanAssumptionsEditor::CheckAliasInput() {
	if (m_AliasLineEdit->text().isEmpty() || QRegExp(m_AliasLineEdit->text()).isValid()) {
		m_AliasLineEdit->setStyleSheet(QString());
	}
	else {
		m_AliasLineEdit->setToolTip(tr("Invalid Regular Expression.\nVisit www.regular-expressions.info for more informations on regular expressions"));
		m_AliasLineEdit->setStyleSheet("background-color: #E79292;");
		return false;
	}
	if (m_AliasLineEdit->text().isEmpty()) return false;
	int CurrRow = m_AliasesList->selectionModel()->currentIndex().row();
	if (m_AliasLineEdit->text().compare(m_AliasesModel->data(m_AliasesModel->index(CurrRow, 0)).toString(), Qt::CaseInsensitive) == 0) { 
		m_AliasLineEdit->setStyleSheet(QString());
		return false;
	}
	if(ScenExist(m_AliasLineEdit->text())) {
			m_AliasLineEdit->setToolTip(tr("The selected alias already refers to a scenario"));
			m_AliasLineEdit->setStyleSheet("background-color: #E79292;");
			return false;
	}
	return true;
}

bool LoanAssumptionsEditor::ScenExist(const QString& Teststr)const {
	bool Result = false;
	for (auto j = m_Assumptions.constBegin(); j != m_Assumptions.constEnd() && !Result; ++j) {
		if (!j.value())continue;
		if (m_DirtyAssumptions.contains(j.key()))continue;
		Result = Teststr.compare(j.value()->GetScenarioName(), Qt::CaseInsensitive) == 0;
		for (auto i = j.value()->GetAliases().constBegin(); i != j.value()->GetAliases().constEnd() && !Result; ++i) {
			Result = Teststr.compare(*i, Qt::CaseInsensitive) == 0;
		}
	}
	for (auto j = m_DirtyAssumptions.constBegin(); j != m_DirtyAssumptions.constEnd() && !Result; ++j) {
		if (!j.value())continue;
		Result = Teststr.compare(j.value()->GetScenarioName(), Qt::CaseInsensitive) == 0;
		for (auto i = j.value()->GetAliases().constBegin(); i != j.value()->GetAliases().constEnd() && !Result; ++i) {
			Result = Teststr.compare(*i, Qt::CaseInsensitive) == 0;
		}
	}
	return Result;
}

LoanAssumption LoanAssumptionsEditor::BuildCurrentAssumption() const {
	LoanAssumption Result(m_ScenarioNameEdit->text());
	for (int i = 0; i<m_AliasesModel->rowCount(); ++i) {
		Result.AddAlias(m_AliasesModel->data(m_AliasesModel->index(i, 0),Qt::EditRole).toString());
	}
	if (m_seniorDateCheck->isChecked())
		Result.SetSeniorLastUpdate(m_SeniorDateEdit->date());
	else
		Result.SetSeniorLastUpdate(QDate());
	for (int i = 0;i< m_SeniorAsumptionsModel->rowCount(); ++i) {
		Result.SetAssumption(
			static_cast<LoanAssumption::AssumptionType>(m_SeniorAsumptionsModel->data(m_SeniorAsumptionsModel->index(i, 0), Qt::UserRole).toInt())
			, LoanAssumption::Senior
			, m_SeniorAsumptionsModel->data(m_SeniorAsumptionsModel->index(i, 1), Qt::EditRole).toString()
		);
	}
	if (m_MezzDateCheck->isChecked())
		Result.SetMezzLastUpdate(m_MezzDateEdit->date());
	else
		Result.SetMezzLastUpdate(QDate());
	for (int i = 0; i < m_MezzAsumptionsModel->rowCount(); ++i) {
		Result.SetAssumption(
			static_cast<LoanAssumption::AssumptionType>(m_MezzAsumptionsModel->data(m_MezzAsumptionsModel->index(i, 0), Qt::UserRole).toInt())
			, LoanAssumption::Mezz
			, m_MezzAsumptionsModel->data(m_MezzAsumptionsModel->index(i, 1), Qt::EditRole).toString()
			);
	}
	return Result;
}

bool LoanAssumptionsEditor::YesNoDialog(const QString& Title, const QString& Mess) {
	QScopedPointer<QMessageBox, QScopedPointerDeleteLater> OkToChange(new QMessageBox(this));
	OkToChange->setIcon(QMessageBox::Question);
	OkToChange->setWindowTitle(Title);
	OkToChange->setText(Mess);
	OkToChange->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	OkToChange->setDefaultButton(QMessageBox::Yes);
	return (OkToChange->exec() == QMessageBox::Yes);
}

void LoanAssumptionsEditor::CheckCurrentDirty() {
	if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
		auto DirtyVersion = BuildCurrentAssumption();
		QSharedPointer<LoanAssumption> OriginalVersion(nullptr);
		auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
		OriginalVersion = m_Assumptions.value(m_ScenariosModel->data(currIdx, Qt::EditRole).toString(), QSharedPointer<LoanAssumption>(nullptr));

		if (OriginalVersion) {
			if (DirtyVersion == *OriginalVersion) {
				m_DirtyAssumptions.remove(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
				m_ScenariosModel->setData(currIdx, QVariant(), Qt::UserRole);
				return;
			}
		}
		if (m_DirtyAssumptions.contains(m_ScenariosModel->data(currIdx, Qt::EditRole).toString())) 
			m_DirtyAssumptions[m_ScenariosModel->data(currIdx, Qt::EditRole).toString()]->operator=(DirtyVersion);
		else 
			m_DirtyAssumptions.insert(m_ScenariosModel->data(currIdx, Qt::EditRole).toString(),QSharedPointer<LoanAssumption>(new LoanAssumption(DirtyVersion)));
		if (m_ScenariosModel->data(currIdx, Qt::UserRole) != RichTextDelegate::Added) {
			m_ScenariosModel->setData(currIdx, RichTextDelegate::Dirty, Qt::UserRole);
		}
	}
}
void LoanAssumptionsEditor::CheckAllDirty() {
	auto CurrDirty = RichTextDelegate::NotDirty;
	for (auto i = m_Assumptions.constBegin(); i != m_Assumptions.constEnd(); ++i) {
		CurrDirty = RichTextDelegate::NotDirty;
		if (m_DirtyAssumptions.contains(i.key())) {
			auto DirtVersion = m_DirtyAssumptions.value(i.key());
			if (DirtVersion && i.value()) {
				if ((*DirtVersion) != *(i.value())) CurrDirty = RichTextDelegate::Dirty;
			}
			else CurrDirty = RichTextDelegate::Erased;
		}
		QModelIndex ScenIdx;
		for (int j = 0; j < m_ScenariosModel->rowCount() && !ScenIdx.isValid(); ++j) {
			if (i.key().compare(
				m_ScenariosModel->data(m_ScenariosModel->index(j, 0), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0
			)ScenIdx = m_ScenariosModel->index(j, 0);
		}
		if (m_ScenariosModel->data(ScenIdx, Qt::UserRole) != RichTextDelegate::Added)
			m_ScenariosModel->setData(ScenIdx, CurrDirty, Qt::UserRole);
	}

}
void LoanAssumptionsEditor::SaveScenario(const QString& key) {
	if (!m_Assumptions.contains(key) || !m_DirtyAssumptions.contains(key)) return;
	bool KeyIsCurrent = false;
	QModelIndex CurrentIdx;
	if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
		CurrentIdx = m_ScenarioList->selectionModel()->currentIndex();
		KeyIsCurrent = key.compare(
			m_ScenariosModel->data(m_SortScenarios->mapToSource(CurrentIdx), Qt::EditRole).toString()
			, Qt::CaseInsensitive
			) == 0;
	}
#ifndef NO_DATABASE
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
			+ "}; "
			+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress; Initial Catalog = ABSDB; Integrated Security = SSPI; Trusted_Connection = Yes;)")
			);
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
#endif // !NO_DATABASE

	if (!m_DirtyAssumptions.value(key)) {
#ifndef NO_DATABASE
		if (!DbOpen) {
			QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
			return;
		}
		QSqlQuery RemoveAssQuery(db);
		RemoveAssQuery.setForwardOnly(true);
		RemoveAssQuery.prepare("{CALL " + GetFromConfig("Database", "RemoveLoanAssumptionStoredProc", "removeLoanAssumption(:scenarioName)") + "}");
		RemoveAssQuery.bindValue(":scenarioName", key);
		if (!RemoveAssQuery.exec()) {
			QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
			return;
		}
#endif // !NO_DATABASE
		m_Assumptions[key] = QSharedPointer<LoanAssumption>(nullptr);
		return DiscardScenario(key);
	}
#ifndef NO_DATABASE
	if (!DbOpen) {
		QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
		return;
	}
	auto CurrAss = m_DirtyAssumptions.value(key);
	QSqlQuery SetSeniorAssQuery(db);
	QString TmpString;
	SetSeniorAssQuery.setForwardOnly(true);
	SetSeniorAssQuery.prepare("{CALL " + GetFromConfig("Database", "SetLoanAssumptionStoredProc", "") + "}");
	SetSeniorAssQuery.bindValue(":OldScenName", key);
	SetSeniorAssQuery.bindValue(":scenarioName", CurrAss->GetScenarioName());
	TmpString = CurrAss->GelAliasString(); SetSeniorAssQuery.bindValue(":scanarioAlias", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	SetSeniorAssQuery.bindValue(":seniorScenario", true);
	TmpString = CurrAss->GetRawSeniorMaturityExtension(); SetSeniorAssQuery.bindValue(":scenarioMaturityExtension", TmpString.isEmpty() ? QVariant(QVariant::Int) : TmpString.toInt());
	TmpString = CurrAss->GetRawSeniorInitialHaircut(); SetSeniorAssQuery.bindValue(":scenarioInitialHaircut", TmpString.isEmpty() ? QVariant(QVariant::Double) : TmpString.toDouble());
	TmpString = CurrAss->GetRawSeniorPrepaymentFee(); SetSeniorAssQuery.bindValue(":scenarioPrepaymentFee", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorDayCount(); SetSeniorAssQuery.bindValue(":scenarioDayCount", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorCPR(); SetSeniorAssQuery.bindValue(":scenarioCPR", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorCDR(); SetSeniorAssQuery.bindValue(":scenarioCDR", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorLS(); SetSeniorAssQuery.bindValue(":scenarioLS", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorRecoveryLag(); SetSeniorAssQuery.bindValue(":scenarioRecoveryLag", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorDelinquency(); SetSeniorAssQuery.bindValue(":scenarioDelinquency", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorDelinquencyLag(); SetSeniorAssQuery.bindValue(":scenarioDelinquencyLag", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorPrice(); SetSeniorAssQuery.bindValue(":scenarioPrice", TmpString.isEmpty() ? QVariant(QVariant::Double) : TmpString.toDouble());
	TmpString = CurrAss->GetRawSeniorHaircut(); SetSeniorAssQuery.bindValue(":scenarioHaircut", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorPrepayMultiplier(); SetSeniorAssQuery.bindValue(":scenarioPrepayMultiplier", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawSeniorLossMultiplier(); SetSeniorAssQuery.bindValue(":scenarioLossMultiplier", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	SetSeniorAssQuery.bindValue(":scenarioUpdateDate", CurrAss->GetSeniorLastUpdate().isNull() ? QVariant(QVariant::String) : CurrAss->GetSeniorLastUpdate().toString("yyyy-MM-dd"));
	if (!SetSeniorAssQuery.exec()) {
		QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
		return;
	}
	QSqlQuery SetMezzAssQuery(db);
	SetMezzAssQuery.setForwardOnly(true);
	SetMezzAssQuery.prepare("{CALL " + GetFromConfig("Database", "SetLoanAssumptionStoredProc", "") + "}");
	SetMezzAssQuery.bindValue(":OldScenName", key);
	SetMezzAssQuery.bindValue(":scenarioName", CurrAss->GetScenarioName());
	TmpString = CurrAss->GelAliasString(); SetMezzAssQuery.bindValue(":scanarioAlias", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	SetMezzAssQuery.bindValue(":seniorScenario", false);
	TmpString = CurrAss->GetRawMezzMaturityExtension(); SetMezzAssQuery.bindValue(":scenarioMaturityExtension", TmpString.isEmpty() ? QVariant(QVariant::Int) : TmpString.toInt());
	TmpString = CurrAss->GetRawMezzInitialHaircut(); SetMezzAssQuery.bindValue(":scenarioInitialHaircut", TmpString.isEmpty() ? QVariant(QVariant::Double) : TmpString.toDouble());
	TmpString = CurrAss->GetRawMezzPrepaymentFee(); SetMezzAssQuery.bindValue(":scenarioPrepaymentFee", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzDayCount(); SetMezzAssQuery.bindValue(":scenarioDayCount", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzCPR(); SetMezzAssQuery.bindValue(":scenarioCPR", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzCDR(); SetMezzAssQuery.bindValue(":scenarioCDR", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzLS(); SetMezzAssQuery.bindValue(":scenarioLS", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzRecoveryLag(); SetMezzAssQuery.bindValue(":scenarioRecoveryLag", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzDelinquency(); SetMezzAssQuery.bindValue(":scenarioDelinquency", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzDelinquencyLag(); SetMezzAssQuery.bindValue(":scenarioDelinquencyLag", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzPrice(); SetMezzAssQuery.bindValue(":scenarioPrice", TmpString.isEmpty() ? QVariant(QVariant::Double) : TmpString.toDouble());
	TmpString = CurrAss->GetRawMezzHaircut(); SetMezzAssQuery.bindValue(":scenarioHaircut", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzPrepayMultiplier(); SetMezzAssQuery.bindValue(":scenarioPrepayMultiplier", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	TmpString = CurrAss->GetRawMezzLossMultiplier(); SetMezzAssQuery.bindValue(":scenarioLossMultiplier", TmpString.isEmpty() ? QVariant(QVariant::String) : TmpString);
	SetMezzAssQuery.bindValue(":scenarioUpdateDate", CurrAss->GetMezzLastUpdate().isNull() ? QVariant(QVariant::String) : CurrAss->GetMezzLastUpdate().toString("yyyy-MM-dd"));
	if (!SetMezzAssQuery.exec()) {
		QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
		return;
	}
#endif // !NO_DATABASE

	if (CurrAss->GetScenarioName() != key) {
		m_DirtyAssumptions.remove(key);
		m_Assumptions.insert(CurrAss->GetScenarioName(), CurrAss);
		m_Assumptions.remove(key);
		for (int j = 0; j < m_ScenariosModel->rowCount(); ++j) {
			if (key.compare(
				m_ScenariosModel->data(m_ScenariosModel->index(j, 0), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0
			) {m_ScenariosModel->removeRow(j); break;}
		}
		m_ScenariosModel->insertRow(m_ScenariosModel->rowCount());
		m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), CurrAss->GetScenarioName(), Qt::EditRole);
		m_ScenarioList->selectionModel()->setCurrentIndex(m_SortScenarios->mapFromSource(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0)), QItemSelectionModel::ClearAndSelect);
	}
	else {
		m_Assumptions[key] = m_DirtyAssumptions.value(key);
		m_DirtyAssumptions.remove(key);
	}
	if (KeyIsCurrent) {
		ActiveAssumption = m_Assumptions.value(key);
		ChangeScenario(CurrentIdx, CurrentIdx);
		CheckCurrentDirty();
		m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	}
}

void LoanAssumptionsEditor::DiscardScenario(const QString& key) {
	if (!m_Assumptions.contains(key)) return;
	bool KeyIsCurrent=false;
	QModelIndex CurrentIdx;
	if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
		CurrentIdx = m_ScenarioList->selectionModel()->currentIndex();
		KeyIsCurrent=key.compare(
			m_ScenariosModel->data(m_SortScenarios->mapToSource(CurrentIdx), Qt::EditRole).toString()
			, Qt::CaseInsensitive
		)==0;
	}
	m_DirtyAssumptions.remove(key);
	if (!m_Assumptions.value(key)) { 
		//Remove Added Scenarios
		QModelIndex ScenIdx;
		for (int j = 0; j < m_ScenariosModel->rowCount() && !ScenIdx.isValid(); ++j) {
			if (key.compare(
				m_ScenariosModel->data(m_ScenariosModel->index(j, 0), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0
				)ScenIdx = m_ScenariosModel->index(j, 0);
		}
		if (ScenIdx.isValid()) {
			m_ScenariosModel->removeRow(ScenIdx.row());
		}
		m_Assumptions.remove(key); 
		ActiveAssumption.reset(nullptr);
	}
	if (KeyIsCurrent) {
		ChangeScenario(CurrentIdx, CurrentIdx);
		CheckCurrentDirty();
		m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	}
}

void LoanAssumptionsEditor::SeniorDateChanged(const QDate&) {
	if (m_seniorDateCheck->isChecked()) CheckCurrentDirty();
}
void LoanAssumptionsEditor::MezzDateChanged(const QDate&) {
	if (m_MezzDateCheck->isChecked()) CheckCurrentDirty();
}



