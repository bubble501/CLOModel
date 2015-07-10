#ifndef StressTest_p_h__
#define StressTest_p_h__
#include "Private/BackwardCompatibilityInterface_p.h"
#include "StressTest.h"
#include <QDate>
#include <QSharedPointer>
#include <QPointer>
#include <QHash>
#include <QSet>
#include "AssumptionSet.h"
class StressTestPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(StressTest)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(StressTest)
public:
    enum {
        AssCPR=0
        , AssCDR = 1
        , AssLS = 2
        , AssRecLag = 3
        , AssDelinq = 4
        , AssDelinqLag = 5
};
#ifdef PrintExecutionTime
    QTime ExecutionTime;
#endif
    qint32 CurrentAssumption[NumStressDimentsions];
    WaterfallCalculator* TranchesCalculator;
    MtgCalculator* BaseCalculator;
    ScenarioApplier* BaseApplier;
    QSet<QString> m_CDRscenarios;
    QSet<QString> m_CPRscenarios;
    QSet<QString> m_LSscenarios;
    QSet<QString> m_RecLagScenarios;
    QSet<QString> m_DelinqScenarios;
    QSet<QString> m_DelinqLagScenarios;
    Waterfall Structure;
    QDate StartDate;
    QHash<AssumptionSet, QSharedPointer<Waterfall> >  Results;
    bool SequentialComputation;
    bool ContinueCalculation;
    bool ShowProgress;
    bool UseFastVersion;
    QPointer<PhasedProgressWidget> ProgressForm;
    QHash<uint, AssumptionSet> m_RainbowTable;
    bool m_ErrorsOccured;
    QSet<QString>* m_AssumptionsRef[NumStressDimentsions];
};
#endif // StressTest_p_h__