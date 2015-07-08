#ifndef TestTable_h__
#define TestTable_h__
#include <QWidget>
#include "clomodellib_global.h"
class Waterfall;
class SummaryViewPrivate;
class CLOMODELLIB_EXPORT SummaryView : public QWidget
{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(SummaryView)
    SummaryView(SummaryViewPrivate* d, QWidget *parent = nullptr);
    SummaryViewPrivate* d_ptr;
public:
	void ShowCallStructure(bool a=true);
	void SetStructure(const Waterfall& a,const Waterfall& ca);
    void SetStructure(const Waterfall& a);
	SummaryView(QWidget* parent=nullptr);
protected slots:
	void PriceChanged();
};
#endif // TestTable_h__