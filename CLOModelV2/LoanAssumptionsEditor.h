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
class LoanAssumptionsEditor : public QWidget
{
	Q_OBJECT

public:
	LoanAssumptionsEditor(QWidget *parent=nullptr);
	enum DocDirtyEnum { NotDirty = 0x0, Edited = 0x1, Added = 0x2, NameEdited = 0x4 };
	Q_DECLARE_FLAGS(DocDirty, DocDirtyEnum)
#ifndef NO_DATABASE
	virtual void FillFromQuery();
#endif
private:
	void FillScenariosModel();
	QHash<QString,QSharedPointer<LoanAssumption> > m_Assumptions;
	QSharedPointer<LoanAssumption> ActiveAssumption;
	QStandardItemModel* m_ScenariosModel;
	QStandardItemModel* m_AliasesModel;
	QSortFilterProxyModel* m_SortScenarios;
	QListView* m_ScenarioList;
	QListView* m_AliasesList;
	QComboBox* m_ScenariosCombo;
	QLineEdit* m_ScenarioNameEdit;
	QLineEdit* m_AliasLineEdit;

	QHash<QString, DocDirty> m_IsDirty;
	QHash<QString, QSharedPointer<LoanAssumption> > m_DirtyAssumptions;
signals:
	void ActiveAssumptionChanged();
private slots:
	void ChangeScenario(const QModelIndex& curr, const QModelIndex& prev);
	//void SaveCurrentScenario();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(LoanAssumptionsEditor::DocDirty)
#endif // LOANASSUMPTIONSMODEL_H
