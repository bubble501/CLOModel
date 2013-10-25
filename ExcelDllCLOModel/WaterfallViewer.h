#ifndef WaterfallViewer_h__
#define WaterfallViewer_h__
#include <QWidget>
#include "WatFalPrior.h"
class QTableWidget;
class WatFalPrior;
class QLabel;
class WaterfallViewer : public QWidget{
	Q_OBJECT
public:
	WaterfallViewer(QWidget* parent=0);
	void ResetSteps();
	void AddStep(const WatFalPrior& a);
	void AddStep(WatFalPrior::WaterfallStepType Step,int GroupTarget,int RedemptionGroup,double RedemptionShare);
private:
	QTableWidget* InterestTable;
	QTableWidget* PrincipalTable;
	QLabel* InterestLabel;
	QLabel* PrincipalLabel;
protected:
	void resizeEvent(QResizeEvent *event);
};
#endif // WaterfallViewer_h__