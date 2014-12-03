#ifndef WATERFALLSTEPHELPERDIALOG_H
#define WATERFALLSTEPHELPERDIALOG_H

#include <QDialog>
#include <QMap>
#include <QSharedPointer>
#include "AbstractTrigger.h"
class QComboBox;
class QStackedWidget;
class TriggerStructHelperWidget;
class QSortFilterProxyModel;
class QStandardItemModel;
class WaterfallStepHelperDialog : public QDialog
{
	Q_OBJECT

public:
	WaterfallStepHelperDialog(QWidget *parent=0);
	QString GetParameters();
	void SetAvailableTriggers(const QHash<quint32, QSharedPointer<AbstractTrigger> >& AvailableTriggers);
	void SetCurrentPars(const QString& a);
	const bool& GetInterestWF() const { return m_InterestWF; }
	void SetInterestWF(const bool& val);
private slots:
	void CheckOkEnabled(int index);
	void ClearParameters() { ResultingParameters.clear(); }
	void SetParameter(int PrmIndex, QString value) { ResultingParameters[PrmIndex] = value; }
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
	void SetDefaults1();
	void SetDefaults100();
	void SetDefaults0();
	void SetbasedOnWaterfall();
	void ResetSoFCombo(int index);
	void SoFComboChanged(int index);
	void ClearAndAccept();
signals:
	void SetTo1(QString);
	void SetTo100(QString);
	void SetTo0(QString);
	void SetSoFCombo(int);

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
private:
	enum {
		AllWaterfalls=0
		, InterestWFonly=1
		, PrincipalWFonly = 2
	};
	enum {NumberOfStepTypes=16};
	QStandardItemModel* StepSelectorModel;
	QSortFilterProxyModel* StepsFilter;
	QWidget* CreateInterestWidget();
	QWidget* CreatePrincipalWidget();
	QWidget* CreateICWidget();
	QWidget* CreateOCWidget();
	QWidget* CreateDeferredInterestWidget();
	QWidget* CreateExcessWidget();
	QWidget* CreateReinvestPrincipalWidget();
	QWidget* CreateReserveReplenishWidget();
	QWidget* CreateTurboWidget();
	QWidget* CreateCurePDLWidget();
	QWidget* CreateFeesFromXSWidget();
	QWidget* CreateAllocPrepayWidget();
	bool FirstCombodeleted;
	bool Cleared;
	QMap<qint32,QString> ResultingParameters;
	QPushButton* AcceptButton;
	QComboBox* StepSelectorCombo;
	QStackedWidget* StepBuilderBase;
	bool m_InterestWF;
	TriggerStructHelperWidget* TriggerBuilder;
};

#endif // WATERFALLSTEPHELPERDIALOG_H
