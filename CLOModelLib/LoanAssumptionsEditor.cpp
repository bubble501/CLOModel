#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#endif
#include <QVariant>
#include "CommonFunctions.h"
#include "LoanAssMatcher.h"
#include "LoanAssumptionsEditor.h"
#include "Mortgage.h"
#include "Private/AssumptionsComboDelegate.h"
#include "Private/CheckAndEditDelegate.h"
#include "Private/InternalItems.h"
#include "Private/LoanAssumptionDelegate.h"
#include "Private/LoanAssumptionsEditor_p.h"
#include "Private/ReadOnlyColProxy.h"
#include "Private/RichTextDelegate.h"
#include "WaterfallCalculator.h"
#include "TrancheCashFlow.h"
#include "Tranche.h"
#include "BaseRateTable.h"
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QCompleter>
#include <QDateTimeEdit>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QScopedPointer>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QSignalBlocker>
LoanAssumptionsEditorPrivate::LoanAssumptionsEditorPrivate(LoanAssumptionsEditor *q)
	:q_ptr(q)
    , ActiveAssumption(nullptr)
    , m_LastPoolColSorted(1)
    , m_LastScannedColSorted(1)
    , m_EnableLoad(true)
    , m_ScanningPools(false)
{}

LoanAssumptionsEditor::LoanAssumptionsEditor(LoanAssumptionsEditorPrivate* d, QWidget *parent)
    :QWidget(parent)
    ,d_ptr(d)
{
    setWindowIcon(QIcon(":/Icons/Logo.png"));
    setWindowTitle(tr("Loan Scenarios Editor"));
    d->BaseTab = new QTabWidget(this);
    d->BaseTab->setTabPosition(QTabWidget::West);
    d->CreateScenarioEditor();
    d->CreatePoolMatcher();
    d->CreateStructureComparison();
    d->CreateModelScanner();

    d->m_ModelNameLabel = new QLabel(this);
    d->m_ModelNameLabel->setText(tr("No Model Loaded"));
    d->LoadPoolButton = new QPushButton(this);
    d->LoadPoolButton->setDefault(true);
    d->LoadPoolButton->setEnabled(d->m_EnableLoad);
    d->LoadPoolButton->setText(tr("Load Model"));
    d->SavePoolButton = new QPushButton(this);
    d->SavePoolButton->setEnabled(false);
    d->SavePoolButton->setText(tr("Save Loan Pool"));
    connect(d->LoadPoolButton, &QPushButton::clicked, this, &LoanAssumptionsEditor::LoadModel);
    connect(d->SavePoolButton, &QPushButton::clicked, this, &LoanAssumptionsEditor::SavePool);

    QGridLayout* MainLay = new QGridLayout(this);
    MainLay->addWidget(d->BaseTab, 0, 0, 1, 4);
    MainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 1, 0);
    MainLay->addWidget(d->m_ModelNameLabel, 1, 1);
    MainLay->addWidget(d->LoadPoolButton, 1, 2);
    MainLay->addWidget(d->SavePoolButton, 1, 3);
}

LoanAssumptionsEditor::LoanAssumptionsEditor(QWidget *parent)
    : LoanAssumptionsEditor(new LoanAssumptionsEditorPrivate(this),parent)
{}

LoanAssumptionsEditor::~LoanAssumptionsEditor()
{
    delete d_ptr;
}

