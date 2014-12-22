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

	QWidget* RightSection = new QWidget(this);
	QVBoxLayout* RightLay = new QVBoxLayout(RightSection);
	RightLay->addWidget(NamesGroup);
	RightLay->addWidget(SeniorGroup);
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
		AddAliasButton->setEnabled(index.isValid() );
		ReplaceAliasButton->setEnabled(index.isValid() && m_AliasesModel->rowCount()>0);
		AddSeniorAssumptionButton->setEnabled(index.isValid());
		RemoveSeniorAssumptionButton->setEnabled(index.isValid() && m_SeniorAsumptionsModel->rowCount()>0);
		m_AliasLineEdit->clear();
	});
	connect(m_SeniorTable->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		RemoveSeniorAssumptionButton->setEnabled(index.isValid());
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
			m_AliasesList->selectionModel()->clear();
			m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::Select);
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
	connect(m_SeniorAsumptionsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(SenioScenaChanged(const QModelIndex&)));
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
		m_SeniorTable->selectionModel()->setCurrentIndex(m_SeniorAsumptionsModel->index(m_SeniorAsumptionsModel->rowCount() - 1, 0), QItemSelectionModel::Select);
	});
	connect(RemoveSeniorAssumptionButton, &QPushButton::clicked, [&](bool) {
		if (m_SeniorTable->selectionModel()->currentIndex().isValid()) {
			m_SeniorAsumptionsModel->removeRow(m_SeniorTable->selectionModel()->currentIndex().row());
		}
		RemoveSeniorAssumptionButton->setEnabled(m_SeniorAsumptionsModel->rowCount() > 0);
	});
	connect(AddScenarioButton, &QPushButton::clicked, [&](bool) {
		QString NewScenName = "Scenario %1";
		{quint64 i = 1; for (; ScenExist(NewScenName.arg(i)); ++i); NewScenName = NewScenName.arg(i); }
		m_Assumptions.insert(NewScenName, QSharedPointer<LoanAssumption>(nullptr));
		m_DirtyAssumptions.insert(NewScenName, QSharedPointer<LoanAssumption>(new LoanAssumption(NewScenName)));
		m_ScenariosModel->insertRow(m_ScenariosModel->rowCount());
		m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), NewScenName, Qt::EditRole);
		m_ScenariosModel->setData(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0), RichTextDelegate::Added, Qt::UserRole);
		m_ScenarioList->selectionModel()->setCurrentIndex(m_SortScenarios->mapFromSource(m_ScenariosModel->index(m_ScenariosModel->rowCount() - 1, 0)), QItemSelectionModel::Select);
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
			if (YesNoDialog(DiscardCurrentButton->text(), tr("Are you sure you want to discard changes to %1").arg(m_SortScenarios->data(currIdx, Qt::EditRole).toString()))) {
				DiscardScenario(m_SortScenarios->data(currIdx, Qt::EditRole).toString());
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
	});
	connect(SaveCurrentButton, &QPushButton::clicked, [&](bool) {
		if (m_ScenarioList->selectionModel()->currentIndex().isValid()) {
			auto currIdx = m_SortScenarios->mapToSource(m_ScenarioList->selectionModel()->currentIndex());
			if (YesNoDialog(SaveCurrentButton->text(), tr("Are you sure you want to save changes to %1").arg(m_SortScenarios->data(currIdx, Qt::EditRole).toString()))) {
				SaveScenario(m_SortScenarios->data(currIdx, Qt::EditRole).toString());
			}
		}
	});
	connect(SaveAllButton, &QPushButton::clicked, [&](bool) {
		if (YesNoDialog(SaveAllButton->text(), tr("Are you sure you want to save changes to %n scenario(s)", "", m_DirtyAssumptions.size()))) {
			auto AllKeys = m_DirtyAssumptions.keys();
			foreach(const QString& k, AllKeys) {
				SaveScenario(k);
			}
		}
	});


	connect(RemoveSeniorAssumptionButton, SIGNAL(clicked()), this,SLOT(CheckCurrentDirty()));
	connect(AddSeniorAssumptionButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(AddAliasButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(RemoveAliasButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(ReplaceAliasButton, SIGNAL(clicked()), this, SLOT(CheckCurrentDirty()));
	connect(m_ScenarioNameEdit, &QLineEdit::textEdited, [&](const QString& a) {if (!a.isEmpty()) CheckCurrentDirty(); });
	connect(m_SeniorTable->itemDelegateForColumn(0), SIGNAL(Edited()), this, SLOT(CheckCurrentDirty()));
	connect(m_SeniorTable->itemDelegateForColumn(1), SIGNAL(Edited()), this, SLOT(CheckCurrentDirty()));
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
	}
	else {
		m_seniorDateCheck->setChecked(false);
		m_SeniorDateEdit->setEnabled(false);
		m_SeniorDateEdit->setDate(QDate::currentDate());
		m_AliasesModel->setRowCount(0);
	}
	

}

void LoanAssumptionsEditor::SenioScenaChanged(const QModelIndex& index) {
	if (index.column() == 0) {
		m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::EditRole);
		auto AssumptionType = m_SeniorAsumptionsModel->data(index, Qt::UserRole).toInt();
		for (int i = 0; i < m_SeniorAsumptionsModel->rowCount() && AssumptionType>=0; ++i) {
			if (i == index.row())continue;
			if (m_SeniorAsumptionsModel->data(m_SeniorAsumptionsModel->index(i, 0), Qt::UserRole) == AssumptionType) {
				m_SeniorAsumptionsModel->setData(index, -1, Qt::UserRole);
				m_SeniorAsumptionsModel->setData(index, QVariant(), Qt::EditRole);
				m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::UserRole);
				m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), QVariant(), Qt::EditRole);
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
		m_DirtyAssumptions[m_ScenariosModel->data(currIdx, Qt::EditRole).toString()] = QSharedPointer<LoanAssumption>(new LoanAssumption(DirtyVersion));
		if (m_ScenariosModel->data(currIdx, Qt::UserRole) != RichTextDelegate::Added) {
			m_ScenariosModel->setData(currIdx, RichTextDelegate::Dirty, Qt::UserRole);
		}
	}
}

void LoanAssumptionsEditor::SaveScenario(const QString& key) {

}

void LoanAssumptionsEditor::DiscardScenario(const QString& key) {

}

