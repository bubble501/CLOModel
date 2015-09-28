#ifndef WATERFALLSTEPHELPERDIALOG_H
#define WATERFALLSTEPHELPERDIALOG_H
#include <QDialog>
#include <QHash>
#include <memory>
#include "AbstractTrigger.h"
#include "clomodellib_global.h"
class WaterfallStepHelperDialogPrivate;
class CLOMODELLIB_EXPORT WaterfallStepHelperDialog : public QDialog
{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(WaterfallStepHelperDialog)
    WaterfallStepHelperDialog(WaterfallStepHelperDialogPrivate* d, QWidget *parent = nullptr);
    WaterfallStepHelperDialogPrivate* d_ptr;
public:
	WaterfallStepHelperDialog(QWidget *parent=nullptr);
    virtual ~WaterfallStepHelperDialog();
	QString GetParameters();
	void SetAvailableTriggers(const QHash<quint32, std::shared_ptr<AbstractTrigger> >& AvailableTriggers);
	void SetCurrentPars(const QString& a);
	const bool& GetInterestWF() const;
	void SetInterestWF(const bool& val);
protected slots:
	void CheckOkEnabled(int index);
	void ClearParameters();
	void SetParameter(int PrmIndex, QString value);
	void SetSeniorityGroup(QString value);
	void SetSeniorityGroupLevel(QString value);
	void SetRedemptionGroup(QString value);
	void SetRedemptionGroupLevel(QString value);
	void SetRedemptionShare(QString value);
	void SetAdditionalCollateralShare(QString value);
	void SetSourceOfFunding(QString value);
	void SetCouponIndex(QString value);
	void SetTestTargetOverride(QString value);
	void SetIRRtoEquityTarget(QString value);
	void SetReserveIndex(QString value);
	void SetPayAccrue(QString value);
	void SetDefaults1();
	void SetDefaults100();
	void SetDefaults0();
	void SetbasedOnWaterfall();
	void ResetSoFCombo(int index);
	void SoFComboChanged(int index);
	void ResetPayAccrueCombo(int index);
	void PayAccrueComboChanged(int index);
	void ClearAndAccept();
signals:
	void SetTo1(QString);
	void SetTo100(QString);
	void SetTo0(QString);
	void SetSoFCombo(int);
	void SetPayAccrueCombo(int);
	void ImportSeniorityGroup(QString value);
	void ImportSeniorityGroupLevel(QString value);
	void ImportRedemptionGroup(QString value);
	void ImportRedemptionGroupLevel(QString value);
	void ImportRedemptionShare(QString value);
	void ImportAdditionalCollateralShare(QString value);
	void ImportSourceOfFunding(QString value);
	void ImportCouponIndex(QString value);
	void ImportTestTargetOverride(QString value);
	void ImportIRRtoEquityTarget(QString value);
	void ImportReserveIndex(QString value);
	void ImportPayAccrue(QString value);
};

#endif // WATERFALLSTEPHELPERDIALOG_H