void LoanAssumptionsEditorPrivate::CreateScenarioEditor()
{
    Q_Q(LoanAssumptionsEditor);
	m_ScenariosModel = new QStandardItemModel(q);
	m_ScenariosModel->setRowCount(0);
	m_ScenariosModel->setColumnCount(1);
	m_ScenariosModel->setHeaderData(0, Qt::Horizontal, "Scenario");

	m_SortScenarios = new QSortFilterProxyModel(q);
	m_SortScenarios->setSourceModel(m_ScenariosModel);

	m_AliasesModel = new QStandardItemModel(q);
	m_AliasesModel->setColumnCount(1);
	m_AliasesModel->setRowCount(0);
	m_AliasesModel->setHeaderData(0, Qt::Horizontal, "Alias");

	m_AliasesList = new QListView(q);
	m_AliasesList->setModel(m_AliasesModel);
	m_AliasesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_AliasesList->setSelectionMode(QAbstractItemView::SingleSelection);

	QLabel* SelectScenarioLabel = new QLabel(q);
	SelectScenarioLabel->setText(q->tr("Select Scenario"));
	QCompleter* ScenariosCompleter = new QCompleter(m_SortScenarios, q);
	ScenariosCompleter->setCompletionColumn(0);
	ScenariosCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	ScenariosCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
	m_ScenariosCombo = new QComboBox(q);
	m_ScenariosCombo->setModel(m_SortScenarios);
	m_ScenariosCombo->setEditable(true);
	m_ScenariosCombo->setModelColumn(0);
	m_ScenariosCombo->setCompleter(ScenariosCompleter);
	m_ScenariosCombo->setInsertPolicy(QComboBox::NoInsert);
	AddScenarioButton = new QPushButton(q);
    AddScenarioButton->setText(q->tr("Add"));
	RemoveScenarioButton = new QPushButton(q);
    RemoveScenarioButton->setText(q->tr("Remove"));
	RemoveScenarioButton->setEnabled(false);

	m_ScenarioList = new QListView(q);
	SafeSetModel(m_ScenarioList, m_SortScenarios);
	m_ScenarioList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_ScenarioList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ScenarioList->setItemDelegate(new RichTextDelegate(m_ScenarioList));


	QLabel* AliasLabel = new QLabel(q);
    AliasLabel->setText(q->tr("Aliases"));
	m_AliasLineEdit = new QLineEdit(q);
	m_AliasLineEdit->setMaxLength(50);
	AddAliasButton = new QPushButton(q);
	AddAliasButton->setText("+");
	AddAliasButton->setEnabled(false);
	RemoveAliasButton = new QPushButton(q);
	RemoveAliasButton->setText("-");
	RemoveAliasButton->setEnabled(false);
	ReplaceAliasButton = new QPushButton(q);
	ReplaceAliasButton->setText("\342\207\205"); //Unicode for up and down arrow
	ReplaceAliasButton->setEnabled(false);



	QGroupBox* NamesGroup = new QGroupBox(q);
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
    NamesGroup->setTitle(q->tr("Identifiers"));
	QLabel* ScenarioNameLabel = new QLabel(q);
    ScenarioNameLabel->setText(q->tr("Scenario Name"));
	m_ScenarioNameEdit = new QLineEdit(q);
	QGridLayout* NamesLay = new QGridLayout(NamesGroup);
	NamesLay->addWidget(ScenarioNameLabel, 0, 0);
	NamesLay->addWidget(m_ScenarioNameEdit, 0, 1, 1, 4);
	NamesLay->addWidget(AliasLabel, 1, 0);
	NamesLay->addWidget(m_AliasLineEdit, 1, 1);
	NamesLay->addWidget(AddAliasButton, 1, 2);
	NamesLay->addWidget(RemoveAliasButton, 1, 3);
	NamesLay->addWidget(ReplaceAliasButton, 1, 4);
	NamesLay->addWidget(m_AliasesList, 2, 1, 1, 4);

	m_SeniorAsumptionsModel = new QStandardItemModel(q);
	m_SeniorAsumptionsModel->setColumnCount(2);
	m_SeniorAsumptionsModel->setRowCount(1);
    m_SeniorAsumptionsModel->setHorizontalHeaderLabels(QStringList() << q->tr("Assumption Type") << q->tr("Assumption value"));
	m_MezzAsumptionsModel = new QStandardItemModel(q);
	m_MezzAsumptionsModel->setColumnCount(2);
	m_MezzAsumptionsModel->setRowCount(1);
    m_MezzAsumptionsModel->setHorizontalHeaderLabels(QStringList() << q->tr("Assumption Type") << q->tr("Assumption value"));

	m_SeniorTable = new QTableView(q);
	SafeSetModel(m_SeniorTable, m_SeniorAsumptionsModel);
	m_SeniorTable->setItemDelegateForColumn(0, new AssumptionsComboDelegate(q));
	m_SeniorTable->setItemDelegateForColumn(1, new LoanAssumptionDelegate(q));
	m_SeniorTable->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_SeniorTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_SeniorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_SeniorTable->horizontalHeader()->setMinimumSectionSize(130);
	m_SeniorTable->horizontalHeader()->setStretchLastSection(true);
	m_SeniorTable->verticalHeader()->hide();
	QGroupBox* SeniorGroup = new QGroupBox(q);
    SeniorGroup->setTitle(q->tr("Assumption for Senior Obligations"));
	QGridLayout* SeniorLay = new QGridLayout(SeniorGroup);
	m_seniorDateCheck = new QCheckBox(q);
    m_seniorDateCheck->setText(q->tr("Scenario Reference Date"));
	m_seniorDateCheck->setLayoutDirection(Qt::RightToLeft);
	m_SeniorDateEdit = new QDateEdit(q);
	m_SeniorDateEdit->setCalendarPopup(true);
	m_SeniorDateEdit->setDate(QDate::currentDate());
	m_SeniorDateEdit->setEnabled(false);
	AddSeniorAssumptionButton = new QPushButton(q);
    AddSeniorAssumptionButton->setText(q->tr("Add"));
	AddSeniorAssumptionButton->setEnabled(false);
	RemoveSeniorAssumptionButton = new QPushButton(q);
    RemoveSeniorAssumptionButton->setText(q->tr("Remove"));
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

	m_MezzTable = new QTableView(q);
	SafeSetModel(m_MezzTable, m_MezzAsumptionsModel);
	m_MezzTable->setItemDelegateForColumn(0, new AssumptionsComboDelegate(q));
	m_MezzTable->setItemDelegateForColumn(1, new LoanAssumptionDelegate(q));
	m_MezzTable->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_MezzTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_MezzTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_MezzTable->horizontalHeader()->setMinimumSectionSize(130);
	m_MezzTable->horizontalHeader()->setStretchLastSection(true);
	m_MezzTable->verticalHeader()->hide();
	QGroupBox* MezzGroup = new QGroupBox(q);
    MezzGroup->setTitle(q->tr("Assumption for Mezzanine Obligations"));
	QGridLayout* MezzLay = new QGridLayout(MezzGroup);
	m_MezzDateCheck = new QCheckBox(q);
    m_MezzDateCheck->setText(q->tr("Scenario Reference Date"));
	m_MezzDateCheck->setLayoutDirection(Qt::RightToLeft);
	m_MezzDateEdit = new QDateEdit(q);
	m_MezzDateEdit->setCalendarPopup(true);
	m_MezzDateEdit->setDate(QDate::currentDate());
	m_MezzDateEdit->setEnabled(false);
	AddMezzAssumptionButton = new QPushButton(q);
    AddMezzAssumptionButton->setText(q->tr("Add"));
	AddMezzAssumptionButton->setEnabled(false);
	RemoveMezzAssumptionButton = new QPushButton(q);
    RemoveMezzAssumptionButton->setText(q->tr("Remove"));
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

	m_ScenarioEditorRightSection = new QWidget(q);
	QVBoxLayout* RightLay = new QVBoxLayout(m_ScenarioEditorRightSection);
	RightLay->addWidget(NamesGroup);
	RightLay->addWidget(SeniorGroup);
	RightLay->addWidget(MezzGroup);
	m_ScenarioEditorRightSection->setMinimumWidth(400);
    m_ScenarioEditorRightSection->setEnabled(false);


	QWidget* LeftSection = new QWidget(q);
	QGridLayout* LeftLay = new QGridLayout(LeftSection);
	LeftLay->addWidget(SelectScenarioLabel, 0, 0, 1, 2);
	LeftLay->addWidget(m_ScenariosCombo, 1, 0, 1, 2);
	LeftLay->addWidget(m_ScenarioList, 2, 0, 1, 2);
	LeftLay->addWidget(AddScenarioButton, 3, 0);
	LeftLay->addWidget(RemoveScenarioButton, 3, 1);
	LeftSection->setMinimumWidth(150);

	QSplitter* MainSplit = new QSplitter(q);
	MainSplit->setOrientation(Qt::Horizontal);
	MainSplit->addWidget(LeftSection);
	MainSplit->addWidget(m_ScenarioEditorRightSection);
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
	QFrame* BottomLine = new QFrame(q);
	BottomLine->setFrameStyle(QFrame::HLine);
	BottomLine->setLineWidth(1);
	BottomLine->setMidLineWidth(0);
	BottomLine->setFrameShadow(QFrame::Sunken);
	DiscardAllButton = new QPushButton(q);
    DiscardAllButton->setText(q->tr("Discard All"));
	DiscardAllButton->setEnabled(false);
	DiscardCurrentButton = new QPushButton(q);
    DiscardCurrentButton->setText(q->tr("Discard"));
	DiscardCurrentButton->setEnabled(false);
	SaveAllButton = new QPushButton(q);
    SaveAllButton->setText(q->tr("Save All"));
	SaveAllButton->setEnabled(false);
	SaveCurrentButton = new QPushButton(q);
    SaveCurrentButton->setText(q->tr("Save"));
	SaveCurrentButton->setEnabled(false);

	
	QWidget* TempTab = new QWidget(q);;
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
    BaseTab->addTab(TempTab, q->tr("Scenarios Editor"));

    q->connect(m_ScenariosModel, &QStandardItemModel::rowsInserted, [&]() {m_ScenarioEditorRightSection->setEnabled(m_ScenariosModel->rowCount() > 0); });
    q->connect(m_ScenariosModel, &QStandardItemModel::rowsRemoved, [&]() {m_ScenarioEditorRightSection->setEnabled(m_ScenariosModel->rowCount() > 0); });
    q->connect(m_ScenariosModel, &QStandardItemModel::modelReset, [&]() {m_ScenarioEditorRightSection->setEnabled(m_ScenariosModel->rowCount() > 0); });
    q->connect(m_ScenariosModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		m_SortScenarios->sort(0);
	});
    q->connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, q, &LoanAssumptionsEditor::ChangeScenario);
    q->connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
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
    q->connect(m_AliasLineEdit, &QLineEdit::textChanged, [&](const QString& a) {AddAliasButton->setEnabled(!a.isEmpty()); });
    q->connect(m_SeniorTable->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		RemoveSeniorAssumptionButton->setEnabled(index.isValid());
	});
    q->connect(m_MezzTable->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		RemoveMezzAssumptionButton->setEnabled(index.isValid());
	});
    q->connect(m_ScenariosCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int row) {
		m_ScenarioList->setCurrentIndex(m_SortScenarios->index(row, 0));
	});
    q->connect(m_AliasesList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.isValid())
			m_AliasLineEdit->setText(m_AliasesModel->data(index).toString());
	});
    q->connect(AddAliasButton, &QPushButton::clicked, [&](bool) {
		if (CheckAliasInput()) {
			m_AliasesModel->insertRow(m_AliasesModel->rowCount());
			m_AliasesModel->setData(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), m_AliasLineEdit->text());
			m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
		}
	});
    q->connect(RemoveAliasButton, &QPushButton::clicked, [&](bool) {
		if (m_AliasesList->selectionModel()->currentIndex().isValid())
			m_AliasesModel->removeRow(m_AliasesList->selectionModel()->currentIndex().row());
		RemoveAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
		ReplaceAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
	});
    q->connect(ReplaceAliasButton, &QPushButton::clicked, [&](bool) {
		if (m_AliasesList->selectionModel()->currentIndex().isValid()) {
			if (CheckAliasInput())
				m_AliasesModel->setData(m_AliasesModel->index(m_AliasesList->selectionModel()->currentIndex().row(), 0), m_AliasLineEdit->text());
		}
	});
    q->connect(m_seniorDateCheck, &QCheckBox::clicked, m_SeniorDateEdit, &QDateEdit::setEnabled);
    q->connect(m_SeniorAsumptionsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), q, SLOT(SeniorScenarioChanged(const QModelIndex&)));
    q->connect(m_MezzDateCheck, &QCheckBox::clicked, m_MezzDateEdit, &QDateEdit::setEnabled);
    q->connect(m_MezzAsumptionsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), q, SLOT(MezzScenarioChanged(const QModelIndex&)));
    q->connect(m_AliasesModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		ReplaceAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
		RemoveAliasButton->setEnabled(m_AliasesModel->rowCount()>0);
	});
    q->connect(m_AliasesModel, &QStandardItemModel::modelReset, [&]() {
		ReplaceAliasButton->setEnabled(m_AliasesModel->rowCount() > 0);
		RemoveAliasButton->setEnabled(m_AliasesModel->rowCount() > 0);
	});
    q->connect(m_SeniorAsumptionsModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
    q->connect(m_SeniorAsumptionsModel, &QStandardItemModel::modelReset, [&]() {
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
    q->connect(AddSeniorAssumptionButton, &QPushButton::clicked, [&](bool) {
		m_SeniorAsumptionsModel->insertRow(m_SeniorAsumptionsModel->rowCount());
		m_SeniorTable->selectionModel()->setCurrentIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	});
    q->connect(RemoveSeniorAssumptionButton, &QPushButton::clicked, [&](bool) {
		if (m_SeniorTable->selectionModel()->currentIndex().isValid()) {
			m_SeniorAsumptionsModel->removeRow(m_SeniorTable->selectionModel()->currentIndex().row());
		}
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
    q->connect(m_MezzAsumptionsModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		RemoveMezzAssumptionButton->setEnabled(m_MezzAsumptionsModel->rowCount() > 0);
	});
    q->connect(m_MezzAsumptionsModel, &QStandardItemModel::modelReset, [&]() {
		RemoveMezzAssumptionButton->setEnabled(m_MezzAsumptionsModel->rowCount() > 0);
	});
    q->connect(AddMezzAssumptionButton, &QPushButton::clicked, [&](bool) {
		m_MezzAsumptionsModel->insertRow(m_MezzAsumptionsModel->rowCount());
		m_MezzTable->selectionModel()->setCurrentIndex(m_MezzAsumptionsModel->index(m_MezzAsumptionsModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	});
    q->connect(RemoveMezzAssumptionButton, &QPushButton::clicked, [&](bool) {
		if (m_MezzTable->selectionModel()->currentIndex().isValid()) {
			m_MezzAsumptionsModel->removeRow(m_MezzTable->selectionModel()->currentIndex().row());
		}
		RemoveMezzAssumptionButton->setEnabled(m_MezzAsumptionsModel->rowCount() > 0);
	});
    q->connect(AddScenarioButton, &QPushButton::clicked, [&]() {
        QString NewScenName = "Scenario %1";
        {
            quint64 i = 1; 
            for (; ScenExist(NewScenName.arg(i)); ++i)
            {}
            NewScenName = NewScenName.arg(i);
        }
        m_Assumptions.insert(NewScenName, QSharedPointer<LoanAssumption>(nullptr));
        m_DirtyAssumptions.insert(NewScenName, QSharedPointer<LoanAssumption>(new LoanAssumption(NewScenName)));
        m_ScenariosModel->insertRow(m_ScenariosModel->rowCount());
        m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), NewScenName, Qt::EditRole);
        m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), RichTextDelegate::Added, Qt::UserRole);
        m_ScenarioList->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::ClearAndSelect);
        m_ScenarioList->selectionModel()->setCurrentIndex(m_SortScenarios->mapFromSource(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0)), QItemSelectionModel::ClearAndSelect);
    });
    q->connect(RemoveScenarioButton, SIGNAL(clicked()), q, SLOT(RemoveScenario()));
    q->connect(m_ScenariosModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex &, int, int) {
		RemoveScenarioButton->setEnabled(m_ScenariosModel->rowCount() > 0);
	});
    q->connect(m_ScenariosModel, &QStandardItemModel::modelReset, [&]() {
		RemoveScenarioButton->setEnabled(m_ScenariosModel->rowCount() > 0);
	});
    q->connect(DiscardCurrentButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
            if (YesNoDialog(DiscardCurrentButton->text(), q_func()->tr("Are you sure you want to discard changes to %1").arg(m_ScenariosModel->data(currIdx, Qt::EditRole).toString()))) {
                q_func()->DiscardScenario(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
			}
		}
	});
    q->connect(DiscardAllButton, &QPushButton::clicked, [&](bool) {
        if (YesNoDialog(DiscardAllButton->text(), q_func()->tr("Are you sure you want to discard changes to %n scenario(s)", "", m_DirtyAssumptions.size()))) {
			const auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
                q_func()->DiscardScenario(k);
			}
		}
        q_func()->CheckAllDirty();
	});
    q->connect(SaveCurrentButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
            if (YesNoDialog(SaveCurrentButton->text(), q_func()->tr("Are you sure you want to save changes to %1").arg(m_ScenariosModel->data(currIdx, Qt::EditRole).toString()))) {
                q_func()->SaveScenario(m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
			}
		}
	});
    q->connect(SaveAllButton, &QPushButton::clicked, [&](bool) {
		auto CurrentIDx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
        if (YesNoDialog(SaveAllButton->text(), q_func()->tr("Are you sure you want to save changes to %n scenario(s)", "", m_DirtyAssumptions.size()))) {
			auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
                q_func()->SaveScenario(k);
			}
		}
        q_func()->CheckAllDirty();
		m_ScenarioList->selectionModel()->setCurrentIndex(m_SortScenarios->mapFromSource(CurrentIDx),QItemSelectionModel::ClearAndSelect);
	});


    q->connect(RemoveSeniorAssumptionButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(AddSeniorAssumptionButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(RemoveMezzAssumptionButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(AddMezzAssumptionButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(AddAliasButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(RemoveAliasButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(ReplaceAliasButton, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_ScenarioNameEdit, &QLineEdit::textEdited, [&](const QString& a) {
        if (!a.isEmpty()) q_func()->CheckCurrentDirty();
	});
    q->connect(m_seniorDateCheck, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_SeniorDateEdit, SIGNAL(dateChanged(const QDate&)), q, SLOT(SeniorDateChanged(const QDate&)), Qt::QueuedConnection);
    q->connect(m_SeniorTable->itemDelegateForColumn(0), SIGNAL(Edited()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_SeniorTable->itemDelegateForColumn(1), SIGNAL(Edited()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_MezzDateCheck, SIGNAL(clicked()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_MezzDateEdit, SIGNAL(dateChanged(const QDate&)), q, SLOT(MezzDateChanged(const QDate&)), Qt::QueuedConnection);
    q->connect(m_MezzTable->itemDelegateForColumn(0), SIGNAL(Edited()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_MezzTable->itemDelegateForColumn(1), SIGNAL(Edited()), q, SLOT(CheckCurrentDirty()));
    q->connect(m_ScenariosModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
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
    q->connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		bool Result = false;
		if (index.isValid()) {
			Result = m_ScenariosModel->data(m_SortScenarios->mapToSource(index), Qt::UserRole).toInt() != RichTextDelegate::NotDirty;
		}
		SaveCurrentButton->setEnabled(Result);
		DiscardCurrentButton->setEnabled(Result);
	});
}
void LoanAssumptionsEditorPrivate::CreatePoolMatcher() {
    Q_Q(LoanAssumptionsEditor);
	m_PoolModel = new QStandardItemModel(q);
	m_PoolModel->setRowCount(0);
	m_PoolModel->setColumnCount(4);
	m_PoolModel->setHorizontalHeaderLabels(QStringList() << "Issuer" << "Facility" << "Current Scenario" << "Suggested Scenario");
	m_PoolSorter = new ReadOnlyColProxy(q);
	m_PoolSorter->SetReadOnlyCol(0);
	m_PoolSorter->SetReadOnlyCol(1);
	m_PoolSorter->setSourceModel(m_PoolModel);

	m_PoolTable = new QTableView(q);
	SafeSetModel(m_PoolTable, m_PoolSorter);
	m_PoolTable->verticalHeader()->hide();
	m_PoolTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_PoolTable->horizontalHeader()->setMinimumSectionSize(130);
	m_PoolTable->horizontalHeader()->setStretchLastSection(true);
	m_PoolTable->setItemDelegateForColumn(2, new CheckAndEditDelegate(q));
	m_PoolTable->setItemDelegateForColumn(3, new CheckAndEditDelegate(q));
	m_PoolTable->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	m_PoolTable->setSelectionMode(QAbstractItemView::SingleSelection);

	
	GuessAssumptionsButton = new QPushButton(q);
	GuessAssumptionsButton->setText(q->tr("Guess Scenarios"));
	GuessAssumptionsButton->setEnabled(false);
	AcceptAllOldButton = new QPushButton(q);
    AcceptAllOldButton->setText(q->tr("Select all Current"));
	AcceptAllOldButton->setEnabled(false);
	AcceptAllNewButton = new QPushButton(q);
    AcceptAllNewButton->setText(q->tr("Select all Suggested"));
	AcceptAllNewButton->setEnabled(false);
	AcceptAllNonEmptyButton = new QPushButton(q);
    AcceptAllNonEmptyButton->setText(q->tr("Select all non-empty Suggested"));
	AcceptAllNonEmptyButton->setEnabled(false);
	

	
    q->connect(GuessAssumptionsButton, &QPushButton::clicked, q, &LoanAssumptionsEditor::GuessAssumptions);
	
    q->connect(m_PoolTable->horizontalHeader(), &QHeaderView::sectionClicked, [&](int a) {
		if (m_LastPoolColSorted == a+1) 
			m_LastPoolColSorted = -(a+1);
		else 
			m_LastPoolColSorted = a+1;
		q_func()->SortPool();
	});
    q->connect(m_PoolModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.column() == (qAbs(m_LastPoolColSorted) - 1))
            q_func()->SortPool();
	});
    q->connect(m_PoolModel, &QStandardItemModel::dataChanged, q, &LoanAssumptionsEditor::SetPoolModelChecks);
    q->connect(m_PoolModel, &QStandardItemModel::rowsInserted, [&](const QModelIndex&, int, int) {
		bool EnableItems = m_PoolModel->rowCount() > 0;
		AcceptAllOldButton->setEnabled(EnableItems);
		AcceptAllNewButton->setEnabled(EnableItems);
		AcceptAllNonEmptyButton->setEnabled(EnableItems);
		GuessAssumptionsButton->setEnabled(EnableItems);
		SavePoolButton->setEnabled(EnableItems);
		CalculateNewStructureButton->setEnabled(EnableItems);
	});
    q->connect(m_PoolModel, &QStandardItemModel::rowsRemoved, [&](const QModelIndex&, int, int) {
		bool EnableItems = m_PoolModel->rowCount() > 0;
		AcceptAllOldButton->setEnabled(EnableItems);
		AcceptAllNewButton->setEnabled(EnableItems);
		AcceptAllNonEmptyButton->setEnabled(EnableItems);
		GuessAssumptionsButton->setEnabled(EnableItems);
		SavePoolButton->setEnabled(EnableItems);
		CalculateNewStructureButton->setEnabled(EnableItems);
	});
    q->connect(AcceptAllOldButton, &QPushButton::clicked, [&]() {
        const QSignalBlocker blocker(m_PoolModel);
		for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
			m_PoolModel->setData(m_PoolModel->index(i, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
			m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
		}
	});
    q->connect(AcceptAllNewButton, &QPushButton::clicked, [&]() {
        const QSignalBlocker blocker(m_PoolModel);
		for (int i = 0; i < m_PoolModel->rowCount(); ++i) {
			m_PoolModel->setData(m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
			m_PoolModel->setData(m_PoolModel->index(i, 2), Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
		}
	});
    q->connect(AcceptAllNonEmptyButton, &QPushButton::clicked, [&]() {
        const QSignalBlocker blocker(m_PoolModel);
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
	});

	QWidget* TempTab = new QWidget(q);;
	QGridLayout* mainLay = new QGridLayout(TempTab);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 0, 1, 3);
	mainLay->addWidget(GuessAssumptionsButton, 0, 3);
	mainLay->addWidget(m_PoolTable, 1, 0,1,4);
	mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 2, 0);
	mainLay->addWidget(AcceptAllOldButton, 2, 1);
	mainLay->addWidget(AcceptAllNewButton, 2, 2);
	mainLay->addWidget(AcceptAllNonEmptyButton, 2, 3);
	BaseTab->addTab(TempTab, q->tr("Pool"));
}

void LoanAssumptionsEditor::FillFromQuery() {
#ifndef NO_DATABASE
    Q_D(LoanAssumptionsEditor);
	d->m_Assumptions.clear();
	Db_Mutex.lock();
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver")
			+ "}; "
			+ GetFromConfig("Database", "DataSource")
			);
		
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
	if (DbOpen) {
		QSqlQuery LoanAssQuery(db);
		LoanAssQuery.setForwardOnly(true);
		LoanAssQuery.prepare("{CALL " + GetFromConfig("Database", "GetAllLoanAssumptionStoredProc") + "}");
		if (LoanAssQuery.exec()) {
			bool CurrentSenior;
			int FieldCount;
			while (LoanAssQuery.next()) {
				FieldCount = 0;
				auto LoanAssRecord = LoanAssQuery.record();
				QString CurrentScenario = LoanAssRecord.value(FieldCount).toString(); ++FieldCount;
				QSharedPointer<LoanAssumption> CurrentAss(nullptr);
                for (auto i = d->m_Assumptions.begin(); i != d->m_Assumptions.end(); ++i) {
					if (i.key().compare(CurrentScenario, Qt::CaseInsensitive) == 0) {
						CurrentAss = i.value();
						break;
					}
				}
				if (!CurrentAss) {
					CurrentAss = QSharedPointer<LoanAssumption>(new LoanAssumption(CurrentScenario));
					if (!LoanAssRecord.isNull(FieldCount)) CurrentAss->SetAliases(LoanAssRecord.value(FieldCount).toString());
                    d->m_Assumptions.insert(CurrentAss->GetScenarioName(), CurrentAss);
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
    d->FillScenariosModel();
#endif
}

void LoanAssumptionsEditor::closeEvent(QCloseEvent * ev)
{
    Q_D(LoanAssumptionsEditor);
    if (d->m_PoolMatcher)
        d->m_PoolMatcher->StopCalculation();
    if (d->m_NewWatFalls)
        d->m_NewWatFalls->StopCalculation();
    if (d->m_NewLoans)
        d->m_NewLoans->StopCalculation();
    d->m_LoanPool.StopCalculation();
    ev->accept();
}
void LoanAssumptionsEditorPrivate::FillScenariosModel()
{
	m_ScenariosModel->setRowCount(m_Assumptions.size());
	int RawIndex = 0;
	for (auto i = m_Assumptions.constBegin(); i != m_Assumptions.constEnd(); ++i, ++RawIndex) {
		m_ScenariosModel->setData(m_ScenariosModel->index(RawIndex, 0), i.value()->GetScenarioName(), Qt::EditRole);
	}
	m_ScenarioList->setCurrentIndex(QModelIndex());
	m_ScenariosCombo->setCurrentIndex(-1);
}

void LoanAssumptionsEditor::ChangeScenario(const QModelIndex& curr, const QModelIndex& prev)
{
    Q_D(LoanAssumptionsEditor);
	if (!curr.isValid()) return;
    d->ActiveAssumption = d->m_DirtyAssumptions.value(d->m_ScenariosModel->data(d->m_SortScenarios->mapToSource(curr)).toString(),
        d->m_Assumptions.value(d->m_ScenariosModel->data(d->m_SortScenarios->mapToSource(curr)).toString(), QSharedPointer<LoanAssumption>(nullptr)));
    d->m_ScenarioNameEdit->setText(d->ActiveAssumption ? d->ActiveAssumption->GetScenarioName() : QString());
    d->m_SeniorAsumptionsModel->setRowCount(0);
    d->m_MezzAsumptionsModel->setRowCount(0);
    if (d->ActiveAssumption) {
        d->m_AliasesModel->setRowCount(d->ActiveAssumption->GetAliases().count());
		int RowIter = 0;
        for (auto i = d->ActiveAssumption->GetAliases().constBegin(); i != d->ActiveAssumption->GetAliases().constEnd(); ++i, ++RowIter) {
            d->m_AliasesModel->setData(d->m_AliasesModel->index(RowIter, 0), *i);
		}
        if (d->ActiveAssumption->GetSeniorLastUpdate().isNull()) {
            d->m_seniorDateCheck->setChecked(false);
            d->m_SeniorDateEdit->setDate(QDate::currentDate());
            d->m_SeniorDateEdit->setEnabled(false);
		}
		else {
            d->m_seniorDateCheck->setChecked(true);
            d->m_SeniorDateEdit->setEnabled(true);
            d->m_SeniorDateEdit->setDate(d->ActiveAssumption->GetSeniorLastUpdate());
		}
		
        if (d->ActiveAssumption->GetMezzLastUpdate().isNull()) {
            d->m_MezzDateCheck->setChecked(false);
            d->m_MezzDateEdit->setDate(QDate::currentDate());
            d->m_MezzDateEdit->setEnabled(false);
		}
		else {
            d->m_MezzDateCheck->setChecked(true);
            d->m_MezzDateEdit->setEnabled(true);
            d->m_MezzDateEdit->setDate(d->ActiveAssumption->GetMezzLastUpdate());
		}
		//Senior Assumptions
		int AssIndex = 0;
        if (!d->ActiveAssumption->GetRawSeniorMaturityExtension().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				,Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorMaturityExtension().toInt()
			, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorInitialHaircut().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorInitialHaircut().toDouble()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorPrepaymentFee().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorPrepaymentFee()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorDayCount().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorDayCount()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorCPR().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorCPR()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorCDR().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorCDR()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorLS().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorLS()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorRecoveryLag().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorRecoveryLag()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorDelinquency().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorDelinquency()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorDelinquencyLag().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorDelinquencyLag()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorPrice().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorPrice().toDouble()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorHaircut().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorHaircut()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorPrepayMultiplier().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorPrepayMultiplier()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawSeniorLossMultiplier().isEmpty()) {
            d->m_SeniorAsumptionsModel->insertRow(d->m_SeniorAsumptionsModel->rowCount());
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_SeniorAsumptionsModel->setData(
                d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_SeniorTable->itemDelegate(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(d->m_SeniorAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawSeniorLossMultiplier()
				, Qt::EditRole);
		}AssIndex++;
		AssIndex = 0;
        if (!d->ActiveAssumption->GetRawMezzMaturityExtension().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzMaturityExtension().toInt()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzInitialHaircut().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzInitialHaircut().toDouble()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzPrepaymentFee().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzPrepaymentFee()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzDayCount().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzDayCount()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzCPR().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzCPR()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzCDR().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzCDR()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzLS().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzLS()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzRecoveryLag().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzRecoveryLag()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzDelinquency().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzDelinquency()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzDelinquencyLag().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzDelinquencyLag()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzPrice().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzPrice().toDouble()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzHaircut().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzHaircut()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzPrepayMultiplier().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzPrepayMultiplier()
				, Qt::EditRole);
		}AssIndex++;
        if (!d->ActiveAssumption->GetRawMezzLossMultiplier().isEmpty()) {
            d->m_MezzAsumptionsModel->insertRow(d->m_MezzAsumptionsModel->rowCount());
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0), AssIndex, Qt::UserRole);
            d->m_MezzAsumptionsModel->setData(
                d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)
                , qobject_cast<AssumptionsComboDelegate*>(d->m_MezzTable->itemDelegate(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0)))->ComboElementforIndex(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 0))
				, Qt::EditRole
				);
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(d->m_MezzAsumptionsModel->rowCount() - 1, 1),
                d->ActiveAssumption->GetRawMezzLossMultiplier()
				, Qt::EditRole);
		}AssIndex++;
	}
	else {
        d->m_seniorDateCheck->setChecked(false);
        d->m_SeniorDateEdit->setEnabled(false);
        d->m_SeniorDateEdit->setDate(QDate::currentDate());
        d->m_MezzDateCheck->setChecked(false);
        d->m_MezzDateEdit->setEnabled(false);
        d->m_MezzDateEdit->setDate(QDate::currentDate());
        d->m_AliasesModel->setRowCount(0);
	}
    d->m_MezzDateCheck->setEnabled(d->ActiveAssumption);
    d->m_seniorDateCheck->setEnabled(d->ActiveAssumption);
    d->m_ScenarioNameEdit->setEnabled(d->ActiveAssumption);
    d->m_AliasLineEdit->setEnabled(d->ActiveAssumption);
    d->AddSeniorAssumptionButton->setEnabled(d->ActiveAssumption);
    d->AddMezzAssumptionButton->setEnabled(d->ActiveAssumption);
    d->RemoveSeniorAssumptionButton->setEnabled(d->ActiveAssumption && d->m_SeniorAsumptionsModel->rowCount()>0);
    d->RemoveMezzAssumptionButton->setEnabled(d->ActiveAssumption && d->m_MezzAsumptionsModel->rowCount()>0);
    d->RemoveAliasButton->setEnabled(d->ActiveAssumption && d->m_AliasesModel->rowCount()>0);
    d->ReplaceAliasButton->setEnabled(d->ActiveAssumption && d->m_AliasesModel->rowCount()>0);
    d->RemoveScenarioButton->setEnabled(d->m_ScenariosModel->rowCount() > 0 && d->ActiveAssumption);
}

void LoanAssumptionsEditor::SeniorScenarioChanged(const QModelIndex& index) {
    Q_D(LoanAssumptionsEditor);
	if (index.column() == 0) {
        d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::EditRole);
        auto AssumptionType = d->m_SeniorAsumptionsModel->data(index, Qt::UserRole).toInt();
        for (int i = 0; i < d->m_SeniorAsumptionsModel->rowCount() && AssumptionType >= 0; ++i) {
			if (i == index.row())continue;
            if (d->m_SeniorAsumptionsModel->data(d->m_SeniorAsumptionsModel->index(i, 0), Qt::UserRole) == AssumptionType) {
                d->m_SeniorAsumptionsModel->setData(index, -1, Qt::UserRole);
                d->m_SeniorAsumptionsModel->setData(index, QVariant(), Qt::EditRole);
                d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::UserRole);
				return;
			}
		}
		switch (AssumptionType) {
		case LoanAssumption::MaturityExtension:
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::IntegerAssumption, Qt::UserRole);
			break;
		case LoanAssumption::InitialHaircut:
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::DoubleAssumption0To100, Qt::UserRole);
			break;
		case LoanAssumption::DayCount:
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::DayCountVectorAssumption, Qt::UserRole);
			break;
		case LoanAssumption::PrepaymentFee:
		case LoanAssumption::CPR:
		case LoanAssumption::CDR:
		case LoanAssumption::LS:
		case LoanAssumption::Delinquency:
		case LoanAssumption::Haircut:
		case LoanAssumption::PrepayMultiplier:
		case LoanAssumption::LossMultiplier:
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::BloombergVectorAssumption, Qt::UserRole);
			break;
		case LoanAssumption::RecoveryLag:
		case LoanAssumption::DelinquencyLag:
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::IntegerVectorAssumption, Qt::UserRole);
			break;
		case LoanAssumption::Price:
            d->m_SeniorAsumptionsModel->setData(d->m_SeniorAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::DoubleAssumption, Qt::UserRole);
			break;
		}
		
	}
	
}
void LoanAssumptionsEditor::MezzScenarioChanged(const QModelIndex& index) {
    Q_D(LoanAssumptionsEditor);
	if (index.column() == 0) {
        d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), QVariant(), Qt::EditRole);
        auto AssumptionType = d->m_MezzAsumptionsModel->data(index, Qt::UserRole).toInt();
        for (int i = 0; i < d->m_MezzAsumptionsModel->rowCount() && AssumptionType >= 0; ++i) {
			if (i == index.row())continue;
            if (d->m_MezzAsumptionsModel->data(d->m_MezzAsumptionsModel->index(i, 0), Qt::UserRole) == AssumptionType) {
                d->m_MezzAsumptionsModel->setData(index, -1, Qt::UserRole);
                d->m_MezzAsumptionsModel->setData(index, QVariant(), Qt::EditRole);
                d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), QVariant(), Qt::UserRole);
				return;
			}
		}
		switch (AssumptionType) {
		case LoanAssumption::MaturityExtension:
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::IntegerAssumption, Qt::UserRole);
			break;
		case LoanAssumption::InitialHaircut:
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::DoubleAssumption0To100, Qt::UserRole);
			break;
		case LoanAssumption::DayCount:
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::DayCountVectorAssumption, Qt::UserRole);
			break;
		case LoanAssumption::PrepaymentFee:
		case LoanAssumption::CPR:
		case LoanAssumption::CDR:
		case LoanAssumption::LS:
		case LoanAssumption::Delinquency:
		case LoanAssumption::Haircut:
		case LoanAssumption::PrepayMultiplier:
		case LoanAssumption::LossMultiplier:
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::BloombergVectorAssumption, Qt::UserRole);
			break;
		case LoanAssumption::RecoveryLag:
		case LoanAssumption::DelinquencyLag:
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::IntegerVectorAssumption, Qt::UserRole);
			break;
		case LoanAssumption::Price:
            d->m_MezzAsumptionsModel->setData(d->m_MezzAsumptionsModel->index(index.row(), 1), LoanAssumptionDelegate::AssumptionType::DoubleAssumption, Qt::UserRole);
			break;
		}

	}

}
bool LoanAssumptionsEditorPrivate::CheckAliasInput() {
    Q_Q(LoanAssumptionsEditor);
	if (m_AliasLineEdit->text().isEmpty() || QRegExp(m_AliasLineEdit->text()).isValid()) {
		m_AliasLineEdit->setStyleSheet(QString());
	}
	else {
		m_AliasLineEdit->setToolTip(q->tr("Invalid Regular Expression.\nVisit www.regular-expressions.info for more informations on regular expressions"));
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
        m_AliasLineEdit->setToolTip(q->tr("The selected alias already refers to a scenario"));
			m_AliasLineEdit->setStyleSheet("background-color: #E79292;");
			return false;
	}
	return true;
}

