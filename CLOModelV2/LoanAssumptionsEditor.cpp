#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
#include "LoanAssumptionsEditor.h"
#include "CommonFunctions.h"
#include <QProgressDialog>
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
#include <QTabWidget>
#include <QFileDialog>
#include "AssumptionsComboDelegate.h"
#include "LoanAssumptionDelegate.h"
#include "RichTextDelegate.h"
#include "Mortgage.h"
#include "ReadOnlyColProxy.h"
#include "CheckAndEditDelegate.h"
#include <QScrollBar>
#include "LoanAssMatcher.h"
#include <QProgressBar>
LoanAssumptionsEditor::LoanAssumptionsEditor(QWidget *parent)
	: QWidget(parent)
	, ActiveAssumption(nullptr)
	, m_LastPoolColSorted(1)
	, m_LastScannedColSorted(1)
	, m_EnableLoad(true)
	, m_ScanningPools(false)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle(tr("Loan Scenarios Editor"));
	BaseTab = new QTabWidget(this);
	BaseTab->setTabPosition(QTabWidget::West);	
	CreateScenarioEditor();
	CreatePoolMatcher();
	CreateStructureComparison();
	CreateModelScanner();

	m_ModelNameLabel = new QLabel(this);
	m_ModelNameLabel->setText(tr("No Model Loaded"));
	LoadPoolButton = new QPushButton(this);
	LoadPoolButton->setDefault(true);
	LoadPoolButton->setEnabled(m_EnableLoad);
	LoadPoolButton->setText(tr("Load Model"));
	SavePoolButton = new QPushButton(this);
	SavePoolButton->setEnabled(false);
	SavePoolButton->setText(tr("Save Loan Pool"));
	connect(LoadPoolButton, SIGNAL(clicked()), this, SLOT(LoadModel()));
	connect(SavePoolButton, &QPushButton::clicked, this, &LoanAssumptionsEditor::SavePool);

	QGridLayout* MainLay = new QGridLayout(this);
	MainLay->addWidget(BaseTab,0,0,1,4);
	MainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred),1,0);
	MainLay->addWidget(m_ModelNameLabel, 1, 1);
	MainLay->addWidget(LoadPoolButton, 1, 2);
	MainLay->addWidget(SavePoolButton, 1, 3);
}
void LoanAssumptionsEditor::CreateScenarioEditor() {
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
	SafeSetModel(m_ScenarioList, m_SortScenarios);
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
	NamesLay->addWidget(m_ScenarioNameEdit, 0, 1, 1, 4);
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
	SafeSetModel(m_SeniorTable, m_SeniorAsumptionsModel);
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
	SafeSetModel(m_MezzTable, m_MezzAsumptionsModel);
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
	LeftLay->addWidget(m_ScenarioList, 2, 0, 1, 2);
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

	
	QWidget* TempTab = new QWidget(this);;
	QGridLayout* mainLay = new QGridLayout(TempTab);
	/*mainLay->setSpacing(0);
	mainLay->setMargin(0);*/
	mainLay->addWidget(MainSplit, 0, 0, 1, 5);
	mainLay->addWidget(BottomLine, 1, 0, 1, 5);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 2, 0);
	mainLay->addWidget(DiscardAllButton, 2, 1);
	mainLay->addWidget(DiscardCurrentButton, 2, 2);
	mainLay->addWidget(SaveAllButton, 2, 3);
	mainLay->addWidget(SaveCurrentButton, 2, 4);
	BaseTab->addTab(TempTab, tr("Scenarios Editor"));

	connect(m_ScenariosModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		m_SortScenarios->sort(0);
	});
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, this, &LoanAssumptionsEditor::ChangeScenario);
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.isValid()) m_ScenariosCombo->setCurrentIndex(index.row());
		else {
			RemoveAliasButton->setEnabled(false);
			AddAliasButton->setEnabled(false);
			ReplaceAliasButton->setEnabled(false );
			AddSeniorAssumptionButton->setEnabled(false);
			RemoveSeniorAssumptionButton->setEnabled(false);
			m_SeniorDateEdit->setEnabled(false);
			m_seniorDateCheck->setEnabled(false);
			AddMezzAssumptionButton->setEnabled(false);
			RemoveMezzAssumptionButton->setEnabled(false);
			m_MezzDateEdit->setEnabled(false);
			m_MezzDateCheck->setEnabled(false);
		}
		
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
		if (m_AliasesList->selectionModel()->currentIndex().isValid()) {
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
	connect(RemoveScenarioButton, SIGNAL(clicked()), this, SLOT(RemoveScenario()));
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
		if (YesNoDialog(DiscardAllButton->text(), tr("Are you sure you want to discard changes to %n scenario(s)", "", m_DirtyAssumptions.size()))) {
			auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
				DiscardScenario(k);
			}
		}
		CheckAllDirty();
	});
	connect(SaveCurrentButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
			if (YesNoDialog(SaveCurrentButton->text(), tr("Are you sure you want to save changes to %1").arg(m_ScenariosModel->data(currIdx, Qt::EditRole).toString()))) {
				SaveScenario(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
			}
		}
	});
	connect(SaveAllButton, &QPushButton::clicked, [&](bool) {
		auto CurrentIDx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
		if (YesNoDialog(SaveAllButton->text(), tr("Are you sure you want to save changes to %n scenario(s)", "", m_DirtyAssumptions.size()))) {
			auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
				SaveScenario(k);
			}
		}
		CheckAllDirty();
		m_ScenarioList->selectionModel()->setCurrentIndex(m_SortScenarios->mapFromSource(CurrentIDx),QItemSelectionModel::ClearAndSelect);
	});


	connect(RemoveSeniorAssumptionButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
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
		bool CurrentDirty, Anydirty;
		Anydirty = CurrentDirty = m_ScenariosModel->data(index, Qt::UserRole).toInt() != RichTextDelegate::NotDirty;
		for (int i = 0; i < m_ScenariosModel->rowCount() && !Anydirty; ++i) {
			Anydirty = m_ScenariosModel->data(m_ScenariosModel->index(i, 0), Qt::UserRole).toInt() != RichTextDelegate::NotDirty;
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
void LoanAssumptionsEditor::CreatePoolMatcher() {
	m_PoolModel = new QStandardItemModel(this);
	m_PoolModel->setRowCount(0);
	m_PoolModel->setColumnCount(4);
	m_PoolModel->setHorizontalHeaderLabels(QStringList() << "Issuer" << "Facility" << "Current Scenario" << "Suggested Scenario");
	m_PoolSorter = new ReadOnlyColProxy(this);
	m_PoolSorter->SetReadOnlyCol(0);
	m_PoolSorter->SetReadOnlyCol(1);
	m_PoolSorter->setSourceModel(m_PoolModel);

	m_PoolTable = new QTableView(this);
	SafeSetModel(m_PoolTable, m_PoolSorter);
	m_PoolTable->verticalHeader()->hide();
	m_PoolTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_PoolTable->horizontalHeader()->setMinimumSectionSize(130);
	m_PoolTable->horizontalHeader()->setStretchLastSection(true);
	m_PoolTable->setItemDelegateForColumn(2, new CheckAndEditDelegate(this));
	m_PoolTable->setItemDelegateForColumn(3, new CheckAndEditDelegate(this));
	m_PoolTable->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_PoolTable->setSelectionMode(QAbstractItemView::SingleSelection);

	
	GuessAssumptionsButton = new QPushButton(this);
	GuessAssumptionsButton->setText(tr("Guess Scenarios"));
	GuessAssumptionsButton->setEnabled(false);
	AcceptAllOldButton = new QPushButton(this);
	AcceptAllOldButton->setText(tr("Select all Current"));
	AcceptAllOldButton->setEnabled(false);
	AcceptAllNewButton = new QPushButton(this);
	AcceptAllNewButton->setText(tr("Select all Suggested"));
	AcceptAllNewButton->setEnabled(false);
	AcceptAllNonEmptyButton = new QPushButton(this);
	AcceptAllNonEmptyButton->setText(tr("Select all non-empty Suggested"));
	AcceptAllNonEmptyButton->setEnabled(false);
	

	
	connect(GuessAssumptionsButton, &QPushButton::clicked, this, &LoanAssumptionsEditor::GuessAssumptions);
	
	connect(m_PoolTable->horizontalHeader(), &QHeaderView::sectionClicked, [&](int a) {
		if (m_LastPoolColSorted == a+1) 
			m_LastPoolColSorted = -(a+1);
		else 
			m_LastPoolColSorted = a+1;
		SortPool();
	});
	connect(m_PoolModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.column() == (qAbs(m_LastPoolColSorted) - 1)) SortPool();
	});
	connect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
	connect(m_PoolModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex&, int,int) {
		bool EnableItems = m_PoolModel->rowCount() > 0;
		AcceptAllOldButton->setEnabled(EnableItems);
		AcceptAllNewButton->setEnabled(EnableItems);
		AcceptAllNonEmptyButton->setEnabled(EnableItems);
		GuessAssumptionsButton->setEnabled(EnableItems);
		SavePoolButton->setEnabled(EnableItems);
		CalculateNewStructureButton->setEnabled(EnableItems);
	});
	connect(m_PoolModel, &QStandardItemModel::rowsRemoved, [&](const QModelIndex&, int, int) {
		bool EnableItems = m_PoolModel->rowCount() > 0;
		AcceptAllOldButton->setEnabled(EnableItems);
		AcceptAllNewButton->setEnabled(EnableItems);
		AcceptAllNonEmptyButton->setEnabled(EnableItems);
		GuessAssumptionsButton->setEnabled(EnableItems);
		SavePoolButton->setEnabled(EnableItems);
		CalculateNewStructureButton->setEnabled(EnableItems);
	});
	connect(AcceptAllOldButton, &QPushButton::clicked, [&]() {
		disconnect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
		for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
			m_PoolModel->setData(m_PoolModel->index(i, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
			m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
		}
		connect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
	});
	connect(AcceptAllNewButton, &QPushButton::clicked, [&]() {
		disconnect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
		for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
			m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
			m_PoolModel->setData(m_PoolModel->index(i, 2), Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
		}
		connect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
	});
	connect(AcceptAllNonEmptyButton, &QPushButton::clicked, [&]() {
		disconnect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
		for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
			if (m_PoolModel->data(m_PoolModel->index(i, 3),Qt::EditRole).toString().isEmpty()){
				m_PoolModel->setData(m_PoolModel->index(i, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
				m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
			}
			else {
				m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
				m_PoolModel->setData(m_PoolModel->index(i, 2), Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
			}
		}
		connect(m_PoolModel, &QStandardItemModel::dataChanged, this, &LoanAssumptionsEditor::SetPoolModelChecks);
	});

	QWidget* TempTab = new QWidget(this);;
	QGridLayout* mainLay = new QGridLayout(TempTab);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 0, 1, 3);
	mainLay->addWidget(GuessAssumptionsButton, 0, 3);
	mainLay->addWidget(m_PoolTable, 1, 0,1,4);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 2, 0);
	mainLay->addWidget(AcceptAllOldButton, 2, 1);
	mainLay->addWidget(AcceptAllNewButton, 2, 2);
	mainLay->addWidget(AcceptAllNonEmptyButton, 2, 3);
	BaseTab->addTab(TempTab, tr("Pool"));
}
#ifndef NO_DATABASE
void LoanAssumptionsEditor::FillFromQuery() {
	m_Assumptions.clear();
	Db_Mutex.lock();
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
		QSqlQuery LoanAssQuery(db);
		LoanAssQuery.setForwardOnly(true);
		LoanAssQuery.prepare("{CALL " + GetFromConfig("Database", "GetAllLoanAssumptionStoredProc", "getAllLoanAssumptions") + "}");
		if (LoanAssQuery.exec()) {
			bool CurrentSenior;
			int FieldCount;
			while (LoanAssQuery.next()) {
				FieldCount = 0;
				auto LoanAssRecord = LoanAssQuery.record();
				QString CurrentScenario = LoanAssRecord.value(FieldCount).toString(); ++FieldCount;
				QSharedPointer<LoanAssumption> CurrentAss(nullptr);
				for (auto i = m_Assumptions.begin(); i != m_Assumptions.end(); ++i) {
					if (i.key().compare(CurrentScenario, Qt::CaseInsensitive) == 0) {
						CurrentAss = i.value();
						break;
					}
				}
				if (!CurrentAss) {
					CurrentAss = QSharedPointer<LoanAssumption>(new LoanAssumption(CurrentScenario));
					if (!LoanAssRecord.isNull(FieldCount)) CurrentAss->SetAliases(LoanAssRecord.value(FieldCount).toString());
					m_Assumptions.insert(CurrentAss->GetScenarioName(), CurrentAss);
				}++FieldCount;
				CurrentSenior = LoanAssRecord.value(FieldCount).toBool(); ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorMaturityExtension(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzMaturityExtension(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorInitialHaircut(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzInitialHaircut(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrepaymentFee(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzPrepaymentFee(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDayCount(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzDayCount(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorCPR(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzCPR(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorCDR(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzCDR(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLS(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzLS(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorRecoveryLag(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzRecoveryLag(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDelinquency(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzDelinquency(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDelinquencyLag(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzDelinquencyLag(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrice(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzPrice(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorHaircut(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzHaircut(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrepayMultiplier(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzPrepayMultiplier(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLossMultiplier(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzLossMultiplier(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLastUpdate(LoanAssRecord.value(FieldCount).toDate());
					else CurrentAss->SetMezzLastUpdate(LoanAssRecord.value(FieldCount).toDate());
				} ++FieldCount;

			}
		}
	}
	Db_Mutex.unlock();
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
	if (!curr.isValid()) return;
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
	m_MezzDateCheck->setEnabled(ActiveAssumption);
	m_seniorDateCheck->setEnabled(ActiveAssumption);
	m_ScenarioNameEdit->setEnabled(ActiveAssumption);
	m_AliasLineEdit->setEnabled(ActiveAssumption);
	AddSeniorAssumptionButton->setEnabled(ActiveAssumption);
	AddMezzAssumptionButton->setEnabled(ActiveAssumption);
	RemoveSeniorAssumptionButton->setEnabled(ActiveAssumption && m_SeniorAsumptionsModel->rowCount()>0);
	RemoveMezzAssumptionButton->setEnabled(ActiveAssumption && m_MezzAsumptionsModel->rowCount()>0);
	RemoveAliasButton->setEnabled(ActiveAssumption && m_AliasesModel->rowCount()>0);
	ReplaceAliasButton->setEnabled(ActiveAssumption && m_AliasesModel->rowCount()>0);
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
		CurrentIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
		KeyIsCurrent = key.compare(
			m_ScenariosModel->data(CurrentIdx, Qt::EditRole).toString()
			, Qt::CaseInsensitive
			) == 0;
	}
#ifndef NO_DATABASE
	QMutexLocker DbLocker(&Db_Mutex);
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
	DbLocker.unlock();
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
		for (int j = 0; j < m_PoolModel->rowCount(); ++j) {
			if (key.compare(
				m_PoolModel->data(m_PoolModel->index(j, 3), Qt::EditRole).toString()
				, Qt::CaseInsensitive
			) == 0) 
				m_PoolModel->setData(m_PoolModel->index(j, 3), CurrAss->GetScenarioName(), Qt::EditRole);
			
		}
	}
	else {
		m_Assumptions[key] = m_DirtyAssumptions.value(key);
		m_DirtyAssumptions.remove(key);
	}
	if (KeyIsCurrent) {
		ActiveAssumption = m_Assumptions.value(key);
		ChangeScenario(m_SortScenarios->mapFromSource(CurrentIdx), m_SortScenarios->mapFromSource(CurrentIdx));
		CheckCurrentDirty();
		m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	}
}

void LoanAssumptionsEditor::DiscardScenario(const QString& key) {
	if (!m_Assumptions.contains(key)) return;
	bool KeyIsCurrent=false;
	QModelIndex CurrentIdx;
	if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
		CurrentIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
		KeyIsCurrent=key.compare(
			m_ScenariosModel->data(CurrentIdx, Qt::EditRole).toString()
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
		for (int j = 0; j < m_PoolModel->rowCount(); ++j) {
			if (key.compare(
				m_PoolModel->data(m_PoolModel->index(j, 3), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0) {
				m_PoolModel->setData(m_PoolModel->index(j, 3), QVariant(), Qt::EditRole);
				m_PoolModel->setData(m_PoolModel->index(j, 3), QVariant(), Qt::UserRole);
			}
		}
		m_DirtyAssumptions.remove(key);
		m_ScenariosModel->removeRow(ScenIdx.row());
		m_Assumptions.remove(key); 
	}
	if (KeyIsCurrent) {
		ChangeScenario(m_SortScenarios->mapFromSource(CurrentIdx), m_SortScenarios->mapFromSource(CurrentIdx));
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

void LoanAssumptionsEditor::AddLoanAssumption(const LoanAssumption& a) {
	m_Assumptions.insert(a.GetScenarioName(), QSharedPointer<LoanAssumption>(new LoanAssumption(a)));
	m_ScenariosModel->insertRow(m_ScenariosModel->rowCount());
	m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), a.GetScenarioName(), Qt::EditRole);
}

void LoanAssumptionsEditor::AddLoanToPool(qint32 index,Mortgage& a) {
	m_PoolModel->insertRow(m_PoolModel->rowCount());
	m_PoolModel->setData(m_PoolModel->index(m_PoolModel->rowCount() - 1, 0), a.GetProperty("Issuer"), Qt::EditRole);
	m_PoolModel->setData(m_PoolModel->index(m_PoolModel->rowCount() - 1, 0), index, Qt::UserRole);
	m_PoolModel->setData(m_PoolModel->index(m_PoolModel->rowCount() - 1, 1), a.GetProperty("Facility"), Qt::EditRole);
	m_PoolModel->setData(m_PoolModel->index(m_PoolModel->rowCount() - 1, 2), a.GetProperty("Scenario"), Qt::EditRole);
	m_PoolModel->setData(m_PoolModel->index(m_PoolModel->rowCount() - 1, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
	m_LoanPool.AddLoan(a, m_LoanPool.GetLoans().size());
}

void LoanAssumptionsEditor::LoadModel() {
	QString ModelPath = QFileDialog::getOpenFileName(this, tr("Open Model"), GetFromConfig("Folders", "UnifiedResultsFolder", QString()), tr("CLO Models (*clom)"));
	if (ModelPath.isNull()) return;
	m_ModelNameLabel->setText(tr("No Model Loaded"));
	m_PoolModel->setRowCount(0);
	m_LastModelLoaded.clear();
	SafeSetModel(m_OldStrDetTable, nullptr);
	m_OriginalTranchesModels.clear();
	m_NewTranchesModels.clear();
	BaseTab->setTabEnabled(2,false);
	QFile file(ModelPath);
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return;
	}
	QProgressDialog LoadProgress(tr("Loading Model"), QString(), 0, 4, this);
	LoadProgress.setCancelButton(nullptr); 
	LoadProgress.setWindowModality(Qt::WindowModal);
	m_LastModelLoaded = ModelPath;
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	m_WtfToExtension.SetLoadProtocolVersion(VersionChecker);
	out >> m_WtfToExtension;
	LoadProgress.setValue(1);
	m_WtfToCall.SetLoadProtocolVersion(VersionChecker);
	out >> m_WtfToCall;
	LoadProgress.setValue(2);
	m_LoanPool.SetLoadProtocolVersion(VersionChecker);
	out >> m_LoanPool;
	LoadProgress.setValue(3);
	file.close();

	m_PoolModel->setRowCount(m_LoanPool.GetLoans().size());
	int RowCounter = 0;
	for (auto i = m_LoanPool.GetLoans().constBegin(); i != m_LoanPool.GetLoans().constEnd(); ++i, ++RowCounter) {
		m_PoolModel->setData(m_PoolModel->index(RowCounter, 0), i.value()->GetProperty("Issuer"), Qt::EditRole);
		m_PoolModel->setData(m_PoolModel->index(RowCounter, 0), i.key(), Qt::UserRole);
		m_PoolModel->setData(m_PoolModel->index(RowCounter, 1), i.value()->GetProperty("Facility"), Qt::EditRole);
		m_PoolModel->setData(m_PoolModel->index(RowCounter, 2), i.value()->GetProperty("Scenario"), Qt::EditRole);
		m_PoolModel->setData(m_PoolModel->index(RowCounter, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
	}
	LoadProgress.setValue(4);

	m_ModelNameLabel->setText(tr("Model Loaded: %1").arg(m_WtfToExtension.GetDealName()));

	m_OriginalStructureModel->setRowCount(m_WtfToExtension.GetTranchesCount());
	for (int i = 0; i < m_WtfToExtension.GetTranchesCount(); ++i) {
		m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, TrancheNameCol), m_WtfToExtension.GetTranche(i)->GetTrancheName(),Qt::EditRole);
		m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, PriceCol), Commarize(m_WtfToExtension.GetTranche(i)->GetPrice(), 2), Qt::EditRole);
		m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, PriceCol), m_WtfToExtension.GetTranche(i)->GetPrice(), Qt::UserRole);
		m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, WALCol), Commarize(m_WtfToExtension.GetTranche(i)->GetWALife(m_WtfToExtension.GetCalculatedMtgPayments().GetDate(0)), 2), Qt::EditRole);
		m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, LossCol), Commarize(m_WtfToExtension.GetTranche(i)->GetLossRate()*100.0, 2)+'%', Qt::EditRole);
		if (m_WtfToCall.GetTranche(i)){
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, CallDateCol), m_WtfToCall.GetCalledPeriod().toString("MMM-yy"), Qt::EditRole);
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, CallDateCol), m_WtfToCall.GetCalledPeriod(), Qt::UserRole);
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, WALCallCol), Commarize(m_WtfToCall.GetTranche(i)->GetWALife(m_WtfToCall.GetCalculatedMtgPayments().GetDate(0)), 2), Qt::EditRole);
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, LossCallCol), Commarize(m_WtfToCall.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
		}
		else {
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, CallDateCol), tr("N/A"), Qt::EditRole);
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, CallDateCol), QVariant(), Qt::UserRole);
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, WALCallCol), tr("N/A"), Qt::EditRole);
			m_OriginalStructureModel->setData(m_OriginalStructureModel->index(i, LossCallCol), tr("N/A"), Qt::EditRole);
		}
		QStandardItemModel* CurrentModel = new QStandardItemModel(this);
		const TrancheCashFlow& CurrentCF= m_WtfToExtension.GetTranche(i)->GetCashFlow();
		auto CFTypes = CurrentCF.AvailableFlows();
		std::sort(CFTypes.begin(), CFTypes.end());
		CurrentModel->setColumnCount(CFTypes.size()+1);
		CurrentModel->setRowCount(CurrentCF.Count());
		CurrentModel->setHeaderData(0, Qt::Horizontal, tr("Date"));
		for (int j = 0; j < CurrentCF.Count(); ++j) {
			CurrentModel->setData(CurrentModel->index(j, 0), CurrentCF.GetDate(j).toString("MMM-yy"), Qt::EditRole);
			CurrentModel->setData(CurrentModel->index(j, 0), CurrentCF.GetDate(j), Qt::UserRole);
		}
		for (int j = 0; j < CFTypes.size(); ++j) {
			CurrentModel->setHeaderData(j+1, Qt::Horizontal, CurrentCF.GetLabel(CFTypes.at(j), tr("Flow %1").arg(j)));
			for (int k = 0; k < CurrentCF.Count(); k++) {
				switch (CFTypes.at(j)) {
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow):
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow) :
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICTarget) :
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCTarget) : {
					double TestVal = CurrentCF.GetFlow(k, CFTypes.at(j));
					if (TestVal>static_cast<double>(TestInfinityThreshold))CurrentModel->setData(CurrentModel->index(k, j + 1), "Infinity", Qt::EditRole);
					else CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(TestVal*100.0, 2) + '%', Qt::EditRole);
				}break;
				default:
					CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(CurrentCF.GetFlow(k, CFTypes.at(j))), Qt::EditRole);
					break;
				}
				
				CurrentModel->setData(CurrentModel->index(k, j + 1), CurrentCF.GetFlow(k, CFTypes.at(j)), Qt::UserRole);
			}
		}
		m_OriginalTranchesModels.append(QSharedPointer<QStandardItemModel>(CurrentModel, [](QStandardItemModel* a) {a->deleteLater(); }));
	}
	BaseTab->setTabEnabled(2, true);
}

void LoanAssumptionsEditor::GuessAssumptions() {
	if (!YesNoDialog(tr("Are you sure?"), tr("This action will potentially override the current inputs.\nAre you sure you want to continue?"))) return;
	int MatchFound = 0;
	QSharedPointer<LoanAssumption> CurrAss;
	QProgressDialog LoadProgress(tr("Searching for Matches"), QString(), 0, m_PoolModel->rowCount()*m_Assumptions.size(), this);
	LoadProgress.setCancelButton(nullptr);
	LoadProgress.setWindowModality(Qt::WindowModal);
	for (int i = 0; i < m_PoolModel->rowCount();++i){
		for (auto j = m_Assumptions.constBegin(); j != m_Assumptions.constEnd(); ++j) {
			CurrAss= m_DirtyAssumptions.value(j.key(), j.value());
			if (!CurrAss) continue;
			if (CurrAss->MatchPattern(m_PoolModel->data(m_PoolModel->index(i, 0), Qt::EditRole).toString())) {
				m_PoolModel->setData(m_PoolModel->index(i, 3), j.key(), Qt::EditRole);
				m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
				++MatchFound;
			}
			else if (CurrAss->MatchPattern(m_PoolModel->data(m_PoolModel->index(i, 1), Qt::EditRole).toString())) {
				m_PoolModel->setData(m_PoolModel->index(i, 3), j.key(), Qt::EditRole);
				m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
				++MatchFound;
			}
			LoadProgress.setValue(LoadProgress.value() + 1);
		}
	}
	QMessageBox::information(this, tr("Results"), tr("%n Match(es) Found", "", MatchFound));
}

void LoanAssumptionsEditor::RemoveScenario() {
	if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
		auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
		if (m_ScenariosModel->data(currIdx, Qt::UserRole) == RichTextDelegate::Added) {
			QString key = m_ScenariosModel->data(currIdx, Qt::EditRole).toString();
			for (int j = 0; j < m_PoolModel->rowCount(); ++j) {
				if (key.compare(
					m_PoolModel->data(m_PoolModel->index(j, 3), Qt::EditRole).toString()
					, Qt::CaseInsensitive
					) == 0) {
					m_PoolModel->setData(m_PoolModel->index(j, 3), QVariant(), Qt::EditRole);
					m_PoolModel->setData(m_PoolModel->index(j, 3), QVariant(), Qt::UserRole);
				}
			}
			m_DirtyAssumptions.remove(key);
			m_ScenariosModel->removeRow(currIdx.row());
		}
		else {
			m_ScenariosModel->setData(currIdx, RichTextDelegate::Erased, Qt::UserRole);
			m_DirtyAssumptions[m_ScenariosModel->data(currIdx, Qt::EditRole).toString()] = QSharedPointer<LoanAssumption>(nullptr);
			ActiveAssumption.reset(nullptr);
		}
		m_ScenarioNameEdit->setEnabled(ActiveAssumption);
		m_AliasLineEdit->setEnabled(ActiveAssumption);
		AddSeniorAssumptionButton->setEnabled(ActiveAssumption);
		AddMezzAssumptionButton->setEnabled(ActiveAssumption);
		m_seniorDateCheck->setEnabled(ActiveAssumption);
		m_SeniorDateEdit->setEnabled(ActiveAssumption);
		m_MezzDateCheck->setEnabled(ActiveAssumption);
		m_MezzDateEdit->setEnabled(ActiveAssumption);
	}
	RemoveScenarioButton->setEnabled(m_ScenariosModel->rowCount() > 0);
}

