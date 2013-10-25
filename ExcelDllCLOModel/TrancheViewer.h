#ifndef TrancheViewer_h__
#define TrancheViewer_h__

#include <QWidget>
class QTableWidget;
class Tranche;
class TrancheViewer : public QWidget{
	Q_OBJECT
public:
	TrancheViewer(QWidget* parent=0);
	void SetTranche(const Tranche& a);
private:
	QTableWidget* MainTable;
protected:
	void resizeEvent(QResizeEvent *event);
};
#endif // TrancheViewer_h__