bool LoanAssumptionsEditorPrivate::ScenExist(const QString& Teststr)const {
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

LoanAssumption LoanAssumptionsEditorPrivate::BuildCurrentAssumption() const {
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

bool LoanAssumptionsEditorPrivate::YesNoDialog(const QString& Title, const QString& Mess) {
    Q_Q(LoanAssumptionsEditor);
	QScopedPointer<QMessageBox, QScopedPointerDeleteLater> OkToChange(new QMessageBox(q));
	OkToChange->setIcon(QMessageBox::Question);
	OkToChange->setWindowTitle(Title);
	OkToChange->setText(Mess);
	OkToChange->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	OkToChange->setDefaultButton(QMessageBox::Yes);
	return (OkToChange->exec() == QMessageBox::Yes);
}

void LoanAssumptionsEditor::CheckCurrentDirty() {
    Q_D(LoanAssumptionsEditor);
	if (d->m_ScenarioList->selectionModel()->currentIndex().isValid()) {
        auto DirtyVersion = d->BuildCurrentAssumption();
		QSharedPointer<LoanAssumption> OriginalVersion(nullptr);
        auto currIdx = d->m_SortScenarios->mapToSource(d->m_ScenarioList->selectionModel()->currentIndex());
        OriginalVersion = d->m_Assumptions.value(d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString(), QSharedPointer<LoanAssumption>(nullptr));

		if (OriginalVersion) {
			if (DirtyVersion == *OriginalVersion) {
                d->m_DirtyAssumptions.remove(d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString());
                d->m_ScenariosModel->setData(currIdx, QVariant(), Qt::UserRole);
				return;
			}
		}
        if (d->m_DirtyAssumptions.contains(d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString()))
            d->m_DirtyAssumptions[d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString()]->operator=(DirtyVersion);
		else 
            d->m_DirtyAssumptions.insert(d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString(), QSharedPointer<LoanAssumption>(new LoanAssumption(DirtyVersion)));
        if (d->m_ScenariosModel->data(currIdx, Qt::UserRole) != RichTextDelegate::Added) {
            d->m_ScenariosModel->setData(currIdx, RichTextDelegate::Dirty, Qt::UserRole);
		}
	}
}
void LoanAssumptionsEditor::CheckAllDirty()
{
    Q_D(LoanAssumptionsEditor);
	auto CurrDirty = RichTextDelegate::NotDirty;
    for (auto i = d->m_Assumptions.constBegin(); i != d->m_Assumptions.constEnd(); ++i) {
		CurrDirty = RichTextDelegate::NotDirty;
        if (d->m_DirtyAssumptions.contains(i.key())) {
            auto DirtVersion = d->m_DirtyAssumptions.value(i.key());
			if (DirtVersion && i.value()) {
				if ((*DirtVersion) != *(i.value())) CurrDirty = RichTextDelegate::Dirty;
			}
			else CurrDirty = RichTextDelegate::Erased;
		}
		QModelIndex ScenIdx;
        for (int j = 0; j < d->m_ScenariosModel->rowCount() && !ScenIdx.isValid(); ++j) {
			if (i.key().compare(
                d->m_ScenariosModel->data(d->m_ScenariosModel->index(j, 0), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0
                )
                ScenIdx = d->m_ScenariosModel->index(j, 0);
		}
        if (d->m_ScenariosModel->data(ScenIdx, Qt::UserRole) != RichTextDelegate::Added)
            d->m_ScenariosModel->setData(ScenIdx, CurrDirty, Qt::UserRole);
	}

}
void LoanAssumptionsEditor::SaveScenario(const QString& key)
{
    Q_D(LoanAssumptionsEditor);
    if (!d->m_Assumptions.contains(key) || !d->m_DirtyAssumptions.contains(key)) return;
	bool KeyIsCurrent = false;
	QModelIndex CurrentIdx;
    if (d->m_ScenarioList->selectionModel()->currentIndex().isValid()) {
        CurrentIdx = d->m_SortScenarios->mapToSource(d->m_ScenarioList->selectionModel()->currentIndex());
		KeyIsCurrent = key.compare(
            d->m_ScenariosModel->data(CurrentIdx, Qt::EditRole).toString()
			, Qt::CaseInsensitive
			) == 0;
	}
#ifndef NO_DATABASE
	QMutexLocker DbLocker(&Db_Mutex);
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver")
			+ "}; "
			+ GetFromConfig("Database", "DataSource")
			);
		
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
#endif // !NO_DATABASE

    if (!d->m_DirtyAssumptions.value(key)) {
#ifndef NO_DATABASE
		if (!DbOpen) {
			QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
			return;
		}
		QSqlQuery RemoveAssQuery(db);
		RemoveAssQuery.setForwardOnly(true);
		RemoveAssQuery.prepare("{CALL " + GetFromConfig("Database", "RemoveLoanAssumptionStoredProc") + "}");
		RemoveAssQuery.bindValue(":scenarioName", key);
		if (!RemoveAssQuery.exec()) {
			QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
			return;
		}
#endif // !NO_DATABASE
        d->m_Assumptions[key] = QSharedPointer<LoanAssumption>(nullptr);
		return DiscardScenario(key);
	}
#ifndef NO_DATABASE
	if (!DbOpen) {
		QMessageBox::critical(this, "Error", "Failed to commit changes to the database, try again later.");
		return;
	}
    auto CurrAss = d->m_DirtyAssumptions.value(key);
	QSqlQuery SetSeniorAssQuery(db);
	QString TmpString;
	SetSeniorAssQuery.setForwardOnly(true);
	SetSeniorAssQuery.prepare("{CALL " + GetFromConfig("Database", "SetLoanAssumptionStoredProc") + "}");
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
	SetMezzAssQuery.prepare("{CALL " + GetFromConfig("Database", "SetLoanAssumptionStoredProc") + "}");
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
        d->m_DirtyAssumptions.remove(key);
        d->m_Assumptions.insert(CurrAss->GetScenarioName(), CurrAss);
        d->m_Assumptions.remove(key);
        for (int j = 0; j < d->m_ScenariosModel->rowCount(); ++j) {
			if (key.compare(
                d->m_ScenariosModel->data(d->m_ScenariosModel->index(j, 0), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0
                ) {
                d->m_ScenariosModel->removeRow(j);
                break;
            }
		}
        d->m_ScenariosModel->insertRow(d->m_ScenariosModel->rowCount());
        d->m_ScenariosModel->setData(d->m_ScenariosModel->index(d->m_ScenariosModel->rowCount() - 1, 0), CurrAss->GetScenarioName(), Qt::EditRole);
        d->m_ScenarioList->selectionModel()->setCurrentIndex(d->m_SortScenarios->mapFromSource(d->m_ScenariosModel->index(d->m_ScenariosModel->rowCount() - 1, 0)), QItemSelectionModel::ClearAndSelect);
        for (int j = 0; j < d->m_PoolModel->rowCount(); ++j) {
			if (key.compare(
                d->m_PoolModel->data(d->m_PoolModel->index(j, 3), Qt::EditRole).toString()
				, Qt::CaseInsensitive
			) == 0) 
            d->m_PoolModel->setData(d->m_PoolModel->index(j, 3), CurrAss->GetScenarioName(), Qt::EditRole);
			
		}
	}
	else {
        d->m_Assumptions[key] = d->m_DirtyAssumptions.value(key);
        d->m_DirtyAssumptions.remove(key);
	}
	if (KeyIsCurrent) {
        d->ActiveAssumption = d->m_Assumptions.value(key);
        ChangeScenario(d->m_SortScenarios->mapFromSource(CurrentIdx), d->m_SortScenarios->mapFromSource(CurrentIdx));
		CheckCurrentDirty();
        d->m_AliasesList->selectionModel()->setCurrentIndex(d->m_AliasesModel->index(d->m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	}
}

void LoanAssumptionsEditor::DiscardScenario(const QString& key) {
    Q_D(LoanAssumptionsEditor);
    if (!d->m_Assumptions.contains(key)) return;
	bool KeyIsCurrent=false;
	QModelIndex CurrentIdx;
    if (d->m_ScenarioList->selectionModel()->currentIndex().isValid()) {
        CurrentIdx = d->m_SortScenarios->mapToSource(d->m_ScenarioList->selectionModel()->currentIndex());
		KeyIsCurrent=key.compare(
            d->m_ScenariosModel->data(CurrentIdx, Qt::EditRole).toString()
			, Qt::CaseInsensitive
		)==0;
	}
    d->m_DirtyAssumptions.remove(key);
    if (!d->m_Assumptions.value(key)) {
		//Remove Added Scenarios
		QModelIndex ScenIdx;
        for (int j = 0; j < d->m_ScenariosModel->rowCount() && !ScenIdx.isValid(); ++j) {
			if (key.compare(
                d->m_ScenariosModel->data(d->m_ScenariosModel->index(j, 0), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0
                )ScenIdx = d->m_ScenariosModel->index(j, 0);
		}
        for (int j = 0; j < d->m_PoolModel->rowCount(); ++j) {
			if (key.compare(
                d->m_PoolModel->data(d->m_PoolModel->index(j, 3), Qt::EditRole).toString()
				, Qt::CaseInsensitive
				) == 0) {
                d->m_PoolModel->setData(d->m_PoolModel->index(j, 3), QVariant(), Qt::EditRole);
                d->m_PoolModel->setData(d->m_PoolModel->index(j, 3), QVariant(), Qt::UserRole);
			}
		}
        d->m_Assumptions.remove(key);
        d->m_ScenariosModel->removeRow(ScenIdx.row());
	}
	else if (KeyIsCurrent) {
        ChangeScenario(d->m_SortScenarios->mapFromSource(CurrentIdx), d->m_SortScenarios->mapFromSource(CurrentIdx));
		CheckCurrentDirty();
        d->m_AliasesList->selectionModel()->setCurrentIndex(d->m_AliasesModel->index(d->m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::ClearAndSelect);
	}
}

void LoanAssumptionsEditor::SeniorDateChanged(const QDate&)
{
    Q_D(LoanAssumptionsEditor);
    if (d->m_seniorDateCheck->isChecked()) 
        CheckCurrentDirty();
}
void LoanAssumptionsEditor::MezzDateChanged(const QDate&)
{
    Q_D(LoanAssumptionsEditor);
    if (d->m_MezzDateCheck->isChecked())
        CheckCurrentDirty();
}

void LoanAssumptionsEditor::AddLoanAssumption(const LoanAssumption& a)
{
    Q_D(LoanAssumptionsEditor);
    d->m_Assumptions.insert(a.GetScenarioName(), QSharedPointer<LoanAssumption>(new LoanAssumption(a)));
    d->m_ScenariosModel->insertRow(d->m_ScenariosModel->rowCount());
    d->m_ScenariosModel->setData(d->m_ScenariosModel->index(d->m_ScenariosModel->rowCount() - 1, 0), a.GetScenarioName(), Qt::EditRole);
}

void LoanAssumptionsEditor::AddLoanToPool(qint32 index, Mortgage& a)
{
    Q_D(LoanAssumptionsEditor);
    d->m_PoolModel->insertRow(d->m_PoolModel->rowCount());
    d->m_PoolModel->setData(d->m_PoolModel->index(d->m_PoolModel->rowCount() - 1, 0), a.GetProperty("Issuer"), Qt::EditRole);
    d->m_PoolModel->setData(d->m_PoolModel->index(d->m_PoolModel->rowCount() - 1, 0), index, Qt::UserRole);
    d->m_PoolModel->setData(d->m_PoolModel->index(d->m_PoolModel->rowCount() - 1, 1), a.GetProperty("Facility"), Qt::EditRole);
    d->m_PoolModel->setData(d->m_PoolModel->index(d->m_PoolModel->rowCount() - 1, 2), a.GetProperty("Scenario"), Qt::EditRole);
    d->m_PoolModel->setData(d->m_PoolModel->index(d->m_PoolModel->rowCount() - 1, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
    d->m_LoanPool.AddLoan(a, d->m_LoanPool.GetLoans().size());
}

void LoanAssumptionsEditor::LoadModel()
{
    Q_D(LoanAssumptionsEditor);
	QString ModelPath = QFileDialog::getOpenFileName(this, tr("Open Model"), GetFromConfig("Folders", "UnifiedResultsFolder"), tr("CLO Models (*clom)"));
	if (ModelPath.isNull()) return;
    d->m_ModelNameLabel->setText(tr("No Model Loaded"));
    d->m_PoolModel->setRowCount(0);
    d->m_LastModelLoaded.clear();
    d->SafeSetModel(d->m_OldStrDetTable, nullptr);
    d->m_OriginalTranchesModels.clear();
    d->m_NewTranchesModels.clear();
    d->BaseTab->setTabEnabled(2, false);
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
	QScopedPointer<QProgressDialog, QScopedPointerDeleteLater> LoadProgress(new QProgressDialog(tr("Loading Model"), QString(), 0, 4, this));
	LoadProgress->setCancelButton(nullptr); 
	LoadProgress->setWindowModality(Qt::WindowModal);
	LoadProgress->show();
    d->m_LastModelLoaded = ModelPath;
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
    d->m_WtfToExtension.SetLoadProtocolVersion(VersionChecker);
    out >> d->m_WtfToExtension;
	LoadProgress->setValue(1);
    d->m_WtfToCall.SetLoadProtocolVersion(VersionChecker);
    out >> d->m_WtfToCall;
	LoadProgress->setValue(2);
    d->m_LoanPool.SetLoadProtocolVersion(VersionChecker);
    out >> d->m_LoanPool;
	LoadProgress->setValue(3);
	file.close();

    d->m_PoolModel->setRowCount(d->m_LoanPool.GetLoans().size());
	int RowCounter = 0;
    for (auto i = d->m_LoanPool.GetLoans().constBegin(); i != d->m_LoanPool.GetLoans().constEnd(); ++i, ++RowCounter) {
        d->m_PoolModel->setData(d->m_PoolModel->index(RowCounter, 0), i.value()->GetProperty("Issuer"), Qt::EditRole);
        d->m_PoolModel->setData(d->m_PoolModel->index(RowCounter, 0), i.key(), Qt::UserRole);
        d->m_PoolModel->setData(d->m_PoolModel->index(RowCounter, 1), i.value()->GetProperty("Facility"), Qt::EditRole);
        d->m_PoolModel->setData(d->m_PoolModel->index(RowCounter, 2), i.value()->GetProperty("Scenario"), Qt::EditRole);
        d->m_PoolModel->setData(d->m_PoolModel->index(RowCounter, 2), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
	}
	LoadProgress->setValue(4);

    d->m_ModelNameLabel->setText(tr("Model Loaded: %1").arg(d->m_WtfToExtension.GetDealName()));

    d->m_OriginalStructureModel->setRowCount(d->m_WtfToExtension.GetTranchesCount());
    for (int i = 0; i < d->m_WtfToExtension.GetTranchesCount(); ++i) {
        d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::TrancheNameCol), d->m_WtfToExtension.GetTranche(i)->GetTrancheName(), Qt::EditRole);
        d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::PriceCol), Commarize(d->m_WtfToExtension.GetTranche(i)->GetPrice(), 2), Qt::EditRole);
        d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::PriceCol), d->m_WtfToExtension.GetTranche(i)->GetPrice(), Qt::UserRole);
        d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::WALCol), Commarize(d->m_WtfToExtension.GetTranche(i)->GetWALife(/*m_WtfToExtension.GetCalculatedMtgPayments().GetDate(0)*/), 2), Qt::EditRole);
        d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::LossCol), Commarize(d->m_WtfToExtension.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
        if (d->m_WtfToCall.GetTranche(i)) {
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), d->m_WtfToCall.GetCalledPeriod().toString("MMM-yy"), Qt::EditRole);
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), d->m_WtfToCall.GetCalledPeriod(), Qt::UserRole);
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::WALCallCol), Commarize(d->m_WtfToCall.GetTranche(i)->GetWALife(/*m_WtfToCall.GetCalculatedMtgPayments().GetDate(0)*/), 2), Qt::EditRole);
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::LossCallCol), Commarize(d->m_WtfToCall.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
		}
		else {
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), tr("N/A"), Qt::EditRole);
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), QVariant(), Qt::UserRole);
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::WALCallCol), tr("N/A"), Qt::EditRole);
            d->m_OriginalStructureModel->setData(d->m_OriginalStructureModel->index(i, LoanAssumptionsEditorPrivate::LossCallCol), tr("N/A"), Qt::EditRole);
		}
		QStandardItemModel* CurrentModel = new QStandardItemModel(this);
        const TrancheCashFlow& CurrentCF = d->m_WtfToExtension.GetTranche(i)->GetCashFlow();
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
                    if (TestVal>static_cast<double>(LoanAssumptionsEditorPrivate::TestInfinityThreshold))CurrentModel->setData(CurrentModel->index(k, j + 1), "Infinity", Qt::EditRole);
					else CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(TestVal*100.0, 2) + '%', Qt::EditRole);
				}break;
				default:
					CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(CurrentCF.GetFlow(k, CFTypes.at(j))), Qt::EditRole);
					break;
				}
				
				CurrentModel->setData(CurrentModel->index(k, j + 1), CurrentCF.GetFlow(k, CFTypes.at(j)), Qt::UserRole);
			}
		}
        d->m_OriginalTranchesModels.append(QSharedPointer<QStandardItemModel>(CurrentModel, [](QStandardItemModel* a) {a->deleteLater(); }));
	}
    d->BaseTab->setTabEnabled(2, true);
}

