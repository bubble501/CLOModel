#ifndef LOANASSUMPTIONSMODEL_H
#define LOANASSUMPTIONSMODEL_H

#include <QWidget>
#include <QHash>
#include <QSharedPointer>
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
class QLabel;
class QAbstractItemView;
class QAbstractItemModel;
class LoanAssumptionsEditor : public QWidget
{
	Q_OBJECT

public:
	LoanAssumptionsEditor(QWidget *parent=nullptr);
	QHash<qint32, QString> GetScenarios() const;
	void AddLoanAssumption(const LoanAssumption& a);
	void AddLoanToPool(qint32 index , Mortgage& a); 
	void SetEnableLoad(bool a);
	int LoanCount() const;
#ifndef NO_DATABASE
	void FillFromQuery();
#endif
private:
	static void SafeSetModel(QAbstractItemView* View, QAbstractItemModel* NewModel);
	void CreateScenarioEditor();
	void CreatePoolMatcher();
	void CreateStructureComparison();
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
	Waterfall m_NewWtfToExtension;
	Waterfall m_WtfToCall;
	Waterfall m_NewWtfToCall;
	QPointer<MtgCalculator> m_NewLoans;
	QString m_LastModelLoaded;
	QPushButton* GuessAssumptionsButton;
	QPushButton* LoadPoolButton;
	QPushButton* AcceptAllOldButton;
	QPushButton* AcceptAllNewButton;
	QPushButton* AcceptAllNonEmptyButton;
	QPushButton* SavePoolButton;
	QPushButton* CalculateNewStructureButton;
	QLabel* m_OldSelectedTrancheLabel;
	QLabel* m_NewSelectedTrancheLabel;
	QHash<QString, QSharedPointer<LoanAssumption> > m_DirtyAssumptions;
	bool m_EnableLoad;
	enum {ColumnsInStructure=11};
	enum {
		TrancheNameCol=0
		, CallDateCol = 1
		, PriceCol = 2
		, IRRCol = 3
		, WALCol = 4
		, DMCol = 5
		, LossCol = 6
		, IRRCallCol = 7
		, WALCallCol = 8
		, DMCallCol = 9
		, LossCallCol = 10
	};
	enum { TestInfinityThreshold=10 };
	QStandardItemModel* m_OriginalStructureModel;
	QStandardItemModel* m_NewStructureModel;
	QList<QSharedPointer<QStandardItemModel> > m_OriginalTranchesModels;
	QList<QSharedPointer<QStandardItemModel> > m_NewTranchesModels;
	QTableView* m_OldStrGenTable;
	QTableView* m_OldStrDetTable;
	QTableView* m_NewStrGenTable;
	QTableView* m_NewStrDetTable;
	QLabel* m_ModelNameLabel;
signals:
	void ActiveAssumptionChanged();
	void PoolSaved();
private slots:
	void CalculateNewStructure();
	void SavePool();
	void SortPool();
	void SetPoolModelChecks(const QModelIndex& index, const QModelIndex&);
	void RemoveScenario();
	void GuessAssumptions();
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
	void OldGenTableSelect(const QModelIndex& index, const QModelIndex&);
	void NewGenTableSelect(const QModelIndex& index, const QModelIndex&);
	void AdjustOldGenTableHeight();
	void AdjustNewGenTableHeight();
	void LoanCalculationFinished();
	void NewTranchesCalculated();
};
#endif // LOANASSUMPTIONSMODEL_H
