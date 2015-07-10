#ifndef StressViewer_p_h__
#define StressViewer_p_h__
#include "StressViewer.h"
#include "Waterfall.h"
#include "StressTest.h"
#include <QPropertyAnimation>
#include <QSet>
#include <QString>
class QTableWidget;
class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QTableModel;
class StressViewerPrivate;
class StressViewerPrivate
{
    DECLARE_PRIVATE_COMMONS(StressViewer)
protected:
    StressViewer* q_ptr;
public:
    AssumptionSet AssembleAssumption(int r, int c) const;
    QString ParamNames[NumStressDimentsions];
    QComboBox* DisplayValuesCombo;
    QDoubleSpinBox* PriceSpin;
    QLabel* PriceLabel;
    QList<QString> AvailableAssumptions[NumStressDimentsions];
    QTableWidget* StressTable;
    StressTest* StressTarget;
    QComboBox* TrancheCombo;
    QComboBox* ConstParCombo[NumStressDimentsions - 2];
    QLabel* ConstParLabel[NumStressDimentsions - 2];
    QComboBox* VariableParCombo[2];
    QLabel* VariableParLabel[2];
    QPropertyAnimation GradientBase;
    bool PriceToBeSet;
    enum
    {
        AssCPR = 0
        , AssCDR = 1
        , AssLS = 2
        , AssRecLag = 3
        , AssDelinq = 4
        , AssDelinqLag = 5
    };

    const QColor GreenFill;
    const QColor YellowFill;
    const QColor RedFill;
    const QColor GrayFill;
};
#endif // StressViewer_p_h__
