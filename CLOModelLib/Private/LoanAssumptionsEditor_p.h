#ifndef LoanAssumptionsEditor_p_h__
#define LoanAssumptionsEditor_p_h__
#include "MtgCalculator.h"
#include "Waterfall.h"
#include "LoanAssumption.h"
#include <memory>
#include <QSharedPointer>
#include "LoanAssumptionsEditor.h"
//#include "WaterfallCalculator.h"
class WaterfallCalculator;
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
class ReadOnlyColProxy;
class QLabel;
class QAbstractItemView;
class QAbstractItemModel;
class QProgressBar;
class LoanAssMatcher;
class LoanAssumptionsEditorPrivate
{
    DECLARE_PRIVATE_COMMONS(LoanAssumptionsEditor)
protected:
    LoanAssumptionsEditor* q_ptr;
public:
    static void SafeSetModel(QAbstractItemView* View, QAbstractItemModel* NewModel);
    void CreateScenarioEditor();
    void CreatePoolMatcher();
    void CreateModelScanner();
    void CreateStructureComparison();
    bool YesNoDialog(const QString& Title, const QString& Mess);
    LoanAssumption BuildCurrentAssumption() const;
    bool ScenExist(const QString& Teststr)const;
    bool CheckAliasInput();
    void FillScenariosModel();
    QHash<QString, std::shared_ptr<LoanAssumption> > m_Assumptions;
    std::shared_ptr<LoanAssumption> ActiveAssumption;
    QStandardItemModel* m_ScenariosModel;
    QStandardItemModel* m_AliasesModel;
    QStandardItemModel* m_SeniorAsumptionsModel;
    QStandardItemModel* m_MezzAsumptionsModel;
    QStandardItemModel* m_ScanPoolsModel;
    QStandardItemModel* m_ScannedModel;
    QSortFilterProxyModel* m_SortScenarios;
    QSortFilterProxyModel* m_ScannedPoolsProxy;
    ReadOnlyColProxy* m_ScannedModelProxy;
    int m_LastScannedColSorted;
    QWidget* m_ScenarioEditorRightSection;
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
    QPushButton* m_ScanPoolsButton;
    QPushButton* m_CancelScanPoolsButton;
    QProgressBar* m_ScanProgress;
    QTabWidget* BaseTab;
    QStandardItemModel* m_PoolModel;
    QTableView* m_PoolTable;
    ReadOnlyColProxy* m_PoolSorter;
    int m_LastPoolColSorted;
    MtgCalculator m_LoanPool;
    Waterfall m_WtfToExtension;
    Waterfall m_NewWtfToExtension;
    Waterfall m_WtfToCall;
    Waterfall m_NewWtfToCall;
    QPointer<MtgCalculator> m_NewLoans;
    QPointer<WaterfallCalculator> m_NewWatFalls;
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
    QHash<QString, std::shared_ptr<LoanAssumption> > m_DirtyAssumptions;
    QListView* m_PoolScanFilterView;
    QListView* m_PoolScanDealsView;
    QTableView* m_PoolScanPoolView;
    QPushButton* m_ClearPoolScanFilterButton;
    LoanAssMatcher* m_PoolMatcher;
    QLineEdit* m_ModelsDirEdit;
    bool m_ScanningPools;
    bool m_EnableLoad;
    enum { ColumnsInStructure = 11 };
    enum
    {
        TrancheNameCol = 0
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
    enum { TestInfinityThreshold = 10 };
    QStandardItemModel* m_OriginalStructureModel;
    QStandardItemModel* m_NewStructureModel;
    QList<QSharedPointer<QStandardItemModel> > m_OriginalTranchesModels;
    QList<QSharedPointer<QStandardItemModel> > m_NewTranchesModels;
    QTableView* m_OldStrGenTable;
    QTableView* m_OldStrDetTable;
    QTableView* m_NewStrGenTable;
    QTableView* m_NewStrDetTable;
    QLabel* m_ModelNameLabel;
};
#endif // LoanAssumptionsEditor_p_h__