void LoanAssumptionsEditor::GuessAssumptions()
{
    Q_D(LoanAssumptionsEditor);
	if (!d->YesNoDialog(tr("Are you sure?"), tr("This action will potentially override the current inputs.\nAre you sure you want to continue?"))) return;
	int MatchFound = 0;
	QSharedPointer<LoanAssumption> CurrAss;
    QScopedPointer<QProgressDialog, QScopedPointerDeleteLater> LoadProgress(new QProgressDialog(tr("Searching for Matches"), QString(), 0, d->m_PoolModel->rowCount()*d->m_Assumptions.size(), this));
	LoadProgress->setCancelButton(nullptr);
	LoadProgress->setWindowModality(Qt::WindowModal);
	LoadProgress->show();
    for (int i = 0; i < d->m_PoolModel->rowCount(); ++i) {
        for (auto j = d->m_Assumptions.constBegin(); j != d->m_Assumptions.constEnd(); ++j) {
            CurrAss = d->m_DirtyAssumptions.value(j.key(), j.value());
			if (!CurrAss) continue;
            if (CurrAss->MatchPattern(d->m_PoolModel->data(d->m_PoolModel->index(i, 0), Qt::EditRole).toString())) {
                if (d->m_PoolModel->data(d->m_PoolModel->index(i, 2)).toString().compare(j.key(), Qt::CaseInsensitive) != 0) {
                    d->m_PoolModel->setData(d->m_PoolModel->index(i, 3), j.key(), Qt::EditRole);
                    d->m_PoolModel->setData(d->m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
					++MatchFound;
				}
			}
            else if (CurrAss->MatchPattern(d->m_PoolModel->data(d->m_PoolModel->index(i, 1), Qt::EditRole).toString())) {
                if (d->m_PoolModel->data(d->m_PoolModel->index(i, 2)).toString().compare(j.key(), Qt::CaseInsensitive) != 0) {
                    d->m_PoolModel->setData(d->m_PoolModel->index(i, 3), j.key(), Qt::EditRole);
                    d->m_PoolModel->setData(d->m_PoolModel->index(i, 3), Qt::Checked, Qt::UserRole + Qt::CheckStateRole);
					++MatchFound;
				}
			}
			LoadProgress->setValue(LoadProgress->value() + 1);
		}
	}
	QMessageBox::information(this, tr("Results"), tr("%n Match(es) Found", "", MatchFound));
}

void LoanAssumptionsEditor::RemoveScenario() {
    Q_D(LoanAssumptionsEditor);
    if (d->m_ScenarioList->selectionModel()->currentIndex().isValid()) {
        auto currIdx = d->m_SortScenarios->mapToSource(d->m_ScenarioList->selectionModel()->currentIndex());
        if (d->m_ScenariosModel->data(currIdx, Qt::UserRole) == RichTextDelegate::Added) {
            QString key = d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString();
            for (int j = 0; j < d->m_PoolModel->rowCount(); ++j) {
				if (key.compare(
                    d->m_PoolModel->data(d->m_PoolModel->index(j, 3), Qt::EditRole).toString()
					, Qt::CaseInsensitive
					) == 0) {
                    d->m_PoolModel->setData(d->m_PoolModel->index(j, 3), QVariant(), Qt::EditRole);
                    d->m_PoolModel->setData(d->m_PoolModel->index(j, 3), QVariant(), Qt::UserRole);
				}
			}
            d->m_DirtyAssumptions.remove(key);
            d->m_Assumptions.remove(key);
            d->m_ScenariosModel->removeRow(currIdx.row());
		}
		else {
            d->m_ScenariosModel->setData(currIdx, RichTextDelegate::Erased, Qt::UserRole);
            d->m_DirtyAssumptions[d->m_ScenariosModel->data(currIdx, Qt::EditRole).toString()] = QSharedPointer<LoanAssumption>(nullptr);
            d->ActiveAssumption.reset(nullptr);
		}
        d->m_ScenarioNameEdit->setEnabled(d->ActiveAssumption);
        d->m_AliasLineEdit->setEnabled(d->ActiveAssumption);
        d->AddSeniorAssumptionButton->setEnabled(d->ActiveAssumption);
        d->AddMezzAssumptionButton->setEnabled(d->ActiveAssumption);
        d->m_seniorDateCheck->setEnabled(d->ActiveAssumption);
        d->m_SeniorDateEdit->setEnabled(d->ActiveAssumption);
        d->m_MezzDateCheck->setEnabled(d->ActiveAssumption);
        d->m_MezzDateEdit->setEnabled(d->ActiveAssumption);
	}
    d->RemoveScenarioButton->setEnabled(d->m_ScenariosModel->rowCount() > 0 && d->ActiveAssumption);
}

void LoanAssumptionsEditor::SetPoolModelChecks(const QModelIndex& index, const QModelIndex&)
{
    Q_D(LoanAssumptionsEditor);
	if (index.column() >= 2) {
		int OtherCol = (index.column() == 2 ? 3 : 2);
        auto CurrVal = d->m_PoolModel->data(index, Qt::UserRole + Qt::CheckStateRole).toInt() != Qt::Checked;
        auto PrevVal = d->m_PoolModel->data(d->m_PoolModel->index(index.row(), OtherCol), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked;
		if (CurrVal != PrevVal) {
            d->m_PoolModel->setData(d->m_PoolModel->index(index.row(), OtherCol), CurrVal ? Qt::Checked : Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
		}
	}
}

void LoanAssumptionsEditor::SortPool()
{
    Q_D(LoanAssumptionsEditor);
    d->m_PoolSorter->sort(qAbs(d->m_LastPoolColSorted) - 1, d->m_LastPoolColSorted<0 ? Qt::DescendingOrder : Qt::AscendingOrder);
}
void LoanAssumptionsEditor::SortScanned()
{
    Q_D(LoanAssumptionsEditor);
    d->m_ScannedModelProxy->sort(qAbs(d->m_LastScannedColSorted) - 1, d->m_LastScannedColSorted<0 ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void LoanAssumptionsEditor::SavePool()
{
    Q_D(LoanAssumptionsEditor);
    if (!d->YesNoDialog(tr("Save Loan Pool"), tr("Are you sure you want to override the previous pool?\nPlease note that changes to the scenarios will not apply until the model is recalculated"))) return;
	emit PoolSaved();
    if (d->m_LastModelLoaded.isEmpty()) return;
	bool SomethingToChange = false;
    for (int i = 0; i < d->m_PoolModel->rowCount(); ++i) {
        if (d->m_PoolModel->data(d->m_PoolModel->index(i, 3), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked) {
            Mortgage* CurrLoan = d->m_LoanPool.GetLoans().value(d->m_PoolModel->data(d->m_PoolModel->index(i, 0), Qt::UserRole).toInt(), nullptr);
			if (CurrLoan) {
                auto NewScenario = d->m_PoolModel->data(d->m_PoolModel->index(i, 3), Qt::EditRole).toString();
				if (NewScenario.isEmpty()) CurrLoan->RemoveProperty("Scenario");
				else CurrLoan->SetProperty("Scenario", NewScenario);
				SomethingToChange = true;
			}
		}
	}
	if (!SomethingToChange) return;
    QFile file(d->m_LastModelLoaded);
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return;
	}
	QScopedPointer<QProgressDialog,QScopedPointerDeleteLater> LoadProgress(new QProgressDialog("Saving Model", QString(), 0, 10, this));
	LoadProgress->setValue(0);
	LoadProgress->setCancelButton(nullptr);
	LoadProgress->setWindowModality(Qt::WindowModal);
	LoadProgress->show();
	QDate OldLiborUpdateDate; out >> OldLiborUpdateDate;
	LoadProgress->setValue(LoadProgress->value() + 1);
	bool OldUseForwardCurve; out >> OldUseForwardCurve;
	LoadProgress->setValue(LoadProgress->value() + 1);
	bool OldBaseCaseToCall; out >> OldBaseCaseToCall;
	LoadProgress->setValue(LoadProgress->value() + 1);
	Waterfall OldExtension; OldExtension.SetLoadProtocolVersion(VersionChecker); out >> OldExtension; 
	LoadProgress->setValue(LoadProgress->value() + 1);
	Waterfall OldCall; OldCall.SetLoadProtocolVersion(VersionChecker); out >> OldCall; 
	LoadProgress->setValue(LoadProgress->value() + 1);
	{MtgCalculator junk;	junk.SetLoadProtocolVersion(VersionChecker); out >> junk; }
	LoadProgress->setValue(LoadProgress->value() + 1);
	ConstantBaseRateTable OldConstTable; OldConstTable.SetLoadProtocolVersion(VersionChecker); out >> OldConstTable;
	LoadProgress->setValue(LoadProgress->value() + 1);
	ForwardBaseRateTable OldFwdTable; OldFwdTable.SetLoadProtocolVersion(VersionChecker); out >> OldFwdTable;
	LoadProgress->setValue(LoadProgress->value() + 1);
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
        << d->m_LoanPool
		<< OldConstTable 
		<< OldFwdTable
	;
	LoadProgress->setValue(LoadProgress->value() + 1);
	file.close();
}

QHash<qint32, QString> LoanAssumptionsEditor::GetScenarios() const
{
    Q_D(const LoanAssumptionsEditor);
	QHash<qint32, QString> Result;
    for (int i = 0; i < d->m_PoolModel->rowCount(); ++i) {
        if (d->m_PoolModel->data(d->m_PoolModel->index(i, 3), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked)
            Result.insert(d->m_PoolModel->data(d->m_PoolModel->index(i, 0), Qt::UserRole).toInt(), d->m_PoolModel->data(d->m_PoolModel->index(i, 3), Qt::EditRole).toString());
		else
            Result.insert(d->m_PoolModel->data(d->m_PoolModel->index(i, 0), Qt::UserRole).toInt(), d->m_PoolModel->data(d->m_PoolModel->index(i, 2), Qt::EditRole).toString());
	}
	return Result;
}

void LoanAssumptionsEditor::SetEnableLoad(bool a)
{
    Q_D(LoanAssumptionsEditor);
    d->m_EnableLoad = a;
    d->LoadPoolButton->setEnabled(d->m_EnableLoad);
}

int LoanAssumptionsEditor::LoanCount() const
{
    Q_D(const LoanAssumptionsEditor);
    return d->m_LoanPool.GetLoans().count();
}

void LoanAssumptionsEditorPrivate::CreateStructureComparison()
{
    Q_Q(LoanAssumptionsEditor);
	m_OriginalStructureModel = new QStandardItemModel(q);
	m_NewStructureModel = new QStandardItemModel(q);
	m_OriginalStructureModel->setColumnCount(ColumnsInStructure);
	m_NewStructureModel->setColumnCount(ColumnsInStructure);
	m_OriginalStructureModel->setRowCount(0);
	m_NewStructureModel->setRowCount(0);
    m_OriginalStructureModel->setHorizontalHeaderLabels(QStringList() << q->tr("Tranche") << q->tr("Call Date") << q->tr("Price") << q->tr("IRR") << q->tr("WAL") << q->tr("DM") << q->tr("Loss") << q->tr("Call IRR") << q->tr("Call WAL") << q->tr("Call DM") << q->tr("Call Loss"));
    m_NewStructureModel->setHorizontalHeaderLabels(QStringList() << q->tr("Tranche") << q->tr("Call Date") << q->tr("Price") << q->tr("IRR") << q->tr("WAL") << q->tr("DM") << q->tr("Loss") << q->tr("Call IRR") << q->tr("Call WAL") << q->tr("Call DM") << q->tr("Call Loss"));

	m_OldStrGenTable = new QTableView(q);
	SafeSetModel(m_OldStrGenTable, m_OriginalStructureModel);
	q->connect(m_OldStrGenTable->selectionModel(), &QItemSelectionModel::currentChanged, q, &LoanAssumptionsEditor::OldGenTableSelect,Qt::QueuedConnection);
	m_OldStrGenTable->verticalHeader()->hide();
	m_OldStrGenTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_OldStrGenTable->horizontalHeader()->setStretchLastSection(true);
	m_OldStrGenTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_OldStrGenTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_OldStrGenTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_OldStrGenTable->horizontalHeader()->setMinimumSectionSize(50);
	m_OldStrGenTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_OldStrGenTable->setAlternatingRowColors(true);
	m_OldStrDetTable = new QTableView(q);
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

	m_NewStrGenTable = new QTableView(q);
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
	m_NewStrDetTable = new QTableView(q);
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

    q->connect(m_OriginalStructureModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), q, SLOT(AdjustOldGenTableHeight()), Qt::QueuedConnection);
    q->connect(m_OriginalStructureModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), q, SLOT(AdjustOldGenTableHeight()), Qt::QueuedConnection);
    q->connect(m_OriginalStructureModel, SIGNAL(modelReset()), q, SLOT(AdjustOldGenTableHeight()), Qt::QueuedConnection);
    q->connect(m_NewStructureModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), q, SLOT(AdjustNewGenTableHeight()), Qt::QueuedConnection);
    q->connect(m_NewStructureModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), q, SLOT(AdjustNewGenTableHeight()), Qt::QueuedConnection);
    q->connect(m_NewStructureModel, SIGNAL(modelReset()), q, SLOT(AdjustNewGenTableHeight()), Qt::QueuedConnection);

    q->connect(m_OldStrGenTable->verticalScrollBar(), &QScrollBar::valueChanged, m_NewStrGenTable->verticalScrollBar(), &QScrollBar::setValue);
    q->connect(m_NewStrGenTable->verticalScrollBar(), &QScrollBar::valueChanged, m_OldStrGenTable->verticalScrollBar(), &QScrollBar::setValue);
    q->connect(m_OldStrDetTable->verticalScrollBar(), &QScrollBar::valueChanged, m_NewStrDetTable->verticalScrollBar(), &QScrollBar::setValue);
    q->connect(m_NewStrDetTable->verticalScrollBar(), &QScrollBar::valueChanged, m_OldStrDetTable->verticalScrollBar(), &QScrollBar::setValue);
    q->connect(m_OldStrGenTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_NewStrGenTable->horizontalScrollBar(), &QScrollBar::setValue);
    q->connect(m_NewStrGenTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_OldStrGenTable->horizontalScrollBar(), &QScrollBar::setValue);
    q->connect(m_OldStrDetTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_NewStrDetTable->horizontalScrollBar(), &QScrollBar::setValue);
    q->connect(m_NewStrDetTable->horizontalScrollBar(), &QScrollBar::valueChanged, m_OldStrDetTable->horizontalScrollBar(), &QScrollBar::setValue);

    q->connect(m_OriginalStructureModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
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
                m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCol), q_func()->tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCol), QVariant(), Qt::UserRole);
                m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCol), q_func()->tr("N/A"), Qt::EditRole);
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
                m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCallCol), q_func()->tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), IRRCallCol), QVariant(), Qt::UserRole);
                m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCallCol), q_func()->tr("N/A"), Qt::EditRole);
				m_OriginalStructureModel->setData(m_OriginalStructureModel->index(index.row(), DMCallCol), QVariant(), Qt::UserRole);
			}
		}
	});
    q->connect(m_NewStructureModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
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
                m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCol), q_func()->tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCol), QVariant(), Qt::UserRole);
                m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCol), q_func()->tr("N/A"), Qt::EditRole);
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
                m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCallCol), q_func()->tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), IRRCallCol), QVariant(), Qt::UserRole);
                m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCallCol), q_func()->tr("N/A"), Qt::EditRole);
				m_NewStructureModel->setData(m_NewStructureModel->index(index.row(), DMCallCol), QVariant(), Qt::UserRole);
			}
		}
	});

	CalculateNewStructureButton = new QPushButton(q);
	CalculateNewStructureButton->setText(q->tr("Assess Impact"));
	CalculateNewStructureButton->setEnabled(false);
    q->connect(CalculateNewStructureButton, &QPushButton::clicked, q, &LoanAssumptionsEditor::CalculateNewStructure);

	QLabel* BeforeLabel = new QLabel(q);
    BeforeLabel->setText(q->tr("Before"));
	BeforeLabel->setStyleSheet("QLabel { qproperty-alignment: 'AlignCenter'; font-size: 14px; font-weight: bold; }");
	QLabel* AfterLabel = new QLabel(q);
    AfterLabel->setText(q->tr("After"));
	AfterLabel->setStyleSheet(BeforeLabel->styleSheet());

	m_OldSelectedTrancheLabel = new QLabel(q);
	m_OldSelectedTrancheLabel->setStyleSheet("QLabel { qproperty-alignment: 'AlignHCenter | AlignBottom'; font-size: 12px;}");
	m_NewSelectedTrancheLabel = new QLabel(q);
	m_NewSelectedTrancheLabel->setStyleSheet(m_OldSelectedTrancheLabel->styleSheet());

	/*QFrame* MidLine = new QFrame(this);
	MidLine->setFrameShape(QFrame::VLine);
	MidLine->setFrameShadow(QFrame::Plain);
	MidLine->setLineWidth(1);*/

	QWidget* NewTab = new QWidget(q);;
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
    BaseTab->addTab(NewTab, q->tr("Notes"));
	BaseTab->setTabEnabled(2, false);
}

