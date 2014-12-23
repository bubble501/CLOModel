#ifndef LOANASSUMPTIONSMODEL_H
#define LOANASSUMPTIONSMODEL_H

#include <QWidget>
#include <QHash>
#include <QSharedPointer>
#include <QFlags>
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
class LoanAssumptionsEditor : public QWidget
{
	Q_OBJECT

public:
	LoanAssumptionsEditor(QWidget *parent=nullptr);
#ifndef NO_DATABASE
	virtual void FillFromQuery();
#endif
private:
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

	QHash<QString, QSharedPointer<LoanAssumption> > m_DirtyAssumptions;
signals:
	void ActiveAssumptionChanged();
private slots:
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
