#ifndef LOANASSUMPTIONSMODEL_H
#define LOANASSUMPTIONSMODEL_H

#include <QWidget>
#include <QHash>
#include <QSharedPointer>
#include <QFlags>
#include "MtgCalculator.h"
#include "Waterfall.h"
#include "LoanAssumption.h"
class QStandardItemModel;
class QListView;
class QSortFilterProxyModel;
class QComboBox;
class QLineEdit;
class QTableView;
class QCheckBox;
class QDateEdit;
class QPushButton;
class QTabWidget;
class PoolTableProxy;
class LoanAssumptionsEditor : public QWidget
{
	Q_OBJECT

public:
	LoanAssumptionsEditor(QWidget *parent=nullptr);
	void AddLoanAssumption(const LoanAssumption& a);
	void AddLoanToPool(Mortgage& a); 
#ifndef NO_DATABASE
	void FillFromQuery();
#endif
private:
	void CreateScenarioEditor();
	void CreatePoolMatcher();
	bool YesNoDialog(const QString& Title, const QString& Mess);
	LoanAssumption BuildCurrentAssumption() const;
	bool ScenExist(const QString& Teststr)const;
	bool CheckAliasInput();
	void FillScenariosModel();
	QHash<QString,QSharedPointer<LoanAssumption> > m_Assumptions;
	QSharedPointer<LoanAssumption> ActiveAssumption;
	QStandardItemModel* m_ScenariosModel;
	QStandardItemModel* m_AliasesModel;
	QStandardItemModel* m_SeniorAsumptionsModel;
	QStandardItemModel* m_MezzAsumptionsModel;
	QSortFilterProxyModel* m_SortScenarios;
	QListView* m_ScenarioList;
	QListView* m_AliasesList;
	QTableView* m_SeniorTable;
	QTableView* m_MezzTable;
	QComboBox* m_ScenariosCombo;
	QLineEdit* m_ScenarioNameEdit;
	QLineEdit* m_AliasLineEdit;
	QCheckBox* m_seniorDateCheck;
	QCheckBox* m_MezzDateCheck;
	QDateEdit* m_SeniorDateEdit;
	QDateEdit* m_MezzDateEdit;
	QPushButton* AddAliasButton;
	QPushButton* RemoveAliasButton;
	QPushButton* ReplaceAliasButton;
	QPushButton* AddSeniorAssumptionButton;
	QPushButton* RemoveSeniorAssumptionButton;
	QPushButton* AddMezzAssumptionButton;
	QPushButton* RemoveMezzAssumptionButton;
	QPushButton* AddScenarioButton;
	QPushButton* RemoveScenarioButton;
	QPushButton* SaveCurrentButton;
	QPushButton* SaveAllButton;
	QPushButton* DiscardCurrentButton;
	QPushButton* DiscardAllButton;
	QTabWidget* BaseTab;
	QStandardItemModel* m_PoolModel;
	QTableView* m_PoolTable;
	PoolTableProxy* m_PoolSorter;
	int m_LastColSorted;
	MtgCalculator m_LoanPool;
	Waterfall m_WtfToExtension;
	Waterfall m_WtfToCall;
	QString m_LastModelLoaded;
	QPushButton* GuessAssumptionsButton;
	QPushButton* LoadPoolButton;
	QCheckBox* OverrideManualScenariosCheck;
	QHash<QString, QSharedPointer<LoanAssumption> > m_DirtyAssumptions;
signals:
	void ActiveAssumptionChanged();
private slots:
	void SetPoolModelChecks(const QModelIndex& index, const QModelIndex&);
	void RemoveScenario();
	void GuessAssumptions(bool OverrideManual);
	void LoadModel();
	void SeniorDateChanged(const QDate&);
	void MezzDateChanged(const QDate&);
	void CheckAllDirty();
	void CheckCurrentDirty();
	void ChangeScenario(const QModelIndex& curr, const QModelIndex&);
	void SeniorScenarioChanged(const QModelIndex& index);
	void MezzScenarioChanged(const QModelIndex& index);
	void SaveScenario(const QString& key);
	void DiscardScenario(const QString& key);
};
#endif // LOANASSUMPTIONSMODEL_H