void LoanAssumptionsEditor::CalculateNewStructure() {
    Q_D(LoanAssumptionsEditor);
    if (d->m_NewLoans || d->m_NewWatFalls) return;
    d->m_NewLoans = new MtgCalculator(this);
    d->m_NewLoans->SetCPRass(d->m_LoanPool.GetCPRass());
    d->m_NewLoans->SetCDRass(d->m_LoanPool.GetCDRass());
    d->m_NewLoans->SetLSass(d->m_LoanPool.GetLSass());
    d->m_NewLoans->SetRecoveryLag(d->m_LoanPool.GetRecoveryLag());
    d->m_NewLoans->SetDelinquency(d->m_LoanPool.GetDelinquency());
    d->m_NewLoans->SetDelinquencyLag(d->m_LoanPool.GetDelinquencyLag());
    d->m_NewLoans->SetStartDate(d->m_LoanPool.GetStartDate());
    for (int i = 0; i < d->m_PoolModel->rowCount(); ++i) {
        Mortgage NewLoan(*(d->m_LoanPool.GetLoans().value(d->m_PoolModel->data(d->m_PoolModel->index(i, 0), Qt::UserRole).toInt())));
        bool UseSuggestion = d->m_PoolModel->data(d->m_PoolModel->index(i, 3), Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked;
        QString ScenarioToApply = d->m_PoolModel->data(d->m_PoolModel->index(i, 2 + (UseSuggestion ? 1 : 0)), Qt::EditRole).toString();
		if (!ScenarioToApply.isEmpty()) {
            auto CurrScen = d->m_DirtyAssumptions.value(ScenarioToApply, d->m_Assumptions.value(ScenarioToApply, QSharedPointer<LoanAssumption>(nullptr)));
			if (CurrScen) {
				NewLoan.SetScenario(*CurrScen);
			}
		}
        d->m_NewLoans->AddLoan(NewLoan, d->m_PoolModel->data(d->m_PoolModel->index(i, 0), Qt::UserRole).toInt());
	}
    d->m_NewWatFalls = new WaterfallCalculator(this);
	
	QProgressDialog* CalcProgress = new QProgressDialog(this);
	CalcProgress->setLabelText(tr("Calculating Cash Flows"));
	CalcProgress->setRange(0,100);
	CalcProgress->setCancelButtonText(tr("Cancel"));
	CalcProgress->setWindowTitle(tr("Calculating"));
	CalcProgress->setAutoClose(false);
	CalcProgress->show();
	
    connect(d->m_NewLoans, &MtgCalculator::Calculated, [&]() {
        d_func()->m_NewWtfToExtension = d_func()->m_WtfToExtension;
        d_func()->m_NewWtfToExtension.ResetMtgFlows();
        if (d_func()->m_NewLoans->GetAggregatedResults()) d_func()->m_NewWtfToExtension.AddMortgagesFlows(*d_func()->m_NewLoans->GetAggregatedResults());
        d_func()->m_NewWatFalls->AddWaterfall(d_func()->m_NewWtfToExtension, 0);
        d_func()->m_NewWtfToCall = d_func()->m_WtfToCall;
        if (d_func()->m_WtfToCall.GetTranchesCount() > 0) {
            d_func()->m_NewWtfToCall.ResetMtgFlows();
            if (d_func()->m_NewLoans->GetAggregatedResults()) d_func()->m_NewWtfToCall.AddMortgagesFlows(*d_func()->m_NewLoans->GetAggregatedResults());
            d_func()->m_NewWatFalls->AddWaterfall(d_func()->m_NewWtfToCall, 1);
		}
        if (!d_func()->m_NewWatFalls->StartCalculation()) { QMessageBox::critical(this, tr("Error"), tr("An error occurred during the calculations of liabilities cash flows")); }
	});
    connect(CalcProgress, &QProgressDialog::canceled, d->m_NewLoans, &MtgCalculator::StopCalculation);
    connect(d->m_NewLoans, &MtgCalculator::ProgressPct, CalcProgress, &QProgressDialog::setValue);
    connect(d->m_NewLoans, &MtgCalculator::BeeError, d->m_NewLoans, &MtgCalculator::StopCalculation);
    connect(d->m_NewLoans, &MtgCalculator::BeeError, [this]() {QMessageBox::critical(this, tr("Error"), tr("An error occurred during the calculations of assets cash flows")); });
    connect(d->m_NewLoans, &MtgCalculator::Calculated, d->m_NewLoans, &MtgCalculator::deleteLater);
    connect(d->m_NewLoans, &MtgCalculator::Stopped, d->m_NewLoans, &MtgCalculator::deleteLater);
    connect(d->m_NewLoans, &MtgCalculator::Stopped, CalcProgress, &MtgCalculator::deleteLater);
    connect(d->m_NewLoans, &MtgCalculator::Stopped, d->m_NewWatFalls, &MtgCalculator::deleteLater);
	
    connect(d->m_NewWatFalls, &WaterfallCalculator::Calculated, this, &LoanAssumptionsEditor::NewTranchesCalculated);
    connect(CalcProgress, &QProgressDialog::canceled, d->m_NewWatFalls, &WaterfallCalculator::StopCalculation);
    connect(d->m_NewWatFalls, &WaterfallCalculator::ProgressPct, CalcProgress, &QProgressDialog::setValue);
    connect(d->m_NewWatFalls, &WaterfallCalculator::BeeError, d->m_NewWatFalls, &WaterfallCalculator::StopCalculation);
    connect(d->m_NewWatFalls, &WaterfallCalculator::BeeError, [this]() {QMessageBox::critical(this, tr("Error"), tr("An error occurred during the calculations of liabilities cash flows")); });
    connect(d->m_NewWatFalls, &WaterfallCalculator::Calculated, d->m_NewWatFalls, &WaterfallCalculator::deleteLater);
    connect(d->m_NewWatFalls, &WaterfallCalculator::Calculated, CalcProgress, &QProgressDialog::deleteLater);
    connect(d->m_NewWatFalls, &WaterfallCalculator::Stopped, d->m_NewWatFalls, &WaterfallCalculator::deleteLater);
    connect(d->m_NewWatFalls, &WaterfallCalculator::Stopped, CalcProgress, &QProgressDialog::deleteLater);
	
    d->m_NewLoans->StartCalculation();
}
void LoanAssumptionsEditor::NewTranchesCalculated() {
    Q_D(LoanAssumptionsEditor);
    d->m_NewWtfToExtension = *(d->m_NewWatFalls->GetResult(0));
    if (d->m_NewWatFalls->GetResult(1))
        d->m_NewWtfToCall = *(d->m_NewWatFalls->GetResult(1));
	//////////////////////////////////////////////////////////////////////////
    d->m_NewStructureModel->setRowCount(d->m_NewWtfToExtension.GetTranchesCount());
    d->SafeSetModel(d->m_NewStrGenTable, d->m_NewStructureModel);
    connect(d->m_NewStrGenTable->selectionModel(), &QItemSelectionModel::currentChanged, this, &LoanAssumptionsEditor::NewGenTableSelect, Qt::QueuedConnection);
    for (int i = 0; i < d->m_NewWtfToExtension.GetTranchesCount(); ++i) {
        d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::TrancheNameCol), d->m_NewWtfToExtension.GetTranche(i)->GetTrancheName(), Qt::EditRole);
        d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::PriceCol), Commarize(d->m_NewWtfToExtension.GetTranche(i)->GetPrice(), 2), Qt::EditRole);
        d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::PriceCol), d->m_NewWtfToExtension.GetTranche(i)->GetPrice(), Qt::UserRole);
        d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::WALCol), Commarize(d->m_NewWtfToExtension.GetTranche(i)->GetWALife(/*m_NewWtfToExtension.GetCalculatedMtgPayments().GetDate(0)*/), 2), Qt::EditRole);
        d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::LossCol), Commarize(d->m_NewWtfToExtension.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
        if (d->m_NewWtfToCall.GetTranche(i)) {
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), d->m_NewWtfToCall.GetCalledPeriod().toString("MMM-yy"), Qt::EditRole);
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), d->m_NewWtfToCall.GetCalledPeriod(), Qt::UserRole);
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::WALCallCol), Commarize(d->m_NewWtfToCall.GetTranche(i)->GetWALife(/*m_NewWtfToCall.GetCalculatedMtgPayments().GetDate(0)*/), 2), Qt::EditRole);
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::LossCallCol), Commarize(d->m_NewWtfToCall.GetTranche(i)->GetLossRate()*100.0, 2) + '%', Qt::EditRole);
		}
		else {
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), tr("N/A"), Qt::EditRole);
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::CallDateCol), QVariant(), Qt::UserRole);
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::WALCallCol), tr("N/A"), Qt::EditRole);
            d->m_NewStructureModel->setData(d->m_NewStructureModel->index(i, LoanAssumptionsEditorPrivate::LossCallCol), tr("N/A"), Qt::EditRole);
		}
		QStandardItemModel* CurrentModel = new QStandardItemModel(this);
        const TrancheCashFlow& CurrentCF = d->m_NewWtfToExtension.GetTranche(i)->GetCashFlow();
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
				case TrancheCashFlow::TrancheFlowType::OCFlow :
				case TrancheCashFlow::TrancheFlowType::ICFlow :
				case TrancheCashFlow::TrancheFlowType::ICTarget :
				case TrancheCashFlow::TrancheFlowType::OCTarget : {
					double TestVal = CurrentCF.GetFlow(k, CFTypes.at(j));
                    if (TestVal>static_cast<double>(LoanAssumptionsEditorPrivate::TestInfinityThreshold))CurrentModel->setData(CurrentModel->index(k, j + 1), "Infinity", Qt::EditRole);
					else CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(TestVal*100.0, 2) + '%', Qt::EditRole);
				}break;
				default:
					CurrentModel->setData(CurrentModel->index(k, j + 1), Commarize(CurrentCF.GetFlow(k, CFTypes.at(j))), Qt::EditRole);
					break;
				}

				CurrentModel->setData(CurrentModel->index(k, j + 1), CurrentCF.GetFlow(k, CFTypes.at(j)), Qt::UserRole);
			}
		}
        d->m_NewTranchesModels.append(QSharedPointer<QStandardItemModel>(CurrentModel, [](QStandardItemModel* a) {a->deleteLater(); }));
	}
	//////////////////////////////////////////////////////////////////////////
    if (d->m_OldStrGenTable->selectionModel()->currentIndex().isValid()) {
        d->m_NewStrGenTable->selectionModel()->setCurrentIndex(d->m_NewStrGenTable->model()->index(d->m_OldStrGenTable->selectionModel()->currentIndex().row(), 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}
void LoanAssumptionsEditorPrivate::SafeSetModel(QAbstractItemView* View, QAbstractItemModel* NewModel) {
	QItemSelectionModel* ModToDelete = View->selectionModel();
	View->setModel(NewModel);
	if (ModToDelete) {
		if (View->selectionModel() != ModToDelete)
			ModToDelete->deleteLater();
	}
}

void LoanAssumptionsEditor::NewGenTableSelect(const QModelIndex& index, const QModelIndex&)
{
    Q_D(LoanAssumptionsEditor);
	if (index.isValid()) {
        d->m_NewSelectedTrancheLabel->setText(d->m_NewStructureModel->data(d->m_NewStructureModel->index(index.row(), LoanAssumptionsEditorPrivate::TrancheNameCol), Qt::EditRole).toString());
        d->SafeSetModel(d->m_NewStrDetTable, d->m_NewTranchesModels.at(index.row()).data());
        if (d->m_OldStrGenTable->model())
            d->m_OldStrGenTable->selectionModel()->setCurrentIndex(d->m_OriginalStructureModel->index(index.row(), 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
	else {
        d->m_NewSelectedTrancheLabel->setText(QString());
        d->SafeSetModel(d->m_NewStrDetTable, nullptr);
        d->m_OldStrGenTable->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::OldGenTableSelect(const QModelIndex& index, const QModelIndex&)
{
    Q_D(LoanAssumptionsEditor);
	if (index.isValid()) {
        d->m_OldSelectedTrancheLabel->setText(d->m_OriginalStructureModel->data(d->m_OriginalStructureModel->index(index.row(), LoanAssumptionsEditorPrivate::TrancheNameCol), Qt::EditRole).toString());
        d->SafeSetModel(d->m_OldStrDetTable, d->m_OriginalTranchesModels.at(index.row()).data());
        if (d->m_NewStrGenTable->model())
            d->m_NewStrGenTable->selectionModel()->setCurrentIndex(d->m_NewStructureModel->index(index.row(), 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
	else {
        d->m_OldSelectedTrancheLabel->setText(QString());
        d->SafeSetModel(d->m_OldStrDetTable, nullptr);
        d->m_NewStrGenTable->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::AdjustOldGenTableHeight()
{
    Q_D(LoanAssumptionsEditor);
    int TotalSize = d->m_OldStrGenTable->horizontalHeader()->height() + 2 * d->m_OldStrGenTable->frameWidth() + d->m_OldStrGenTable->horizontalScrollBar()->height();
    for (int i = 0; i<d->m_OldStrGenTable->model()->rowCount(); ++i)
        TotalSize += d->m_OldStrGenTable->rowHeight(i);
    d->m_OldStrGenTable->setMinimumHeight(TotalSize);
    if (d->m_OldStrGenTable->model()->rowCount() > 0) {
        d->m_OldStrGenTable->selectionModel()->setCurrentIndex(d->m_OldStrGenTable->model()->index(0, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditor::AdjustNewGenTableHeight()
{
    Q_D(LoanAssumptionsEditor);
    int TotalSize = d->m_NewStrGenTable->horizontalHeader()->height() + 2 * d->m_NewStrGenTable->frameWidth() + d->m_NewStrGenTable->horizontalScrollBar()->height();
    for (int i = 0; i<d->m_NewStrGenTable->model()->rowCount(); ++i)
        TotalSize += d->m_NewStrGenTable->rowHeight(i);
    d->m_NewStrGenTable->setMinimumHeight(TotalSize);
    if (d->m_NewStrGenTable->model()->rowCount() > 0) {
        d->m_NewStrGenTable->selectionModel()->setCurrentIndex(d->m_NewStrGenTable->model()->index(0, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void LoanAssumptionsEditorPrivate::CreateModelScanner()
{
    Q_Q(LoanAssumptionsEditor);
    m_PoolMatcher = new LoanAssMatcher(q);

    m_ScanPoolsModel = new QStandardItemModel(q);
    m_ScanPoolsModel->setRowCount(0);
    m_ScanPoolsModel->setColumnCount(1);
    m_ScannedPoolsProxy = new QSortFilterProxyModel(q);
    m_ScannedPoolsProxy->setSourceModel(m_ScanPoolsModel);
    m_ScannedPoolsProxy->setFilterRole(Qt::UserRole);
    m_ScannedPoolsProxy->setFilterKeyColumn(0);
    m_ScannedModel = new QStandardItemModel(q);
    m_ScannedModel->setColumnCount(4);
    m_ScannedModel->setRowCount(0);
    m_ScannedModel->setHorizontalHeaderLabels(QStringList() << q->tr("Issuer") << q->tr("Facility") << q->tr("Current Scenario") << q->tr("Detected Scenario"));
    m_ScannedModelProxy = new ReadOnlyColProxy(q);
    m_ScannedModelProxy->setSourceModel(m_ScannedModel);
    m_ScannedModelProxy->SetReadOnlyCol(QList<qint32>() << 0 << 1 << 2);
    m_ScannedModelProxy->setFilterKeyColumn(0);
    m_ScannedModelProxy->setFilterRole(Qt::UserRole);

    m_PoolScanFilterView = new QListView(q);
    SafeSetModel(m_PoolScanFilterView, m_SortScenarios);
    m_PoolScanFilterView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_PoolScanFilterView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_PoolScanFilterView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_PoolScanDealsView = new QListView(q);
    m_PoolScanDealsView->setModel(m_ScannedPoolsProxy);
    m_PoolScanDealsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_PoolScanDealsView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_PoolScanDealsView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    m_PoolScanPoolView = new QTableView(q);
    m_PoolScanPoolView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    m_PoolScanPoolView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_PoolScanPoolView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_PoolScanPoolView->horizontalHeader()->setMinimumSectionSize(70);
    m_PoolScanPoolView->horizontalHeader()->setStretchLastSection(true);
    m_PoolScanPoolView->verticalHeader()->hide();
    SafeSetModel(m_PoolScanPoolView, m_ScannedModelProxy);


    QLabel* PoolScanFilterLaber = new QLabel(q);
    PoolScanFilterLaber->setText(q->tr("Filter by Scenario"));
    m_ClearPoolScanFilterButton = new QPushButton(q);
    m_ClearPoolScanFilterButton->setText(q->tr("Clear Selection"));
    m_ClearPoolScanFilterButton->setEnabled(false);
    QLabel* ScannedPoolLaber = new QLabel(q);
    ScannedPoolLaber->setText(q->tr("Loan Pools"));

    QGroupBox* LeftGroup = new QGroupBox(q);
    LeftGroup->setTitle(q->tr("Models"));
    QGridLayout* LeftLay = new QGridLayout(LeftGroup);
    LeftLay->addWidget(PoolScanFilterLaber, 0, 0);
    LeftLay->addWidget(m_ClearPoolScanFilterButton, 0, 1);
    LeftLay->addWidget(m_PoolScanFilterView, 1, 0, 1, 2);
    LeftLay->addWidget(ScannedPoolLaber, 2, 0, 1, 2);
    LeftLay->addWidget(m_PoolScanDealsView, 3, 0, 1, 2);

    m_ScanPoolsButton = new QPushButton(q);
    m_ScanPoolsButton->setText(q->tr("Start Scan"));
    m_ModelsDirEdit = new QLineEdit(q);
    m_ModelsDirEdit->setText(GetFromConfig("Folders", "UnifiedResultsFolder"));
    QLabel* ModelFolderlabel = new QLabel(q);
    ModelFolderlabel->setText(q->tr("Models Folder"));
    QPushButton *ModelsDirBrowseButton = new QPushButton(q);
    ModelsDirBrowseButton->setText("Browse"); //Ellipsis (...)
    ModelsDirBrowseButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    m_ScanProgress = new QProgressBar(q);
    m_ScanProgress->setTextVisible(true);
    m_ScanProgress->setRange(0, 100);
    m_ScanProgress->hide();
    m_ScanProgress->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    QHBoxLayout* BottomLay = new QHBoxLayout;
    BottomLay->addWidget(ModelFolderlabel);
    BottomLay->addWidget(m_ModelsDirEdit);
    BottomLay->addWidget(ModelsDirBrowseButton);
    BottomLay->addWidget(m_ScanPoolsButton);

    QWidget* TempTab = new QWidget(q);
    QGridLayout* mainLay = new QGridLayout(TempTab);
    mainLay->addWidget(LeftGroup, 0, 0);
    mainLay->addWidget(m_PoolScanPoolView, 0, 1);
    mainLay->addWidget(m_ScanProgress, 1, 0);
    mainLay->addLayout(BottomLay, 1, 1);
    BaseTab->addTab(TempTab, q->tr("Scan Models"));

    q->connect(m_ClearPoolScanFilterButton, &QPushButton::clicked, m_PoolScanFilterView->selectionModel(), &QItemSelectionModel::clearSelection);
    q->connect(m_PoolScanFilterView->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection& index, const QItemSelection&) {
        m_ClearPoolScanFilterButton->setEnabled(!index.isEmpty()); 
    });
    q->connect(m_PoolScanDealsView->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
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
    q->connect(m_PoolScanPoolView->horizontalHeader(), &QHeaderView::sectionClicked, [&](int a) {
		if (m_LastScannedColSorted == a + 1)
			m_LastScannedColSorted = -(a + 1);
		else
			m_LastScannedColSorted = a + 1;
		q_func()->SortScanned();
	});
    q->connect(m_ScannedModel, &QStandardItemModel::dataChanged, [&](const QModelIndex& index, const QModelIndex&) {
		if (index.column() == (qAbs(m_LastScannedColSorted) - 1))
            q_func()->SortScanned();
	});
    q->connect(m_PoolMatcher, &LoanAssMatcher::ProgressPct, m_ScanProgress, &QProgressBar::setValue);
    q->connect(m_PoolScanFilterView->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection &, const QItemSelection &) {
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
    q->connect(ModelsDirBrowseButton, &QPushButton::clicked, [&]() {
		if (!m_ModelsDirEdit->isEnabled()) return;
        QString NewFolder = QFileDialog::getExistingDirectory(q_func(), q_func()->tr("Select Models Directory"), m_ModelsDirEdit->text());
		if (NewFolder.isEmpty()) return;
		m_ModelsDirEdit->setText(NewFolder);
	});
    q->connect(m_ScanPoolsButton, &QPushButton::clicked, [&]() {
		if (m_ScanningPools) {
            m_ScanPoolsButton->setText(q_func()->tr("Start Scan"));
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
            m_ScanPoolsButton->setText(q_func()->tr("Stop"));
			m_PoolMatcher->StartCalculation();
		}
	});
    q->connect(m_PoolMatcher, &LoanAssMatcher::Calculated, [&]() {
		m_ScanningPools = false;
        m_ScanPoolsButton->setText(q_func()->tr("Start Scan"));
		m_ScanProgress->hide();
	});
    q->connect(m_PoolMatcher, &LoanAssMatcher::BeeCalculated, [&](int index) {
		auto CurrentResult = m_PoolMatcher->GetResult(index);
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
			m_ScanPoolsModel->setData(m_ScanPoolsModel->index(m_ScanPoolsModel->rowCount() - 1, 0), index , Qt::UserRole + 2);
			m_ScannedPoolsProxy->setFilterKeyColumn(0);
		}
	});
}