void LoanAssumptionsEditor::SetPoolModelChecks(const QModelIndex& index, const QModelIndex&) {
	if (index.column() >= 2) {
		int OtherCol = (index.column() == 2 ? 3 : 2);
		auto CurrVal = m_PoolModel->data(index, Qt::UserRole + Qt::CheckStateRole).toInt() != Qt::Checked;
		auto PrevVal = m_PoolModel->data(m_PoolModel->index(index.row(), OtherCol), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked;
		if (CurrVal != PrevVal) {
			m_PoolModel->setData(m_PoolModel->index(index.row(), OtherCol), CurrVal ? Qt::Checked : Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
		}
	}
}

void LoanAssumptionsEditor::SortPool() {
	m_PoolSorter->sort(qAbs(m_LastPoolColSorted) - 1, m_LastPoolColSorted<0 ? Qt::DescendingOrder : Qt::AscendingOrder);
}
void LoanAssumptionsEditor::SortScanned() {
	m_ScannedModelProxy->sort(qAbs(m_LastScannedColSorted) - 1, m_LastScannedColSorted<0 ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void LoanAssumptionsEditor::SavePool() {
	if (!YesNoDialog(tr("Save Loan Pool"), tr("Are you sure you want to override the previous pool?\nPlease note that changes to the scenarios will not apply until the model is recalculated"))) return;
	emit PoolSaved();
	if (m_LastModelLoaded.isEmpty()) return;
	bool SomethingToChange = false;
	for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
		if (m_PoolModel->data(m_PoolModel->index(i, 3), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked) {
			Mortgage* CurrLoan = m_LoanPool.GetLoans().value(m_PoolModel->data(m_PoolModel->index(i, 0), Qt::UserRole).toInt(), nullptr);
			if (CurrLoan) {
				auto NewScenario = m_PoolModel->data(m_PoolModel->index(i, 3), Qt::EditRole).toString();
				if (NewScenario.isEmpty()) CurrLoan->RemoveProperty("Scenario");
				else CurrLoan->SetProperty("Scenario", NewScenario);
				SomethingToChange = true;
			}
		}
	}
	if (!SomethingToChange) return;
	QFile file(m_LastModelLoaded);
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return;
	}
	QProgressDialog LoadProgress("Saving Model", QString(), 0, 10, this); 
	LoadProgress.setValue(0);
	LoadProgress.setCancelButton(nullptr);
	LoadProgress.setWindowModality(Qt::WindowModal);
	QDate OldLiborUpdateDate; out >> OldLiborUpdateDate;
	LoadProgress.setValue(LoadProgress.value()+1);
	bool OldUseForwardCurve; out >> OldUseForwardCurve;
	LoadProgress.setValue(LoadProgress.value() + 1);
	bool OldBaseCaseToCall; out >> OldBaseCaseToCall;
	LoadProgress.setValue(LoadProgress.value() + 1);
	Waterfall OldExtension; OldExtension.SetLoadProtocolVersion(VersionChecker); out >> OldExtension; 
	LoadProgress.setValue(LoadProgress.value() + 1);
	Waterfall OldCall; OldCall.SetLoadProtocolVersion(VersionChecker); out >> OldCall; 
	LoadProgress.setValue(LoadProgress.value() + 1);
	{MtgCalculator junk;	junk.SetLoadProtocolVersion(VersionChecker); out >> junk; }
	LoadProgress.setValue(LoadProgress.value() + 1);
	ConstantBaseRateTable OldConstTable; OldConstTable.SetLoadProtocolVersion(VersionChecker); out >> OldConstTable;
	LoadProgress.setValue(LoadProgress.value() + 1);
	ForwardBaseRateTable OldFwdTable; OldFwdTable.SetLoadProtocolVersion(VersionChecker); out >> OldFwdTable;
	LoadProgress.setValue(LoadProgress.value() + 1);
	file.close();
	file.open(QIODevice::WriteOnly);
	QDataStream In(&file);
	In.setVersion(QDataStream::Qt_5_3);
	In 
		<< qint32(ModelVersionNumber)
		<< OldLiborUpdateDate
		<< OldUseForwardCurve 
		<< OldBaseCaseToCall 
		<< OldExtension
		<< OldCall
		<< m_LoanPool 
		<< OldConstTable 
		<< OldFwdTable
	;
	LoadProgress.setValue(LoadProgress.value() + 1);
	file.close();
}

QHash<qint32, QString> LoanAssumptionsEditor::GetScenarios() const {
	QHash<qint32, QString> Result;
	for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
		if (m_PoolModel->data(m_PoolModel->index(i, 3), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked) 
			Result.insert(m_PoolModel->data(m_PoolModel->index(i, 0), Qt::UserRole).toInt(), m_PoolModel->data(m_PoolModel->index(i, 3), Qt::EditRole).toString());
		else
			Result.insert(m_PoolModel->data(m_PoolModel->index(i, 0), Qt::UserRole).toInt(), m_PoolModel->data(m_PoolModel->index(i, 2), Qt::EditRole).toString());
	}
	return Result;
}

void LoanAssumptionsEditor::SetEnableLoad(bool a) {
	m_EnableLoad = a;
	LoadPoolButton->setEnabled(m_EnableLoad);
}

int LoanAssumptionsEditor::LoanCount() const {
	return m_LoanPool.GetLoans().count();
}

void LoanAssumptionsEditor::CreateStructureComparison() {
	m_OriginalStructureModel = new QStandardItemModel(this);
	m_NewStructureModel = new QStandardItemModel(this);
	m_OriginalStructureModel->setColumnCount(ColumnsInStructure);
	m_NewStructureModel->setColumnCount(ColumnsInStructure);
	m_OriginalStructureModel->setRowCount(0);
	m_NewStructureModel->setRowCount(0);
	m_OriginalStructureModel->setHorizontalHeaderLabels(QStringList() << tr("Tranche") << tr("Call Date") << tr("Price") << tr("IRR") << tr("WAL") << tr("DM") << tr("Loss") << tr("Call IRR") << tr("Call WAL") << tr("Call DM") << tr("Call Loss"));
	m_NewStructureModel->setHorizontalHeaderLabels(QStringList() << tr("Tranche") << tr("Call Date") << tr("Price") << tr("IRR") << tr("WAL") << tr("DM") << tr("Loss") << tr("Call IRR") << tr("Call WAL") << tr("Call DM") << tr("Call Loss"));

	m_OldStrGenTable = new QTableView(this);
	SafeSetModel(m_OldStrGenTable, m_OriginalStructureModel);
	connect(m_OldStrGenTable->selectionModel(), &QItemSelectionModel::currentChanged, this, &LoanAssumptionsEditor::OldGenTableSelect,Qt::QueuedConnection);
	m_OldStrGenTable->verticalHeader()->hide();
	m_OldStrGenTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_OldStrGenTable->horizontalHeader()->setStretchLastSection(true);
	m_OldStrGenTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_OldStrGenTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_OldStrGenTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_OldStrGenTable->horizontalHeader()->setMinimumSectionSize(50);
	m_OldStrGenTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_OldStrGenTable->setAlternatingRowColors(true);
	m_OldStrDetTable = new QTableView(this);
	SafeSetModel(m_OldStrDetTable, nullptr);
	m_OldStrDetTable->verticalHeader()->hide();
	m_OldStrDetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_OldStrDetTable->horizontalHeader()->setStretchLastSection(true);
	m_OldStrDetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_OldStrDetTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_OldStrDetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_OldStrDetTable->horizontalHeader()->setMinimumSectionSize(50);
	m_OldStrDetTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	m_OldStrDetTable->setAlternatingRowColors(true);

	m_NewStrGenTable = new QTableView(this);
	SafeSetModel(m_NewStrGenTable, nullptr);
	m_NewStrGenTable->verticalHeader()->hide();
	m_NewStrGenTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_NewStrGenTable->horizontalHeader()->setStretchLastSection(true);
	m_NewStrGenTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_NewStrGenTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_NewStrGenTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_NewStrGenTable->horizontalHeader()->setMinimumSectionSize(50);
	m_NewStrGenTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_NewStrGenTable->setAlternatingRowColors(true);
	m_NewStrDetTable = new QTableView(this);
	SafeSetModel(m_NewStrDetTable, nullptr);
	m_NewStrDetTable->verticalHeader()->hide();
	m_NewStrDetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_NewStrDetTable->horizontalHeader()->setStretchLastSection(true);
	m_NewStrDetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_NewStrDetTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_NewStrDetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_NewStrDetTable->horizontalHeader()->setMinimumSectionSize(50);
	m_NewStrDetTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	m_NewStrDetTable->setAlternatingRowColors(true);

	connect(m_OriginalStructureModel, SIGNAL(rowsInserted(const QModelIndex&,int, int)), this, SLOT(AdjustOldGenTableHeight()), Qt::QueuedConnection);
	connect(m_OriginalStructureModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(AdjustOldGenTableHeight()), Qt::QueuedConnection);
	connect(m_OriginalStructureModel, SIGNAL(modelReset()), this, SLOT(AdjustOldGenTableHeight()), Qt::QueuedConnection);
	connect(m_NewStructureModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(AdjustNewGenTableHeight()), Qt::QueuedConnection);
	connect(m_NewStructureModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(AdjustNewGenTableHeight()), Qt::QueuedConnection);
	connect(m_NewStructureModel, SIGNAL(modelReset()), this, SLOT(AdjustNewGenTableHeight()), Qt::QueuedConnection);

	connect(m_OldStrGenTable->verticalScrollBar(), &QScrollBar::valueChanged, m_NewStrGenTable->verticalScrollBar(), &QScrollBar::setValue);
	connect(m_NewStrGenTable->verticalScrollBar(), &QScrollBar::valueChanged, m_OldStrGenTable->verticalScrollBar(), &QScrollBar::setValue);
	connect(m_OldStrDetTable->verticalScrollBar(), &QScrollBar::valueChanged, m_NewStrDetTable->verticalScrollBar(), &QScrollBar::setValue);
	connect(m_NewStrDetTable->verticalScrollBar(), &QScrollBar::valueChanged, m_OldStrDetTable->verticalScrollBar(), &QScrollBar::setValue);
	connect(m_OldStrGenTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_NewStrGenTable->horizontalScrollBar(), &QScrollBar::setValue);
	connect(m_NewStrGenTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_OldStrGenTable->horizontalScrollBar(), &QScrollBar::setValue);
	connect(m_OldStrDetTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_NewStrDetTable->horizontalScrollBar(), &QScrollBar::setValue);
	connect(m_NewStrDetTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_OldStrDetTable->horizontalScrollBar(), &QScrollBar::setValue);

	connect(m_OriginalStructureModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.column() == PriceCol) {
			const Tranche* ApplicableExtTran = m_WtfToExtension.GetTranche(index.row());
			if (ApplicableExtTran) {
				double NewIRR = ApplicableExtTran->GetIRR(m_OriginalStructureModel->data(index, Qt::UserRole).toDouble());
				double NewDM = ApplicableExtTran->GetDiscountMargin(m_OriginalStructureModel->data(index, Qt::UserRole).toDouble());
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCol), Commarize(NewIRR*100.0, 2) + '%', Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCol), NewIRR, Qt::UserRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCol), Commarize(NewDM), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCol), NewDM, Qt::UserRole);
			}
			else {
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCol), tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCol), QVariant(), Qt::UserRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCol), tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCol), QVariant(), Qt::UserRole);
			}
			const Tranche* ApplicableCallTran = m_WtfToCall.GetTranche(index.row());
			if (ApplicableCallTran) {
				double NewIRR = ApplicableCallTran->GetIRR(m_OriginalStructureModel->data(index, Qt::UserRole).toDouble());
				double NewDM = ApplicableCallTran->GetDiscountMargin(m_OriginalStructureModel->data(index, Qt::UserRole).toDouble());
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCallCol), Commarize(NewIRR*100.0, 2) + '%', Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCallCol), NewIRR, Qt::UserRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCallCol), Commarize(NewDM), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCallCol), NewDM, Qt::UserRole);
			}
			else {
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCallCol), tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCallCol), QVariant(), Qt::UserRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCallCol), tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCallCol), QVariant(), Qt::UserRole);
			}
		}
	});
	connect(m_NewStructureModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.column() == PriceCol) {
			const Tranche* ApplicableExtTran = m_WtfToExtension.GetTranche(index.row());
			if (ApplicableExtTran) {
				double NewIRR = ApplicableExtTran->GetIRR(m_NewStructureModel->data(index, Qt::UserRole).toDouble());
				double NewDM = ApplicableExtTran->GetDiscountMargin(m_NewStructureModel->data(index, Qt::UserRole).toDouble());
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCol), Commarize(NewIRR*100.0, 2) + '%', Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCol), NewIRR, Qt::UserRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCol), Commarize(NewDM), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCol), NewDM, Qt::UserRole);
			}
			else {
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCol), tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCol), QVariant(), Qt::UserRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCol), tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCol), QVariant(), Qt::UserRole);
			}
			const Tranche* ApplicableCallTran = m_WtfToCall.GetTranche(index.row());
			if (ApplicableCallTran) {
				double NewIRR = ApplicableCallTran->GetIRR(m_NewStructureModel->data(index, Qt::UserRole).toDouble());
				double NewDM = ApplicableCallTran->GetDiscountMargin(m_NewStructureModel->data(index, Qt::UserRole).toDouble());
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCallCol), Commarize(NewIRR*100.0, 2) + '%', Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCallCol), NewIRR, Qt::UserRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCallCol), Commarize(NewDM), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCallCol), NewDM, Qt::UserRole);
			}
			else {
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCallCol), tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCallCol), QVariant(), Qt::UserRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCallCol), tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCallCol), QVariant(), Qt::UserRole);
			}
		}
	});

	CalculateNewStructureButton = new QPushButton(this);
	CalculateNewStructureButton->setText(tr("Assess Impact"));
	CalculateNewStructureButton->setEnabled(false);
	connect(CalculateNewStructureButton, &QPushButton::clicked, this, &LoanAssumptionsEditor::CalculateNewStructure);

	QLabel* BeforeLabel = new QLabel(this);
	BeforeLabel->setText(tr("Before"));
	BeforeLabel->setStyleSheet("QLabel { qproperty-alignment: 'AlignCenter'; font-size: 14px; font-weight: bold; }");
	QLabel* AfterLabel = new QLabel(this);
	AfterLabel->setText(tr("After"));
	AfterLabel->setStyleSheet(BeforeLabel->styleSheet());

	m_OldSelectedTrancheLabel = new QLabel(this);
	m_OldSelectedTrancheLabel->setStyleSheet("QLabel { qproperty-alignment: 'AlignHCenter | AlignBottom'; font-size: 12px;}");
	m_NewSelectedTrancheLabel = new QLabel(this);
	m_NewSelectedTrancheLabel->setStyleSheet(m_OldSelectedTrancheLabel->styleSheet());

	/*QFrame* MidLine = new QFrame(this);
	MidLine->setFrameShape(QFrame::VLine);
	MidLine->setFrameShadow(QFrame::Plain);
	MidLine->setLineWidth(1);*/

	QWidget* NewTab = new QWidget(this);;
	QGridLayout* mainLay = new QGridLayout(NewTab);
	mainLay->setHorizontalSpacing(mainLay->horizontalSpacing() * 4);
	mainLay->addWidget(BeforeLabel, 0, 0);
	//mainLay->addWidget(MidLine, 0, 1, 4, 1);
	mainLay->addWidget(AfterLabel, 0, 1);
	mainLay->addWidget(m_OldStrGenTable, 1, 0);
 	mainLay->addWidget(m_NewStrGenTable, 1, 1);
	mainLay->addItem(new QSpacerItem(5, 5, QSizePolicy::Preferred, QSizePolicy::Fixed),2,0,1,2);
	mainLay->addWidget(m_OldSelectedTrancheLabel, 3, 0);
	mainLay->addWidget(m_NewSelectedTrancheLabel, 3,1);
 	mainLay->addWidget(m_OldStrDetTable, 4, 0);
	mainLay->addWidget(m_NewStrDetTable, 4, 1);
	mainLay->addWidget(CalculateNewStructureButton, 5, 0, 1, 2);
	BaseTab->addTab(NewTab, tr("Notes"));
	BaseTab->setTabEnabled(2, false);
}

