#ifndef LOANASSUMPTIONSMODEL_H
#define LOANASSUMPTIONSMODEL_H
#include <QWidget>
#include <QHash>
#include <QSharedPointer>
#include "clomodellib_global.h"
class LoanAssumptionsEditorPrivate;
class Mortgage;
class LoanAssumption;
class CLOMODELLIB_EXPORT LoanAssumptionsEditor : public QWidget
{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(LoanAssumptionsEditor)
    LoanAssumptionsEditor(LoanAssumptionsEditorPrivate* d, QWidget *parent = nullptr);
    LoanAssumptionsEditorPrivate* d_ptr;
    void closeEvent(QCloseEvent * ev);
public:
	LoanAssumptionsEditor(QWidget *parent=nullptr);
    virtual ~LoanAssumptionsEditor();
	QHash<qint32, QString> GetScenarios() const;
	void AddLoanAssumption(const LoanAssumption& a);
	void AddLoanToPool(qint32 index , Mortgage& a); 
	void SetEnableLoad(bool a);
	int LoanCount() const;
	void FillFromQuery();
signals:
	void ActiveAssumptionChanged();
	void PoolSaved();
protected slots:
	void CalculateNewStructure();
	void SavePool();
	void SortPool();
	void SortScanned();
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
	void NewTranchesCalculated();
};
#endif // LOANASSUMPTIONSMODEL_H
