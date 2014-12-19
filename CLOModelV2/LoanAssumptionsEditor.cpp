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
#include "AssumptionsComboDelegate.h"
#include "LoanAssumptionDelegate.h"
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
	

	m_ScenarioList = new QListView(this);
	m_ScenarioList->setModel(m_SortScenarios);
	m_ScenarioList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_ScenarioList->setSelectionMode(QAbstractItemView::SingleSelection);


	QLabel* AliasLabel = new QLabel(this);
	AliasLabel->setText(tr("Aliases"));
	m_AliasLineEdit = new QLineEdit(this);
	QPushButton* AddAliasButton = new QPushButton(this);
	AddAliasButton->setText("+");
	QPushButton* RemoveAliasButton = new QPushButton(this);
	RemoveAliasButton->setText("-");
	QPushButton* ReplaceAliasButton = new QPushButton(this);
	ReplaceAliasButton->setText("\342\207\205"); //Unicode for up and down arrow
	
	

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
	SeniorGroup->setStyleSheet(
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
		);
	SeniorGroup->setTitle(tr("Assumption for Senior Obligations"));
	QGridLayout* SeniorLay = new QGridLayout(SeniorGroup);
	m_seniorDateCheck = new QCheckBox(this);
	m_seniorDateCheck->setText(tr("Scenario Reference Date"));
	m_seniorDateCheck->setLayoutDirection(Qt::RightToLeft);
	m_SeniorDateEdit = new QDateEdit(this);
	m_SeniorDateEdit->setCalendarPopup(true);
	m_SeniorDateEdit->setDate(QDate::currentDate());
	m_SeniorDateEdit->setEnabled(false);

	SeniorLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 0);
	SeniorLay->addWidget(m_seniorDateCheck, 0, 1);
	SeniorLay->addWidget(m_SeniorDateEdit, 0, 2);
	SeniorLay->addWidget(m_SeniorTable, 1, 0, 1, 3);

	QVBoxLayout* RightLay = new QVBoxLayout;
	RightLay->addWidget(NamesGroup);
	RightLay->addWidget(SeniorGroup);

	QLabel* SelectScenarioLabel = new QLabel(this);
	SelectScenarioLabel->setText(tr("Select Scenario"));
	QGridLayout* mainLay = new QGridLayout(this);
	mainLay->addWidget(SelectScenarioLabel, 0, 0);
	mainLay->addWidget(m_ScenariosCombo, 1, 0);
	mainLay->addWidget(m_ScenarioList, 2, 0);
	mainLay->addLayout(RightLay, 0, 1, 3, 1);
	
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, this, &LoanAssumptionsEditor::ChangeScenario);
	connect(m_ScenarioList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		m_ScenariosCombo->setCurrentIndex(index.row());
	});
	connect(m_ScenariosCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int row) {
		m_ScenarioList->setCurrentIndex(m_SortScenarios->index(row, 0));
	});
	connect(m_AliasesList->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex& index, const QModelIndex&) {
		m_AliasLineEdit->setText(m_AliasesModel->data(index).toString());
	});
	connect(AddAliasButton, &QPushButton::clicked, [&](bool) {
		if (!m_AliasLineEdit->text().isEmpty()) {
			for (int i = 0; i < m_AliasesModel->rowCount(); ++i) {
				if (m_AliasesModel->data(m_AliasesModel->index(i, 0)).toString().compare(m_AliasLineEdit->text(), Qt::CaseInsensitive) == 0) return;
			}
			m_AliasesModel->insertRow(m_AliasesModel->rowCount());
			m_AliasesModel->setData(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), m_AliasLineEdit->text());
			m_AliasesList->selectionModel()->clear();
			m_AliasesList->selectionModel()->setCurrentIndex(m_AliasesModel->index(m_AliasesModel->rowCount() - 1, 0), QItemSelectionModel::Select);
		}
	});
	connect(RemoveAliasButton, &QPushButton::clicked, [&](bool) {
		m_AliasesModel->removeRow(
			m_AliasesList->selectionModel()->currentIndex().row()
		);
	});
	connect(ReplaceAliasButton, &QPushButton::clicked, [&](bool) {
		if (m_AliasLineEdit->text().isEmpty()) return;
		int CurrRow = m_AliasesList->selectionModel()->currentIndex().row();
		if (m_AliasLineEdit->text().compare(m_AliasesModel->data(m_AliasesModel->index(CurrRow, 0)).toString(), Qt::CaseInsensitive) == 0) return;
		for (int i = 0; i < m_AliasesModel->rowCount(); ++i) {
			if (i == CurrRow) continue;
			if (m_AliasesModel->data(m_AliasesModel->index(i, 0)).toString().compare(m_AliasLineEdit->text(), Qt::CaseInsensitive) == 0) return;
		}
		m_AliasesModel->setData(m_AliasesModel->index(CurrRow, 0), m_AliasLineEdit->text());
	});
	connect(m_seniorDateCheck, &QCheckBox::clicked, m_SeniorDateEdit, &QDateEdit::setEnabled);
	connect(m_SeniorAsumptionsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(SenioScenaChanged(const QModelIndex&)));
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
		m_ScenariosModel->setData(m_ScenariosModel->index(RawIndex, 0), i.value()->GetScenarioName());
	}
	m_SortScenarios->sort(0);
	m_ScenarioList->setCurrentIndex(QModelIndex());
}

void LoanAssumptionsEditor::ChangeScenario(const QModelIndex& curr, const QModelIndex& prev) {
	/*if (
		ActiveAssumption 
		&& m_ScenariosModel->data(m_SortScenarios->mapToSource(curr)).toString() != ActiveAssumption->GetScenarioName()
		&& m_IsDirty!=NotDirty
	) {
		QScopedPointer<QMessageBox, QScopedPointerDeleteLater> OkToChange(new QMessageBox(this));
		OkToChange->setIcon(QMessageBox::Warning);
		OkToChange->setText(tr("Do you want to save the changes you made to %1", "Name of the Loan scenario").arg(ActiveAssumption->GetScenarioName()));
		OkToChange->setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		OkToChange->setDefaultButton(QMessageBox::Save);
		switch (OkToChange->exec()) {
		case QMessageBox::Cancel:
			return m_ScenarioList->setCurrentIndex(prev);
		case QMessageBox::Save:
			SaveCurrentScenario();
			break;
		}
	}*/
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
		case LoanAssumption::CPR:
		case LoanAssumption::CDR:
		case LoanAssumption::LS:
		case LoanAssumption::Delinquency:
		case LoanAssumption::Haircut:
		case LoanAssumption::PrepayMultiplier:
		case LoanAssumption::LossMultiplier:
			m_SeniorAsumptionsModel->setData(m_SeniorAsumptionsModel->index(index.row(), 1), static_cast<qint8>(LoanAssumptionDelegate::AssumptionType::BloombergVectorAssumption), Qt::UserRole);
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