void LoanAssumptionsEditor::CalculateNewStructure() {
	if (m_NewLoans || m_NewWatFalls) return;
	m_NewLoans = new MtgCalculator(this);
	m_NewLoans->SetCPRass(m_LoanPool.GetCPRass());
	m_NewLoans->SetCDRass(m_LoanPool.GetCDRass());
	m_NewLoans->SetLSass(m_LoanPool.GetLSass());
	m_NewLoans->SetRecoveryLag(m_LoanPool.GetRecoveryLag());
	m_NewLoans->SetDelinquency(m_LoanPool.GetDelinquency());
	m_NewLoans->SetDelinquencyLag(m_LoanPool.GetDelinquencyLag());
	m_NewLoans->SetStartDate(m_LoanPool.GetStartDate());
	for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
		Mortgage NewLoan(*(m_LoanPool.GetLoans().value(m_PoolModel->data(m_PoolModel->index(i, 0), Qt::UserRole).toInt())));
		bool UseSuggestion = m_PoolModel->data(m_PoolModel->index(i, 3), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked;
		QString ScenarioToApply = m_PoolModel->data(m_PoolModel->index(i, 2 + (UseSuggestion ? 1 : 0)), Qt::EditRole).toString();
		if (!ScenarioToApply.isEmpty()) {
			auto CurrScen = m_DirtyAssumptions.value(ScenarioToApply, m_Assumptions.value(ScenarioToApply, QSharedPointer<LoanAssumption>(nullptr)));
			if (CurrScen) {
				NewLoan.SetScenario(*CurrScen);
			}
		}
		m_NewLoans->AddLoan(NewLoan, m_PoolModel->data(m_PoolModel->index(i, 0), Qt::UserRole).toInt());
	}
	m_NewWatFalls = new WaterfallCalculator(this);
	
	QProgressDialog* CalcProgress = new QProgressDialog(this);
	CalcProgress->setLabelText(tr("Calculating Assets Cash Flows"));
	CalcProgress->setRange(0,100);
	CalcProgress->setCancelButtonText(tr("Cancel"));
	CalcProgress->setWindowTitle(tr("Calculating"));
	CalcProgress->show();

	QProgressDialog* CalcProgress2 = new QProgressDialog(this);
	CalcProgress2->setLabelText(tr("Calculating Liabilities Cash Flows"));
	CalcProgress2->setRange(0, 100);
	CalcProgress2->setCancelButtonText(tr("Cancel"));
	CalcProgress2->setWindowTitle(tr("Calculating"));
	CalcProgress2->hide();
	
	connect(m_NewLoans, &MtgCalculator::Calculated, [&]() {
		m_NewWtfToExtension = m_WtfToExtension;
		m_NewWtfToExtension.ResetMtgFlows();
		m_NewWtfToExtension.AddMortgagesFlows(m_NewLoans->GetAggregatedResults());
		m_NewWatFalls->AddWaterfall(m_NewWtfToExtension, 0);
		if (m_WtfToCall.GetTranchesCount() > 0) {
			m_NewWtfToCall = m_WtfToCall;
			m_NewWtfToCall.ResetMtgFlows();
			m_NewWtfToCall.AddMortgagesFlows(m_NewLoans->GetAggregatedResults());
			m_NewWatFalls->AddWaterfall(m_NewWtfToCall, 1);
		}
		if (!m_NewWatFalls->StartCalculation()) { QMessageBox::critical(this, tr("Error"), tr("An error occurred during the calculations of liabilities cash flows")); }
	});
	connect(m_NewLoans, &MtgCalculator::Calculated, CalcProgress2, &QProgressDialog::show);
	connect(CalcProgress, &QProgressDialog::canceled, m_NewLoans, &MtgCalculator::StopCalculation);
	connect(m_NewLoans, &MtgCalculator::ProgressPct, CalcProgress, &QProgressDialog::setValue);
	connect(m_NewLoans, &MtgCalculator::BeeError, m_NewLoans, &MtgCalculator::StopCalculation);
	connect(m_NewLoans, &MtgCalculator::BeeError, [this]() {QMessageBox::critical(this,tr("Error"),tr("An error occurred during the calculations of assets cash flows")); });
	connect(m_NewLoans, &MtgCalculator::Calculated, m_NewLoans, &MtgCalculator::deleteLater);
	connect(m_NewLoans, &MtgCalculator::Calculated, CalcProgress, &QProgressDialog::deleteLater);
	connect(m_NewLoans, &MtgCalculator::Stopped, m_NewLoans, &MtgCalculator::deleteLater);
	connect(m_NewLoans, &MtgCalculator::Stopped, CalcProgress, &MtgCalculator::deleteLater);
	connect(m_NewLoans, &MtgCalculator::Stopped, m_NewWatFalls, &MtgCalculator::deleteLater);
	
	connect(m_NewWatFalls, &WaterfallCalculator::Calculated, this, &LoanAssumptionsEditor::NewTranchesCalculated);
	connect(CalcProgress2, &QProgressDialog::canceled, m_NewWatFalls, &WaterfallCalculator::StopCalculation);
	connect(m_NewWatFalls, &WaterfallCalculator::ProgressPct, CalcProgress2, &QProgressDialog::setValue);
	connect(m_NewWatFalls, &WaterfallCalculator::BeeError, m_NewWatFalls, &WaterfallCalculator::StopCalculation);
	connect(m_NewWatFalls, &WaterfallCalculator::BeeError, [this]() {QMessageBox::critical(this, tr("Error"), tr("An error occurred during the calculations of liabilities cash flows")); });
	connect(m_NewWatFalls, &WaterfallCalculator::Calculated, m_NewWatFalls, &WaterfallCalculator::deleteLater);
	connect(m_NewWatFalls, &WaterfallCalculator::Calculated, CalcProgress2, &QProgressDialog::deleteLater);
	connect(m_NewWatFalls, &WaterfallCalculator::Stopped, m_NewWatFalls, &WaterfallCalculator::deleteLater);
	connect(m_NewWatFalls, &WaterfallCalculator::Stopped, CalcProgress2, &QProgressDialog::deleteLater);
	
	m_NewLoans->StartCalculation();
}
void LoanAssumptionsEditor::NewTranchesCalculated() {
	m_NewWtfToExtension = *(m_NewWatFalls->GetResult(0));
	if (m_NewWatFalls->GetResult(1))
		m_NewWtfToCall = *(m_NewWatFalls->GetResult(1));
	//////////////////////////////////////////////////////////////////////////
	m_NewStructureModel->setRowCount(m_NewWtfToExtension.GetTranchesCount());
	SafeSetModel(m_NewStrGenTable, m_NewStructureModel);
	connect(m_NewStrGenTable->selectionModel(), &QItemSelectionModel::currentChanged, this, &LoanAssumptionsEditor::NewGenTableSelect, Qt::QueuedConnection);
	for (int i = 0; i < m_NewWtfToExtension.GetTranchesCount(); ++i) {
		m_NewStructureModel->setData(m_NewStructureModel->index(i, TrancheNameCol), m_NewWtfToExtension.GetTranche(i)->GetTrancheName(), Qt::EditRole);
		m_NewStructureModel->setData(m_NewStructureModel->index(i, PriceCol), Commarize(m_NewWtfToExtension.GetTranche(i)->GetPrice(), 2), Qt::EditRole);
		m_NewStructureModel->setData(m_NewStructureModel->index(i, PriceCol), m_NewWtfToExtension.GetTranche(i)->GetPrice(), Qt::UserRole);
		m_NewStructureModel->setData(m_NewStructureModel->index(i, WALCol), Commarize(m_NewWtfToExtension.GetTranche(i)->GetWALife(m_NewWtfToExtension.GetCalculatedMtgPayments().GetDate(0)), 2), Qt::EditRole);
		m_NewStructureModel->setData(m_NewStructureModel->index(i, LossCol), Commarize(m_NewWtfToExtension.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
		if (m_NewWtfToCall.GetTranche(i)) {
			m_NewStructureModel->setData(m_NewStructureModel->index(i, CallDateCol), m_NewWtfToCall.GetCalledPeriod().toString("MMM-yy"), Qt::EditRole);
			m_NewStructureModel->setData(m_NewStructureModel->index(i, CallDateCol), m_NewWtfToCall.GetCalledPeriod(), Qt::UserRole);
			m_NewStructureModel->setData(m_NewStructureModel->index(i, WALCallCol), Commarize(m_NewWtfToCall.GetTranche(i)->GetWALife(m_NewWtfToCall.GetCalculatedMtgPayments().GetDate(0)), 2), Qt::EditRole);
			m_NewStructureModel->setData(m_NewStructureModel->index(i, LossCallCol), Commarize(m_NewWtfToCall.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
		}
		else {
			m_NewStructureModel->setData(m_NewStructureModel->index(i, CallDateCol), tr("N/A"), Qt::EditRole);
			m_NewStructureModel->setData(m_NewStructureModel->index(i, CallDateCol), QVariant(), Qt::UserRole);
			m_NewStructureModel->setData(m_NewStructureModel->index(i, WALCallCol), tr("N/A"), Qt::EditRole);
			m_NewStructureModel->setData(m_NewStructureModel->index(i, LossCallCol), tr("N/A"), Qt::EditRole);
		}
		QStandardItemModel* CurrentModel = new QStandardItemModel(this);
		const TrancheCashFlow& CurrentCF = m_NewWtfToExtension.GetTranche(i)->GetCashFlow();
		auto CFTypes = CurrentCF.AvailableFlows();
		std::sort(CFTypes.begin(), CFTypes.end());
		CurrentModel->setColumnCount(CFTypes.size() + 1);
		CurrentModel->setRowCount(CurrentCF.Count());
		CurrentModel->setHeaderData(0, Qt::Horizontal, tr("Date"));
		for (int j = 0; j < CurrentCF.Count(); ++j) {
			CurrentModel->setData(CurrentModel->index(j, 0), CurrentCF.GetDate(j).toString("MMM-yy"), Qt::EditRole);
			CurrentModel->setData(CurrentModel->index(j, 0), CurrentCF.GetDate(j), Qt::UserRole);
		}
		for (int j = 0; j < CFTypes.size(); ++j) {
			CurrentModel->setHeaderData(j + 1, Qt::Horizontal, CurrentCF.GetLabel(CFTypes.at(j), tr("Flow %1").arg(j)));
			for (int k = 0; k < CurrentCF.Count(); k++) {
				switch (CFTypes.at(j)) {
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow) :
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow) :
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICTarget) :
				case static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCTarget) : {
					double TestVal = CurrentCF.GetFlow(k, CFTypes.at(j));
					if (TestVal>static_cast<double>(TestInfinityThreshold))CurrentModel->setData(CurrentModel->index(k, j + 1), "Infinity", Qt::EditRole);
					else CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(TestVal*100.0, 2) + '%', Qt::EditRole);
				}break;
				default:
					CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(CurrentCF.GetFlow(k, CFTypes.at(j))), Qt::EditRole);
					break;
				}

				CurrentModel->setData(CurrentModel->index(k, j + 1), CurrentCF.GetFlow(k, CFTypes.at(j)), Qt::UserRole);
			}
		}
		m_NewTranchesModels.append(QSharedPointer<QStandardItemModel>(CurrentModel, [](QStandardItemModel* a) {a->deleteLater(); }));
	}
	//////////////////////////////////////////////////////////////////////////
	if (m_OldStrGenTable->selectionModel()->currentIndex().isValid()) {
		m_NewStrGenTable->selectionModel()->setCurrentIndex(m_NewStrGenTable->model()->index(m_OldStrGenTable->selectionModel()->currentIndex().row(), 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}
void LoanAssumptionsEditor::SafeSetModel(QAbstractItemView* View, QAbstractItemModel* NewModel) {
	QItemSelectionModel* ModToDelete = View->selectionModel();
	View->setModel(NewModel);
	if (ModToDelete) {
		if (View->selectionModel() != ModToDelete)
			ModToDelete->deleteLater();
	}
}

void LoanAssumptionsEditor::NewGenTableSelect(const QModelIndex& index, const QModelIndex&) {
	if (index.isValid()) {
		m_NewSelectedTrancheLabel->setText(m_NewStructureModel->data(m_NewStructureModel->index(index.row(), TrancheNameCol), Qt::EditRole).toString());
		SafeSetModel(m_NewStrDetTable, m_NewTranchesModels.at(index.row()).data());
		if (m_OldStrGenTable->model())
			m_OldStrGenTable->selectionModel()->setCurrentIndex(m_OriginalStructureModel->index(index.row(), 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
	else {
		m_NewSelectedTrancheLabel->setText(QString());
		SafeSetModel(m_NewStrDetTable, nullptr);
		m_OldStrGenTable->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::OldGenTableSelect(const QModelIndex& index, const QModelIndex&) {
	if (index.isValid()) {
		m_OldSelectedTrancheLabel->setText(m_OriginalStructureModel->data(m_OriginalStructureModel->index(index.row(), TrancheNameCol), Qt::EditRole).toString());
		SafeSetModel(m_OldStrDetTable, m_OriginalTranchesModels.at(index.row()).data());
		if (m_NewStrGenTable->model())
			m_NewStrGenTable->selectionModel()->setCurrentIndex(m_NewStructureModel->index(index.row(), 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
	else {
		m_OldSelectedTrancheLabel->setText(QString());
		SafeSetModel(m_OldStrDetTable, nullptr);
		m_NewStrGenTable->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::AdjustOldGenTableHeight() {
	int TotalSize = m_OldStrGenTable->horizontalHeader()->height() + 2 * m_OldStrGenTable->frameWidth() + m_OldStrGenTable->horizontalScrollBar()->height();
	for (int i = 0; i<m_OldStrGenTable->model()->rowCount(); ++i)
		TotalSize += m_OldStrGenTable->rowHeight(i);
	m_OldStrGenTable->setMinimumHeight(TotalSize);
	if (m_OldStrGenTable->model()->rowCount() > 0) {
		m_OldStrGenTable->selectionModel()->setCurrentIndex(m_OldStrGenTable->model()->index(0, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::AdjustNewGenTableHeight() {
	int TotalSize = m_NewStrGenTable->horizontalHeader()->height() + 2 * m_NewStrGenTable->frameWidth() + m_NewStrGenTable->horizontalScrollBar()->height();
	for (int i = 0; i<m_NewStrGenTable->model()->rowCount(); ++i)
		TotalSize += m_NewStrGenTable->rowHeight(i);
	m_NewStrGenTable->setMinimumHeight(TotalSize);
	if (m_NewStrGenTable->model()->rowCount() > 0) {
		m_NewStrGenTable->selectionModel()->setCurrentIndex(m_NewStrGenTable->model()->index(0, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::CreateModelScanner() {
	m_PoolMatcher = new LoanAssMatcher(this);

	m_ScanPoolsModel = new QStandardItemModel(this);
	m_ScanPoolsModel->setRowCount(0);
	m_ScanPoolsModel->setColumnCount(1);
	m_ScannedPoolsProxy = new QSortFilterProxyModel(this);
	m_ScannedPoolsProxy->setSourceModel(m_ScanPoolsModel);
	m_ScannedPoolsProxy->setFilterRole(Qt::UserRole);
	m_ScannedPoolsProxy->setFilterKeyColumn(0);
	m_ScannedModel = new QStandardItemModel(this);
	m_ScannedModel->setColumnCount(4);
	m_ScannedModel->setRowCount(0);
	m_ScannedModel->setHorizontalHeaderLabels(QStringList() << tr("Issuer") << tr("Facility") << tr("Current Scenario") << tr("Detected Scenario"));
	m_ScannedModelProxy = new ReadOnlyColProxy(this);
	m_ScannedModelProxy->setSourceModel(m_ScannedModel);
	m_ScannedModelProxy->SetReadOnlyCol(QList<qint32>() << 0 << 1 << 2);
	m_ScannedModelProxy->setFilterKeyColumn(0);
	m_ScannedModelProxy->setFilterRole(Qt::UserRole);

	m_PoolScanFilterView = new QListView(this);
	SafeSetModel(m_PoolScanFilterView, m_SortScenarios);
	m_PoolScanFilterView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_PoolScanFilterView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_PoolScanFilterView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	m_PoolScanDealsView = new QListView(this);
	m_PoolScanDealsView->setModel(m_ScannedPoolsProxy);
	m_PoolScanDealsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_PoolScanDealsView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_PoolScanDealsView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_PoolScanPoolView = new QTableView(this);
	m_PoolScanPoolView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_PoolScanPoolView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_PoolScanPoolView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_PoolScanPoolView->horizontalHeader()->setMinimumSectionSize(70);
	m_PoolScanPoolView->horizontalHeader()->setStretchLastSection(true);
	m_PoolScanPoolView->verticalHeader()->hide();
	SafeSetModel(m_PoolScanPoolView, m_ScannedModelProxy);


	QLabel* PoolScanFilterLaber = new QLabel(this);
	PoolScanFilterLaber->setText(tr("Filter by Scenario"));
	QLabel* ScannedPoolLaber = new QLabel(this);
	ScannedPoolLaber->setText(tr("Loan Pools"));

	QGroupBox* LeftGroup = new QGroupBox(this);
	LeftGroup->setTitle(tr("Models"));
	QVBoxLayout* LeftLay = new QVBoxLayout(LeftGroup);
	LeftLay->addWidget(PoolScanFilterLaber);
	LeftLay->addWidget(m_PoolScanFilterView);
	LeftLay->addWidget(ScannedPoolLaber);
	LeftLay->addWidget(m_PoolScanDealsView);

	m_ScanPoolsButton = new QPushButton(this);
	m_ScanPoolsButton->setText(tr("Start Scan"));
	m_ModelsDirEdit = new QLineEdit(this);
	m_ModelsDirEdit->setText(GetFromConfig("Folders", "UnifiedResultsFolder"));
	QLabel* ModelFolderlabel = new QLabel(this);
	ModelFolderlabel->setText(tr("Models Folder"));
	QPushButton *ModelsDirBrowseButton = new QPushButton(this);
	ModelsDirBrowseButton->setText("Browse"); //Ellipsis (...)
	ModelsDirBrowseButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	m_ScanProgress = new QProgressBar(this);
	m_ScanProgress->setTextVisible(true);
	m_ScanProgress->setRange(0, 100);
	m_ScanProgress->hide();
	m_ScanProgress->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


	QHBoxLayout* BottomLay = new QHBoxLayout;
	BottomLay->addWidget(ModelFolderlabel);
	BottomLay->addWidget(m_ModelsDirEdit);
	BottomLay->addWidget(ModelsDirBrowseButton);
	BottomLay->addWidget(m_ScanPoolsButton);

	QWidget* TempTab = new QWidget(this);
	QGridLayout* mainLay = new QGridLayout(TempTab);
	mainLay->addWidget(LeftGroup, 0, 0);
	mainLay->addWidget(m_PoolScanPoolView, 0, 1);
	mainLay->addWidget(m_ScanProgress, 1, 0);
	mainLay->addLayout(BottomLay, 1, 1);
	BaseTab->addTab(TempTab, tr("Scan Models"));

	connect(m_PoolScanDealsView->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (!index.isValid()) {
			m_ScannedModel->setRowCount(0);
			return;
		}
		QModelIndex CurrentIndex = m_ScannedPoolsProxy->mapToSource(index);
		auto CurrAss=m_PoolMatcher->GetResult(m_ScanPoolsModel->data(CurrentIndex, Qt::UserRole + 2).toInt());
		m_ScannedModel->setRowCount(CurrAss->ScenarioCount());
		for (int i = 0; i < CurrAss->ScenarioCount(); ++i) {
			m_ScannedModel->setData(m_ScannedModel->index(i, 0), CurrAss->GetIssuer(i), Qt::EditRole);
			m_ScannedModel->setData(m_ScannedModel->index(i, 0), "#,#"+CurrAss->GetCurrScen(i) + "#,#" + CurrAss->GetDetectScen(i) + "#,#", Qt::UserRole);
			m_ScannedModel->setData(m_ScannedModel->index(i, 1), CurrAss->GetFacility(i), Qt::EditRole);
			m_ScannedModel->setData(m_ScannedModel->index(i, 2), CurrAss->GetCurrScen(i), Qt::EditRole);
			m_ScannedModel->setData(m_ScannedModel->index(i, 3), CurrAss->GetDetectScen(i), Qt::EditRole);
		}
	});
	connect(m_PoolScanPoolView->horizontalHeader(), &QHeaderView::sectionClicked, [&](int a) {
		if (m_LastScannedColSorted == a + 1)
			m_LastScannedColSorted = -(a + 1);
		else
			m_LastScannedColSorted = a + 1;
		SortScanned();
	});
	connect(m_ScannedModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.column() == (qAbs(m_LastScannedColSorted) - 1)) SortScanned();
	});
	connect(m_PoolMatcher, &LoanAssMatcher::ProgressPct, m_ScanProgress, &QProgressBar::setValue);
	connect(m_PoolScanFilterView->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection & , const QItemSelection &) {
		auto SelectedItems = m_PoolScanFilterView->selectionModel()->selectedIndexes();
		if (SelectedItems.isEmpty()) {
			m_ScannedPoolsProxy->setFilterRegExp(QString());
			m_ScannedModelProxy->setFilterRegExp(QString());
		}
		else {
			QString FilterRegExp;
			for (auto i = SelectedItems.constBegin(); i != SelectedItems.constEnd(); ++i) {
				FilterRegExp += "(?:#,#" + i->model()->data(*i, Qt::EditRole).toString() + "#,#)|";
			}
			FilterRegExp.chop(1);
			m_ScannedPoolsProxy->setFilterRegExp(QRegExp(FilterRegExp, Qt::CaseInsensitive));
			m_ScannedModelProxy->setFilterRegExp(QRegExp(FilterRegExp, Qt::CaseInsensitive));
		}
	});
	connect(ModelsDirBrowseButton, &QPushButton::clicked, [&]() {
		if (!m_ModelsDirEdit->isEnabled()) return;
		QString NewFolder = QFileDialog::getExistingDirectory(this, tr("Select Models Directory"), m_ModelsDirEdit->text());
		if (NewFolder.isEmpty()) return;
		m_ModelsDirEdit->setText(NewFolder);
	});
	connect(m_ScanPoolsButton, &QPushButton::clicked, [&]() {
		if (m_ScanningPools) {
			m_ScanPoolsButton->setText(tr("Start Scan"));
			m_PoolMatcher->StopCalculation();
			m_ScanningPools = false;
			m_ScanProgress->hide();
		}
		else{
			m_ScanPoolsModel->setRowCount(0);
			m_ScanningPools = true;
			m_PoolMatcher->Reset();
			m_PoolMatcher->SetFolderToScan(m_ModelsDirEdit->text());
			for (auto i = m_Assumptions.constBegin(); i != m_Assumptions.constEnd(); ++i) {
				m_PoolMatcher->AddAssumption(i.key(), *(m_DirtyAssumptions.value(i.key(), i.value())));
			}
			m_ScanProgress->show();
			m_ScanPoolsButton->setText(tr("Stop"));
			m_PoolMatcher->StartCalculation();
		}
	});
	connect(m_PoolMatcher, &LoanAssMatcher::Calculated, [&]() {
		m_ScanningPools = false;
		m_ScanPoolsButton->setText(tr("Start Scan"));
		m_ScanProgress->hide();
	});
	connect(m_PoolMatcher, &LoanAssMatcher::BeeCalculated, [&](int index) {
		auto CurrentResult = m_PoolMatcher->GetResult(index-1);
		if (!CurrentResult) return;
		if (CurrentResult->isValid()) {
			m_ScanPoolsModel->insertRow(m_ScanPoolsModel->rowCount());
			m_ScanPoolsModel->setData(m_ScanPoolsModel->index(m_ScanPoolsModel->rowCount() - 1, 0), CurrentResult->GetDealName(), Qt::EditRole);
			QString FilterString("#,#");
			for (int i = 0; i < CurrentResult->ScenarioCount(); ++i) {
				if (!FilterString.contains("#,#"+CurrentResult->GetDetectScen(i) + "#,#"))
					FilterString += CurrentResult->GetDetectScen(i) + "#,#";
			}
			m_ScanPoolsModel->setData(m_ScanPoolsModel->index(m_ScanPoolsModel->rowCount() - 1, 0), FilterString, Qt::UserRole);
			m_ScanPoolsModel->setData(m_ScanPoolsModel->index(m_ScanPoolsModel->rowCount() - 1, 0), CurrentResult->GetFilePath(), Qt::UserRole + 1);
			m_ScanPoolsModel->setData(m_ScanPoolsModel->index(m_ScanPoolsModel->rowCount() - 1, 0), index - 1, Qt::UserRole + 2);
			m_ScannedPoolsProxy->setFilterKeyColumn(0);
		}
	});
}









